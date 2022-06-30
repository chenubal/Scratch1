#include <iostream>
#include <vector>
#include <variant>
#include <string>

namespace jh
{
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

	using Angle = TaggedType<double, struct AngleTag>;
	using Radiant = TaggedType<double, struct RadiantTag>;
	using AngleType = std::variant<Angle, Radiant>;

	std::string unit(AngleType a)
	{
		return std::visit(
			overload(
				[](Angle) { return std::string("\370"); },
				[](Radiant) { return std::string("rad");; },
				[](auto) { return std::string(""); }
		), a);
	}

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
				[](LengthMLS) { return std::string("mls"); }
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
	std::cout << "Value: " << toStr(convert<T>(l)) << "\n";
}


int main(int, char**)
{
	jh::AngleType x = Angle(33);
	std::cout << "Value: " << toStr(x) << "\n";
	std::cout << "Value: " << value(x) << "\n";
	x = Radiant(44);
	std::cout << "Value: " << toStr(x) << "\n";
	print(LengthM(11));
	print(LengthMLS(2));
	print<LengthMM>(LengthM(11));
	print<LengthKM>(LengthMLS(2));
	return 0;

}

