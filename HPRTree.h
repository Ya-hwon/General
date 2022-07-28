#ifndef JHPRTree
#define JHPRTree

#include "RemoveList.h"
#include "external/hilbert_curves.h"
#include <limits>
#include <vector>
#include <mutex>

namespace J {

	constexpr std::size_t NODE_CAPACITY = 16;
	constexpr std::size_t HILBERT_LEVEL = 12;
	constexpr std::size_t H = (1 << HILBERT_LEVEL) - 1;

	typedef double defaultCoordType;

	template<typename numberType = defaultCoordType>
	struct Point {
		numberType x, y;

		Point(const numberType& x, const numberType& y) : x(x), y(y) {

		}

		numberType minx() const {
			return x;
		}

		numberType maxx() const {
			return x;
		}

		numberType miny() const {
			return y;
		}

		numberType maxy() const {
			return y;
		}

		numberType midx() const {
			return x;
		}

		numberType midy() const {
			return y;
		}
	};

	template<typename numberType = defaultCoordType>
	struct Envelope {
		numberType _minX, _maxX, _minY, _maxY;

		Envelope() : _minX(std::numeric_limits<numberType>::max()), _maxX(std::numeric_limits<numberType>::lowest()), _minY(std::numeric_limits<numberType>::max()), _maxY(std::numeric_limits<numberType>::lowest()) {

		}

		Envelope(const numberType& minx, const numberType& maxx, const numberType& miny, const numberType& maxy) : _minX(minx), _maxX(maxx), _minY(miny), _maxY(maxy) {

		}

		Envelope(const numberType& x, const numberType& y) : _minX(x), _maxX(x), _minY(y), _maxY(y) {

		}

		bool intersects(const Envelope<numberType>& other) const {
			return !(other._minX > _maxX || other._maxX < _minX || other._minY > _maxY || other._maxY < _minY);
		}

		bool intersects(const Point<numberType>& other) const {
			return !(other.x > _maxX || other.x < _minX || other.y > _maxY || other.y < _minY);
		}

		void expand_to_include(const Envelope<numberType>& other) {
			if (other._minX < _minX) _minX = other._minX;
			if (other._maxX > _maxX) _maxX = other._maxX;
			if (other._minY < _minY) _minY = other._minY;
			if (other._maxY > _maxY) _maxY = other._maxY;
		}

		void expand_to_include(const Point<numberType>& other) {
			if (other.x < _minX) _minX = other.x;
			if (other.x > _maxX) _maxX = other.x;
			if (other.y < _minY) _minY = other.y;
			if (other.y > _maxY) _maxY = other.y;
		}

		numberType minx() const {
			return _minX;
		}

		numberType maxx() const {
			return _maxX;
		}

		numberType miny() const {
			return _minY;
		}

		numberType maxy() const {
			return _maxY;
		}

		numberType midx() const {
			return _minX + width() / 2;
		}

