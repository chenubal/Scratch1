#include <iostream>
#include <string>
#include <variant>
#include <vector>

// https://arne-mertz.de/2018/05/overload-build-a-variant-visitor-on-the-fly/

using namespace std;

using ints = vector<int>;

template<typename T>
ostream& operator<<(ostream& os, const vector<T>& vec)
{
	for (auto& el : vec) os << el << ' ';
	return os;
}

using attribute = std::variant<int, float, string, ints>;
using attributes = std::vector<attribute>;

//struct printer
//{
//	void operator()(int i) const { cout << "P-int: " << i << "\n"; }
//	void operator()(double d) const { cout << "P-double: " << d << "\n"; }
//	void operator()(string const& s) const { cout << "P-string: " << s << "\n"; }
//	void operator()(ints const& vi) const { cout << "V-string: " << vi << "\n"; }
//};

//template<class... Ts>
//struct overloaded : Ts...
//{
//	using Ts::operator()...;
//};
//template<class... Ts>
//overloaded(Ts...)->overloaded<Ts...>;


int main() 
{

	attributes vec = { 10, 1.5f, "hello", ints{7,8,9} };
	if (auto pval = get_if<int>(&vec[1]))
		cout << "variant ist ein <int>: " << *pval << '\n';
	else
		cout << "variant ist kein <int>" << '\n';

	cout << "*****************************************************\n";
	for (auto const& v : vec)
		visit([](auto&& x) {cout << x << "\n"; }, v);

	//cout << "*****************************************************\n";
	//for (auto&& v : vec)
	//	visit(printer(), v);

	//cout << "*****************************************************\n";
	//for (auto& v : vec)
	//{
	//	visit(overloaded
	//		{
	//			[](auto    arg) { cout << "auto:   " << arg << "\n"; },
	//			[](double  arg) { cout << "double: " << arg << "\n"; },
	//			[](string& arg) { cout << "string: " << arg << "\n"; },
	//			[](ints& arg) { cout << "ints: " << arg << "\n"; },
	//		}
	//	, v);
	//}
}