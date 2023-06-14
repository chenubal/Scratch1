#include <iostream>
#include <vector>
#include <variant>
#include <string>
#include <algorithm>
#include <numeric>
#include <map>
#include <functional>
#include <optional>

namespace jh
{
	const double PI = 3.141592653589793238463;

	template<typename... Functions>
	struct overload : Functions...
	{
		using Functions::operator()...;
		overload(Functions... functions) : Functions(functions)... {}
	};

	template <typename T, typename Tag>
	class TaggedType
	{
	public:
		explicit TaggedType(T const& v) : value(v) {}
		explicit TaggedType(T&& v) : value(std::move(v)) {}
		const T& operator*() const { return value; }
	private:
		T value;
	};

	template<class T>
	double value(T v)
	{
		return std::visit([](auto tt) { return *tt; }, v);
	}

	template<class T>
	std::string labeledValue(T v)
	{
		return std::to_string(value(v)) + unit(v);
	}
}
namespace units
{
	using namespace jh;
	///////////////////////////////////////////////////////////////////////////////////////

	using AngleInDeg = TaggedType<double, struct AngleTag>;
	using AngleInRad = TaggedType<double, struct RadiantTag>;
	using Angle = std::variant<AngleInDeg, AngleInRad>;

	std::string unit(Angle angle)
	{
		auto v = overload
		(
			[](AngleInDeg) { return std::string("\370"); },
			[](AngleInRad) { return std::string("rad"); }
		);
		return std::visit(v, angle);
	}

	double factor(Angle angle)
	{
		auto v = overload
		(
			[](AngleInDeg) { return 1.0; },
			[](AngleInRad) { return 180.0 / PI; }
		);
		return std::visit(v, angle);
	}

	template<class T = AngleInDeg>
	Angle as(Angle angle)
	{
		if (!std::holds_alternative<T>(angle))
		{
			auto q = factor(angle) / factor(Angle(T(0)));
			angle = T(std::visit([&](auto x) { return  *x * q; }, angle));
		}
		return angle;
	}

	bool operator==(Angle left, Angle right) { return value(as(left)) == value(as(right)); }
	bool operator!=(Angle left, Angle right) { return !(left == right); }

	///////////////////////////////////////////////////////////////////////////////////////
	using LengthPM = TaggedType<double, struct LengthPMTag>;
	using LengthNM = TaggedType<double, struct LengthNMTag>;
	using LengthUM = TaggedType<double, struct LengthUMTag>;
	using LengthMM = TaggedType<double, struct LengthMMTag>;
	using LengthM = TaggedType<double, struct LengthMTag>;
	using LengthKM = TaggedType<double, struct LengthKMTag>;
	using LengthMiles = TaggedType<double, struct LengthMilesTag>;

	LengthPM operator"" _pm(long double val) { return LengthPM(val); }
	LengthNM operator"" _nm(long double val) { return LengthNM(val); }
	LengthUM operator"" _um(long double val) { return LengthUM(val); }
	LengthMM operator"" _mm(long double val) { return LengthMM(val); }
	LengthM operator"" _m(long double val) { return LengthM(val); }
	LengthKM operator"" _km(long double val) { return LengthKM(val); }
	LengthMiles operator"" _mil(long double val) { return LengthMiles(val); }

	using Length = std::variant<LengthPM, LengthNM, LengthUM, LengthMM, LengthM, LengthKM, LengthMiles>;

	std::string unit(Length length)
	{
		auto v = overload(
			[](LengthPM) { return std::string("pm"); },
			[](LengthNM) { return std::string("nm"); },
			[](LengthUM) { return std::string("µm"); },
			[](LengthMM) { return std::string("mm"); },
			[](LengthM) { return std::string("m"); },
			[](LengthKM) { return std::string("km"); },
			[](LengthMiles) { return std::string("mil"); });
		return std::visit(v, length);
	}

	double factor(Length length)
	{
		auto v = overload(
			[](LengthPM) { return 1e-12; },
			[](LengthNM) { return 1e-9; },
			[](LengthUM) { return 1e-6; },
			[](LengthMM) { return 1e-3; },
			[](LengthM) { return 1.0; },
			[](LengthKM) { return 1e3; },
			[](LengthMiles) { return 1609.34; });
		return std::visit(v, length);
	}

	template<class T = LengthM>
	Length convert(Length length)
	{
		if (!std::holds_alternative<T>(length))
		{
			auto q = factor(length) / factor(Length(T(0)));
			length = T(std::visit([&](auto x) { return  *x * q; }, length));
		}
		return length;
	}

	bool operator<(Length left, Length right) { return value(convert(left)) < value(convert(right)); }
	bool operator==(Length left, Length right) { return value(convert(left)) == value(convert(right)); }
	bool operator!=(Length left, Length right) { return !(left == right); }

	///////////////////////////////////////////////////////////////////////////////////////

	using Kelvin = TaggedType<double, struct KelvinTag>;
	using Celsius = TaggedType<double, struct CelsiusTag>;
	using Fahrenheit = TaggedType<double, struct FahrenheitTag>;
	using Temperature = std::variant<Kelvin, Celsius, Fahrenheit>;

	// NOTE: keys have to match the indexes of Temperature
	static std::map<unsigned, std::function<Temperature(double)>> temperatureMaker = 
	{ 
		{0u, [](double v) {return Kelvin(v); } },
		{1u, [](double v) {return Celsius(v); } },
		{2u, [](double v) {return Fahrenheit(v); } } 
	};

