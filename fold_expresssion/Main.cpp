// foldExpression.cpp

#include <iostream>


template<typename... Args>
bool all(Args... args) { return (... && args); }

struct myFalse { operator bool() { return false; } };
struct myTrue { operator bool() { return true; } };

template<typename... Args>
double sum(Args... args) { return (0 + ... + args); }

template<typename... Args>
double nsum(Args... args) { return (0 - ... - args); }

template<typename ...Args>
std::ostream& printer(std::ostream& os, Args&&... args) { (os << ... << args); return os; }


int main()
{
	std::cout << std::boolalpha;

	std::cout << "all(): " << all() << std::endl;

	std::cout << "all(!00): " << !all(myFalse(), myFalse()) << std::endl;
	std::cout << "all(!01): " << !all(myFalse(), myTrue()) << std::endl;
	std::cout << "all(10): " << all(myTrue(), myFalse()) << std::endl;
	std::cout << "all(11): " << all(myTrue(), myTrue()) << std::endl;

	std::cout << "sum " << sum() << std::endl;
	std::cout << "sum " << sum(1, 2, 3, 6.2, 8) << std::endl;
	std::cout << "nsum " << nsum(1, 2, 3, 6.2, 8) << std::endl;
	printer(std::cout, 1, 2, 3, 6.2, "jep");

}
