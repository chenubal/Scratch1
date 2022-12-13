#pragma once

#include <ctime>
#include <chrono>

namespace jh
{
	using clk = std::chrono::high_resolution_clock;

	auto now() { return clk::now(); }


	/// @brief Simple stop watch for time measurements (default in µs)
	template<class  T = std::chrono::milliseconds>
	struct stopwatch
	{
		stopwatch() : start(now()) { }
		typename T::rep reset() { auto c = count(); start = now(); return c; }
		T duration() {return std::chrono::duration_cast<T>(now() - start); }
		typename T::rep count() { return duration().count(); }
	private:
		clk::time_point start;
	};
}

