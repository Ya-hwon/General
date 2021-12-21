#pragma once

#include <functional>

#ifndef JRemoveList
#define JRemoveList

namespace J {

	template<typename T, bool resizable = true, bool forceContiguousMemory = true>
	class RemoveList {

	private:
		struct elem {
			T data;
			elem* next;
		};
		std::vector<elem*> deleteList;
		elem* data;
		elem* first;
		size_t _capacity;
		size_t dataSize;
		size_t _size;
		size_t writeIndex;
		elem* previous;
	public:
		~RemoveList() {
			delete[] data;
			if (!forceContiguousMemory)for (auto& elem : deleteList) delete[] elem;
		}
		RemoveList(const size_t& size) :_capacity(size), dataSize(_capacity), _size(0), writeIndex(0), data(new elem[size]), first(data), previous(data) {

		}
		void add(const T& value) {
			if (resizable && writeIndex == dataSize) {
				if (forceContiguousMemory) {
					dataSize *= 2;
					elem* newdata = new elem[dataSize];
					size_t index = 0;
					for (elem* current = first; current != nullptr; current = current->next, index++) {
						newdata[index].data = current->data;
						newdata[index].next = newdata + index + 1;
					}
					previous = &newdata[index - 1];
					delete[] data;
					data = newdata;
					first = newdata;
				} else {
					dataSize = _capacity;
					deleteList.push_back(data);
					data = new elem[_capacity];
					_capacity *= 2;
					writeIndex = 0;
				}
			}
			previous->next = &data[writeIndex];
			previous = &data[writeIndex];
			data[writeIndex].next = nullptr;
			data[writeIndex++].data = value;
			_size++;
		}
		size_t removeIf(const std::function<bool(const T&)>& filter) {
			if (_size == 0)return 0;
			size_t count = 0;

			while (filter(first->data)) {
				first = first->next;
				_size--;
				count++;
			}

			for (elem* previous = first, *current = first->next; current != nullptr; current = current->next) {
				if (filter(current->data)) {
					previous->next = current->next;
					count++;
					_size--;
					continue;
				}
				previous = current;
			}
			return count;
		}
		void forEach(const std::function<void(T&)> function) {
			if (_size == 0)return;
			for (elem* current = first; current != nullptr; current = current->next) {
				function(current->data);
			}
		}
		size_t capacity() const {
			if (forceContiguousMemory) return dataSize;
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
		}
	};

}

#endif