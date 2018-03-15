#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <string>

int main()
{
	using mymap_t = std::map<std::string, std::string>;
	mymap_t m{ { "k1","v1" },{ "k2","v2" } };

	auto p = std::find_if(m.cbegin(), m.cend(), [](mymap_t::value_type const &x)
	{
		return x.first == "k2";
	});

	if (p != m.end())std::cout << m[p->first] << "\n";
}
