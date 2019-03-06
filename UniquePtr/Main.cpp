#include <iostream>
#include <vector>
#include <memory>
#include <iosfwd>
#include <algorithm>

struct Position
{
	Position(double x, double y) : x(x), y(y) {}
	double x=0.0, y=0.0;
};

std::ostream& operator<<(std::ostream& os, Position const& p)
{
	os << "(" << p.x << "," << p.y << ")";
	return os;
}


struct Velocity
{
	Velocity(double cx, double cy) : cx(cx), cy(cy) {}
	double cx = 0.0, cy = 0.0;
};

std::ostream& operator<<(std::ostream& os, Velocity const& v)
{
	os << "[" << v.cx << "," << v.cy << "]";
	return os;
}

struct Vehicle
{
	virtual Position position() const = 0;
	virtual Velocity velocity() const = 0;
	virtual ~Vehicle() = default;
};

std::ostream& operator<<(std::ostream& os, Vehicle const& v)
{
	os << "V=" <<v.velocity() << " @ Position " << v.position();
	return os;
}


struct Car : public Vehicle
{
	Position position() const override { return p; }
	Velocity velocity() const override { return {10,0}; }
	Position p{10.0,10.0};
};



struct Van : public Vehicle
{
	Position position() const override { return p; }
	Velocity velocity() const override { return { 0,20 }; }
	Position p{ 20.0,20.0 };
};

struct Bike : public Vehicle
{
	Bike(unsigned n=0) : n(n) {}
	Position position() const override { return p; }
	Velocity velocity() const override { return { 7,7 }; }
	unsigned n;
	Position p{ 30.0,30.0 };
};

using vecicles_t = std::vector<std::unique_ptr<Vehicle>>;

int main(int, char**)
{
	vecicles_t vehicles;
	vehicles.push_back(std::make_unique<Van>());
	vehicles.push_back(std::make_unique<Car>());
	vehicles.push_back(std::make_unique<Bike>(500));
	for (auto&& v : vehicles)	v->position();
	std::for_each(vehicles.rbegin(), vehicles.rend(), [](auto && v) {std::cout << *v << "\n"; });
}

