#include <iostream>
#include <string>
#include <vector>
#include <functional>

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
	for (auto& el : vec) { os << el << ' '; }
	return os;
}

template<class T, class F = std::function<void(T*)> >
struct sandbox
{
	sandbox(T* c, F st = nullptr, F td = nullptr) : client(c), setup(st), teardown(td) { run(setup); }
	sandbox(T& c, F st = nullptr, F td = nullptr) : sandbox(&c, st, td) {}
	~sandbox() { run(teardown); }
	const T* operator->() const { return client; }
	T* operator->() { return client; }
	const T& operator*() const { return *client; }
	T& operator*() { return *client; }
private:
	void run(F f) { if (f) f(client); }
	T* client = nullptr;
	F setup, teardown;
};

// CTAD
template<typename T, typename F>
sandbox(T*, F)->sandbox<T>;
template<typename T, typename F, typename G>
sandbox(T*, F, G)->sandbox<T>;
template<typename T, typename F>
sandbox(T&, F)->sandbox<T>;
template<typename T, typename F, typename G>
sandbox(T&, F, G)->sandbox<T>;

int main()
{
	std::vector<std::string> vec = { "Hello", "from", "GCC",  "!" };
	auto td = [&](auto c) { std::cout << c->size() << "\n"; };
	auto sb = sandbox(vec, td, [&](auto c) { std::cout << c->size() << "\n"; });
	std::cout << *sb << std::endl;
	sb->clear();

}
