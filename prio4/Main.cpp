#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <tuple>

struct Cmd
{
	Cmd( const char* c) : cmd(c) {}
	Cmd(std::string const&c) : cmd(c) {}
	Cmd(std::string &&c) : cmd(std::move(c)) {}
	std::string cmd;
	void operator()(int p) const { std::cout << "run " << cmd << "(" << p << ")\n"; }
};
bool operator<(Cmd const& pc1, Cmd const& pc2) { return pc1.cmd < pc2.cmd; };

using PrioCmd = std::pair< int, Cmd>;
using Commands = std::vector <PrioCmd>;

void addCommand(Commands & V, int prio, Cmd const& c)
{
	V.emplace_back(prio, c);
	std::push_heap(V.begin(), V.end());
}

void process(Commands & V)
{
	while (!V.empty())
	{
		auto& p = V.front();
		p.second(p.first);

		std::pop_heap(V.begin(), V.end()); // move front -> back
		V.pop_back();
	}
}

int main()
{
	std::cout << "Priority(E) with heap: -------------------------------------------\n";

	Commands V = { {-100, "A"},{ 200, "B"},{ 200, "C"}};
	V.emplace_back(-200, "D");
	std::make_heap(V.begin(), V.end());

	addCommand (V, 0, "X");
	process(V);

	// + multiple container (with random access iterator)
	// + multiple keys
	// + no expensive sort
	// - no simple add/remove => error prone
}