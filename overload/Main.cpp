#include <iostream>
#include <string>
#include <vector>
#include <functional>


using interface = std::function<void(int)>;

template<class T>
struct overload : interface { using I::operator(); };
template<class T> overload(T)->overload<T>;


int main()
{

	auto b = overload{ [](double s) {std::cout << "s=" << s << "\n"; } };
	auto c = overload{ [](double) {std::cout << "E\n"; } };
	b(10.3);
	c(0);
}