		numberType midy() const {
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

		size_t get_layer_size(const size_t& layerIndex) const {
			return layerStartIndex[layerIndex + 1] - layerStartIndex[layerIndex];
		}

		void sort_items() {
			const coordType strideX = extent.width() / H;
			const coordType strideY = extent.height() / H;

			const coordType extentMinX = extent.minx();

			std::sort(items.begin(), items.end(), [&](const item& lhs, const item& rhs) {

				const int xlhs = (int)((lhs.geom.midx() - extentMinX) / strideX);
				const int ylhs = (int)((lhs.geom.midy() - extentMinX) / strideY);

				const int xrhs = (int)((rhs.geom.midx() - extentMinX) / strideX);
				const int yrhs = (int)((rhs.geom.midy() - extentMinX) / strideY);

				const int indexlhs = hilbertXYToIndex(HILBERT_LEVEL, xlhs, ylhs);
				const int indexrhs = hilbertXYToIndex(HILBERT_LEVEL, xrhs, yrhs);

				if (indexlhs < indexrhs) return true;
				return false;
				});
		}

		void compute_leaf_nodes() {
			for (std::size_t i = 0; i < layerStartIndex[1]; i++) {
				for (std::size_t j = 0; j <= NODE_CAPACITY; j++) {
					const std::size_t itemIndex = NODE_CAPACITY * i + j;
					if (itemIndex >= items.size()) return;
					nodeBounds[i].expand_to_include(items[itemIndex].geom);
				}
			}
		}

		void compute_layer_nodes() {
			for (std::size_t i = 1; i < layerStartIndex.size() - 1; i++) {
				const std::size_t layerStart = layerStartIndex[i];
				const std::size_t childLayerStart = layerStartIndex[i - 1];
				const std::size_t layerSize = get_layer_size(i);
				const std::size_t childLayerEnd = layerStart;
				for (std::size_t j = 0; j < layerSize; j++) {
					const std::size_t childStart = childLayerStart + NODE_CAPACITY * j;
					for (std::size_t k = 0; k <= NODE_CAPACITY; k++) {
						const std::size_t index = childStart + k;
						if (index >= childLayerEnd) break;
						nodeBounds[layerStart + j].expand_to_include(nodeBounds[index]);
					}
				}
			}
		}

		void compute_layer_start_indices() {

			std::size_t itemCount = items.size();
			std::size_t index = 0;

			do {
				layerStartIndex.push_back(index);

				const std::size_t mult = itemCount / NODE_CAPACITY;
				const std::size_t total = mult * NODE_CAPACITY;
				itemCount = mult;
				if (total != itemCount) {
					itemCount++;
				}

				index = index + itemCount;
			} while (itemCount > 1);	// log16(itemCount) indices
		}

		void query_node_children(const std::size_t& layerIndex, const std::size_t& blockOffset, const Envelope<coordType>& queryEnvelope, RemoveList<elemType, true>& removeList) const {
			const std::size_t layerStart = layerStartIndex[layerIndex];
			const std::size_t layerEnd = layerStartIndex[layerIndex + 1];
			for (int i = 0; i < NODE_CAPACITY; i++) {
				const std::size_t nodeOffset = blockOffset + i;
				if (layerStart + nodeOffset >= layerEnd) return;
				query_node(layerIndex, nodeOffset, queryEnvelope, removeList);
			}
		}

		void query_items(const std::size_t& blockStart, const Envelope<coordType>& queryEnvelope, RemoveList<elemType, true>& removeList) const {
			for (std::size_t i = 0; i < NODE_CAPACITY; i++) {
				const std::size_t itemIndex = blockStart + i;
				if (itemIndex >= items.size()) return;
				const item& currentItem = items[itemIndex];
				if (queryEnvelope.intersects(currentItem.geom)) {
					removeList.add(currentItem.data);
				}
			}
		}

		void query_node(const std::size_t& layerIndex, const size_t& nodeOffset, const Envelope<coordType>& queryEnvelope, RemoveList<elemType, true>& removeList) const {
			const std::size_t layerStart = layerStartIndex[layerIndex];
			const std::size_t nodeIndex = layerStart + nodeOffset;
			if (!queryEnvelope.intersects(nodeBounds[nodeIndex])) return;
			const std::size_t childNodesOffset = nodeOffset * NODE_CAPACITY;
			if (layerIndex == 0) {
				query_items(childNodesOffset, queryEnvelope, removeList);
			} else {
				query_node_children(layerIndex - 1, childNodesOffset, queryEnvelope, removeList);
			}
		}

	public:
		HPRTree() : nodeBounds(nullptr) {}
		HPRTree(const std::size_t& capacity) : nodeBounds(nullptr) {
			items.reserve(capacity);
		}
		~HPRTree() {
			delete[] nodeBounds;
		}
		void reserve(const std::size_t& size) {
			items.reserve(size);
		}
		void query(const Envelope<coordType>& queryEnvelope, RemoveList<elemType, true>& removeList) const {
			if (!extent.intersects(queryEnvelope)) return;

			const std::size_t layerIndex = layerStartIndex.size() - 2;
			const std::size_t layerSize = get_layer_size(layerIndex);

			for (std::size_t i = 0; i < layerSize; i++) {
				query_node(layerIndex, i, queryEnvelope, removeList);
			}
		}
		void add(const elemType& elem, const indexGeom& geom) {
			items.push_back({ geom, elem });
			extent.expand_to_include(geom);
		}

		double avg_entries() const {
			return items.size() / (extent.height() * extent.width());
		}
		std::size_t current_size_bytes() const {
			return sizeof(std::size_t) * layerStartIndex.capacity() +
				sizeof(item) * items.capacity() +
				sizeof(Envelope<coordType>) +
				sizeof(Envelope<coordType>) * layerStartIndex[layerStartIndex.size() - 1];
		}
		std::size_t size() const {
			return items.size();
		}
		void build(const bool shrinkToFit = false) {

			if (shrinkToFit)items.shrink_to_fit();	// might make sense as potentially very big

			sort_items();

			compute_layer_start_indices();

			//if (shrinkToFit)layerStartIndex.shrink_to_fit();	//propably not a good idea as relatively small

			const std::size_t nodeCount = layerStartIndex[layerStartIndex.size() - 1];

			nodeBounds = new Envelope<coordType>[nodeCount];

			compute_leaf_nodes();
			compute_layer_nodes();
		}
	};
}

#endif