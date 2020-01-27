#include <iostream>
#include <string>
#include <variant>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <optional>

// https://arne-mertz.de/2018/05/overload-build-a-variant-visitor-on-the-fly/
// https://en.cppreference.com/w/cpp/utility/variant
// https://en.cppreference.com/w/cpp/utility/variant/visit
// https://www.boost.org/doc/libs/1_64_0/doc/html/variant.html

using namespace std;

using ints = vector<int>;

template<typename T>
ostream& operator<<(ostream& os, const vector<T>& vec)
{
	for (auto& el : vec) os << el << ' ';
	return os;
}


using attribute_variant = variant<int, double, string, ints>; //new
using attribute_t = pair<string, attribute_variant>;
using attributes_t = unordered_map<string,attribute_variant>;


namespace helper
{
	optional<attribute_variant> find(attributes_t const& attrs, string const& s) //new
	{
		auto hasKey = [&](auto const&a) {return a.first == s; };
		if (auto it = find_if(attrs.begin(), attrs.end(), hasKey); it != attrs.end())
			return it->second;
		return {};
	}

	struct printer
	{
		printer(ostream& os = std::cout) :os(os) {}
		void operator()(int i) const { os << "P-int: " << i << "\n"; }
		void operator()(double d) const { os << "P-double: " << d << "\n"; }
		void operator()(string const& s) const { os << "P-string: " << s << "\n"; }
		void operator()(ints const& vi) const { os << "P-ints: " << vi << "\n"; }
	private:
		ostream& os;
	};

	void print(attribute_variant const& a, ostream& os = std::cout)
	{
		visit(printer(os), a);
	}

	ostream& operator<<(ostream& os, const attribute_t& a)
	{
		os << a.first << ": ";
		print(a.second, os);
		return os;
	}

	template<class... Ts> //new
	struct overloaded : Ts...
	{
		using Ts::operator()...;
	};

	template<class... Ts> //new
	overloaded(Ts...)->overloaded<Ts...>;
}

int main() 
{
	using namespace helper;

	attributes_t attributes = { {"one",10}, {"two",1.5}, {"three","hello"}, {"four",ints{7,8,9}} };
	auto key = "four"s;

	if (true)
	{
		cout << "***** simple access ************************************************\n";
		auto hasKey = [&](auto const&a) {return a.first == key; }; //new
		if (auto attrIt = find_if(attributes.begin(), attributes.end(), hasKey); attrIt != attributes.end()) //new
		{
			if (auto pav = get_if<int>(&(attrIt->second))) //new
				cout << "1. variant<int>: value = " << *pav << '\n';
			else
				cout << "1. no variant<int>" << '\n';
		}
	}
	if (true)
	{
		cout << "***** simple access 2 **********************************************\n";
		if (auto attr = find(attributes, key))
		{
			cout << "2. variant<int>: ";
			if (auto pAttr = get_if<ints>(&(*attr)))
				cout << "value=" << *pAttr;
			else
				cout << "empty";
			cout << "\n";
		}
	}
	if (true)
	{
		cout << "***** with auto lambda ************************************************\n";
		auto lprint = [](attribute_t const& attr)
		{
			visit([name = attr.first](auto&& value) {cout << name << "=" << value << "\n"; }, attr.second); //new
		};
		for (auto const& attr : attributes)
			lprint(attr);
	}
	if (true)
	{
		cout << "***** with printer class ************************************************\n";
		for (auto const& attr : attributes)
			print(attr.second);
	}
	if (true)
	{
		for (auto const& attr : attributes)
			cout << attr;
	}
	if (true)
	{
		cout << "***** with lambda overload ************************************************\n";
		for (auto const& attr : attributes)
		{
			visit(overloaded
				{
					[](auto   x) { cout << "auto:   " << x << "\n"; },
					[](double x) { cout << "double: " << x << "\n"; },
					[](string x) { cout << "string: " << x << "\n"; },
					[](ints const&  x) { cout << "ints:   " << x << "\n"; },
				}
			, attr.second);
		}
	}
}