#include <iostream>
#include <vector>
#include <variant>
#include <string>

namespace jh
{
	const double PI = 3.141592653589793238463;

	template<typename... Functions>
	struct overload : Functions...
	{
		using Functions::operator()...;
		overload(Functions... functions) : Functions(functions)... {}
	};

	template <typename T, typename Parameter>
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
	double value(T l)
	{
		return std::visit([](auto l) { return *l; }, l);
	}

	template<class T>
	std::string asLabel(T l)
	{
		return std::to_string(value(l)) + unit(l);
	}


	///////////////////////////////////////////////////////////////////////////////////////

	using Angle = TaggedType<double, struct AngleTag>;
	using Radiant = TaggedType<double, struct RadiantTag>;
	using AngleType = std::variant<Angle, Radiant>;

	std::string unit(AngleType a)
	{
		auto v = overload([](Angle) { return std::string("\370"); },[](Radiant) { return std::string("rad"); });
		return std::visit( v, a);
	}

	double factor(AngleType a)
	{
		auto v = overload( [](Angle) { return 1.0; }, [](Radiant) { return 180.0 / PI; });
		return std::visit(v, a);
	}

	template<class T = Angle>
	AngleType convert(AngleType a)
	{
		if (!std::holds_alternative<T>(a))
		{
			auto q = factor(a) / factor(AngleType(T(0)));
			a = T(std::visit([&](auto l) { return  *l * q; }, a));
		}
		return a;
	}

	///////////////////////////////////////////////////////////////////////////////////////
	using LengthPM = TaggedType<double, struct LengthPMTag>;
	using LengthNM = TaggedType<double, struct LengthNMTag>;
	using LengthUM = TaggedType<double, struct LengthUMTag>;
	using LengthMM = TaggedType<double, struct LengthMMTag>;
	using LengthM = TaggedType<double, struct LengthMTag>;
	using LengthKM = TaggedType<double, struct LengthKMTag>;
	using LengthMiles = TaggedType<double, struct LengthMilesTag>;

	using LengthType = std::variant<LengthPM, LengthNM, LengthUM, LengthMM, LengthM, LengthKM, LengthMiles>;

	std::string unit(LengthType l)
	{
		auto v = overload(
			[](LengthPM) { return std::string("pm"); },
			[](LengthNM) { return std::string("nm"); },
			[](LengthUM) { return std::string("�m"); },
			[](LengthMM) { return std::string("mm"); },
			[](LengthM) { return std::string("m"); },
			[](LengthKM) { return std::string("km"); },
			[](LengthMiles) { return std::string("mil"); });
		return std::visit(v, l);
	}

	double factor(LengthType l)
	{
		auto v = overload(
			[](LengthPM l) { return 1e-12; },
			[](LengthNM l) { return 1e-9; },
			[](LengthUM l) { return 1e-6; },
			[](LengthMM l) { return 1e-3; },
			[](LengthM l) { return 1.0; },
			[](LengthKM l) { return 1e3; },
			[](LengthMiles l) { return 1609.34; });
		return std::visit(v, l);
	}

	template<class T = LengthM>
	LengthType convert(LengthType l)
	{
		if (!std::holds_alternative<T>(l))
		{
			auto q = factor(l) / factor(LengthType(T(0)));
			l = T(std::visit([&](auto l) { return  *l * q; }, l));
		}
		return l;
	}

	///////////////////////////////////////////////////////////////////////////////////////
	
	using Kelvin = TaggedType<double, struct KelvinTag>;
	using Celsius = TaggedType<double, struct CelsiusTag>;
	using Fahrenheit = TaggedType<double, struct FahrenheitTag>;

	using Temperature = std::variant<Kelvin, Celsius, Fahrenheit>;

	std::string unit(Temperature t)
	{
		auto v =	overload(
				[](Kelvin) { return std::string("K"); },
				[](Celsius) { return std::string("\370C"); },
				[](Fahrenheit) { return std::string("\370F"); }	);
		return std::visit(v, t);
	}

	///Temperature conversion K = C + 270; F = C*1.8 + 32

	/// Scaling factor w.r.t Celsius
	double factor(Temperature t)
	{
		auto v =	overload(
				[](Kelvin) { return 1.0; },
				[](Celsius) { return 1.0; },
				[](Fahrenheit) { return 1.8; });
		return std::visit(v, t);
	}

	/// Scaling offset w.r.t Celsius
	double offset(Temperature t)
	{
		auto v =	overload(
				[](Kelvin) { return 273.15; },
				[](Celsius) { return 0.0; },
				[](Fahrenheit) { return 32.0; });
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

	bool operator==(Temperature a, Temperature b) { return value(convert(a)) == value(convert(b)); }
	bool operator!=(Temperature a, Temperature b) { return !(a == b); }

}

using namespace jh;

template<class T = LengthM>
void print(LengthType l)
{
	std::cout << "Length: " << asLabel(convert<T>(l)) << "\n";
}


LengthPM operator"" _pm(long double val) { return LengthPM(val); }
LengthNM operator"" _nm(long double val) { return LengthNM(val); }
LengthUM operator"" _um(long double val) { return LengthUM(val); }
LengthMM operator"" _mm(long double val) { return LengthMM(val); }
LengthM operator"" _m(long double val) { return LengthM(val); }
LengthKM operator"" _km(long double val) { return LengthKM(val); }
LengthMiles operator"" _mi(long double val) {return LengthMiles(val);}

#define NL std::cout <<"\n"

int main(int, char**)
{
	auto test = [](std::string const l, auto v) {	std::cout << l << " " << v << "\n";	};
	AngleType x = Angle(45);
	test("angle:", asLabel(x));
	test("angle:", value(x));
	NL;
	test("angle:", asLabel(convert<Angle>(x)));
	test("angle:", asLabel(convert<Radiant>(x)));
	x = Radiant(jh::PI/4.0);
	test("angle:", asLabel(convert<Angle>(x)));
	test("angle:", asLabel(convert<Radiant>(x)));
	NL;
	print(LengthM(11));
	print(LengthMiles(2));
	print<LengthMM>(LengthM(11));
	print<LengthM>(2.0_um);
	print<LengthMM>(3.0_nm);
	print<LengthKM>(4.0_mi);
	NL;
	Temperature t = Celsius(30.0);
	test("temperature:", asLabel(convert<Celsius>(t)));
	test("temperature:", asLabel(convert<Kelvin>(t)));
	test("temperature:", asLabel(convert<Fahrenheit>(t)));
	NL;
	t = Fahrenheit(86);
	test("temperature:", asLabel(convert<Celsius>(t)) );
	test("temperature:", asLabel(convert<Kelvin>(t)) );
	test("temperature:", asLabel(convert<Fahrenheit>(t)));
	NL;
	t = Kelvin(303.15);
	test("temperature:", asLabel(convert<Celsius>(t)));
	test("temperature:", asLabel(convert<Kelvin>(t)));
	test("temperature:", asLabel(convert<Fahrenheit>(t)));
	NL;
	test("temperature:", asLabel(convert<Kelvin>(Celsius(30.0))));
	test("temperature:", asLabel(convert<Celsius>(Kelvin(303.15))));
	test("temperature:", asLabel(convert<Celsius>(Fahrenheit(212))));
	test("temperature:", asLabel(convert<Fahrenheit>(Celsius(100))));
	test("equal temperature:", Celsius(100) == Fahrenheit(212) ? "true" : "false");
	test("different temperature:", Celsius(100) != Kelvin(100) ? "true" : "false");

	return 0;

}
