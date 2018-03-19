#include "Buffer.h"
#include "Image.h"
 
int main()
{
	//std::cout << "Type n: ";
	//unsigned n; std::cin >> n;
	//auto f = createBuffer(n);
	//f->store("myfile.im7");
	//delete f;
	//auto m = loadHeader("myfile.im7");
	//auto g = createBuffer(m);
	//delete g;

	auto i=0U, k = 5U;

	Image<double> I(k+1, k);
	for (auto &x : I) x = 1.1*i++;
	std::cout << I << "\n---\n";

	std::cout << (I * I.clone<short>()) << "\n";
	std::cout << apply<double,int>(I, [](double x)->int {return x*x; }) << "\n";
}
