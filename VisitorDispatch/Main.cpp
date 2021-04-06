#include <iostream>
#include <string>
#include <vector>
#include <variant>

using std::string;

struct Model 
{
	virtual string name() const = 0;
};
struct ModelA : Model 
{
	string name() const override { return "Model A"; }
};
struct ModelB : Model 
{
	string name() const override { return "Model B"; }
};

struct ViewCreator
{
	void RunAwayFrom(Model const& model) { std::cout << "Run Away From " << model.name() << std::endl; }
	void TryToPet(Model const& model) { std::cout << "Try To Pet " << model.name() << std::endl; }
};

struct ReactVisitor 
{
	void operator()(ModelA *c) { creator->TryToPet(*c); }
	void operator()(ModelB *d) { creator->RunAwayFrom(*d); }
	ViewCreator *creator = nullptr;
};

using ModelsPtr = std::variant<ModelA*, ModelB*>;


int main() {
	ViewCreator p;
	ReactVisitor rv{ &p };
	for (auto&& animal : std::vector<ModelsPtr>({ new ModelB, new ModelA }))
		std::visit(rv, animal);
	return 0;
}