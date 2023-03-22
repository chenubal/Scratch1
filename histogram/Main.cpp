#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include  <math.h>
#include <numeric>

namespace jh
{
	
	template<class T> // value_type with '<'
	using frequency_map = std::map<T, unsigned>;

	template<class C> // C<value_type> is iterable and value_type with '<'
	auto frequency(C const& container)
	{
		using M = frequency_map<typename C::value_type>;
		return std::accumulate(std::begin(container),std::end(container), M{}, [](auto &s, auto&& x) {s[x]++; return s; });
	}

	template<class T>
	using sample_t = std::tuple<T, unsigned>;

	template<class T>
	auto makeEmptyHistogram(T lower, T upper, T step) // upper > lower, step > 0
	{
		using S = sample_t<T>;

		auto numSamples = unsigned(floor((upper - lower) / step) + 1);
		std::vector<S> histogram(numSamples);

		auto createSample = [&,x = lower]() mutable { return S{ std::exchange(x,x + step),0 }; };
		for (auto& sample : histogram)  sample = createSample();
		return histogram;
	}


	template<class C, class T = typename C::value_type>
	auto makeHistogram(C const& container, T lower, T upper, T step)
	{
		auto histogram = makeEmptyHistogram<T>(lower, upper, step);

		for (auto&& sample : frequency(container))
		{
			if (T y = sample.first; (y >= lower && y <= upper))
			{
				auto l = unsigned(floor((y - lower) / step));
				std::get<1>(histogram[l]) += sample.second;
			}
		}
		return histogram;
	}

	template<class C, class T = typename C::value_type>
	auto makeHistogram2(C const& container, T lower, T upper, T step)
	{
		auto histogram = makeEmptyHistogram<T>( lower, upper, step);
		auto slot = [&](auto x) { return unsigned(floor((x - lower) / step)); };
		auto drop = [&](auto x) { if (x >= lower && x <= upper) std::get<1>(histogram[slot(x)]) += 1; };
		for (auto&& value : container) 
			drop(value);
		return histogram;
	}
}

int main()
{
    std::vector<int> vec = {-200,-200, 200,400,200,100,1000,300, 1000,400,310,400,400,-200};
	 for (auto x : jh::makeHistogram2(vec, -200, 1000, 200))
		 std::cout << std::get<0>(x) << "->" << std::get<1>(x) << "\n";
	 std::cout << "-----------\n";
	 for (auto x : jh::makeHistogram(vec, -200, 1000, 200))
		 std::cout << std::get<0>(x) << "->" << std::get<1>(x) << "\n";

}
