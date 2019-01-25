#include <iostream>
#include <map>
#include <string>

struct Cmd
{
	std::string cmd;
	Cmd(std::string const& cmd) : cmd(cmd) {}
	void operator()(int x) const { std::cout << "run " << cmd << "(" << x << ")\n"; }
};


int main()
{

	std::map<int, Cmd> M = { {100, {"A"}},{-1000, {"B"}}, {200,{"C"}}, {20,{"D"}} };

	std::cout << "Priority with map: -------------------------------------------\n";
	while (!M.empty())
	{
		auto[x, f] = *M.rbegin();
		f(x);
		M.erase(x);
	}

}