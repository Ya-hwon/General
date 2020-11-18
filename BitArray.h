
#pragma once

#ifndef JBitArray
#define JBitArray

#ifndef _CSTDINT_
#include <cstdint>
#endif

///
///----------------------------------------
///
///exception 0: Index out of range (or mode is broken)
///
///exception 1: Allocation failed
///
///----------------------------------------
///


namespace J {
	class Bitarray {
		void * storage = nullptr;
		size_t mode;
		static constexpr size_t registersize = sizeof(size_t) * 8;
		size_t doesneed;
		size_t nrBits;

	public:
		Bitarray(size_t bitb, bool nulled = true) {
			nrBits = bit;
			mode = fitsbest(bit);

			if (mode == 3) {	//reg
				storage = realloc(nullptr, sizeof(uintptr_t)*doesneed);
				if (storage == nullptr)throw 1;
				if(nulled)for (size_t x = doesneed-1; x >= 0; ) { ((uintptr_t *)storage)[x] = 0; if (x == 0)break; x--; }
			}
			else if (mode == 2) {	//32b
				storage = realloc(nullptr, sizeof(uint32_t)*doesneed);
				if (storage == nullptr)throw 1;
				if(nulled)for (size_t x = doesneed-1; x >= 0; ) { ((uint32_t *)storage)[x] = 0; if (x == 0)break; x--; }
			}
			else if (mode == 1) {	//16b
				storage = realloc(nullptr, sizeof(uint16_t)*doesneed);
				if (storage == nullptr)throw 1;
				if(nulled)for (size_t x = doesneed-1; x >= 0; ) { ((uint16_t *)storage)[x] = 0; if (x == 0)break; x--; }
			}
			else if (mode == 0) {	//8b
				storage = realloc(nullptr, sizeof(uint8_t)*doesneed);
				if (storage == nullptr)throw 1;
				if(nulled)for (size_t x = doesneed-1; x >= 0; ) { ((uint8_t *)storage)[x] = 0; if (x == 0)break; x--; }
			}
		}
		Bitarray(size_t bit, bool nulled = true, size_t pmode) {
			nrBits = bit;
			mode = pmode;

			if (mode == 3) {	//reg
				storage = realloc(nullptr, sizeof(uintptr_t)*doesneed);
				if (storage == nullptr)throw 1;
				if(nulled)for (size_t x = doesneed-1; x >= 0; ) { ((uintptr_t *)storage)[x] = 0; if (x == 0)break; x--; }
			}
			else if (mode == 2) {	//32b
				storage = realloc(nullptr, sizeof(uint32_t)*doesneed);
				if (storage == nullptr)throw 1;
				if(nulled)for (size_t x = doesneed-1; x >= 0; ) { ((uint32_t *)storage)[x] = 0; if (x == 0)break; x--; }
			}
			else if (mode == 1) {	//16b
				storage = realloc(nullptr, sizeof(uint16_t)*doesneed);
				if (storage == nullptr)throw 1;
				if(nulled)for (size_t x = doesneed-1; x >= 0; ) { ((uint16_t *)storage)[x] = 0; if (x == 0)break; x--; }
			}
			else if (mode == 0) {	//8b
				storage = realloc(nullptr, sizeof(uint8_t)*doesneed);
				if (storage == nullptr)throw 1;
				if(nulled)for (size_t x = doesneed-1; x >= 0; ) { ((uint8_t *)storage)[x] = 0; if (x == 0)break; x--; }
			}
		}
		~Bitarray() {
			free(storage);
		}

		void resize(size_t newSize) {
			nrBits = newSize;
			size_t prevdoesneed = doesneed;
			size_t prevmode = mode;
			size_t prevBits;
			size_t newBits;

			size_t prevmodesize = (prevmode == 3 ? sizeof(uintptr_t) : prevmode == 2 ? 4 : prevmode == 1 ? 2 : 1);
			prevBits = prevdoesneed * (prevmode == 3 ? sizeof(uintptr_t)*8 : prevmode == 2 ? 32 : prevmode == 1 ? 16 : 8);
			mode = fitsbest(newSize);
			size_t modesize = (mode == 3 ? sizeof(uintptr_t) : mode == 2 ? 4 : mode == 1 ? 2 : 1);
			newBits = doesneed * (mode == 3 ? sizeof(uintptr_t)*8 : mode == 2 ? 32 : mode == 1 ? 16 : 8);

			if (prevmode == 3) {	//reg
				if (mode == 3) {
					ReallocBitarray<uintptr_t, uintptr_t>(prevBits, newBits, modesize, prevmodesize);
				}
				else if (mode == 2) {
					ReallocBitarray<uint32_t, uintptr_t>(prevBits, newBits, modesize, prevmodesize);
				}
				else if (mode == 1) {
					ReallocBitarray<uint16_t, uintptr_t>(prevBits, newBits, modesize, prevmodesize);
				}
				else if (mode == 0) {
					ReallocBitarray<uint8_t, uintptr_t>(prevBits, newBits, modesize, prevmodesize);
				}
			}
			if (prevmode == 2) {	//32b
				if (mode == 3) {
					ReallocBitarray<uintptr_t, uint32_t>(prevBits, newBits, modesize, prevmodesize);
				}
				else if (mode == 2) {
					ReallocBitarray<uint32_t, uint32_t>(prevBits, newBits, modesize, prevmodesize);
				}
				else if (mode == 1) {
					ReallocBitarray<uint16_t, uint32_t>(prevBits, newBits, modesize, prevmodesize);
				}
				else if (mode == 0) {
					ReallocBitarray<uint8_t, uint32_t>(prevBits, newBits, modesize, prevmodesize);
				}
			}
			if (prevmode == 1) {	//16b
				if (mode == 3) {
					ReallocBitarray<uintptr_t, uint16_t>(prevBits, newBits, modesize, prevmodesize);
				}
				else if (mode == 2) {
					ReallocBitarray<uint32_t, uint16_t>(prevBits, newBits, modesize, prevmodesize);
				}
				else if (mode == 1) {
					ReallocBitarray<uint16_t, uint16_t>(prevBits, newBits, modesize, prevmodesize);
				}
				else if (mode == 0) {
					ReallocBitarray<uint8_t, uint16_t>(prevBits, newBits, modesize, prevmodesize);
				}
			}
			if (prevmode == 0) {	//8b
				if (mode == 3) {
					ReallocBitarray<uintptr_t, uint8_t>(prevBits, newBits, modesize, prevmodesize);
				}
				else if (mode == 2) {
					ReallocBitarray<uint32_t, uint8_t>(prevBits, newBits, modesize, prevmodesize);
				}
				else if (mode == 1) {
					ReallocBitarray<uint16_t, uint8_t>(prevBits, newBits, modesize, prevmodesize);
				}
				else if (mode == 0) {
					ReallocBitarray<uint8_t, uint8_t>(prevBits, newBits, modesize, prevmodesize);
				}
			}
		}

