#pragma once

#include "RemoveList.h"
#include "external/hilbert_curves.h"
#include <Timer.h>
#include <limits>
#include <vector>
#include <mutex>

#ifndef JHPRTree
#define JHPRTree

namespace J {

	template <typename T>
	constexpr T constexpr_pow(T num, size_t pow) {
		return pow == 0 ? 1 : num * constexpr_pow(num, pow - 1);
	}

	constexpr size_t NODE_CAPACITY = 16;
	constexpr size_t HILBERT_LEVEL = 12;
	constexpr size_t H = constexpr_pow(2, HILBERT_LEVEL) - 1;

	constexpr bool USE_RADIX_SORT = false;

	typedef double defaultCoordType;

	template<typename numberType = defaultCoordType>
	struct Point {
		numberType x, y;

		Point(const numberType& x, const numberType& y) : x(x), y(y) {

		}

		numberType minX() const {
			return x;
		}

		numberType maxX() const {
			return x;
		}

		numberType minY() const {
			return y;
		}

		numberType maxY() const {
			return y;
		}

		numberType midX() const {
			return x;
		}

		numberType midY() const {
			return y;
		}
	};

	template<typename numberType = defaultCoordType>
	struct Envelope {
		numberType _minX, _maxX, _minY, _maxY;

		Envelope() : _minX(std::numeric_limits<numberType>::max()), _maxX(std::numeric_limits<numberType>::lowest()), _minY(std::numeric_limits<numberType>::max()), _maxY(std::numeric_limits<numberType>::lowest()) {

		}

		Envelope(const numberType& minX, const numberType& maxX, const numberType& minY, const numberType& maxY) : _minX(minX), _maxX(maxX), _minY(minY), _maxY(maxY) {

		}

		Envelope(const numberType& x, const numberType& y) : _minX(x), _maxX(x), _minY(y), _maxY(y) {

		}

		bool intersects(const Envelope<numberType>& other) const {
			return !(other._minX > _maxX || other._maxX < _minX || other._minY > _maxY || other._maxY < _minY);
		}

		bool intersects(const Point<numberType>& other) const {
			return !(other.x > _maxX || other.x < _minX || other.y > _maxY || other.y < _minY);
		}

		void expandToInclude(const Envelope<numberType>& other) {
			if (other._minX < _minX) _minX = other._minX;
			if (other._maxX > _maxX) _maxX = other._maxX;
			if (other._minY < _minY) _minY = other._minY;
			if (other._maxY > _maxY) _maxY = other._maxY;
		}

		void expandToInclude(const Point<numberType>& other) {
			if (other.x < _minX) _minX = other.x;
			if (other.x > _maxX) _maxX = other.x;
			if (other.y < _minY) _minY = other.y;
			if (other.y > _maxY) _maxY = other.y;
		}

		numberType minX() const {
			return _minX;
		}

		numberType maxX() const {
			return _maxX;
		}

		numberType minY() const {
			return _minY;
		}

		numberType maxY() const {
			return _maxY;
		}

		numberType midX() const {
			return _minX + width() / 2;
		}

		numberType midY() const {
			return _minY + height() / 2;
		}

		numberType width() const {
			return _maxX - _minX;
		}

		numberType height() const {
			return _maxY - _minY;
		}
	};

	template<typename elemType, typename coordType = defaultCoordType>
	class HPRTree {
	private:

		typedef Point<coordType> indexGeom;

		struct item {
			indexGeom geom;
			elemType data;
		};

		Envelope<coordType> extent;
		std::vector<item> items;
		std::vector<size_t> layerStartIndex;
		Envelope<coordType>* nodeBounds;

		size_t getLayerSize(const size_t& layerIndex) {
			return layerStartIndex[layerIndex + 1] - layerStartIndex[layerIndex];
		}

		void sortItems() {
			const coordType strideX = extent.width() / H;
			const coordType strideY = extent.height() / H;

			if constexpr (USE_RADIX_SORT) {

				struct indexedItem {
					item i;
					size_t index;
				};

				std::vector<indexedItem> indexedItems;
				indexedItems.reserve(items.size());

				size_t max = 0;

				for (size_t i = 0; i < items.size(); i++) {
					const size_t index = hilbertXYToIndex(HILBERT_LEVEL, (int)((items[i].geom.midX() - extent.minX()) / strideX), (int)((items[i].geom.midY() - extent.minX()) / strideY));
					if (index > max)max = index;
					indexedItems.push_back({ items[i], index });
				}

				constexpr size_t packing = 12;	// im leaving this here for now because it may be sensible to be relative to #items?
				constexpr size_t radix = 1 << packing;

				size_t maxSteps = 0;
				while (max > 0) {
					maxSteps++;
					max >>= packing;
				}
				for (size_t i = 0; i < maxSteps; i++) {
					std::queue<indexedItem> buckets[radix];

					for (auto& indexedItem : indexedItems) {
						buckets[(indexedItem.index >> (i * packing)) % radix].push(indexedItem);
					}

					size_t index = 0;

					for (auto& bucket : buckets) {
						while (!bucket.empty()) {
							(indexedItems)[index++] = bucket.front();
							bucket.pop();
						}
					}
				}
				for (size_t i = 0; i < indexedItems.size(); i++)items[i] = (indexedItems)[i].i;
			} else {
				std::sort(items.begin(), items.end(), [&](const item& lhs, const item& rhs) {

					const int xlhs = (int)((lhs.geom.midX() - extent.minX()) / strideX);
					const int ylhs = (int)((lhs.geom.midY() - extent.minX()) / strideY);

					const int xrhs = (int)((rhs.geom.midX() - extent.minX()) / strideX);
					const int yrhs = (int)((rhs.geom.midY() - extent.minX()) / strideY);

					const int indexlhs = hilbertXYToIndex(HILBERT_LEVEL, xlhs, ylhs);
					const int indexrhs = hilbertXYToIndex(HILBERT_LEVEL, xrhs, yrhs);

					if (indexlhs < indexrhs) return true;
					return false;
				});
			}
		}

