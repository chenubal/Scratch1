#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <array>

template <typename T, typename Tag>
class TaggedType
{
public:
	using value_t = T;
	explicit TaggedType(T const& v) : value(v) {}
	explicit TaggedType(T&& v) : value(std::move(v)) {}
	const T& operator*() const { return value; }
private:
	T value;
};

using Row = TaggedType<size_t, struct TagRow>;
using Col = TaggedType<size_t, struct TagCol>;

////////////////////////// Square Mat /////////////////////////////////////////

template<class T>
struct SqrMat
{
	SqrMat(size_t n = 1, T init=0) : rows(n), data(n*n, init) {}

	template<class S>
	SqrMat(SqrMat<S> const& o) : SqrMat(o.size()) { *this = o; }

	size_t size() const { return rows; }

	auto begin() { return data.begin(); }
	auto end() { return data.end(); }
	auto begin() const { return data.cbegin(); }
	auto end() const { return data.cend(); }

	T& operator()(Col i, Row j) { return data[kth(i, j)]; }
	T const& operator()(Col i, Row j) const { return data[kth(i, j)]; }

	template<class S>
	SqrMat<T>& operator=(S s) { for (auto &x : data) x = T(s); return *this; }
	template<class S>
	SqrMat<T>& operator+=(S s) { for (auto &x : data) x += T(s); return *this; }
	template<class S>
	SqrMat<T>& operator*=(S s) { for (auto &x : data) x *= T(s); return *this; }

	void apply(std::function<double(double)> f) { for (auto& x : data) { x = T(f(x)); } }

	template<class S>
	SqrMat<T>& operator+=(SqrMat<S> const& m) { komp(m);  auto p = begin(); for (auto x : m) *p++ += T(x); return *this; }
	template<class S>
	SqrMat<T>& operator*=(SqrMat<S> const& m) { komp(m); auto p = begin(); for (auto x : m) *p++ *= T(x); return *this; }
	template<class S>
	SqrMat<T>& operator=(SqrMat<S> const & m) { komp(m); auto p = begin(); for (auto x : m) *p++ = T(x); return *this; }

	template<class S>
	inline void komp(SqrMat<S> const& m) { if (size()!=m.size() )throw std::invalid_argument("non matching size");}
	inline size_t kth(Col i, Row j) const { return *j * rows + *i; }
protected:
	size_t rows;
	std::vector<T> data;
};

////////////////////////// Matrix /////////////////////////////////////////

using Matrix = std::variant<SqrMat<uint8_t>, SqrMat<int>, SqrMat<float>, SqrMat<double>>;

template<typename... Functions>
struct overload : Functions...
{
	using Functions::operator()...;
	overload(Functions... functions) : Functions(functions)... {}
};

size_t size(Matrix const& m)
{
	return std::visit([](auto const& mat) { return mat.size(); }, m);
}

template<class T = double>
T get(Matrix const& m, Col i, Row j)
{
	return std::visit([=](auto const& mat) { return T(mat(i, j)); }, m);
}

void set(Matrix& m, Col i, Row j, double s)
{
	auto v = overload(
		[](auto&) {/*error*/},
		[=](SqrMat<uint8_t>& mat) { mat(i, j) = uint8_t(s); },
		[=](SqrMat<int>& mat) { mat(i, j) = int(s); },
		[=](SqrMat<float>& mat) { mat(i, j) = float(s); },
		[=](SqrMat<double>& mat) { mat(i, j) = double(s); });
	return std::visit(v, m);
}

Matrix& operator+=(Matrix& m, double s)
{
	std::visit([=](auto& mat) {mat += s; }, m);
	return m;
}

Matrix& operator*=(Matrix& m, double s)
{
	std::visit([=](auto& mat) {mat *= s; }, m);
	return m;
}

Matrix& operator+=(Matrix& m1, Matrix const& m2)
{
	auto v = [&](auto& matA)
	{
		std::visit([&](auto& matB) {matA += matB; }, m2);
	};
	std::visit(v, m1);
	return m1;
}

Matrix& operator*=(Matrix& m1, Matrix const& m2)
{
	std::visit([&](auto& matA)
	{
		std::visit([&](auto& matB) {matA *= matB; }, m2);
	}, m1);
	return m1;
}

Matrix operator+(Matrix const& m1, Matrix const& m2)
{
	Matrix mat = m1;
	return mat += m2;
}

Matrix operator*(Matrix const& m1, Matrix const& m2)
{
	Matrix mat = m1;
	return mat *= m2;
}

