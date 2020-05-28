#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <sstream>
#include <thread>
#include <xtgmath.h>
#include <random>

namespace chrono_tools
{
	auto now() { return std::chrono::system_clock::now(); }

	/// @brief Returns a formatted time string of now (local time)
	/// @param fmt See 'std::put_time' formatting options.
	using clk = std::chrono::system_clock;

	inline std::tm loctime(std::time_t * t)
	{
		std::tm ltm;
		localtime_s(&ltm, t);
		return ltm;
	}

	std::string time_g(char const* fmt)
	{
		std::stringstream ss;
		std::time_t t_c = clk::to_time_t(clk::now());
		ss << std::put_time(&loctime(&t_c), fmt);
		return ss.str();
	}
	/// @brief Returns a date string of now
	inline std::string today() { return time_g("%F"); }

	/// @brief Returns a date string of now
	inline std::string time() { return time_g("%T"); }

	/// @brief Returns a standard date string of now
	inline std::string localtime() { return time_g("%c"); }

	template<class  T = std::chrono::milliseconds>
	void sleep(unsigned n) { std::this_thread::sleep_for(T(n)); };
	/// @brief Simple stop watch for time measurements (default in µs)
	template<class  T = std::chrono::milliseconds>
	struct stopwatch
	{
		stopwatch() : start(now()) { }
		typename T::rep reset() { auto c = count(); start = now(); return c; }
		T duration() { auto t = now(); return std::chrono::duration_cast<T>(t - start); }
		typename T::rep count() { return duration().count(); }
	private:
		std::chrono::system_clock::time_point start;
	};
}

void ch_demo()
{
	std::cout << "Today " << chrono_tools::today() << '\n';
	std::cout << "Now " << chrono_tools::time() << '\n';
	std::cout << "Local " << chrono_tools::localtime() << '\n';
	std::cout << "StdTime " << chrono_tools::time_g("%c") << '\n';

}
int double2int(double d)
{
	d += 6755399441055744.0;
	return reinterpret_cast<int&>(d);
}
void sw_demo()
{
	chrono_tools::stopwatch<std::chrono::microseconds> sw;
	double d = -1233445.9;
	srand(static_cast <unsigned> (time(0)));
	for (int i=0; i<10000000; i++)
	{
		std::round(100.0*static_cast <double> (rand()) / static_cast <double> (RAND_MAX));
	}
	std::cout << sw.reset() << "us\n";
}
void main()
{
	//ch_demo();
	sw_demo();
}
