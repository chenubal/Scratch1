#include "Buffer.h"
 
int main()
{
	std::cout << "Type n: ";
	unsigned n; std::cin >> n;
	auto f = createBuffer(n);
	f->store("myfile.im7");
	delete f;
	auto m = loadHeader("myfile.im7");
	auto g = createBuffer(m);
	delete g;
}
