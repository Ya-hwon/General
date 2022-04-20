#ifndef JRemoveList
#define JRemoveList

#include <functional>
#include <cassert>

namespace J {

	template<typename T, bool resizable = true>
	class RemoveList {

	private:
		struct elem {
			T data;
			elem* next;
		};
		elem* data;
		elem* first;
		size_t _capacity;
		size_t _size;
		size_t writeIndex;
		elem* previous;
	public:
		~RemoveList() {
			delete[] data;
		}
		RemoveList(const size_t& size) : _capacity(size), _size(0), writeIndex(0), data(new elem[size]), first(data), previous(data) {
			assert(size != 0);
		}
		void add(const T& value) {
			if constexpr (resizable) {
				if (writeIndex == _capacity) {
					_capacity *= 2;
					elem* newdata = new elem[_capacity];
					size_t index = 0;
					for (elem* current = first; current != nullptr; current = current->next, index++) {
						newdata[index].data = current->data;
						newdata[index].next = newdata + index + 1;
					}
					previous = &newdata[index - 1];
					delete[] data;
					data = newdata;
					first = newdata;
				}
			}
			previous->next = &data[writeIndex];
			previous = &data[writeIndex];
			data[writeIndex].next = nullptr;
			data[writeIndex++].data = value;
			_size++;
		}
		void removeIf(const std::function<bool(const T&)>& filter) {
			if (_size == 0)return;

			while (filter(first->data)) {
				first = first->next;
				_size--;
				if (!first) return;
			}

			for (elem* previous = first, *current = first->next; current != nullptr; current = current->next) {
				if (filter(current->data)) {
					previous->next = current->next;
					_size--;
					continue;
				}
				previous = current;
			}
			return;
		}

		void forEach(const std::function<void(T&)> function) {
			if (_size == 0)return;
			for (elem* current = first; current != nullptr; current = current->next) {
				function(current->data);
			}
		}
		size_t capacity() const {
			return _capacity;
		}
		size_t size() const {
			return _size;
		}
		[[nodiscard]] T* toArray() const {
			if (_size == 0)return nullptr;
			T* outArray = new T[_size];
			elem* current = first;
			for (size_t i = 0; current != nullptr; current = current->next, i++) {
				outArray[i] = current->data;
			}
			return outArray;
		}
	};

}

#endif