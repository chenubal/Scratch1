#include <cmath>

template<class T= double>
struct point2_t
{
	using value_type = T;
	T x, y;
	bool operator==(point2_t<T> const& p) const { return x == p.x && y==p.y; }
	bool operator!=(point2_t<T> const& p) const { return !( p == *this); }
	bool operator<(point2_t<T> const& p) const { return x < p.x; }
};

template<class T>
double distance(T const& p1, T const& p2)
{
	auto dx = p1.x - p2.x;
	auto dy = p1.y - p2.y;
	return sqrt(dx*dx + dy*dy);
}

template<class T>
struct point3_t
{
	using value_type = T;
	T x, y,z;
	bool operator==(point3_t<T> const& p) const { return x == p.x && y == p.y  && z == p.z;}
	bool operator!=(point3_t<T> const& p) const { return !(p == *this); }
};

template<class T>
double distance(point3_t<T> const& p1, point3_t<T> const& p2)
{
	auto dx = p1.x - p2.x;
	auto dy = p1.y - p2.y;
	auto dz = p1.z - p2.z;
	return sqrt(dx*dx + dy*dy + dz*dz);
}

//------ Convenience ----------------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include <tuple>

using Point2 = point2_t<double>;
using Point3 = point3_t<double>;
using Shift2 = std::pair<Point2::value_type, Point2::value_type>;
using Shift3 = std::tuple<Point3::value_type, Point3::value_type, Point3::value_type>;

Point2 shift(Point2 const& p,  Shift2 const& s)
{ 
	return{ p.x + s.first, p.y + s.second }; 
}

Point3 shift(Point3 const& p, Shift3 const& s)
{ 
	return{ p.x + std::get<0>(s), p.y + std::get<1>(s), p.z + std::get<2>(s) }; 
}

std::ostream& operator<<(std::ostream &os, Point2 const& p) 
{ 
	os << "(" << p.x << "|" << p.y << ")";  return os; 
}

std::ostream& operator<<(std::ostream &os, Point3 const& p) 
{ 
	os << "(" << p.x << "|" << p.y << "|" << p.z << ")";  return os; 
}

Point3 convert(Point2 const& p) { return{ p.x, p.y, 0.0 }; }
Point2 convert(Point3 const& p) { return{ p.x, p.y }; }

//----- Application -----------------------------------------------------------------------------------------------------------------

#include <algorithm>
#include <vector>

using Points = std::vector<Point2>;

int main()
{
	Point2 p0{0,0};
	Points  points{ { 1,6.1 },{ -10,100 },{ 4,-55 }, p0 , shift(p0,{ 4,-4 }) };
	for (auto &p : points) 	std::cout << "P=" << p << "\n";
	
	//std::sort(points.begin(), points.end());
	std::sort(points.begin(), points.end(), [](Point2 const& p1, Point2 const& p2) {return p1.y < p2.y; });

	for (auto const&p : points) std::cout << "Q=" << p << "\n";

	Point2 x{ 44,77 }; 
	auto a = 10;
	std::cout << "sqrt(2)=" << distance(x, shift(x,{ a,-a })) / 10.0 << "\n";

	std::cout << convert({ 1,2,3 }) << "\n";
}