
#include <iostream>
#include <vector>
#include <random>
#include <numeric>
#include "../Tools/chrono_tools.h"
#include "../Tools/zipper.h"
#include <execution>
#include <iterator>
#include <map>

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
	auto f = [](double x) {return x;/*std::sin(x);*/ };
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

	runTest("zip for_each par", [&]
	{
		std::vector<T> w(N);
		auto threeVecs = jh::zip(w, v1, v2);

		jh::stopwatch<std::chrono::microseconds> sw;
		std::for_each(std::execution::par_unseq, threeVecs.begin(), threeVecs.end(), sum_a);
		std::cout << "" << sw.count() << "us ";
		std::cout << "ok: " << (w == w_ref);
	});

	runTest("zip for_each seq", [&]
	{
		std::vector<T> w(N);
		auto threeVecs = jh::zip(w, v1, v2);

		jh::stopwatch<std::chrono::microseconds> sw;
		std::for_each(threeVecs.begin(), threeVecs.end(), sum_a);
		std::cout << "" << sw.count() << "us ";
		std::cout << "ok: " << (w == w_ref);
	});

	runTest("zip trafo seg", [&]
	{
		std::vector<T> w(N);
		auto twoVecs = jh::zip(v1, v2);

		jh::stopwatch<std::chrono::microseconds> sw;
		std::transform(twoVecs.begin(), twoVecs.end(), w.begin(), tuple_sum);
		std::cout << "" << sw.count() << "us ";
		std::cout << "ok: " << (w == w_ref);
	});

	runTest("zip trafo par", [&]
	{
		std::vector<T> w(N);
		auto twoVecs = jh::zip(v1, v2);

		jh::stopwatch<std::chrono::microseconds> sw;
		std::transform(std::execution::par, twoVecs.begin(), twoVecs.end(), w.begin(), tuple_sum);
		std::cout << "" << sw.count() << "us ";
		std::cout << "ok: " << (w == w_ref);
	});

	runTest("zip range-for", [&]
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

	runTest("Drop", [&]
	{
		std::vector<T> w(20, 1);
		auto sp = jh::span(w.begin() + 3, 5u);
		for (auto&& x : sp) x = 7;
		for (auto&& x : jh::drop(w,3)) std::cout << x << " ";
	});

	runTest("Slice", [&]
	{
		std::vector<T> w(20, 1);
		std::iota(w.begin(), w.end(),1);
		auto sl = jh::make_slice(w, 3);
		for (auto&& x : sl) x =0;
		for (auto&& x : w) std::cout << x << " ";
	});

	runTest("Skip", [&]
	{
		std::vector<T> w(20, 1);
		std::iota(w.begin(), w.end(), 1);
		for (auto&& x : jh::make_ignore(w, 3)) std::cout << x << " ";
	   std::cout << "\n";
		for (auto&& x : jh::make_ignore(w, 6)) x = -1;
		std::cout << w << " ";
	});

	runTest("Zip with containers", [&]
	{
		const std::map<std::string, int> M = { {"x",7},{"y",8},{"u",5}, {"v",55} };
		std::vector<double> V = { 1.1,2.2,3.3,4.4 };
		unsigned W[] = { 4,5,6,7 };

		for (auto&& [v, w, p] : jh::zip(V, W,M)) v *= w + p.second;
			
		const auto Z = jh::zip(V, W, M);
		for (auto const& [v, w ,p] : Z)
			std::cout << "v="<<v << "\tw=" << w << "\tp=" << p.first << "|" << p.second << "\n";
	});




}

