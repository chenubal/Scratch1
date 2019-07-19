#include <iostream>
#include <string>
#include <tuple>

#define OP_DEF(op) [[nodiscard]] constexpr bool operator op (const T& o) const{	return client().tie() op o.tie(); }

template <typename T>
class TieComparable
{
private:
	constexpr T const& client() const { return static_cast<const T&>(*this); }
	TieComparable() = default;
	~TieComparable<T>() = default;
	TieComparable<T>(const TieComparable<T>& other) = default;
	TieComparable<T>(TieComparable<T>&& other) = default;
	TieComparable<T>& operator=(const TieComparable<T>& other) = default;
	TieComparable<T>& operator=(TieComparable<T>&& other) = default;

	friend T;

public:
	[[nodiscard]] constexpr bool operator== (const T& o) const { return client().tie() == o.tie(); }
	OP_DEF( != )
	OP_DEF( <= )
	OP_DEF( >= )
	OP_DEF( < )
	OP_DEF( > )

};

struct TestClass : public TieComparable<TestClass> // CRTP
{
	TestClass(int i, double d, std::string s) : i(i), d(d), s(s) {}
	int i;
	double d;
	std::string s;
	const auto tie() const { return std::tie(i, d, s); }
};


int main()
{ 
	TestClass s1{ 0,2.0,"test1" };
	TestClass s2{ 0,1.0,"test1" };

	std::cout << "op ==: " << (s1 == s2) << std::endl;
	std::cout << "op !=: " << (s1 != s2) << std::endl;
	std::cout << "op >=: " << (s1 >= s2) << std::endl;
	std::cout << "op <=: " << (s1 <= s2) << std::endl;
	std::cout << "op > : " << (s1  > s2) << std::endl;
	std::cout << "op < : " << (s1  < s2) << std::endl;
}
