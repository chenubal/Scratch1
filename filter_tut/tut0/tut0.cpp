#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
	for (auto& el : vec)
	{
		os << el << ' ';
	}
	return os;
}

int main(int, char**)
{
	std::vector<int> v = { 1,2,3,4,5,6,7,8,9 };
	std::vector<int> w;
	std::copy_if(v.cbegin(), v.cend(), std::back_inserter(w), [](int x) {return x % 2; });

	std::cout << w <<"\n";
	return 0;
}

// Problem kompliziert, fehleranfällig, starr

