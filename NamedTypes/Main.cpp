#include <iostream>
#include <vector>
#include <variant>
#include <string>
#include <algorithm>
#include <numeric>

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
	std::string asLabel(T v)
	{
		return std::to_string(value(v)) + unit(v);
	}
}
namespace units
{
	using namespace jh;
	///////////////////////////////////////////////////////////////////////////////////////

	using AngleDeg = TaggedType<double, struct AngleTag>;
	using AngleRad = TaggedType<double, struct RadiantTag>;
	using Angle = std::variant<AngleDeg, AngleRad>;

	std::string unit(Angle a)
	{
		auto v = overload
		(
			[](AngleDeg) { return std::string("\370"); }, 
			[](AngleRad) { return std::string("rad"); }
		);
		return std::visit(v, a);
	}

	double factor(Angle a)
	{
		auto v = overload
		(
			[](AngleDeg) { return 1.0; }, 
			[](AngleRad) { return 180.0 / PI; }
		);
		return std::visit(v, a);
	}

	template<class T = AngleDeg>
	Angle convert(Angle a)
	{
		if (!std::holds_alternative<T>(a))
		{
			auto q = factor(a) / factor(Angle(T(0)));
			a = T(std::visit([&](auto l) { return  *l * q; }, a));
		}
		return a;
	}

	bool operator==(Angle a, Angle b) { return value(convert(a)) == value(convert(b)); }
	bool operator!=(Angle a, Angle b) { return !(a == b); }

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

	std::string unit(Length l)
	{
		auto v = overload(
			[](LengthPM) { return std::string("pm"); },
			[](LengthNM) { return std::string("nm"); },
			[](LengthUM) { return std::string("µm"); },
			[](LengthMM) { return std::string("mm"); },
			[](LengthM) { return std::string("m"); },
			[](LengthKM) { return std::string("km"); },
			[](LengthMiles) { return std::string("mil"); });
		return std::visit(v, l);
	}

	double factor(Length l)
	{
		auto v = overload(
			[](LengthPM) { return 1e-12; },
			[](LengthNM) { return 1e-9; },
			[](LengthUM) { return 1e-6; },
			[](LengthMM) { return 1e-3; },
			[](LengthM) { return 1.0; },
			[](LengthKM) { return 1e3; },
			[](LengthMiles) { return 1609.34; });
		return std::visit(v, l);
	}

	template<class T = LengthM>
	Length convert(Length l)
	{
		if (!std::holds_alternative<T>(l))
		{
			auto q = factor(l) / factor(Length(T(0)));
			l = T(std::visit([&](auto x) { return  *x * q; }, l));
		}
		return l;
	}

	bool operator<(Length a, Length b) { return value(convert(a)) < value(convert(b)); }
	bool operator==(Length a, Length b) { return value(convert(a)) == value(convert(b)); }
	bool operator!=(Length a, Length b) { return !(a == b); }

	///////////////////////////////////////////////////////////////////////////////////////

	using Kelvin = TaggedType<double, struct KelvinTag>;
	using Celsius = TaggedType<double, struct CelsiusTag>;
	using Fahrenheit = TaggedType<double, struct FahrenheitTag>;

	using Temperature = std::variant<Kelvin, Celsius, Fahrenheit>;

	std::string unit(Temperature t)
	{
		auto v = overload
		(
			[](Kelvin) { return std::string("K"); },
			[](Celsius) { return std::string("\370C"); },
			[](Fahrenheit) { return std::string("\370F"); }
		);
		return std::visit(v, t);
	}

	///Temperature conversion: K = C + 273.15; F = C*1.8 + 32

	/// Scaling factor w.r.t Celsius
	double factor(Temperature t)
	{
		auto v = overload
		(
			[](auto) { return 1.0; },
			[](Fahrenheit) { return 1.8; }
		);
		return std::visit(v, t);
	}

