#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <variant>
#include <algorithm>
#include <fstream>
#include <iomanip>

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
	if (!vec.empty())
	{
		os << vec.front();
		std::for_each(std::next(vec.begin()), vec.end(), [&](auto const& x) {os << ";" << x; });
	}
	return os;
}

using tag_t = std::string;
using attribute_t = std::variant<bool, int, double, std::string, std::vector<int>,std::vector<double>>;
using attributes_t = std::map<tag_t, attribute_t>;

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...)->overload<Ts...>;

const std::size_t maxPrecision = std::numeric_limits<double>::digits;

std::string makeTagString(tag_t const& tag, attribute_t const& attr)
{
	std::stringstream ss; 
	ss << std::setprecision(maxPrecision);
	auto intro = [&] {return std::string("<") + tag + " value='"; };
	auto outType = [&] (auto&& tstr){return std::string("' type='")+tstr+"'/>";  };
	auto p = overload
	{
			[&](bool  x) {  ss << intro() << (x ? "true" : "false") << outType("bool"); },
			[&](int  x) {  ss << intro() << x << outType("int"); },
			[&](double  x) {  ss << intro() << x << outType("double"); },
			[&](std::string const& x) {    ss << "<" << tag << " type='string'>" << x << "</" << tag << ">"; },
			[&](std::vector<int> const&  x) {  ss << "<" << tag << " type='IntVec'>" << x << "</" << tag <<">"; },
			[&](std::vector<double> const&  x) {   ss << "<" << tag << " type='DblVec'>" << x << "</" << tag << ">"; },
			[&](auto&&  x) {  ss << intro() << x << "' type=unknown/>"; }
	};
	std::visit(p, attr);
	return ss.str();
}
void serialze(attributes_t const& attrs, std::ostream& os = std::cout)
{
	os << "<Attributes>\n";
	for (auto&&[tag, value] : attrs)
		os << "\t" << makeTagString(tag, value) << "\n";
	os << "</Attributes>\n";
}
 
int main()
{
	attributes_t attrs;
	attrs["MyBool"] = true;
	attrs["MyDouble"] = 44.0;
	attrs["MyInt"] = -1;
	attrs["MyString"] = std::string("abc");
	attrs["MyIntVec"] = std::vector<int>{ 1,2,4 };
	attrs["MyDoubleVec"] = std::vector<double>{ 1.0/3.0,2.222222222,5.0/7.0,3.5 };

	serialze(attrs);
	std::ofstream of("jh-attributes.xml", std::ios::out);
	serialze(attrs,of);
	if (auto pval = std::get_if<int>(&attrs["intTag"]))
		std::cout << "variant value: " << *pval << '\n';
	else
		std::cout << "failed to get value!" << '\n';
}
