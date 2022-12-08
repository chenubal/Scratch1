#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <variant>
#include <algorithm>
#include <fstream>
#include <iomanip>

namespace jh
{
	// Attributes definition
	using attribute_tag = std::string;
	using attribute_ints = std::vector<int>;
	using attribute_doubles = std::vector<double>;
	using attribute_value = std::variant<bool, int, double, std::string, attribute_ints, attribute_doubles>;
	using attribute = std::pair < attribute_tag, attribute_value>;
	using attributeMap = std::map<attribute_tag, attribute_value>;

	// Attributes utilities
	template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
	template<class... Ts> overload(Ts...)->overload<Ts...>;

	template<typename T>
	std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
	{
		if (!vec.empty())
		{
			os << vec.front();
			std::for_each(std::next(vec.begin()), vec.end(), [&](auto const& x) {os << " " << x; });
		}
		return os;
	}

}

// Base functions
std::ostream& operator<<(std::ostream& os, jh::attribute_value const& attr)
{
	std::visit([&](auto const& a) { os << a; }, attr);
	return os;
}

std::string toString(jh::attribute_value const& attr)
{
	std::stringstream  ss; ss << attr;
	return ss.str();
}

namespace test
{
	const std::size_t maxPrecision = std::numeric_limits<double>::digits;

	std::string makeTagString(jh::attribute_tag const& tag, jh::attribute_value const& attr)
	{
		std::stringstream ss;
		ss << std::setprecision(/*maxPrecision*/10);
		auto intro = [&] {return std::string("<attribute name='") + tag + "' value='"; };
		auto outType = [&](auto&& tstr) {return std::string("' type='") + tstr + "'/>";  };
		auto p = jh::overload
		{
				[&](bool  x) {  ss << intro() << (x ? "true" : "false") << outType("bool"); },
				[&](int  x) {  ss << intro() << x << outType("int"); },
				[&](double  x) {  ss << intro() << x << outType("double"); },
				[&](std::string const& x) {    ss << "<attribute name='" << tag << "' type='string'>" << x << "</attribute>"; },
				[&](jh::attribute_ints const&  x) {  ss << "<attribute name='" << tag << "' type='IntVec'>" << x << "</attribute>"; },
				[&](jh::attribute_doubles const&  x) {   ss << "<attribute name='" << tag << "' type='DblVec'>" << x << "</attribute>"; },
				[&](auto&&  x) {  ss << intro() << x << "' type=unknown/>"; }
		};
		std::visit(p, attr);
		return ss.str();
	}

	void serialze(jh::attributeMap const& attrs, std::ostream& os = std::cout)
	{
		os << "<Attributes>\n";
		for (auto&&[tag, value] : attrs)
			os << "\t" << makeTagString(tag, value) << "\n";
		os << "</Attributes>\n";
	}
}
 
using namespace jh;
int main()
{
	attributeMap attrs;
	attrs["MyBool"] = true;
	attrs["MyDouble"] = 44.0;
	attrs["MyInt"] = -1;
	attrs["MyString"] = std::string("abc");
	attrs["MyIntVec"] = std::vector<int>{ 1,2,4 };
	attrs["MyDoubleVec"] = std::vector<double>{ 1.0/3.0,2.222222222,5.0/7.0,3.5 };

	test::serialze(attrs);
	std::ofstream of("jh-attributes.xml", std::ios::out);
	test::serialze(attrs,of);

	if (auto pval = std::get_if<int>(&attrs["MyInt"]))
		std::cout << "variant value: " << *pval << '\n';
	else
		std::cout << "failed to get value!" << '\n';

	std::cout << "ostream: " << attrs["MyIntVec"] << "\n";
	std::cout << "toString: " << toString(attrs["MyIntVec"]) << "\n";

	std::cout << "compare: " << (attribute_value(3) == attribute_value(3.0) ? "true" : "false") << "\n";
	std::cout << "compare: " << (attribute_value(4) == attribute_value(4) ? "true" : "false") << "\n";
	std::cout << "compare: " << (attribute_value(4) == attribute_value(5) ? "true" : "false") << "\n";
	std::cout << "compare: " << (attribute_value(4) < attribute_value(5.0) ? "true" : "false") << "\n";
}
