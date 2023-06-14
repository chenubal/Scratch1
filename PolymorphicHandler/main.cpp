#include <span>
#include <iostream>
#include <vector>

using Element = int;
using SpanE = std::span<Element>;

struct HandlerInterace
{
	virtual void handle(SpanE const& elements) = 0;
};

template<class Functor>
struct HandlerLoop : public HandlerInterace
{
	void handle(SpanE const& elements) override
	{
		for (auto x : elements) f(x);
	}
	Functor f;
};

struct PrintA
{
	void operator()(Element const& e) { std::cout << "A: " << e << "\n"; }
};

struct PrintB
{
	void operator()(Element const& e) { std::cout << "B: " << e << "\n"; }
};

std::unique_ptr<HandlerInterace> makeHandler(unsigned n)
{
	if (n == 0)
		return std::make_unique<HandlerLoop<PrintA>>();
	else
		return std::make_unique<HandlerLoop<PrintB>>();
}

int main(int, char**)
{
	std::vector<Element> v = { 1,4,8 };
	makeHandler(0)->handle(v);
	v = { 11,14,18,16 };
	makeHandler(1)->handle(v);
	return 0;
}