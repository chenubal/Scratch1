#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <string>

struct Position
{
	Position(double x, double y) : x(x), y(y) {}
	double x=0.0, y=0.0;
};

std::ostream& operator<<(std::ostream& os, Position const& p)
{
	return os << "(" << p.x << "," << p.y << ")";
}

struct Velocity
{
	Velocity(double cx, double cy) : cx(cx), cy(cy) {}
	double cx = 0.0, cy = 0.0;
};

std::ostream& operator<<(std::ostream& os, Velocity const& v)
{
	return os << "[" << v.cx << "," << v.cy << "]";
}

struct Vehicle
{
	virtual Position position() const = 0;
	virtual Velocity velocity() const = 0;
	virtual std::string name() const = 0;
	virtual ~Vehicle() = default;
};

std::ostream& operator<<(std::ostream& os, Vehicle const& v)
{
	return os << v.name() << " V=" << v.velocity() << " @ Position " << v.position();
}

struct Car : public Vehicle
{
	Position position() const override { return p; }
	Velocity velocity() const override { return {10,0}; }
	std::string name() const override { return "Car"; }
	Position p{ 10.0,10.0 };
};

struct Van : public Vehicle
{
	Position position() const override { return p; }
	Velocity velocity() const override { return { 0,20 }; }
	std::string name() const override { return "Van"; }
	Position p{ 20.0,20.0 };
};

struct Bike : public Vehicle
{
	Bike(unsigned n=0) : n(n) {}
	Position position() const override { return p; }
	Velocity velocity() const override { return { 7,7 }; }
	std::string name() const override { return "Bike"; }
	Position p{ 30.0,30.0 };
	unsigned n;
};

using vecicles_t = std::vector<std::unique_ptr<Vehicle>>;

int main()
{
	vecicles_t vehicles;
	vehicles.push_back(std::make_unique<Van>());
	vehicles.push_back(std::make_unique<Car>());
	vehicles.push_back(std::make_unique<Bike>(500));
	std::for_each(vehicles.rbegin(), vehicles.rend(), [](auto && v) {std::cout << *v << "\n"; });
}

