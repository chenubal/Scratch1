#include "../container_traits.h"

#include <vector>
#include <deque>
#include <list>
#include <set>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace lv
{
	//--------------------------------------------------------------------------------------------
	// Traits for common STL container
	//--------------------------------------------------------------------------------------------

	template<class... Args>
	struct container_traits<std::vector<Args...>> : public sequence_container_traits<std::vector<Args...>> {};

	template<class... Args>
	struct container_traits<std::list<Args...>> : public sequence_container_traits<std::list<Args...>> {};

	template<class... Args>
	struct container_traits<std::deque<Args...>> : public sequence_container_traits<std::deque<Args...>> {};

	template<class... Args>
	struct container_traits<std::basic_string<Args...>> : public sequence_container_traits<std::basic_string<Args...>> {};

	template<class... Args>
	struct container_traits<std::set<Args...>> : public associative_container_traits<std::set<Args...>> {};

	template<class... Args>
	struct container_traits<std::multiset<Args...>> : public associative_container_traits<std::multiset<Args...>> {};

	template<class... Args>
	struct container_traits<std::map<Args...>> : public associative_container_traits<std::map<Args...>> {};

	template<class... Args>
	struct container_traits<std::multimap<Args...>> : public associative_container_traits<std::multimap<Args...>> {};

	template<class... Args>
	struct container_traits<std::unordered_map<Args...>> : public associative_container_traits<std::unordered_map<Args...>> {};

	template<class... Args>
	struct container_traits<std::unordered_multimap<Args...>> : public associative_container_traits<std::unordered_multimap<Args...>> {};

	template<class... Args>
	struct container_traits<std::unordered_set<Args...>> : public associative_container_traits<std::unordered_set<Args...>> {};

	template<class... Args>
	struct container_traits<std::unordered_multiset<Args...>> : public associative_container_traits<std::unordered_multiset<Args...>> {};

}

//--------------------------------------------------------------------------------------------
//---------- Tests ---------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

#include <iostream>

using namespace std;

struct TestStruct
{
	TestStruct(double x) : x(x), y(int(2 * x)) {}
	double x;
	int y;
	operator int() { return y; }
};

ostream& operator<<(ostream& os, const TestStruct& s)
{
	os << s.x << '-' << s.y;
	return os;
}

using VT = list<TestStruct>;
using ST = unordered_set<int>;
using MT = unordered_map<int, double>;

int main(int, char**)
{
	//--------------------------------------------------------------------------------------------
	VT v{ 0, 1,2,3,4,5,6,7,8,9 };
	cout << "\nVector = ";
	auto even = [](VT::value_type x) {return int(x) % 2 == 0; };
	for (auto x : lv::filter(v, even))
		cout << x << " ";

	//--------------------------------------------------------------------------------------------
	ST s{ 0, 1,2,3,4,5,6,7,8,9 };
	cout << "\nSet = ";
	auto mult3 = [](ST::value_type x) {return int(x) % 3 == 0; };
	for (auto x : lv::filter(s, mult3))
		cout << x << " ";

	//--------------------------------------------------------------------------------------------
	MT m{ { 0,11.0 },{ 1,12.0 },{ 2,13.0 },{ 3,14.0 },{ 4,15.0 },{ 5,16.0 },{ 6,17.0 },{ 7,18.0 } };
	cout << "\nMap = ";
	for (auto x : lv::filter(m, [](MT::value_type x) {return int(x.second) % 2 == 0; }))
		cout << x.first << "|" << x.second << " ";

	//--------------------------------------------------------------------------------------------
	auto isValidChar = [invalid = string("_-g")](char c) {return invalid.find(c) == string::npos; };
	cout << "\nstring = " << lv::filter(string("dsf_----___das_gjfh---_gdh"), isValidChar);

	cout << "\n";
	return 0;
}

