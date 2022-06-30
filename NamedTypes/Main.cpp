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

	///////////////////////////////////////////////////////////////////////////////////////

	using Angle = TaggedType<double, struct AngleTag>;
	using Radiant = TaggedType<double, struct RadiantTag>;
	using AngleType = std::variant<Angle, Radiant>;

	std::string unit(AngleType a)
	{
		return std::visit(
			overload(
				[](Angle) { return std::string("\370"); },
				[](Radiant) { return std::string("rad"); }
		), a);
	}

	double factor(AngleType l)
	{
		return std::visit(
			overload(
				[](Angle ) { return 1.0; },
				[](Radiant ) { return 180.0/PI; }
		), l);
	}

	template<class T = Angle>
	AngleType convert(AngleType l)
	{
		auto q = factor(l) / factor(AngleType(T(0)));
		return T(std::visit([&](auto l) { return  *l * q; }, l));
	}

	///////////////////////////////////////////////////////////////////////////////////////
	using LengthMM = TaggedType<double, struct LengthMMTag>;
	using LengthM = TaggedType<double, struct LengthMTag>;
	using LengthKM = TaggedType<double, struct LengthKMTag>;
	using LengthMLS = TaggedType<double, struct LengthMLSTag>;
	using LengthType = std::variant<LengthMM, LengthM, LengthKM, LengthMLS>;

	std::string unit(LengthType l)
	{
		return std::visit(
			overload(
				[](LengthMM) { return std::string("mm"); },
				[](LengthM) { return std::string("m"); },
				[](LengthKM) { return std::string("km"); },
				[](LengthMLS) { return std::string("mil"); }
		), l);
	}

	double factor(LengthType l)
	{
		return std::visit(
			overload(
				[](LengthMM l) { return 0.001; },
				[](LengthM l) { return 1.0; },
				[](LengthKM l) { return 1000.0; },
				[](LengthMLS l) { return 1609.34; }
		), l);
	}

	template<class T = LengthM>
	LengthType convert(LengthType l)
	{
		auto q = factor(l) / factor(LengthType(T(0)));
		return T(std::visit([&](auto l) { return  *l * q; }, l));
	}

	///////////////////////////////////////////////////////////////////////////////////////
	
	using Kelvin = TaggedType<double, struct KelvinTag>;
	using Celsius = TaggedType<double, struct CelsiusTag>;
	using Fahrenheit = TaggedType<double, struct FahrenheitTag>;
	using Temperature = std::variant<Kelvin, Celsius, Fahrenheit>;

	std::string unit(Temperature t)
	{
		return std::visit(
			overload(
				[](Kelvin) { return std::string("\370K"); },
				[](Celsius) { return std::string("\370C"); },
				[](Fahrenheit) { return std::string("\370F"); }
		), t);
	}

	///Temperature conversion K = C + 270; F = C*1.8 + 32

	/// Scaling factor w.r.t Celsius
	double factor(Temperature t)
	{
		return std::visit(
			overload(
				[](Kelvin) { return 1.0; },
				[](Celsius) { return 1.0; },
				[](Fahrenheit) { return 1.8; }
		), t);
	}

	/// Scaling offset w.r.t Celsius
	double offset(Temperature t)
	{
		return std::visit(
			overload(
				[](Kelvin) { return 271.0; },
				[](Celsius) { return 0.0; },
				[](Fahrenheit) { return 32.0; }
		), t);
	}

	Temperature convertC(Temperature t)
	{
		auto q = factor(t);
		auto o = offset(t);
		return Celsius(std::visit([&](auto l) { return  ((*l) - o) / q; }, t));
	}

	template<class T = Celsius>
	Temperature convert(Temperature t)
	{
		auto tc = convertC(t);
		auto q = factor(T(0));
		auto o = offset(T(0));
		return T(std::visit([&](auto l) { return  (*l) * q  + o ; }, tc));
	}

	///////////////////////////////////////////////////////////////////////////////////////

	template<class T>
	double value(T l)
	{
		return std::visit([](auto l){ return *l; }, l);
	}

	template<class T>
	std::string toStr(T l)
	{
		return std::to_string(value(l)) + unit(l);
	}
}

using namespace jh;

template<class T = LengthM>
void print(LengthType l)
{
	std::cout << "Length: " << toStr(convert<T>(l)) << "\n";
}

#define NL std::cout <<"\n";

int main(int, char**)
{
	jh::AngleType x = Angle(45);
	std::cout << "angle: " << toStr(x) << "\n";
	std::cout << "angle: " << value(x) << "\n";
	NL
	std::cout << "angle: " << toStr(convert<Angle>(x)) << "\n";
	std::cout << "angle: " << toStr(convert<Radiant>(x)) << "\n";
	x = Radiant(jh::PI/4.0);
	std::cout << "angle: " << toStr(convert<Angle>(x)) << "\n";
	std::cout << "angle: " << toStr(convert<Radiant>(x)) << "\n";
	NL
	print(LengthM(11));
	print(LengthMLS(2));
	print<LengthMM>(LengthM(11));
	print<LengthKM>(LengthMLS(2));
	NL
	Temperature t = Celsius(30.0);
	std::cout << "temperature: " << toStr(convert<Celsius>(t)) << "\n";
	std::cout << "temperature: " << toStr(convert<Kelvin>(t)) << "\n";
	std::cout << "temperature: " << toStr(convert<Fahrenheit>(t)) << "\n";
	NL
	t = Fahrenheit(86);
	std::cout << "temperature: " << toStr(convert<Celsius>(t)) << "\n";
	std::cout << "temperature: " << toStr(convert<Kelvin>(t)) << "\n";
	std::cout << "temperature: " << toStr(convert<Fahrenheit>(t)) << "\n";
	NL
	t = Kelvin(301);
	std::cout << "temperature: " << toStr(convert<Celsius>(t)) << "\n";
	std::cout << "temperature: " << toStr(convert<Kelvin>(t)) << "\n";
	std::cout << "temperature: " << toStr(convert<Fahrenheit>(t)) << "\n";
	return 0;

}

