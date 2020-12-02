#pragma once

#ifndef EnumRot
#define EnumRot

namespace J {
	template<typename enumname, enumname max, enumname min = (enumname)0, enumname start = (enumname)0>
	class enumRot {
		enumname current;
	public:
		enumRot() {
			current = start;
		}
		enumRot rot(bool right = true) {
			if (right) {
				current = (current == max) ? min : (enumname)((int)current + 1);
			}
			else {
				current = (current == min) ? max : (enumname)((int)current - 1);
			}
			return *this;
		}

		void reset() { current = start; }
		enumname val() { return current; }
		operator enumname() { return current; }

		enumRot operator++(int) {
			enumRot c = *this;
			rot();
			return c;
		}
		enumRot operator--(int) {
			enumRot c = *this;
			rot(false);
			return c;
		}

		enumRot operator++() { return rot(); }
		enumRot operator--() { return rot(false);}
		enumRot operator=(enumname t) { return current = t; }

		friend std::ostream& operator<<(std::ostream& os, const enumRot eT) {
			os << (int)eT.current;
			return os;
		}
	};
}

#endif
