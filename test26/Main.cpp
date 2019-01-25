#include <iostream>

struct X
{
	int a;
	double b;
};
//using X = std::tuple<int, double>;

X fun() { return { 7,33.3 }; }

int main(int, char *[])
{
	auto [a,b] = fun();
	std::cout << "==>" << b << "\n";

	auto t = std::pair<int, int>{ 0, 1 };
	auto [ x, _] = t;
	std::cout << "==>" << x << "\n";
	//std::cout << "==>" << value << "\n";

}