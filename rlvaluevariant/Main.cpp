#include <iostream>
#include <vector>
#include <variant>
#include <string>
#include <type_traits>


namespace jh
{
	template<typename T>
	struct Value
	{
		T value_;
		explicit Value(T&& value) : value_(std::move(value)) {}
		T& get() { return value_; }
		T const& get() const { return value_; }
	};

	template<typename T>
	using Storage = std::variant<Value<T>, std::reference_wrapper<T const>, std::reference_wrapper<T>>;
	

	template<typename T>
	T const& getConstReference(Storage<T> const& storage)
	{
		return std::visit([](auto const& x)-> T const& {return x.get(); }, storage);
	}

	template<typename T>
	T& getReference(Storage<T>& storage)
	{
		if (storage.index() == 1) throw std::runtime_error{"invalid cast to 'const&'"};
		return std::visit([](auto& x)-> T& {return x.get(); }, storage);
	}

	class TestClass
	{
	public:
		explicit TestClass(std::string& value) : storage_(std::ref(value)) {}
		explicit TestClass(std::string const& value) : storage_(std::cref(value)) {}
		explicit TestClass(std::string&& value) : storage_(Value(std::move(value))) {}

		void print() const { std::cout << getConstReference(storage_) << '\n'; }

	private:
		Storage<std::string> storage_;
	};
};




int main(int, char**)
{
	{
		std::string s = "y by ref";
		jh::TestClass t{ s };
		t.print();
	}
	{
		const std::string s = "y by cref";
		jh::TestClass t{ s };
		t.print();
	}
	{
		jh::TestClass t{ std::string{"y by rvalue"} };
		t.print();
	}
	return 0;
}

