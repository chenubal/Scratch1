#include <iostream>
#include <vector>
#include <memory>

struct AbstractPrinter
{
	virtual void print() = 0;
	virtual ~AbstractPrinter() = default;
};

class Printer : public AbstractPrinter
{
public:
	Printer(int i = 0) : i(i) { std::cout << "create X\n"; }
	~Printer() { std::cout << "delete X\n"; }
	void print() override { std::cout << "print X: " << i << "\n"; }
private:
	const int i;
};

using SA = std::unique_ptr<AbstractPrinter>;

int main()
{
	std::vector<SA> printers;

	printers.emplace_back(new Printer(10));
	printers.emplace_back(new Printer(20));
	printers.emplace_back(new Printer(30));
	printers.emplace_back(new Printer(40));
	printers.emplace_back(nullptr);
	std::cout << "#Printer = " << printers.size() << "\n";
	std::swap(printers[0], printers[2]);

	for (auto& p : printers)
		if (p) p->print();
}
