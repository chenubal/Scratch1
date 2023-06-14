#include <span>
#include <iostream>
#include <vector>

using Element = int;
using SpanE = std::span<Element>;

struct HandlerInterace
{
	virtual void handle(SpanE elements) = 0;
};

template<class Derived>
struct HandlerLoop : public HandlerInterace
{
	void handle(SpanE elements) override
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

std::unique_ptr<HandlerInterace> makeHandler(unsigned n)
{
	if (n == 0)
		return std::make_unique<HandlerLoop<HandlerA>>();
	else
		return std::make_unique<HandlerLoop<HandlerB>>();
}

int main(int, char**)
{
	std::vector<Element> v = { 1,4,8 };
	makeHandler(0)->handle(v);
	v = { 11,14,18,16 };
	makeHandler(1)->handle(v);
	return 0;
}