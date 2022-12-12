
#include <iostream>
#include <vector>
#include <random>
#include <numeric>
#include "../Tools/chrono_tools.h"
#include "../Tools/zipper.h"
#include <execution>
#include <iterator>


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
	auto add = [](T a, T b) { return a + b; };
	auto add_pair = [](auto&& p) { return std::get<0>(p) + std::get<1>(p); };

	{
		jh::stopwatch<std::chrono::microseconds> sw;
		std::transform(v1.begin(), v1.end(), v2.begin(), w0.begin(), add);
		std::cout << "" << sw.reset() << "us Init w seq\n\n";
	}

	{
		std::vector<T> w(N);

		jh::stopwatch<std::chrono::microseconds> sw;
		std::transform(v1.begin(), v1.end(), v2.begin(), w.begin(), add);
		std::cout << "" << sw.reset() << "us ";
		std::cout << "ok: " << (w==w0) << " trafo seq\n";
	}

	{
		std::vector<T> w(N);

		jh::stopwatch<std::chrono::microseconds> sw;
		std::transform(std::execution::par, v1.begin(), v1.end(), v2.begin(), w.begin(), add);
		std::cout << "" << sw.reset() << "us ";
		std::cout << "ok: " << (w == w0) << " trafo par\n";
	}

	{
		std::vector<T> w(N);
		auto z = jh::zip(w, v1, v2);

		jh::stopwatch<std::chrono::microseconds> sw;
		std::for_each(std::execution::par_unseq, z.begin(), z.end(), [](auto&& t) {auto [a, b, c] = t; a = b + c; });
		std::cout << "" << sw.reset() << "us ";
		std::cout << "ok: " << (w == w0) << " zip for_each par\n";
	}

	{
		std::vector<T> w(N);
		auto z = jh::zip(w, v1, v2);
	
		jh::stopwatch<std::chrono::microseconds> sw;
		std::for_each(z.begin(), z.end(), [](auto&& t) {auto [a, b, c] = t; a = b + c; });
		std::cout << "" << sw.reset() << "us ";
		std::cout << "ok: " << (w == w0) << " zip for_each seq\n";
	}

	{
		std::vector<T> w(N);
		auto z2 = jh::zip(v1, v2);

		jh::stopwatch<std::chrono::microseconds> sw;
		std::transform(z2.begin(), z2.end(), w.begin(), add_pair);
		std::cout << "" << sw.reset() << "us ";
		std::cout << "ok: " << (w == w0) << " zip trafo seq\n";
	}

	{
		std::vector<T> w(N);
		auto z2 = jh::zip(v1, v2);

		jh::stopwatch<std::chrono::microseconds> sw;
		std::transform(std::execution::par, z2.begin(), z2.end(), w.begin(),add_pair);
		std::cout << "" << sw.reset() << "us ";
		std::cout << "ok: " << (w == w0) << " zip trafo par\n";
	}

	{
		std::vector<T> w(N);
		jh::stopwatch<std::chrono::microseconds> sw;
		for (auto && [a,b,c] : jh::zip(w, v1, v2) ) 	a = b + c; 
		std::cout << "" << sw.reset() << "us ";
		std::cout << "ok: " << (w == w0) << " zip range-for\n";
	}

}

