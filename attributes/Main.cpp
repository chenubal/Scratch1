#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <variant>

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
	for (auto& el : vec) os << el << ' ';
	return os;
}

using attribute_t = std::variant<int, float, double, std::string, std::vector<int>>;
using attributes_t = std::map<std::string, attribute_t>;

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...)->overload<Ts...>;

auto printer = overload
{
		[](int i) { std::cout << "i=" << i << "\n"; },
		[](float f) { std::cout << "f=" << f << "\n";  },
		[](double d) { std::cout << "d=" << d << "\n";  },
		[](std::string& s) { std::cout << "s=" << s << "\n"; },
		[](std::vector<int> const& v) { std::cout << "v=" << v << "\n"; }
};

int main()
{
	// Make an attribute container
	attributes_t attrs;
	attrs["doubleTag"] = 44.0;
	attrs["stringTag"] = "abc";
	attrs["vectIntTag"] = std::vector<int>{ 1,2,4 };

	// Loop printing attributes in two ways
	for (auto a : attrs) 
		std::visit( printer, a.second);
	for (auto a : attrs)
		std::visit([](auto i) { std::cout << "value=" << i << "\n"; }, a.second);


	// Get vector attribute
	auto vec = std::get<std::vector<int>>(attrs["vectIntTag"]);
	std::cout << vec << std::endl;

	// Change attribute
	attrs["vectIntTag"] = "xvy";
	std::cout << std::get<std::string>(attrs["vectIntTag"]) << std::endl;

	// Test/get attribute
	if (auto pval = std::get_if<int>(&attrs["doubleTag"]))
		std::cout << "variant value: " << *pval << '\n';
	else
		std::cout << "no int\n";
	// Test/get attribute
	if (auto pval = std::get_if<double>(&attrs["doubleTag"]))
		std::cout << "variant value: " << *pval << '\n';
	else
		std::cout << "no double\n";
}
