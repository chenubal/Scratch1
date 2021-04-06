#include <iostream>
#include <vector>

struct M
{
	virtual void operator()() = 0;
};
struct X : public M
{
	void operator()() override { std::cout << "X\n"; }
};
struct Y : public M
{
	void operator()() override { std::cout << "Y\n"; }
};
struct Z : public M
{
	void operator()() override { std::cout << "Z\n"; }
};


struct W
{
	virtual void paint() = 0;
	virtual M* model() = 0;
};

struct WX : public W
{
	void paint() override { std::cout << "paint WX\n"; }
	M* model() override { return (X*)(nullptr); }
};
struct WY : public W
{
	void paint() override { std::cout << "paint WY\n"; }
	M* model() override { return (Y*)(nullptr); }
};
struct WZ : public W
{
	void paint() override { std::cout << "paint WZ\n"; }
	M* model() override { return (Z*)(nullptr); }
};

W* newDialog(X const&)
{
	return new WX();
}

W* newDialog(Y const&)
{
	return new WY();
}
W* newDialog(Z const&)
{
	return new WZ();
}

int main(int, char**)
{
	using V = std::vector<std::unique_ptr<M>>;
	V v;
	v.emplace_back(std::make_unique<X>());
	v.emplace_back(std::make_unique<Y>());
	v.emplace_back(std::make_unique<Z>());
	for (auto&& x : v) (*x)();

	Y x;
	auto w = newDialog(x);
	w->paint();
	delete w;
	return 0;

}

