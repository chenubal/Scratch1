#include <iostream>
#include <vector>

template <typename T, typename I = size_t>
class Countable {
public:
	Countable() { ++counter; }
	~Countable() { --counter; }
	static I num() { return counter; }
private:
	static I counter;
};
template <typename T, typename I> I Countable<T, I>::counter = I();

//-------------------------------------------------------------------------------------------------------------------

template <typename T>
class Equality {
	friend bool operator ==(T const &a, T const &b)
	{
		return a.compare(b) == 0;
	}
	friend bool operator !=(T const &a, T const &b)
	{
		return a.compare(b) != 0;
	}
};

template <typename T>
class Comparable {
	friend bool operator <(T const &a, T const &b)
	{
		return a.compare(b) < 0;
	}
	friend bool operator <=(T const &a, T const &b)
	{
		return a.compare(b) <= 0;
	}
	friend bool operator >(T const &a, T const &b)
	{
		return a.compare(b) > 0;
	}
	friend bool operator >=(T const &a, T const &b)
	{
		return a.compare(b) >= 0;
	}
};

//----------------------------------------------------------------------------------------

class C1 : public Countable<C1> { int i; };
class C2 : public Countable<C2> { double d; };

class C3 : public Equality<C3>, public Comparable<C3>
{
protected:
	int l;
public:
	C3(int k = 0) : l(k) {}
	int compare(C3 const &rhs) const { auto t = l - rhs.l; return (t ? (t < 0 ? -1 : 1) : 0); }
};

template<template <typename...> class P>
class C4 : public P<C4<P>> {};

template<template <typename...> class P>
class C5 : public P<C5<P>> 
{
protected:
	int l;
public:
	C5(int k = 0) : l(k) {}
	int compare(C5 const &rhs) const { auto t = l - rhs.l; return (t ? (t < 0 ? -1 : 1) : 0); }
};
	
int main(int, char**) 
{
	std::vector<C1> v1(10);
	std::vector<C2> v2(33);
	std::cout << "C1 has " << C1::num() << " instances\n";
	std::cout << "C2 has " << C2::num() << " instances\n";

	C3 x(5), y(7), z(5);
	std::cout << "LT = " <<  (x < y) << " \n";
	std::cout << "GT = " << !(x > y) << " \n";
	std::cout << "E = " << !(x == y) << " \n";
	std::cout << "E = " <<  (x == z) << " \n";
	std::cout << "NE = " << (x != y) << " \n";
	std::cout << "NE = " << !(x != z) << " \n";

	std::vector<C4<Countable>> v3(3);
	std::cout << "C4 has " << C4<Countable>::num() << " instances\n";

	C5<Comparable> u(9), w(10);
	std::cout << "LT = " << (u < w) << " \n";

}

