#include <iostream>
#include <any>
#include <variant>
#include <memory>
#include <optional>

std::any fun_any(bool b)
{
	if (b) return 3.0;
	else return {};
}

std::variant<std::monostate, double>  fun_variant(bool b)
{
	if (b) return 4.0;
	else return {};
}

std::unique_ptr<double> fun_unique_ptr(bool b)
{
	if (b) return std::make_unique<double>(5.0);
	else return {};
}

std::optional<double> fun_optional(bool b)
{
	if (b) return 6.0;
	else return {};
}

int main()
{
	// Switchable test suite
	auto  test_suite = [](bool flag)
	{
		// Some printer
		auto print_none = []() { std::cout << "none\n"; };
		auto print_value = [](auto v) {  std::cout << "v=" << v << "\n"; };
		// Run tests
		if (auto x = fun_any(flag); x.has_value())
			print_value(std::any_cast<double>(x));
		else
			print_none();

		if (auto x = fun_variant(flag); std::holds_alternative<double>(x))
			print_value(std::get<double>(x));
		else
			print_none();

		if (auto x = fun_unique_ptr(flag))
			print_value(*x);
		else
			print_none();

		if (auto x = fun_optional(flag))
			print_value(*x);
		else
			print_none();
	};
	// Run test suites
	test_suite(true);
	test_suite(false);
}
