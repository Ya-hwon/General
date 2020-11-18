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

#ifdef JStr
		//String overload of swap
		template<> void swap(String& a, String& b) {
			char * temp2 = new char[0];
			temp2 = a._GetStr();
			a._SetStr(b._GetStr());
			b._SetStr(temp2);
			a._SetLength(a.updateLength());
			b._SetLength(b.updateLength());
			return;
		}
#endif

	unsigned int strlen(char * a) {
		int x = 0;
		for (; a[x] != '\0'; x++);
		return x;
	}

	unsigned int strlen(const char * a) {
		int x = 0;
		for (; a[x] != '\0'; x++);
		return x;
	}
}

#endif
