
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
	using T = double;
   std::vector<T> v1(N);
	std::vector<T> v2(N);
	std::vector<T> w_ref(N);
	auto gen = [a=100.0] {return T(/*std::round(a/RAND_MAX * rand())*/ 3.33); };
   std::generate(v1.begin(), v1.end(), gen);
	std::generate(v2.begin(), v2.end(), gen);
	auto f = [](double x) {return std::sin(x); };
	auto add = [&](T a, T b) { return f(a + b); };
	auto tuple_sum = [&](auto&& tpl) { return  f(std::get<0>(tpl) + std::get<1>(tpl)); };
	auto sum_a = [&](auto&& t) {auto&& [x, a, b] = t; x = f(a + b); };

	auto runTest = [](auto const& title, auto f)
	{
		std::cout << "Running test: " << title << "\n";
		f();
		std::cout << "\n\n";
	};

	runTest("Init ref seq", [&] 
	{
		jh::stopwatch<std::chrono::microseconds> sw;
		std::transform(v1.begin(), v1.end(), v2.begin(), w_ref.begin(), add);
		std::cout << "" << sw.reset() << "us";
	});

	runTest("Trafo seq", [&] 
	{
		std::vector<T> w(N);

		jh::stopwatch<std::chrono::microseconds> sw;
		std::transform(v1.begin(), v1.end(), v2.begin(), w.begin(), add);
		std::cout << "" << sw.count() << "us ";
		std::cout << "ok: " << (w == w_ref);
	});

	runTest("Trafo par", [&]
	{
		std::vector<T> w(N);

		jh::stopwatch<std::chrono::microseconds> sw;
		std::transform(std::execution::par, v1.begin(), v1.end(), v2.begin(), w.begin(), add);
		std::cout << "" << sw.count() << "us ";
		std::cout << "ok: " << (w == w_ref);
	});

	runTest("Zip for_each par", [&]
	{
		std::vector<T> w(N);
		auto threeVecs = jh::zip(w, v1, v2);

		jh::stopwatch<std::chrono::microseconds> sw;
		std::for_each(std::execution::par_unseq, threeVecs.begin(), threeVecs.end(), sum_a);
		std::cout << "" << sw.count() << "us ";
		std::cout << "ok: " << (w == w_ref);
	});

	runTest("Zip for_each seq", [&]
	{
		std::vector<T> w(N);
		auto threeVecs = jh::zip(w, v1, v2);

		jh::stopwatch<std::chrono::microseconds> sw;
		std::for_each(threeVecs.begin(), threeVecs.end(), sum_a);
		std::cout << "" << sw.count() << "us ";
		std::cout << "ok: " << (w == w_ref);
	});

	runTest("Zip trafo seg", [&]
	{
		std::vector<T> w(N);
		auto twoVecs = jh::zip(v1, v2);

		jh::stopwatch<std::chrono::microseconds> sw;
		std::transform(twoVecs.begin(), twoVecs.end(), w.begin(), tuple_sum);
		std::cout << "" << sw.count() << "us ";
		std::cout << "ok: " << (w == w_ref);
	});

	runTest("Zip trafo par", [&]
	{
		std::vector<T> w(N);
		auto twoVecs = jh::zip(v1, v2);

		jh::stopwatch<std::chrono::microseconds> sw;
		std::transform(std::execution::par, twoVecs.begin(), twoVecs.end(), w.begin(), tuple_sum);
		std::cout << "" << sw.count() << "us ";
		std::cout << "ok: " << (w == w_ref);
	});

	runTest("Zip range-for", [&]
	{
		std::vector<T> w(N);
		jh::stopwatch<std::chrono::microseconds> sw;
		for (auto&& [x, a, b] : jh::zip(w, v1, v2)) 	x = add(a, b);
		std::cout << "" << sw.count() << "us ";
		std::cout << "ok: " << (w == w_ref);
	});

	runTest("Span", [&]
	{
		std::vector<T> w(20,1);
		auto sp = jh::span(w.begin()+3, 5u);
		for (auto&& x : sp) x = 7;
		for (auto&& x : w) std::cout << x << " ";
	});

}

