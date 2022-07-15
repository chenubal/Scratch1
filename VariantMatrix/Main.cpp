#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <algorithm>
#include <functional>

template<typename... Functions>
struct overload : Functions...
{
	using Functions::operator()...;
	overload(Functions... functions) : Functions(functions)... {}
};

template<class T>
struct Mat
{
	Mat(size_t n = 1) : rows(n), data(n*n, T()) {}

	size_t size() const { return rows; }

	auto begin() { return data.begin(); }
	auto end() { return data.end(); }
	auto begin() const { return data.cbegin(); }
	auto end() const { return data.cend(); }

	T& operator()(size_t i, size_t j) { return data[kth(i, j)]; }
	T const& operator()(size_t i, size_t j) const { return data[kth(i, j)]; }

	template<class S>
	Mat<T>& operator=(S s) { for (auto &x : data) x = T(s); return *this; }
	template<class S>
	Mat<T>& operator+=(S s) { for (auto &x : data) x += T(s); return *this; }
	template<class S>
	Mat<T>& operator*=(S s) { for (auto &x : data) x *= T(s); return *this; }

	void apply(std::function<double(double)> f) { for (auto& x : data) { x = T(f(x)); } }

	template<class S>
	Mat<T>& operator+=(Mat<S> const& m) { auto p = begin(); for (auto x : m) *p++ += T(x); return *this; }
	template<class S>
	Mat<T>& operator*=(Mat<S> const& m) { auto p = begin(); for (auto x : m) *p++ *= T(x); return *this; }
	template<class S>
	Mat<T>& operator=(Mat<S> const & m) {auto p = begin(); for (auto x : m) *p++ = T(x); return *this; }


	inline size_t kth(size_t i, size_t j) const { return j * rows + i; }
protected:
	size_t rows;
	std::vector<T> data;
};


////////////////////////// Matrix /////////////////////////////////////////

using Matrix = std::variant<Mat<uint8_t>, Mat<int>, Mat<float>, Mat<double>>;

size_t size(Matrix const& m)
{
	return std::visit(overload([](auto const& mt) { return mt.size(); }), m);
}

template<class T = double>
T get(Matrix const& m, size_t i, size_t j)
{
	return std::visit(overload([=](auto const& mt) { return T(mt(i, j)); }), m);
}

void set(Matrix& m, size_t i, size_t j, double v)
{
	auto op = overload(
		[](auto&) {/*error*/},
		[=](Mat<uint8_t>& mt) {mt(i, j) = uint8_t(v); },
		[=](Mat<int>& mt) {mt(i, j) = int(v); },
		[=](Mat<float>& mt) {mt(i, j) = float(v); },
		[=](Mat<double>& mt) {mt(i, j) = double(v); });
	return std::visit(op, m);
}


Matrix& operator+=(Matrix& m, double v)
{
	auto op = overload([=](auto& mt) {mt += v; });
	std::visit(op, m);
	return m;
}

Matrix& operator*=(Matrix& m, double v)
{
	auto op = overload([=](auto& mt) {mt *= v; });
	std::visit(op, m);
	return m;
}

Matrix& operator+=(Matrix& m1, Matrix const& m2)
{
	auto outer = overload([&](auto& mo)
	{
		auto inner = overload([&](auto& mi) {mo += mi; });
		std::visit(inner, m2);
	});
	std::visit(outer, m1);
	return m1;
}

Matrix& operator*=(Matrix& m1, Matrix const& m2)
{
	auto outer = overload([&](auto& mo)
	{
		auto inner = overload([&](auto& mi) {mo *= mi; });
		std::visit(inner, m2);
	});
	std::visit(outer, m1);
	return m1;
}

Matrix operator+(Matrix const& m1, Matrix const& m2)
{
	Matrix t = m1;
	return t += m2;
}

Matrix operator*(Matrix const& m1, Matrix const& m2)
{
	Matrix t = m1;
	return t *= m2;
}

Matrix toDouble(Matrix t)
{
	if (!std::holds_alternative<Mat<double>>(t))
	{
		Mat<double> tmp;
		std::visit([&](auto m) { tmp = m; }, t);
		return tmp;
	}
	return t;
}

void execute(Matrix& m1, std::function<double(double)> fun)
{
	auto outer = overload([&](auto& mo){ mo.apply(fun); });
	std::visit(outer, m1);
}

int main()
{
	auto dmat1 = Mat<double>(5);
	dmat1 = 10.3;
	dmat1 *= 2;
	dmat1 += 2;

	Matrix M1 = dmat1;
	std::cout << "sz1=" << size(M1) << "\n";
	std::cout << "x1=" << get(M1, 3, 3) << "\n";
	set(M1, 3, 3, 14.5);
	std::cout << "x1=" << get(M1, 3, 3) << "\n";
	M1 += 4.5;
	std::cout << "x1=" << get(M1, 3, 3) << "\n";

	auto M2 = toDouble(M1);
	M2 += 1.1;
	std::cout << "x2=" << get(M2, 3, 3) << "\n";
	auto M3 = M2;
	M3 += M1;
	std::cout << "x3=" << get(M3, 3, 3) << "\n"; 
	auto M4 = M3 + M1;
	std::cout << "x4=" << get(M4, 3, 3) << "\n";
	auto M5 = M3 * M1;
	M5 *= 0.1;
	std::cout << "x5=" << get(M5, 3, 3) << "\n";
	execute(M5, [](double x)->double {return x * x; });
	std::cout << "x5=" << get(M5, 3, 3) << "\n";
}
