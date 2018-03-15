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
	Image<double> Ii(k+1, k);

	for (auto &x : Ii) x = 1.1*i++;
	auto Id = Ii.clone<bool>();

	std::cout << Id << "\n";

	auto K = Ii;
	std::cout << K << "\n";
}
