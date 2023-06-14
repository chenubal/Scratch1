#include "factory.h"
#include <valarray>


int main(int, char**)
{
	std::valarray<Element> v = { 1,4,8 };
	makeHandler(Method::A)->handle(v);
	v += 100;
	makeHandler(Method::B)->handle(v);
	v += 100;
	makeHandler(Method::C)->handle(v);
	return 0;
}