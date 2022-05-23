#include <iostream>
#include <vector>
#include <variant>
#include <string>
#include <type_traits>
#include <functional>


namespace jh
{
	template<typename T>
	using RW = std::reference_wrapper<T>;
	template<typename T>
	using RWC = std::reference_wrapper<T const>;

	template<typename T>
	struct Value
	{
		T data;
		explicit Value(T&& v) : data(std::move(v)) {}
	};

	template<typename T>
	using Storage = std::variant<Value<T>, RWC<T>, RW<T>>;

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
				[](Value<T> const& x) -> T const& { return x.data; },
				[](RW<T> const& x) -> T const& { return x.get(); },
				[](RWC<T> const& x) -> T const& { return x.get(); }
			),	storage);
	}

	template<typename T>
	T& getReference(Storage<T>& storage)
	{
		return std::visit(
			overload(
				[](Value<T>& x) -> T& { return x.data; },
				[](RW<T>& x) -> T& { return x.get(); },
				[](RWC<T>&)->T& { throw std::runtime_error{ "invalid conversion" }; }
		),storage);
	}

	template<typename T = std::string>
	struct TestClass
	{
		explicit TestClass(T& value) : storage(std::ref(value)) {}
		explicit TestClass(T const& value) : storage(std::cref(value)) {}
		explicit TestClass(T&& value) : storage(Value(std::move(value))) {}

		void print() const { std::cout << getConstReference(storage) << '\n'; }
		T& data() { return getReference(storage); }
		T const& data() const { return getConstReference(storage); }

		Storage<T> storage;
	};
};

int main(int, char**)
{
	{
		std::string s = "y by ref";
		jh::TestClass t{ s };
		t.print();
		s += " changed";
		t.print();
		auto& r =t.data();
		r.append(" edited");
		std::cout << s << "\n";
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

