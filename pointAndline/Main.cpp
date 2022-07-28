#include <iostream>
#include <vector>
#include <optional>

namespace jh
{
	struct linmap 
	{
		double a = 0, b = 0;
		double operator()(double x) const { return a * x + b; }
	};

	struct point { double x = 0.0, y = 0.0; };

	bool operator==(point const& p1, point const& p2) { return { p1.x == p2.x && p1.y == p2.y }; }
	point operator*(point const& p1, double s) { return { p1.x*s,p1.y *s }; }
	point operator+(point const& p1, point const& p2) { return { p1.x + p2.x,p1.y + p2.y }; }
	point operator-(point const& p1, point const& p2) { return { p1.x - p2.x,p1.y - p2.y }; }
	std::ostream& operator<<(std::ostream& os, point const& p) { os << "[" << p.x << "," << p.y << "]"; return os; }

	std::optional<linmap> lmap(point const& p1, point const& p2)
	{
		auto q = p2 - p1;
		if (q.x != 0.0)
		{
			auto a = q.y / q.x;
			return linmap{ a, p1.y - a*p1.x};
		}
		return {};
	}
	point mid(point const& p1, point const& p2)
	{
		return p1 + (p2 - p1)*0.5;
	}
}


using namespace jh;
int main(int, char**)
{
	point p1{ 5.0,6.0 }, p2{ 10.0,12.0 };
	if (auto l = lmap(p1, p2))
	{
		auto f = *l;
		std::cout << point{ 5.0, f(5.0) } << "\n";
		std::cout << point{ 7.5, f(7.5) } << "\n";
		std::cout << point{ 10.0, f(10.0) } << "\n";
	}
	std::cout << mid(p1,p2) << "\n";
	return 0;

}

