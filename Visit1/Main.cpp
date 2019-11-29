#include <iostream>
#include <string>
#include <variant>
#include <vector>
#include <unordered_map>

// https://arne-mertz.de/2018/05/overload-build-a-variant-visitor-on-the-fly/

using namespace std;

template<typename T>
ostream& operator<<(ostream& os, const vector<T>& vec)
{
	for (auto& el : vec) os << el << ' ';
	return os;
}

using ints = vector<int>;
using attribute_variant = variant<int, double, string, ints>;
using attribute_t = pair<string, attribute_variant>;
using attributes_t = unordered_map<string,attribute_variant>;

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
	static printer p(os);
	visit(p, a);
}

template<class... Ts>
struct overloaded : Ts...
{
	using Ts::operator()...;
};
template<class... Ts>
overloaded(Ts...)->overloaded<Ts...>;


int main() 
{

	attributes_t attributes = { {"one",10}, {"two",1.5}, {"three","hello"}, {"four",ints{7,8,9}} };

	cout << "***** simple access ************************************************\n";
	if (auto pa = get_if<int>(&attributes["one"]))
		cout << "variant<int>: value=" << *pa << '\n';
	else
		cout << "no variant<int>" << '\n';

	cout << "***** with auto lambda ************************************************\n";
	auto lprint = [](attribute_t const& attr)
	{
		visit([name = attr.first](auto&& value) {cout << name << "=" << value << "\n"; }, attr.second);
	};
	for (auto const& attr : attributes)
		lprint(attr);

	cout << "***** with printer class ************************************************\n";
	for (auto const& attr : attributes)
		print(attr.second);

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