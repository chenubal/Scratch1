#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

struct PrioCmd
{
	PrioCmd(std::string const&c, int p) : cmd(c), priority(p) {}
	std::string cmd;
	int priority;
	void operator()() const { std::cout << "run " << cmd << "(" << priority << ")\n"; }
};
bool operator<(PrioCmd const& pc1, PrioCmd const& pc2){ return pc1.priority < pc2.priority; };

int main()
{
	std::cout << "Priority with heap: -------------------------------------------\n";

	std::vector <PrioCmd> V = { {"A",-100},{"B",100},{"C",200},{"D",100} };

	std::make_heap(V.begin(), V.end());

	V.emplace_back("X", 0);
	std::push_heap(V.begin(), V.end());

	while (!V.empty())
	{
		V.front()();

		std::pop_heap(V.begin(), V.end()); // move front -> back
		V.pop_back();
	}

	// + multiple container (with random access iterator)
	// + multiple keys
	// + no expensive sort
	// - no simple add/remove => error prone
}