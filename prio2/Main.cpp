#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

struct PrioCmd
{
	std::string cmd;
	int priority;
	void operator()() const { std::cout << "run " << cmd << "(" << priority << ")\n"; }
};

bool operator<(PrioCmd const& pc1, PrioCmd const& pc2) { return pc1.priority < pc2.priority; }

int main()
{
	std::cout << "Priority with sort: -------------------------------------------\n";

	std::vector <PrioCmd> V = { {"A",-100},{"B",100},{"C",200},{"D",10} };
	if (auto f = std::find_if(V.begin(), V.end(), [](PrioCmd const& c)->bool {return c.priority == 10; });f != V.end())
	{
		std::cout << f->priority << "\n";
	}

	std::sort(V.begin(), V.end());
	while (!V.empty())
	{
		V.back()();
		V.pop_back();
	}

	// + multiple container
	// + multiple keys
	// - inverse modeling
	// - expensive sort
}