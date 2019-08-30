#include <iostream>
#include <vector>


template<class F, class ... Args>
void print(F f, Args&& ...args)
{
	std::cout << f(std::forward<Args>(args)...) << "\n";
}

struct TestOp
{
	TestOp(int i = 0) :i(i) {}
	int i;
	int operator()(int k = 1) { return k * (++i); }
};

struct staticTestOp
{
	static int i;
	int operator()(int k=1) {return k * (++i); }
};

int staticTestOp::i = 0;

int main(int, char**)
{
	staticTestOp sop1, sop2;
	TestOp op(5);

	print([]() {return 111; });
	print([](int i) {return i * 10; }, 5);
	print(sop1, 10);
	print(sop2, 10);
	print(sop1, 10);
	print(sop2);
	print(op);
	print(op);
	return 0;
}

