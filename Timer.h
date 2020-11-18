#pragma once

#ifndef JTimer
#define JTimer

namespace J {
	class Timer {

	private:
		std::chrono::time_point<std::chrono::steady_clock> start, finish;
		std::chrono::duration<double> duration;
		std::ostream& os;
		bool quiet;
		bool paused = 0;

	public:
		Timer(bool silenced = 1, std::ostream& dings = std::cout) : os{ dings } {
			duration = duration.zero();
			quiet = silenced;
			start = std::chrono::high_resolution_clock::now();
			if(!quiet)os << "Timer started\n";
		}
		~Timer() {
			finish = std::chrono::high_resolution_clock::now();
			duration = duration + (finish - start);
			double ms = duration.count() * 1000.0;
			if (!quiet)os << ms << "ms \n";
		}
		void end() {
			this->~Timer();
		}
		double GetTime() {
				finish = std::chrono::high_resolution_clock::now();
				duration = duration + (finish - start);
				double ms = duration.count() * 1000.0;
				start = std::chrono::high_resolution_clock::now();
				return ms;
		}
		void restart() {
			finish = std::chrono::high_resolution_clock::now();
			duration = duration + (finish - start);
			double ms = duration.count() * 1000.0;
			if (!quiet)os << ms << "ms \n";
			if (!quiet)os << "Restarted\n";
			duration = duration.zero();
			start = std::chrono::high_resolution_clock::now();
		}
		void pause() {
			if (!paused) {
				finish = std::chrono::high_resolution_clock::now();
				duration = duration + (finish - start);
				paused = 1;
				if (!quiet)os << "Paused\n";
			}
		}
		void resume() {
			if (paused) {
				start = std::chrono::high_resolution_clock::now();
				paused = 0;
				if (!quiet)os << "Resumed\n";
			}
		}
	};

}
#endif