	std::optional<Temperature> createTemperature(unsigned i, double v = 0)
	{
		if (temperatureMaker.count(i))
			return temperatureMaker.at(i)(v);
		return {};
	}

	std::string unit(Temperature temperature)
	{
		auto getUnit = overload
		(
			[](Kelvin) { return std::string("K"); },
			[](Celsius) { return std::string("\370C"); },
			[](Fahrenheit) { return std::string("\370F"); }
		);
		return std::visit(getUnit, temperature);
	}

	///Temperature conversion: K = C + 273.15; F = C*1.8 + 32

	/// Scaling factor w.r.t. temperature Celsius
	double factor(Temperature temperature)
	{
		auto getFactor = overload
		(
			[](auto) { return 1.0; },
			[](Fahrenheit) { return 1.8; }
		);
		return std::visit(getFactor, temperature);
	}

	/// Scaling offset w.r.t. temperature Celsius
	double offset(Temperature temperature)
	{
		auto getOffset = overload
		(
			[](Kelvin) { return 273.15; },
			[](Celsius) { return 0.0; },
			[](Fahrenheit) { return 32.0; }
		);
		return std::visit(getOffset, temperature);
	}

	Temperature toCelsius(Temperature temperature)
	{
		if (!std::holds_alternative<Celsius>(temperature))
		{
			auto q = factor(temperature);
			auto o = offset(temperature);
			temperature = Celsius(std::visit([&](auto x) { return  ((*x) - o) / q; }, temperature));
		}
		return temperature;
	}

	template<class T = Celsius>
	Temperature as(Temperature temperature)
	{
		if (!std::holds_alternative<T>(temperature))
		{
			auto q = factor(T(0));
			auto o = offset(T(0));
			temperature = T(std::visit([&](auto x) { return  (*x) * q + o; }, toCelsius(temperature)));
		}
		return temperature;
	}

	bool operator<(Temperature left, Temperature right) { return value(toCelsius(left)) < value(toCelsius(right)); }
	bool operator==(Temperature left, Temperature right) { return value(toCelsius(left)) == value(toCelsius(right)); }
	bool operator!=(Temperature left, Temperature right) { return !(left == right); }

}

using namespace units;

template<class T = LengthM>
void print(Length length)
{
	std::cout << "Length: " << labeledValue(convert<T>(length)) << "\n";
}

#define NL std::cout <<"\n"

int main(int, char**)
{
	auto test = [](std::string const label, auto value) {	std::cout << label << " " << value << "\n";	};
	Angle angle = AngleInDeg(45);
	test("angle:", labeledValue(angle));
	test("angle:", value(angle));
	NL;
	test("angle:", labeledValue(as<AngleInDeg>(angle)));
	test("angle:", labeledValue(as<AngleInRad>(angle)));
	angle = AngleInRad(jh::PI / 4.0);
	test("angle:", labeledValue(as<AngleInDeg>(angle)));
	test("angle:", labeledValue(as<AngleInRad>(angle)));
	NL;
	Length length = LengthM(11);
	print(length);
	print(LengthMiles(2));
	print<LengthMM>(LengthM(11));
	print<LengthM>(2.0_um);
	print<LengthMM>(3.0_nm);
	print<LengthKM>(4.0_mil);
	NL;
	Temperature temperature = Celsius(30.0);
	test("temperature:", labeledValue(as<Celsius>(temperature)));
	test("temperature:", labeledValue(as<Kelvin>(temperature)));
	test("temperature:", labeledValue(as<Fahrenheit>(temperature)));
	NL;
	temperature = Fahrenheit(86);
	test("temperature:", labeledValue(as<Celsius>(temperature)));
	test("temperature:", labeledValue(as<Kelvin>(temperature)));
	test("temperature:", labeledValue(as<Fahrenheit>(temperature)));
	NL;
	temperature = Kelvin(303.15);
	test("temperature:", labeledValue(as<Celsius>(temperature)));
	test("temperature:", labeledValue(as<Kelvin>(temperature)));
	test("temperature:", labeledValue(as<Fahrenheit>(temperature)));
	NL;
	test("temperature:", labeledValue(as<Kelvin>(Celsius(30.0))));
	test("temperature:", labeledValue(as<Celsius>(Kelvin(303.15))));
	test("temperature:", labeledValue(as<Celsius>(Fahrenheit(212))));
	test("temperature:", labeledValue(as<Fahrenheit>(Celsius(100))));
	test("equal temperature:", Celsius(100) == Fahrenheit(212) ? "true" : "false");
	test("diff. temperature:", Celsius(100) != Kelvin(100) ? "true" : "false");

	std::vector<Temperature> temperatures{ Fahrenheit(212), Kelvin(310), Celsius(5), Fahrenheit(88) };
	for (auto&& t : temperatures)	std::cout << labeledValue(as<Kelvin>(t)) << " ";
	NL;
	std::sort(temperatures.begin(), temperatures.end());
	for (auto&& t : temperatures)
		std::cout << labeledValue(as<Kelvin>(t)) << "=" << labeledValue(t) << " ";
	NL;
	auto acc = [](Kelvin const& s, Temperature const& t) {return Kelvin(*s + value(as<Kelvin>(t))); };
	Temperature avg = Kelvin(*std::accumulate(temperatures.begin(), temperatures.end(), Kelvin(0.0), acc) / temperatures.size());
	test("avg(t) = ", labeledValue(avg));
	return 0;

}

