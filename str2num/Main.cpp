#include <iostream>
#include <sstream>


template<class T>
T string2num(std::string const& s)
{
	T v;
	std::istringstream(s) >> std::scientific >> v;
	return v;
}

int main()
{
	std::string inp("    8.67538E-0005 ");
	std::cout << "unsigned: " << string2num<unsigned>(inp) << std::endl;
	std::cout << "double: " << string2num<double>(inp) << std::endl;
	std::cout << "float: " << string2num<float>(inp) << std::endl;
	std::cout << "float: " << string2num<float>("  014.55   ") << std::endl;
}
