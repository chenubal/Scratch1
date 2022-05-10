

// typeErasure.cpp

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <numeric>

#define ONL std::cout << '\n'


struct Bar
{
	std::string print() const { return "Bar"; }
};

struct Foo
{
	std::string print() const { return s; }
	std::string s = "Foo";
};

std::string print(Bar const& b) { return b.print(); }
std::string print(Foo const& f) { return f.print(); }

class PrintObject
{

public:
	template <typename T>
	PrintObject(T&& obj) : object(std::make_shared<Wrapper<T>>(std::forward<T>(obj))) {}

	std::string print() const { return object->print(); }

	struct Interface
	{
		virtual ~Interface() = default;
		virtual std::string print() const = 0;
	};

	template< typename T >
	struct Wrapper : Interface
	{
		Wrapper(const T& t) : object(t) {}
		std::string print() const override { return ::print(object); }
	private:
		T object;
	};

	std::shared_ptr<const Interface> object;
};


std::string print(std::vector<PrintObject> const& vec)
{
	auto acc = [](auto const& s, auto const& x) {return s + x.print() + "|"; };
	return std::accumulate(vec.begin(), vec.end(), std::string(), acc);
}

int main()
{
	Foo f{ "fooooooooo" };
	std::vector<PrintObject> vec0{ std::move(f), Bar() }; 
	std::vector<PrintObject> vec1{ vec0, Bar(),vec0, Foo() }; 
	std::vector<PrintObject> vec2{ vec1,vec1, Foo(), Bar() }; 

	std::cout << print(vec2) << '\n';
	ONL;
	for (auto&& x : vec2) std::cout << x.print() << "\n";
	ONL;
}

