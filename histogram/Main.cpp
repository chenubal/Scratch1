#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include  <math.h>

namespace jh
{
	template<class T>
	using frequency_map = std::map<T, unsigned>;

	template<class T>
	using sample = std::tuple<T, unsigned>;

	template<class C>
	auto frequency(C const& v)
	{
		frequency_map<typename C::value_type> m;
		for (auto&& x : v) m[x]++;
		return m;
	}

	template<class T>
	auto histogram(std::vector<T> const& v, T s, T e, T sz)
	{
		using S = sample<T>;
		auto numSamples = unsigned(floor((e - s) / sz) + 1);
		std::vector<S> histo(numSamples);
		auto gen = [&]() {static auto ss = s; return S{ std::exchange(ss,ss + sz),0 }; };
		std::generate(histo.begin(), histo.end(), gen);
		for (auto&& x : frequency(v))
		{
			if (T y = x.first; (y >= s && y <= e))
			{
				auto l = unsigned(floor((y - s) / sz));
				std::get<1>(histo[l]) += x.second;
			}
		}
		return histo;
	}

	template<class T>
	auto histogram2(std::vector<T> const& v, T s, T e, T sz)
	{
		using S = sample<T>;

		auto numSamples = unsigned(floor((e - s) / sz) + 1);
		std::vector<S> histo(numSamples);

		auto gen = [&]() {static auto ss = s; return S{ std::exchange(ss,ss + sz),0 }; };
		std::generate(histo.begin(), histo.end(), gen);

		for (auto&& x : v)
		{
			if (x >= s && x <= e)
			{
				auto l = unsigned(floor((x - s) / sz));
				std::get<1>(histo[l]) += 1;
			}
		}
		return histo;
	}
}

int main()
{
    std::vector<int> vec = {-200,-200, 200,400,200,100,1000,300, 1000,400,310,400,400,-200};
    for(auto x : jh::histogram(vec,-200,1000,200))
       std::cout << std::get<0>(x) << "->" <<std::get<1>(x) << "\n";
	 for (auto x : jh::histogram2(vec, -200, 1000, 200))
		 std::cout << std::get<0>(x) << "->" << std::get<1>(x) << "\n";

}
