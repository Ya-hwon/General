#pragma once

#include <functional>

#ifndef JRemoveList
#define JRemoveList

namespace J {

	template<typename T, bool resizable = false>
	class RemoveList {

	private:
		struct elem {
			T _data;
			elem* next;
		};
		elem* _data;
		elem* first;
		size_t allocSize;
		size_t _size;
		size_t writeIndex;
	public:
		~RemoveList() {
			delete[] _data;
		}
		RemoveList(const size_t& size) :allocSize(size), _size(0), writeIndex(0), _data(new elem[size]), first(_data) {

		}
		void add(const T& value) {
			if (resizable && writeIndex == allocSize) {
				allocSize *= 2;
				elem* new_data = new elem[allocSize];
				size_t index = 0;
				for (elem* current = first; current != nullptr; current = current->next, index++) {
					new_data[index]._data = current->_data;
					new_data[index].next = new_data + index + 1;
				}
				delete[] _data;
				_data = new_data;
				first = new_data;
			}
			_data[writeIndex].next = (writeIndex == allocSize - 1) ? nullptr : (_data + writeIndex + 1);
			_data[writeIndex++]._data = value;
			_size++;
		}
		size_t removeIf(const std::function<bool(const T&)>& filter) {
			size_t count = 0;
			elem* current = first;
			if (filter(current->_data)) {
				first = current->next;
				_size--;
				count++;
			}
			elem* previous = current;
			current = current->next;
			for (; current != nullptr; previous = current, current = current->next) {
				if (filter(current->_data)) {
					previous->next = current->next;
					count++;
					_size--;
				}
			}
			return count;
		}
		void forEach(const std::function<void(T&)> function) {
			for (elem* current = first; current != nullptr; current = current->next) {
				function(current->_data);
			}
		}
		size_t capacity() const {
			return allocSize;
		}
		size_t size() const {
			return _size;
		}
		[[nodiscard]] T* toArray() const {
			T* outArray = new T[_size];
			size_t i = 0;
			for (elem* current = first; i < _size; current = current->next, i++) {
				outArray[i] = current->_data;
			}
		}
	};

}

#endif