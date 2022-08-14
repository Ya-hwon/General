#ifndef JHPRTree
#define JHPRTree

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
		numberType _minx, _maxx, _miny, _maxy;

		Envelope() : _minx(std::numeric_limits<numberType>::max()), _maxx(std::numeric_limits<numberType>::lowest()), _miny(std::numeric_limits<numberType>::max()), _maxy(std::numeric_limits<numberType>::lowest()) {

		}

		Envelope(const numberType& minx, const numberType& maxx, const numberType& miny, const numberType& maxy) : _minx(minx), _maxx(maxx), _miny(miny), _maxy(maxy) {

		}

		Envelope(const numberType& x, const numberType& y) : _minx(x), _maxx(x), _miny(y), _maxy(y) {

		}

		bool intersects(const Envelope<numberType>& other) const {
			return !(other._minx > _maxx || other._maxx < _minx || other._miny > _maxy || other._maxy < _miny);
		}

		bool intersects(const Point<numberType>& other) const {
			return !(other.x > _maxx || other.x < _minx || other.y > _maxy || other.y < _miny);
		}

		void expand_to_include(const Envelope<numberType>& other) {
			if (other._minx < _minx) _minx = other._minx;
			if (other._maxx > _maxx) _maxx = other._maxx;
			if (other._miny < _miny) _miny = other._miny;
			if (other._maxy > _maxy) _maxy = other._maxy;
		}

		void expand_to_include(const Point<numberType>& other) {
			if (other.x < _minx) _minx = other.x;
			if (other.x > _maxx) _maxx = other.x;
			if (other.y < _miny) _miny = other.y;
			if (other.y > _maxy) _maxy = other.y;
		}

		numberType minx() const {
			return _minx;
		}

		numberType maxx() const {
			return _maxx;
		}

		numberType miny() const {
			return _miny;
		}

		numberType maxy() const {
			return _maxy;
		}

		numberType midx() const {
			return _minx + width() / 2;
		}

		numberType midy() const {
			return _miny + height() / 2;
		}

		numberType width() const {
			return _maxx - _minx;
		}

		numberType height() const {
			return _maxy - _miny;
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
		std::vector<size_t> layer_start_index;
		Envelope<coordType>* node_bounds;

		size_t get_layer_size(const size_t& layer_index) const {
			return layer_start_index[layer_index + 1] - layer_start_index[layer_index];
		}

		void sort_items() {
			const coordType stride_x = extent.width() / H;
			const coordType stride_y = extent.height() / H;

			const coordType extent_min_x = extent.minx();

			std::sort(items.begin(), items.end(), [&](const item& lhs, const item& rhs) {

				const int xlhs = (int)((lhs.geom.midx() - extent_min_x) / stride_x);
				const int ylhs = (int)((lhs.geom.midy() - extent_min_x) / stride_y);

				const int xrhs = (int)((rhs.geom.midx() - extent_min_x) / stride_x);
				const int yrhs = (int)((rhs.geom.midy() - extent_min_x) / stride_y);

				const int indexlhs = hilbertXYToIndex(HILBERT_LEVEL, xlhs, ylhs);
				const int indexrhs = hilbertXYToIndex(HILBERT_LEVEL, xrhs, yrhs);

				if (indexlhs < indexrhs) return true;
				return false;
				});
		}

		void compute_leaf_nodes() {
			for (std::size_t i = 0; i < layer_start_index[1]; i++) {
				for (std::size_t j = 0; j <= NODE_CAPACITY; j++) {
					const std::size_t item_index = NODE_CAPACITY * i + j;
					if (item_index >= items.size()) return;
					node_bounds[i].expand_to_include(items[item_index].geom);
				}
			}
		}

		void compute_layer_nodes() {
			for (std::size_t i = 1; i < layer_start_index.size() - 1; i++) {
				const std::size_t layer_start = layer_start_index[i];
				const std::size_t child_layer_start = layer_start_index[i - 1];
				const std::size_t layer_size = get_layer_size(i);
				const std::size_t child_layer_end = layer_start;
				for (std::size_t j = 0; j < layer_size; j++) {
					const std::size_t child_start = child_layer_start + NODE_CAPACITY * j;
					for (std::size_t k = 0; k <= NODE_CAPACITY; k++) {
						const std::size_t index = child_start + k;
						if (index >= child_layer_end) break;
						node_bounds[layer_start + j].expand_to_include(node_bounds[index]);
					}
				}
			}
		}

		void compute_layer_start_indices() {

			std::size_t item_count = items.size();
			std::size_t index = 0;

			do {
				layer_start_index.push_back(index);

				const std::size_t mult = item_count / NODE_CAPACITY;
				const std::size_t total = mult * NODE_CAPACITY;
				item_count = mult;
				if (total != item_count) {
					item_count++;
				}

				index = index + item_count;
			} while (item_count > 1);	// log16(item_count) indices
		}

		void query_node_children(const std::size_t& layer_index, const std::size_t& blockOffset, const Envelope<coordType>& query_envelope, std::vector<elemType>& candidate_list) const {
			const std::size_t layer_start = layer_start_index[layer_index];
			const std::size_t layer_end = layer_start_index[layer_index + 1];
			for (int i = 0; i < NODE_CAPACITY; i++) {
				const std::size_t node_offset = blockOffset + i;
				if (layer_start + node_offset >= layer_end) return;
				query_node(layer_index, node_offset, query_envelope, candidate_list);
			}
		}
		void query_items(const std::size_t& block_start, const Envelope<coordType>& query_envelope, std::vector<elemType>& candidate_list) const {
			for (std::size_t i = 0; i < NODE_CAPACITY; i++) {
				const std::size_t item_index = block_start + i;
				if (item_index >= items.size()) return;
				const item& current_item = items[item_index];
				if (query_envelope.intersects(current_item.geom)) {
					candidate_list.push_back(current_item.data);
				}
			}
		}

		void query_node(const std::size_t& layer_index, const size_t& node_offset, const Envelope<coordType>& query_envelope, std::vector<elemType>& candidate_list) const {
			const std::size_t layer_start = layer_start_index[layer_index];
			const std::size_t node_index = layer_start + node_offset;
			if (!query_envelope.intersects(node_bounds[node_index])) return;
			const std::size_t child_nodes_offset = node_offset * NODE_CAPACITY;
			if (layer_index == 0) {
				query_items(child_nodes_offset, query_envelope, candidate_list);
			} else {
				query_node_children(layer_index - 1, child_nodes_offset, query_envelope, candidate_list);
			}
		}

	public:
		HPRTree() : node_bounds(nullptr) {}
		HPRTree(const std::size_t& capacity) : node_bounds(nullptr) {
			items.reserve(capacity);
		}
		~HPRTree() {
			delete[] node_bounds;
		}
		void reserve(const std::size_t& size) {
			items.reserve(size);
		}
		std::vector<elemType> query(const Envelope<coordType>& query_envelope) const {
			std::vector<elemType> candidate_list;
			candidate_list.reserve((std::size_t)(avg_entries() * query_envelope.width() * query_envelope.height()));
			query(query_envelope, candidate_list);
			return candidate_list;
		}
		void query(const Envelope<coordType>& query_envelope, std::vector<elemType>& candidate_list) const {
			if (!extent.intersects(query_envelope)) return;

			const std::size_t layer_index = layer_start_index.size() - 2;
			const std::size_t layer_size = get_layer_size(layer_index);

			for (std::size_t i = 0; i < layer_size; i++) {
				query_node(layer_index, i, query_envelope, candidate_list);
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
			return sizeof(std::size_t) * layer_start_index.capacity() +
				sizeof(item) * items.capacity() +
				sizeof(Envelope<coordType>) +
				sizeof(Envelope<coordType>) * layer_start_index[layer_start_index.size() - 1];
		}
		std::size_t size() const {
			return items.size();
		}
		void build(const bool shrink_to_fit = false) {

			if (shrink_to_fit)items.shrink_to_fit();	// might make sense as potentially very big

			sort_items();

			compute_layer_start_indices();

			//if (shrink_to_fit)layer_start_index.shrink_to_fit();	//propably not a good idea as relatively small

			const std::size_t node_count = layer_start_index[layer_start_index.size() - 1];

			node_bounds = new Envelope<coordType>[node_count];

			compute_leaf_nodes();
			compute_layer_nodes();
		}
	};
}

#endif