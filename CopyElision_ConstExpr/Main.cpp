#include <iostream>
#include <string>
#include <vector>

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
	for (auto& el : vec)
	{
		os << el << ' ';
	}
	return os;
}

void algorithm_signed(int) { std::cout << "signed\n"; }
void algorithm_unsigned(unsigned) { std::cout << "unsigned\n"; }


template <typename T>
void algorithm(T t)
{
	if constexpr (std::is_signed<T>::value)
		algorithm_signed(t);
	else
		if constexpr (std::is_unsigned<T>::value)
			algorithm_unsigned(t);
		else
			static_assert(std::is_signed<T>::value || std::is_unsigned<T>::value, "Must be signed or unsigned!");
}

struct X
{
	X()
	{
		static int i = 55;
		c = i++;
		std::cout << "X " << c << " \n";
	}
	void print() const { std::cout << "print " << c << "\n"; }
	int c = 0;
};

void fun(X  x)
{
	x.print();
}

int main()
{
	algorithm(1);
	X y, z;
	fun(z);
	fun(X());
}