		bool get(size_t index) {
			if (index < nrBits) {
				if (mode == 3) {	//reg
					size_t a = getStorageIndex(index);
					auto cval = ((uintptr_t*)storage)[a];
					cval = (cval >> (index - a * registersize)) & 1;
					return (cval % 2);
				}
				if (mode == 2) {	//32b
					size_t a = getStorageIndex(index);
					auto cval = ((uint32_t*)storage)[a];
					cval = (cval >> (index - a * 32)) & 1;
					return (cval % 2);
				}
				if (mode == 1) {	//16b
					size_t a = getStorageIndex(index);
					auto cval = ((uint16_t*)storage)[a];
					cval = (cval >> (index - a * 16)) & 1;
					return (cval % 2);
				}
				if (mode == 0) {	//8b
					size_t a = getStorageIndex(index);
					auto cval = ((uint8_t*)storage)[a];
					cval = (cval >> (index - a * 8)) & 1;
					return (cval % 2);
				}
				return 0;
			}
			throw 0;
		}

		void set(size_t index, bool val) {
			if (index < nrBits) {
				if (mode == 3) {
					bool cval = this->get(index);
					size_t a = getStorageIndex(index);
					if (cval != val)((uintptr_t *)storage)[a] ^= (1 << index - a * registersize);
					return;
				}
				if (mode == 2) {
					bool cval = this->get(index);
					size_t a = getStorageIndex(index);
					if (cval != val)((uint32_t *)storage)[a] ^= (1 << index - a * 32);
					return;
				}
				if (mode == 1) {
					bool cval = this->get(index);
					size_t a = getStorageIndex(index);
					if (cval != val)((uint16_t *)storage)[a] ^= (1 << index - a * 16);
					return;
				}
				if (mode == 0) {
					bool cval = this->get(index);
					size_t a = getStorageIndex(index);
					if (cval != val)((uint8_t *)storage)[a] ^= (1 << index - a * 8);
					return;
				}
			}
			throw 0;
		}

		size_t availableBits() {
			return nrBits;
		}

	private:
		size_t fitsbest(size_t bit) {
			size_t wouldneed[4];
			for (size_t x = 0;; x++) { wouldneed[0] = x; if (x * 8 >= bit)break; }
			for (size_t x = 0;; x++) { wouldneed[1] = x; if (x * 16 >= bit)break; }
			for (size_t x = 0;; x++) { wouldneed[2] = x; if (x * 32 >= bit)break; }
			for (size_t x = 0;; x++) { wouldneed[3] = x; if (x * registersize >= bit)break; }
			size_t eff[4];
			eff[0] = wouldneed[0] + (wouldneed[0] * 8 - bit);
			eff[1] = wouldneed[1] + (wouldneed[1] * 16 - bit);
			eff[2] = wouldneed[2] + (wouldneed[2] * 32 - bit);
			eff[3] = wouldneed[3] + (wouldneed[3] * registersize - bit);
			short index = primitiveindexmin(eff[3], eff[2], eff[1], eff[0]);
			doesneed = wouldneed[index];
			return index;
		}
		short primitiveindexmin(size_t a, size_t b, size_t c, size_t d) {
			if (a <= b && a <= c && a <= d)return 3;
			if (b <= a && b <= c && b <= d)return 2;
			if (c <= a && c <= b && c <= d)return 1;
			return 0;
		}
		size_t getStorageIndex(size_t index) {
			if (index < nrBits) {
				for (int x = 1;; x++) {
					if (index < (x * (mode == 3 ? registersize : mode == 2 ? 32 : mode == 1 ? 16 : 8)))return --x;
				}
			}
			throw 0;
		}
		template<typename A, typename B>void ReallocBitarray(size_t prevBits, size_t newBits, size_t modesize, size_t prevmodesize) {
			storage = realloc(storage, sizeof(A)*doesneed);
			if (storage == nullptr)throw 1;
			if (prevBits < newBits) {
				for (size_t x = doesneed * (modesize / prevmodesize) - 1; x >= prevBits / (newBits / prevBits); --x) { ((B *)storage)[x] = 0; if (x == 0)break; }
			}
		}
	};
}
#ifndef
