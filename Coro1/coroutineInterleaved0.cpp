#include <coroutine>
#include <cstdio>
#include <exception>  // std::terminate
#include <iostream>
#include <new>
#include <utility>
#include <vector>

template<class G,class T>
struct pt
{
	T value{};
	G  get_return_object() { return G{ this }; }
	std::suspend_never  initial_suspend() noexcept { return {}; }
	std::suspend_always final_suspend() noexcept { return {}; }
	std::suspend_always yield_value(T v) { value = v;  return {}; }

	void unhandled_exception() {}
	void return_void() noexcept {}
};

using test_t = float;

struct Generator
{
	using promise_type = pt<Generator, test_t>;
	using handle_type = std::coroutine_handle<promise_type>;

	handle_type m_Handle{};

	explicit Generator(promise_type* p) : m_Handle{ handle_type::from_promise(*p) } {}

	Generator(Generator&& rhs) : m_Handle{ std::exchange(rhs.m_Handle, nullptr) } {}

	~Generator() { if (m_Handle) { m_Handle.destroy(); } }

	test_t value() const { return m_Handle.promise().value; }
	bool finished() const { return m_Handle.done(); }
	void resume() { if (not finished()) { m_Handle.resume(); } }

	struct sentinel {};

	struct iterator
	{
		handle_type m_Handle{};
		bool operator==(sentinel) const { return m_Handle.done(); }
		iterator& operator++()	{ m_Handle.resume(); return *this;	}
		const test_t operator*() const { return m_Handle.promise().value; }
	};

	iterator begin() { return { m_Handle }; }
	sentinel end() { return {}; }

};

Generator interleaved(std::vector<test_t> a, std::vector<test_t> b)
{
	auto makeGen = [](std::vector<test_t>& v) -> Generator
	{
		for (const auto& e : v) { co_yield e; }
	};

	auto x = makeGen(a);
	auto y = makeGen(b);

	while (not x.finished() or not y.finished()) {
		if (not x.finished()) {
			co_yield x.value();
			x.resume();
		}

		if (not y.finished()) {
			co_yield y.value();
			y.resume();
		}
	}
}

void BaseTest()
{
	std::vector< test_t> a{ 2, 4, 6, 8 };
	std::vector< test_t> b{ 3, 5, 7, 9 };

	Generator g{ interleaved(std::move(a), std::move(b)) };

	while (not g.finished()) 
	{
		std::cout << g.value() << '\n';
		g.resume();
	}
}

void IteratorTest()
{
	for (const auto& e : Generator{ interleaved( { 1, 3, 5, 7, 9 },{ 2, 4, 6, 8,10,11,12 }) })
	{ 
		std::cout << e << '\n'; 
	}
}

int main()
{
	//BaseTest();
	IteratorTest();
}
