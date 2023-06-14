#include "factory.h"

int main(int, char**)
{
	std::vector<Element> v = { 1,4,8 };
	makeHandler(Method::A)->handle(v);
	v = { 11,14,18,16 };
	makeHandler(Method::B)->handle(v);
	return 0;
}