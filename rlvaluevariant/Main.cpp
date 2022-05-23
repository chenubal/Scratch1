#include <iostream>
#include <vector>
#include <variant>
#include <string>


namespace jh
{
	template<typename T>
	struct NonConstReference
	{
		T& value_;
		explicit NonConstReference(T& value) : value_(value) {};
	};

	template<typename T>
	struct ConstReference
	{
		T const& value_;
		explicit ConstReference(T const& value) : value_(value) {};
	};

	template<typename T>
	struct Value
	{
		T value_;
		explicit Value(T&& value) : value_(std::move(value)) {}
	};

	template<typename T>
	using Storage = std::variant<Value<T>, ConstReference<T>, NonConstReference<T>>;

	template<typename... Functions>
	struct overload : Functions...
	{
		using Functions::operator()...;
		overload(Functions... functions) : Functions(functions)... {}
	};

	template<typename T>
	T const& getConstReference(Storage<T> const& storage)
	{
		return std::visit(
			overload(
				[](Value<T> const& value) -> T const& { return value.value_; },
				[](NonConstReference<T> const& value) -> T const& { return value.value_; },
				[](ConstReference<T> const& value) -> T const& { return value.value_; }
			),
			storage
		);
	}
	
	template<typename T>
	T& getReference(Storage<T>& storage)
	{
		return std::visit(
			overload(
				[](Value<T>& value) -> T& { return value.value_; },
				[](NonConstReference<T>& value) -> T& { return value.value_; },
				[](ConstReference<T>&) -> T& { throw std::runtime_error{ "Cannot get a non const reference from a const reference" }; }
			),
			storage
		);
	}
};


class X
{
public:
	explicit X(std::string& value) : storage_(jh::NonConstReference(value)) {}
	explicit X(std::string const& value) : storage_(jh::ConstReference(value)) {}
	explicit X(std::string&& value) : storage_(jh::Value(std::move(value))) {}

	void print() const {	std::cout << jh::getConstReference(storage_) << '\n';	}

private:
	jh::Storage<std::string> storage_;
};



int main(int, char**)
{

	std::string s = "x1";
	X x1{ s };
	x1.print();

	X x2{ std::string{"x2"} };
	x2.print();
	return 0;

}

