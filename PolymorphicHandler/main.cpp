#include <span>
#include <iostream>
#include <vector>

using Element = int;
using SPE = std::span<Element>;

struct HandlerInterace
{
	virtual void handle(SPE elements) = 0;
};

template<class Derived>
struct HandlerHost : public HandlerInterace
{
	void handle(SPE elements) override
	{
		for (auto x : elements) d.handle(std::move(x));
	}
	Derived d;
};

struct HandlerA
{
	void handle(Element e) { std::cout << "A: " << e << "\n"; }
};

struct HandlerB
{
	void handle(Element e) { std::cout << "B: " << e << "\n"; }
};

int main(int, char**)
{
	std::vector<Element> v = { 1,4,8 };
	HandlerHost<HandlerA>().handle(v);
	v = { 11,14,18,16 };
	HandlerHost<HandlerB>().handle(v);
	return 0;
}