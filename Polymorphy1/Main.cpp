#include "Buffer.h"
#include "Image.h"
#include "Geometry.h"

using namespace std;
 
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

	//auto i=0U, k = 5U;

	//Image<double> I(k+1, k);
	//for (auto &x : I) x = 1.1*i++;
	//std::cout << I << "\n---\n";

	//std::cout << (I * I.clone<short>()) << "\n";
	//std::cout << apply<double,int>(I, [](double x)->int {return x*x; }) << "\n";

	//geo::Point<double> P{ 1, 1 };
	//auto Q = P;
	//cout << Q.move({2,2}) << "\n";
	//geo::Vector<double> u{0,0};
	//u += {3, 4};
	//auto u2 = u;
	geo::Point<double> P{ 1,1 };
	geo::Point<double> Q{ 10,10 };
	geo::Line l(P, Q);
	geo::Point<double> X = { 2+1,-2+1 };
	cout << l.projection(X) << "\n";
	cout << l.distance(X) << "\n";

}
