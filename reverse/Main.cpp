#include <iostream>
#include <string>
#include <vector>
#include <type_traits>
#include <variant>

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
		), storage);
	}

	template<typename T>
	T& getReference(Storage<T>& storage)
	{
		return std::visit(
			overload(
				[](Value<T>& x) -> T& { return x.data; },
				[](RW<T>& x) -> T& { return x.get(); },
				[](RWC<T>&)->T& { throw std::runtime_error{ "invalid conversion" }; }
		), storage);
	}

	template <typename T>
	struct reverse
	{
		explicit reverse(T& value) : storage(std::ref(value)) {}
		explicit reverse(T const& value) : storage(std::cref(value)) {}
		explicit reverse(T&& value) : storage(Value(std::move(value))) {}

		T& data() { return getReference(storage); }
		T const& data() const { return getConstReference(storage); }

		auto begin() { return std::rbegin(data()); }
		auto end() { return std::rend(data()); }
		auto begin() const {	return std::rbegin(data());}
		auto end() const { return std::rend(data()); }
	private:
		Storage<T> storage;
	};
}


int main()
{
	using Ints = std::vector<int>;

	auto print = [](auto const& container) {for (auto const& v : container)	std::cout << v << ", ";	std::cout << std::endl; };
	auto print_r = [&](auto const& container) { std::cout << "R: "; print(jh::reverse(container));};

	{
		std::cout << "lvalue\n";
		Ints values = { 1,2,3,4,5 };

		for (auto && v : values) v += 10;
		print(values);

		for (auto && v : jh::reverse(values)) v += 10;
		print_r(values);
	}

	{
		std::cout << "lvalue const\n";
		const Ints values = { 1,2,3,4,5 };
		print(values);
		print_r(values);
	}

	{
		std::cout << "rvalue\n";
		print_r(Ints{ 270, 280, 290 });
		print_r(std::initializer_list<int>{ 770, 880, 990 });

		for (auto const& v : jh::reverse(std::initializer_list<int>{ 70, 80, 90 }))
			std::cout << v << ", ";
		std::cout << std::endl;

		for (auto const& v : jh::reverse(Ints{ 370, 380, 390 }))
			std::cout << v << ", ";
		std::cout << std::endl;
	}

}
