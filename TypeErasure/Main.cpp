

// typeErasure.cpp

#include <iostream>
#include <memory>
#include <string>
#include <vector>

class PrintObject 
{                                             

public:
	template <typename T>                              
	PrintObject(T&& obj) : object(std::make_shared<Wrapper<T>>(std::forward<T>(obj))) {}

	std::string print() const { return object->print();}

	struct Interface 
	{                                        
		virtual ~Interface() = default;
		virtual std::string print() const = 0;
	};

	template< typename T >                                   
	struct Wrapper : Interface 
	{
		Wrapper(const T& t) : object(t) {}
		std::string print() const override { return object.print();}
	private:
		T object;
	};

	std::shared_ptr<const Interface> object;
};


void print(std::vector<PrintObject> vec) 
{                   
	for (const auto& v : vec) 
		std::cout << v.print() << '\n';
}

struct Bar 
{
	std::string print() const { return "Bar"; }
};

struct Foo 
{
	std::string print() const { return s;	}
	std::string s = "Josef";
};

int main() {

	std::cout << '\n';

	Foo f{"Heers"};

	std::vector<PrintObject> vec{ PrintObject(std::move(f)), PrintObject(Bar()) };  // (1)

	print(vec);

	std::cout << '\n';

}

