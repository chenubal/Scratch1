
#include <iostream>
#include <vector>
#include <random>
#include <numeric>
#include "../Tools/chrono_tools.h"
#include "../Tools/zipper.h"
#include <execution>


int main()
{

   constexpr size_t N = 10'000'000;
	using T = int;
   std::vector<T> v1(N);
	std::vector<T> v2(N);
	std::vector<T> w0(N);
	auto gen = [a=100.0] {return T(/*std::round(a/RAND_MAX * rand())*/ 3.33); };
   std::generate(v1.begin(), v1.end(), gen);
	std::generate(v2.begin(), v2.end(), gen);

	{
		jh::stopwatch<std::chrono::microseconds> sw;
		std::transform(v1.begin(), v1.end(), v2.begin(), w0.begin(), [](T a, T b) { return a + b; });
		std::cout << "" << sw.reset() << "us\n\n";
	}

	{
		std::vector<T> w(N);
		jh::stopwatch<std::chrono::microseconds> sw;
		std::transform(std::execution::par, v1.begin(), v1.end(), v2.begin(), w.begin(), [](T a, T b) { return a + b; });
		std::cout << "" << sw.reset() << "us ";
		std::cout << "ok: " << (w==w0) << "\n";
	}

	{
		std::vector<T> w(N);
		jh::stopwatch<std::chrono::microseconds> sw;
		auto z = jh::zip(w, v1, v2);
		std::for_each(z.begin(), z.end(), [](auto&& t) {auto [a, b, c] = t; a = b + c; });
		std::cout << "" << sw.reset() << "us ";
		std::cout << "ok: " << (w == w0) << "\n";
	}


}

