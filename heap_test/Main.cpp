#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>

struct Cmd
{
	std::string cmd;
	Cmd(std::string const& c) : cmd(c) {}
	void operator()(int prio) const { std::cout << "run " << cmd << "(" << prio << ")\n"; }
};

struct PrioCmd
{
	std::string cmd;
	int priority;
	void operator()() const { std::cout << "run " << cmd << "(" << priority << ")\n"; }
};

int main()
{
	std::vector <PrioCmd> V = { {"A",-100},{"B",100},{"C",200},{"D",10} };

	std::map<int, Cmd> M;
	for (auto &&x : V) M.emplace(x.priority, x.cmd);

	auto pcCompare = [](PrioCmd const& pc1, PrioCmd const& pc2)->bool { return pc1.priority < pc2.priority; };

	std::cout << "Priority with map: -------------------------------------------\n";
	auto ms = M.size();
	for (auto j = 0; j < ms; j++)
	{
		auto x = M.rbegin();
		x->second(x->first);
		M.erase(x->first);
	}

	auto W = V;
	std::cout << "Priority with sort: -------------------------------------------\n";
	std::sort(W.begin(), W.end(), pcCompare);
	auto ws = W.size();
	for (auto j = 0U; j < ws; j++)
	{
		W.back()();
		W.pop_back();
	}

	std::cout << "Priority with heap: -------------------------------------------\n";
	std::make_heap(V.begin(), V.end(), pcCompare);
	auto vs = V.size();
	for (auto j = 0U; j < vs; j++)
	{
		V.front()();
		std::pop_heap(V.begin(), V.end(), pcCompare);
		V.pop_back();
	}
}