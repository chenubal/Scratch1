#include "boost/variant.hpp"
#include <iostream>

class my_visitor : public boost::static_visitor<int>
{
public:
	int operator()(int i) const
	{
		return i;
	}

	int operator()(double d) const
	{
		return int(d);
	}
};

int main()
{
	boost::variant< int, double > u(33.4f);
	std::cout << u << "\n";

	int result = boost::apply_visitor([](auto x)->int {return int(x); }, u);
	std::cout << result << "\n";
}