template<class T=double>
Matrix convert(Matrix const& m)
{
	if (!std::holds_alternative<SqrMat<T>>(m))
	{
		return std::visit([&](auto const& mat) { return SqrMat<T>(mat); }, m);
	}
	return m;
}

void execute(Matrix& m, std::function<double(double)> fun)
{
	std::visit(overload([&](auto& mat) { mat.apply(fun); }), m);
}

bool sameType(Matrix const& m1, Matrix const& m2)
{
	return m1.index() == m2.index();
}

int main()
{
	{
		std::cout << "\n-------- Size, Get, Set --------------\n";
		Matrix M1 = SqrMat<int>(5);
		std::cout << "sz1=" << size(M1) << "\n";
		std::cout << "x1=" << get(M1, Col(3), Row(3)) << "\n";
		set(M1, Col(3), Row(3), 14);
		std::cout << "x1=" << get(M1, Col(3), Row(3)) << "\n";
		M1 += 4;
		std::cout << "x1=" << get(M1, Col(3), Row(3)) << "\n";
	}

	{
		std::cout << "\n------ Ops +=, *=, +, *, convert, execute ----------------\n";
		Matrix M1 = SqrMat<int>(5);
		M1 += 2;
		std::cout << "x1=" << get(M1, Col(3), Row(3)) << "\n";

		auto M2 = convert(M1);
		M2 += 1.1;
		std::cout << "x2=" << get(M2, Col(3), Row(3)) << "\n";
		auto M3 = M2;
		M3 += M1;
		std::cout << "x3=" << get(M3, Col(3), Row(3)) << "\n";
		auto M4 = M3 + M2;
		std::cout << "x4=" << get(M4, Col(3), Row(3)) << "\n";
		auto M5 = M3 * M2;
		M5 *= 0.1;
		std::cout << "x5=" << get(M5, Col(3), Row(3)) << "\n";
		execute(M5, [](double x)->double {return x * x; });
		std::cout << "x5=" << get(M5, Col(3), Row(3)) << "\n";
	}

	{
		std::cout << "\n---- Types double + int -----\n";
		Matrix M1 = SqrMat<double>(5, 100.1);
		Matrix M2 = SqrMat<int>(5, 99);
		auto M3 = M1 + M2;
		std::cout << "m3=" << get(M3, Col(3), Row(3)) << "\n";
		auto M4 = M2 + M1;
		std::cout << "m4=" << get(M4, Col(3), Row(3)) << "\n";
	}

	{
		std::cout << "\n----Types double * int -----\n";
		Matrix M1 = SqrMat<double>(5, 100.1);
		Matrix M2 = SqrMat<int>(5, 99);
		auto M3 = M1 * M2;
		std::cout << "m3=" << get(M3, Col(3), Row(3)) << "\n";
		auto M4 = M2 * M1;
		std::cout << "m4=" << get(M4, Col(3), Row(3)) << "\n";
	}

	{
		std::cout << "\n----Types float + int -----\n";
		Matrix M1 = SqrMat<float>(5, 100.1f);
		Matrix M2 = SqrMat<int>(5, 99);
		auto M3 = M1 + M2;
		std::cout << "m3=" << get(M3, Col(3), Row(3)) << "\n";
		auto M4 = M2 + M1;
		std::cout << "m4=" << get(M4, Col(3), Row(3)) << "\n";
	}
	{
		std::cout << "\n----Types float + uint8 -----\n";
		Matrix M1 = SqrMat<float>(5, 100.1f);
		Matrix M2 = SqrMat<uint8_t>(5, 254);
		auto M3 = M1 + M2;
		std::cout << "m3=" << get(M3, Col(3), Row(3)) << "\n";
		auto M4 = M2 + M1;
		std::cout << "m4=" << get(M4, Col(3), Row(3)) << "\n";
	}
	{
		std::cout << "\n----Types float + uint8 + double -----\n";
		Matrix M1 = SqrMat<float>(5, 100.1f);
		Matrix M2 = SqrMat<uint8_t>(5, 254);
		auto M3 = SqrMat<double>(5, 700);
		try
		{
			auto M4 = M3 + M2 + M1;
			std::cout << "m4=" << get(M4, Col(3), Row(3)) << "\n";
			auto M5 = M3 + (M2 + M1);
			std::cout << "m5=" << get(M5, Col(3), Row(3)) << "\n";
		}
		catch (std::exception& e)
		{
			std::cout << e.what() << "\n";
		}
	}
}
