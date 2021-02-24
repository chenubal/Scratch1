#include <iostream>
#include <vector>
#include <any>
#include <functional>

template<typename I, typename T>
auto makeGetter()
{
	using A = std::any&;
	return [](A storage)->I& { return std::any_cast<T&>(storage); };
}

template<typename Interface>
struct Implementation
{
	using I =  Interface;
public:
	template<typename T>
	Implementation(T&& object) : storage{ std::forward<T>(object) }, getter{ makeGetter<I,T>() } {}

	I* operator->() { return &getter(storage); }

private:
	std::any storage;
	std::function<I&(std::any &)> getter;
};

struct IDog
{
	virtual void barf() const = 0;
	virtual ~IDog() = default;
};

struct Chiuaua : public IDog
{
	void barf() const override { std::cout << "biff-\n"; }
};

struct Monster : public IDog
{
	void barf() const override { std::cout << "buff++\n"; }
};

using Dog = Implementation<IDog>;
using Dogs = std::vector<Dog>;

Dog createChiuaua() { return Chiuaua{}; }
Dog createMonster() { return Monster{}; }


int main()
{
	Dogs dogs{ createChiuaua(), createMonster() };
	for (auto & dog : dogs)
		dog->barf();
}
