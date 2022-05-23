#include <iostream>
#include <vector>
#include <variant>
#include <string>
#include <type_traits>


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
	struct ValueHolder
	{
		T value_;
		explicit ValueHolder(T&& value) : value_(std::move(value)) {}
	};

	template<typename T>
	using Storage = std::variant<ValueHolder<T>, ConstReference<T>, NonConstReference<T>>;

	template<typename T>
	T const& getConstReference(Storage<T> const& storage)
	{
		return std::visit([](auto const& x)-> T const& {return x.value_; }, storage);
	}
	
	template<typename T>
	T& getReference(Storage<T>& storage)
	{
		return s std::visit([](auto& x)-> T& {return x.value_; }, storage);
	}

	///////////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	struct Value
	{
		T value_;
		explicit Value(T&& value) : value_(std::move(value)) {}
		T& get() { return value_; }
		T const& get() const { return value_; }
	};

	template<typename T>
	using Storage2 = std::variant<Value<T>, std::reference_wrapper<T const>, std::reference_wrapper<T>>;
	

	template<typename T>
	T const& getConstReference(Storage2<T> const& storage)
	{
		return std::visit([](auto const& x)-> T const& {return x.get(); }, storage);
	}

	template<typename T>
	T& getReference(Storage2<T>& storage)
	{
		if (storage.index() == 1) throw std::runtime_error{"invalid cast to 'const&'"};
		return std::visit([](auto& x)-> T& {return x.get(); }, storage);
	}
};


class X
{
public:
	explicit X(std::string& value) : storage_(jh::NonConstReference(value)) {}
	explicit X(std::string const& value) : storage_(jh::ConstReference(value)) {}
	explicit X(std::string&& value) : storage_(jh::ValueHolder(std::move(value))) {}

	void print() const 
	{	
		std::cout << jh::getConstReference(storage_) << '\n';
	}

private:
	jh::Storage<std::string> storage_;
};

class Y
{
public:
	explicit Y(std::string& value) : storage_(std::ref(value)) {}
	explicit Y(std::string const& value) : storage_(std::cref(value)) {}
	explicit Y(std::string&& value) : storage_(jh::Value(std::move(value))) {}

	void print() const 
	{ 
		std::cout << jh::getConstReference(storage_) << '\n';
	}

private:
	jh::Storage2<std::string> storage_;
};

int main(int, char**)
{

	{
		std::string s = "x1";
		X x1{ s };
		x1.print();

		X x2{ std::string{"x2"} };
		x2.print();
	}

	{
		std::string s = "y1";
		Y y1{ s };
		y1.print();

		Y y2{ std::string{"y2"} };
		y2.print();
	}


	return 0;
}

