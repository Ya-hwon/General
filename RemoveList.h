#pragma once

#include <functional>

#ifndef JRemoveList
#define JRemoveList

namespace J {

	template<typename T>
	class RemoveList {

	private:
		struct elem {
			T _data;
			elem* next;
		};
		elem* const _data;
		elem* first;
		const size_t size;
		size_t index = 0;
	public:
		~RemoveList() {
			delete[] _data;
		}
		RemoveList(const size_t& size) :size(size), _data(new elem[size]), first(_data) {

		}
		void add(const T& value) {
			if (index < size) {
				_data[index].next = (index == size - 1) ? nullptr : (_data + index + 1);
				_data[index++]._data = value;
			}
		}
		size_t removeIf(const std::function<bool(const T&)>& filter) {
			size_t count = 0;
			elem* current = first;
			if (filter(current->_data)) first = current->next;
			elem* previous = current;
			current = current->next;
			for (; current != nullptr; previous = current, current = current->next) {
				if (filter(current->_data)) {
					previous->next = current->next;
					count++;
				}
			}
			return count;
		}
		void forEach(const std::function<void(T&)> function) {
			for (elem* current = first; current != nullptr; current = current->next) {
				function(current->_data);
			}
		}
	};

}

#endif