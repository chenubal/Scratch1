#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include  <math.h>
#include <numeric>

namespace jh
{
	
	template<class T> // T with '<'
	using frequency_map = std::map<T, unsigned>;

	template<class C> // C<T> is iterable and T with '<'
	auto frequency(C const& container)
	{
		using M = frequency_map<typename C::value_type>;
		return std::accumulate(std::begin(container),std::end(container), M{}, [](auto &s, auto&& x) {s[x]++; return s; });
	}

	template<class T>
	using sample_t = std::tuple<T, unsigned>;

	template<class C, class T = typename C::value_type>
	auto makeHistogram(C const& container, T lower, T upper, T slot)
	{
		using S = sample_t<T>;

		auto numSamples = unsigned(floor((upper - lower) / slot) + 1);
		std::vector<S> histogram(numSamples);

		auto createSample = [&]() {static auto next = lower; return S{ std::exchange(next,next + slot),0 }; };
		for (auto& sample : histogram)  sample = createSample();

		for (auto&& sample : frequency(container))
		{
			if (T y = sample.first; (y >= lower && y <= upper))
			{
				auto l = unsigned(floor((y - lower) / slot));
				std::get<1>(histogram[l]) += sample.second;
			}
		}
		return histogram;
	}

	template<class C, class T = typename C::value_type>
	auto makeHistogram2(C const& container, T lower, T upper, T slot)
	{
		using S = sample_t<T>;
		auto idx = [&](auto x) { return unsigned(floor((x - lower) / slot)); };

		std::vector<S> histogram(idx(upper) + 1);

		auto createSample = [&]() {static auto ss = lower; return S{ std::exchange(ss,ss + slot),0 }; };
		for (auto& sample : histogram)  
			sample = createSample();

		auto count = [&](auto x) { if (x >= lower && x <= upper) std::get<1>(histogram[idx(x)]) += 1; };
		for (auto&& value : container) 
			count(value);
		return histogram;
	}
}

int main()
{
    std::vector<int> vec = {-200,-200, 200,400,200,100,1000,300, 1000,400,310,400,400,-200};
    for(auto x : jh::makeHistogram(vec,-200,1000,200))
       std::cout << std::get<0>(x) << "->" <<std::get<1>(x) << "\n";
	 for (auto x : jh::makeHistogram2(vec, -200, 1000, 200))
		 std::cout << std::get<0>(x) << "->" << std::get<1>(x) << "\n";

}
