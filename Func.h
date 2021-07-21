#pragma once

#ifndef JFuncs
#define JFuncs

namespace J {
		//meant for primitive types
		template<class T> void swap(T& a, T& b) { a = a ^ b; b = b ^ a; a = a ^ b; return; }

		float d_sqrt(float num){//inverse
			int i;
			float x,y;
			x = num*0.5;
			y=num;
			i = *(int*)&y;
			i = 0x5f3759df-(i>>1);
			y=*(float*)&i;
			y *= (1.5 - (x*y*y));
			y *= (1.5 - (x*y*y));
			return num*y;
		}

		uint f_randi(uint32 index){
			index = (index<<13)^index;
			return ((index*(index*index*15731+789221)+1376312589)&0x7fffffff);
		}
}

#endif
