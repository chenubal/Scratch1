#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <functional>


struct A
{
	void print(std::string const& s) const { std::cout << "A: " << s << "\n"; }
};

struct B
{
	void print(std::string const& s) const { std::cout << "B: " << s << "\n"; }
};

struct C
{
	void print(std::string const& s) const { std::cout << "C: " << s << "\n"; }
};


using V = std::variant<A, B, C>;


struct Printer : public V
{
	using V::V;
	void print(std::string const&s) const
	{
		auto f = [&s](auto const& p) {p.print("P: " + s); };
		std::visit(f, static_cast<V const>(*this));
	}
};

int main()
{
	std::vector<Printer> allPrinter = { C(),A(),B() };

	for (auto const& p : allPrinter)
		p.print("456");
}
