#pragma once

#ifndef JTimer
#define JTimer

#define _TIMEELEMOUT(X) auto X = duration / std::chrono::X( 1 ); if ( X != 0 ) {std::cout << X << #X << " " ;} duration -= ( X * std::chrono::X( 1 ) )

namespace J {
	class Timer {

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> begin, end;
		std::chrono::duration<long long, std::nano> duration;
		bool paused;
		bool RAII;
		const char* name;
	public:
		Timer( const char* name = "Timer", bool RAII = true ) : name( name ), RAII( RAII ) {
			if ( RAII ) {
				start();
			}
		}
		~Timer() {
			if ( RAII ) {
				stop();
			}
		}
		void stop() {
			end = std::chrono::high_resolution_clock::now();
			duration = duration + ( end - begin );
			std::cout << name << " stopped after ";
			_TIMEELEMOUT( hours );
			_TIMEELEMOUT( minutes );
			_TIMEELEMOUT( seconds );
			_TIMEELEMOUT( milliseconds );
			_TIMEELEMOUT( microseconds );
			_TIMEELEMOUT( nanoseconds );
		}
		long long GetTimeNs() {
			end = std::chrono::high_resolution_clock::now();
			duration = duration + ( end - begin );
			begin = std::chrono::high_resolution_clock::now();
			return duration.count();
		}
		void start() {
			duration = duration.zero();
			std::cout << name << " started\n";
			begin = std::chrono::high_resolution_clock::now();
		}
		void pause() {
			if ( !paused ) {
				end = std::chrono::high_resolution_clock::now();
				duration = duration + ( end - begin );
				paused = true;
				std::cout << name << " paused\n";
			}
		}
		void resume() {
			if ( paused ) {
				paused = false;
				std::cout << name << " resumed\n";
				begin = std::chrono::high_resolution_clock::now();
			}
		}
	};

}

#endif