	/// Scaling offset w.r.t Celsius
	double offset(Temperature t)
	{
		auto v = overload
		(
			[](Kelvin) { return 273.15; },
			[](Celsius) { return 0.0; },
			[](Fahrenheit) { return 32.0; }
		);
		return std::visit(v, t);
	}

	Temperature toCelsius(Temperature t)
	{
		if (!std::holds_alternative<Celsius>(t))
		{
			auto q = factor(t);
			auto o = offset(t);
			t = Celsius(std::visit([&](auto l) { return  ((*l) - o) / q; }, t));
		}
		return t;
	}

	template<class T = Celsius>
	Temperature convert(Temperature t)
	{
		if (!std::holds_alternative<T>(t))
		{
			auto q = factor(T(0));
			auto o = offset(T(0));
			t = T(std::visit([&](auto l) { return  (*l) * q + o; }, toCelsius(t)));
		}
		return t;
	}

	bool operator<(Temperature a, Temperature b) { return value(convert(a)) < value(convert(b)); }
	bool operator==(Temperature a, Temperature b) { return value(convert(a)) == value(convert(b)); }
	bool operator!=(Temperature a, Temperature b) { return !(a == b); }

}

using namespace units;

template<class T = LengthM>
void print(Length l)
{
	std::cout << "Length: " << asLabel(convert<T>(l)) << "\n";
}

#define NL std::cout <<"\n"

int main(int, char**)
{
	auto test = [](std::string const l, auto v) {	std::cout << l << " " << v << "\n";	};
	Angle angle = AngleDeg(45);
	test("angle:", asLabel(angle));
	test("angle:", value(angle));
	NL;
	test("angle:", asLabel(convert<AngleDeg>(angle)));
	test("angle:", asLabel(convert<AngleRad>(angle)));
	angle = AngleRad(jh::PI / 4.0);
	test("angle:", asLabel(convert<AngleDeg>(angle)));
	test("angle:", asLabel(convert<AngleRad>(angle)));
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
	test("temperature:", asLabel(convert<Celsius>(temperature)));
	test("temperature:", asLabel(convert<Kelvin>(temperature)));
	test("temperature:", asLabel(convert<Fahrenheit>(temperature)));
	NL;
	temperature = Fahrenheit(86);
	test("temperature:", asLabel(convert<Celsius>(temperature)));
	test("temperature:", asLabel(convert<Kelvin>(temperature)));
	test("temperature:", asLabel(convert<Fahrenheit>(temperature)));
	NL;
	temperature = Kelvin(303.15);
	test("temperature:", asLabel(convert<Celsius>(temperature)));
	test("temperature:", asLabel(convert<Kelvin>(temperature)));
	test("temperature:", asLabel(convert<Fahrenheit>(temperature)));
	NL;
	test("temperature:", asLabel(convert<Kelvin>(Celsius(30.0))));
	test("temperature:", asLabel(convert<Celsius>(Kelvin(303.15))));
	test("temperature:", asLabel(convert<Celsius>(Fahrenheit(212))));
	test("temperature:", asLabel(convert<Fahrenheit>(Celsius(100))));
	test("equal temperature:", Celsius(100) == Fahrenheit(212) ? "true" : "false");
	test("diff. temperature:", Celsius(100) != Kelvin(100) ? "true" : "false");

	std::vector<Temperature> temps{ Fahrenheit(212), Kelvin(310), Celsius(5), Fahrenheit(88) };
	for (auto&& x : temps)	std::cout << asLabel(convert<Kelvin>(x)) << " ";
	NL;
	std::sort(temps.begin(), temps.end());
	for (auto&& t : temps)
		std::cout << asLabel(convert<Kelvin>(t)) << "=" << asLabel(t) << " ";
	NL;
	auto acc = [](Kelvin const& s, Temperature const& t) {return Kelvin(*s + value(convert<Kelvin>(t))); };
	Temperature avg = Kelvin(*std::accumulate(temps.begin(), temps.end(), Kelvin(0.0), acc) / temps.size());
	test("avg(t) = ", asLabel(avg));
	return 0;

}

