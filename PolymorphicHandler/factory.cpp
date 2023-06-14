#include "factory.h"
#include <iostream>

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

std::unique_ptr<HandlerInterace> makeHandler(Method method)
{
	switch (method)
	{
	case Method::A:
		return std::make_unique<HandlerLoop<PrintA>>();
	case Method::B:
		return std::make_unique<HandlerLoop<PrintB>>();
	default:
		return {};
	}
}
