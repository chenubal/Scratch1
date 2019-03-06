#include <iostream>
#include <variant>
#include <string>

std::string barf(double x) { return std::string("double ") + std::to_string(x); }
std::string barf(int x) { return std::string("int ") + std::to_string(x); }
std::string barf(std::string const&x) { return std::string("string ") + x; }

int main()
{
	auto printer = [](const auto& t) { std::cout << "PV: " << barf(t) << '\n'; };

	std::variant<int, float, std::string> var{ "heers" };

	std::visit(printer, var);

	return 0;
}