		void computeLeafNodes() {
			for (size_t i = 0; i < layerStartIndex[1]; i++) {
				for (size_t j = 0; j <= NODE_CAPACITY; j++) {
					const size_t itemIndex = NODE_CAPACITY * i + j;
					if (itemIndex >= items.size()) return;
					nodeBounds[i].expandToInclude(items[itemIndex].geom);
				}
			}
		}

		void computeLayerNodes() {
			for (size_t i = 1; i < layerStartIndex.size() - 1; i++) {
				const size_t layerStart = layerStartIndex[i];
				const size_t childLayerStart = layerStartIndex[i - 1];
				const size_t layerSize = getLayerSize(i);
				const size_t childLayerEnd = layerStart;
				for (size_t j = 0; j < layerSize; j++) {
					const size_t childStart = childLayerStart + NODE_CAPACITY * j;
					for (size_t k = 0; k <= NODE_CAPACITY; k++) {
						const size_t index = childStart + k;
						if (index >= childLayerEnd) break;
						nodeBounds[layerStart + j].expandToInclude(nodeBounds[index]);
					}
				}
			}
		}

		void computeLayerStartIndices() {

			size_t itemCount = items.size();
			size_t index = 0;

			do {
				layerStartIndex.push_back(index);

				const size_t mult = itemCount / NODE_CAPACITY;
				const size_t total = mult * NODE_CAPACITY;
				itemCount = mult;
				if (total != itemCount) {
					itemCount++;
				}

				index = index + itemCount;
			} while (itemCount > 1);	// log16(itemCount) indices
		}

		void queryNodeChildren(const size_t& layerIndex, const size_t& blockOffset, const Envelope<coordType>& queryEnvelope, RemoveList<elemType, true>& removeList) {
			const size_t layerStart = layerStartIndex[layerIndex];
			const size_t layerEnd = layerStartIndex[layerIndex + 1];
			for (int i = 0; i < NODE_CAPACITY; i++) {
				const size_t nodeOffset = blockOffset + i;
				if (layerStart + nodeOffset >= layerEnd) return;
				queryNode(layerIndex, nodeOffset, queryEnvelope, removeList);
			}
		}

		void queryItems(const size_t& blockStart, const Envelope<coordType>& queryEnvelope, RemoveList<elemType, true>& removeList) {
			for (size_t i = 0; i < NODE_CAPACITY; i++) {
				const size_t itemIndex = blockStart + i;
				if (itemIndex >= items.size()) return;
				item& currentItem = items[itemIndex];
				if (queryEnvelope.intersects(currentItem.geom)) {
					removeList.add(currentItem.data);
				}
			}
		}

		void queryNode(const size_t& layerIndex, const size_t& nodeOffset, const Envelope<coordType>& queryEnvelope, RemoveList<elemType, true>& removeList) {
			const size_t layerStart = layerStartIndex[layerIndex];
			const size_t nodeIndex = layerStart + nodeOffset;
			if (!queryEnvelope.intersects(nodeBounds[nodeIndex])) return;
			if (layerIndex == 0) {
				const size_t childNodesOffset = nodeOffset * NODE_CAPACITY;
				queryItems(childNodesOffset, queryEnvelope, removeList);
			} else {
				const size_t childNodesOffset = nodeOffset * NODE_CAPACITY;
				queryNodeChildren(layerIndex - 1, childNodesOffset, queryEnvelope, removeList);
			}
		}

	public:
		HPRTree() : nodeBounds(nullptr) {}
		~HPRTree() {
			delete[] nodeBounds;
		}
		void reserve(const size_t& size) {
			items.reserve(size);
		}
		void query(const Envelope<coordType>& queryEnvelope, RemoveList<elemType, true>& removeList) {
			if (!extent.intersects(queryEnvelope)) return;

			const size_t layerIndex = layerStartIndex.size() - 2;
			const size_t layerSize = getLayerSize(layerIndex);

			for (size_t i = 0; i < layerSize; i++) {
				queryNode(layerIndex, i, queryEnvelope, removeList);
			}
		}
		void add(const elemType& elem, const indexGeom& geom) {
			items.push_back({ geom, elem });
			extent.expandToInclude(geom);
		}
		void build(const bool shrinkToFit = false) {

			if (shrinkToFit)items.shrink_to_fit();	// might make sense as potentially very big

			sortItems();

			computeLayerStartIndices();

			//layerStartIndex.shrink_to_fit();	//propably not a good idea as relatively small

			const size_t nodeCount = layerStartIndex[layerStartIndex.size() - 1];

			nodeBounds = new Envelope<coordType>[nodeCount];

			computeLeafNodes();
			computeLayerNodes();
		}
	};
}

#endif