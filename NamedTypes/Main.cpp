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
	class NamedType
	{
	public:
		explicit NamedType(T const& v) : value(v) {}
		explicit NamedType(T&& v) : value(std::move(v)) {}
		const T& operator*() const { return value; }
	private:
		T value;
	};


	using Angle = NamedType<double, struct AngleTag>;
	using Radiant = NamedType<double, struct RadiantTag>;
	using AngleType = std::variant<Angle, Radiant>;

	std::string unit(AngleType a)
	{
		return std::visit(
			overload(
				[](Angle) { return std::string("\370"); },
				[](Radiant) { return std::string("rad");; }
		), a);
	}

	std::string toStr(AngleType a)
	{
		return std::visit(
			overload(
				[](Angle a) { return std::to_string(*a) + std::string("\370"); },
				[](Radiant a) { return std::to_string(*a) + std::string("rad");; }
		), a);
	}

	using LengthMM = NamedType<double, struct LengthMMTag>;
	using LengthM = NamedType<double, struct LengthMTag>;
	using LengthKM = NamedType<double, struct LengthKMTag>;
	using LengthMLS = NamedType<double, struct LengthMLSTag>;
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

	double value(LengthType l)
	{
		return std::visit([](auto l)->double { return *l; }, l);
	}
	std::string toStr(LengthType l)
	{
		return std::to_string(value(l)) + unit(l);
	}
}

using namespace jh;

void fun(LengthType l)
{
	std::cout << "Value: " << toStr(l) << "\n";
}


int main(int, char**)
{
	jh::AngleType x = Angle(33);
	std::cout << "Value: " << toStr(x) << "\n";
	x = Radiant(44);
	std::cout << "Value: " << toStr(x) << "\n";
	fun(LengthM(22));
	fun(LengthMLS(55));
	return 0;

}

