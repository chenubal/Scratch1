#include "Buffer.h"
#include "Image.h"
#include "Geometry.h"

using namespace std;
 
int main()
{
	using namespace geo;
	//cout << "Type n: ";
	//unsigned n; cin >> n;
	//auto f = createBuffer(n);
	//f->store("myfile.im7");
	//delete f;
	//auto m = loadHeader("myfile.im7");
	//auto g = createBuffer(m);
	//delete g;

	auto i=0U, k = 5U;

	cv2::Image<double> I(k+1, k);
	for (auto &x : I) x = 1.1*i++;
	cout << I << "\n---\n";

	cout << (I * cv2::Image<short>(I)) << "\n";
	cout << cv2::apply<double,int>(I, [](double x) {return int(x*x); }) << "\n";

	Point<> P1{ 1, 1 };
	auto Q1 = P1;
	cout << Q1 + Vector<int>{2,2} << "\n";
	Vector<> u{0,0};
	u += {3, 4};
	auto u2 = u;
	cout << (u == u2) << "\n";

	Point<> P{ 1,1 };
	Point<> Q{ 10,10 };
	Line l(P, Q);
	cout << l(1) << "\n";
	Point<> X = { 2+1,-2+1 };
	cout << l.projection(X) << "\n";
	cout << l.distance(X) << "\n";

	Vector<> vv = { 1.1,2.2 };
	auto ww = Vector<int>(vv);
	cout << ww << "\n";

}
