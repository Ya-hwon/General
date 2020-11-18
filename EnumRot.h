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
		enumname rot(bool right = true) {
			if (right) {
				current = (current == max) ? min : (enumname)((int)current + 1);
			}
			else {
				current = (current == min) ? max : (enumname)((int)current - 1);
			}
			return current;
		}
		void reset() { current = start; }
		enumname val() { return current; }
		operator enumname() { return current; }
	};
}

#endif
