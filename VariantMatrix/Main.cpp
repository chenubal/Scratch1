#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <array>
#include <utility>
#include <xutility>
#include <fstream>
#include <optional>


namespace tools
{
	template<typename... Functions>
	struct overload : Functions...
	{
		using Functions::operator()...;
		overload(Functions... functions) : Functions(functions)... {}
	};

	template <typename... T>
	struct combine
	{
		class iterator : std::iterator<std::forward_iterator_tag, std::tuple<decltype(*std::declval<T>().begin())...>>
		{
		private:
			std::tuple<decltype(std::declval<T>().begin())...> iterator_pack;
			template <std::size_t... I>
			auto deref(std::index_sequence<I...>) const
			{
				return typename iterator::value_type{ *std::get<I>(iterator_pack)... };
			}
			template <std::size_t... I>
			void increment(std::index_sequence<I...>)
			{
				auto l = { (++std::get<I>(iterator_pack), 0)... };
			}
		public:
			explicit iterator(decltype(iterator_pack) iters) : iterator_pack{ std::move(iters) } {}
			iterator& operator++()
			{
				increment(std::index_sequence_for<T...>{});
				return *this;
			}

			iterator operator++(int)
			{
				auto saved{ *this };
				increment(std::index_sequence_for<T...>{});
				return saved;
			}
			bool operator!=(const iterator& other) const
			{
				return iterator_pack != other.iterator_pack;
			}
			const auto operator*() const { return deref(std::index_sequence_for<T...>{}); }
			auto operator*() { return deref(std::index_sequence_for<T...>{}); }
		};

		combine(T&... seqs) : begin_{ std::make_tuple(seqs.begin()...) }, end_{ std::make_tuple(seqs.end()...) } {}

		const iterator begin() const { return begin_; }
		const iterator end() const { return end_; }
		iterator begin() { return begin_; }
		iterator end() { return end_; }

	private:
		iterator begin_;
		iterator end_;
	};

	// User CTAD
	template<typename ...T>
	combine(T&&...)->combine<T...>;
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

}



using Row = tools::TaggedType<size_t, struct TagRow>;
using Col = tools::TaggedType<size_t, struct TagCol>;
using MonadicFun = std::function<double(double)>;
using DyadicFun = std::function<double(double, double)>;

////////////////////////// Simple Mat /////////////////////////////////////////

template<class T>
struct Mat
{
	using value_type = T;

	Mat(size_t n = 0, size_t m = 0, T init=0) : numRows(n), values(n*m, init) {}

	template<class S>
	Mat(Mat<S> const& o) : Mat(o.rows(),o.cols()) { *this = o; }

	size_t size() const { return values.size(); }
	size_t rows() const { return numRows; }
	size_t cols() const { return size()/rows(); }
	auto dims() const { return std::make_pair( cols(), rows() ); }

	auto begin() { return values.begin(); }
	auto end() { return values.end(); }
	auto begin() const { return values.cbegin(); }
	auto end() const { return values.cend(); }
	const auto raw() const { return values.data(); }

	T& operator()(Col i, Row j) { return values[kth(i, j)]; }
	T const& operator()(Col i, Row j) const { return values[kth(i, j)]; }

	template<class S>
	Mat<T>& operator=(S s) { for (auto &x : values) x = T(s); return *this; }
	template<class S>
	Mat<T>& operator+=(S s) { for (auto &x : values) x += T(s); return *this; }
	template<class S>
	Mat<T>& operator*=(S s) { for (auto &x : values) x *= T(s); return *this; }

	void apply(MonadicFun f) { for (auto& x : values) { x = T(f(x)); } }

	template<class S>
	void apply(Mat<S> const& m, DyadicFun f) { for (auto&& [x,y] : tools::combine(*this,m)) { x = T(f(x, y)); } }

	template<class S>
	Mat<T>& operator+=(Mat<S> const& m) { checkDims(m); for (auto&&[x, y] : tools::combine(*this, m)) x += T(y); return *this; }
	template<class S>
	Mat<T>& operator*=(Mat<S> const& m) { checkDims(m); for (auto&&[x, y] : tools::combine(*this, m)) x *= T(y); return *this; }
	template<class S>
	Mat<T>& operator=(Mat<S> const & m) { checkDims(m); for (auto&&[x, y] : tools::combine(*this, m)) x = T(y); return *this; }

	template<class S>
	inline void checkDims(Mat<S> const& m)  { if (dims() != m.dims()) throw std::invalid_argument("non matching size"); }

	inline size_t kth(Col i, Row j) const { return *j * rows() + *i; }
protected:
	size_t numRows;
	std::vector<T> values;
};

struct alignas(128) fheader 
{ 
	fheader(size_t n=0, size_t m=0, size_t k=0, size_t t=0) : cols(n), rows(m), numBytes(k), type(k) {}
	size_t magic = 31415;
	size_t cols = 0;
	size_t rows = 0; 
	size_t numBytes = 0;
	size_t type = 0;
	size_t start = sizeof(fheader);
};

template< class T>
bool writeBin(Mat<T>const& m, std::string const& path, size_t t)
{
	auto done = false;
	if (auto os = std::ofstream(path, std::ios::binary))
	{
		fheader h(m.cols(), m.rows(), m.size() * sizeof(T), t);
		os.write(reinterpret_cast<char const*>(&h), sizeof h);
		os.write(reinterpret_cast<char const*>(m.raw()), h.numBytes);
		done = true;
	}
	return done;
}


std::optional<fheader> readHeader(std::ifstream & os)
{
	if (os)
	{
		fheader h;
		os.read(reinterpret_cast<char*>(&h),sizeof h);
		if (h.magic == 31415)
			return h;
	}
	return {};
}
void readData(std::ifstream & os, char* p, size_t n)
{
	if (os)	os.read(p, n);
}


////////////////////////// Matrix /////////////////////////////////////////


using Matrix = std::variant<Mat<uint8_t>, Mat<uint16_t>, Mat<int>, Mat<float>, Mat<double>>;


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
	static auto v = tools::overload(
		[](auto&) {/*error*/},
		[=](Mat<uint8_t>& mat) { mat(i, j) = uint8_t(s); },
		[=](Mat<uint16_t>& mat) { mat(i, j) = uint16_t(s); },
		[=](Mat<int>& mat) { mat(i, j) = int(s); },
		[=](Mat<float>& mat) { mat(i, j) = float(s); },
		[=](Mat<double>& mat) { mat(i, j) = double(s); });
	return std::visit(v, m);
}

Matrix& operator*=(Matrix& m, double s)
{
	return std::visit([&](auto& mat)->Matrix& {mat *= s; return m; }, m);
}

Matrix& operator+=(Matrix& m, double s)
{
	return std::visit([&](auto& mat)->Matrix& {mat += s; return m; }, m);
}

Matrix& operator+=(Matrix& m1, Matrix const& m2)
{
	std::visit([&](auto& matA, auto const& matB) {matA += matB; }, m1, m2);
	return m1;
}

Matrix& operator*=(Matrix& m1, Matrix const& m2)
{
	std::visit([&](auto& matA, auto const& matB) {matA *= matB; }, m1, m2);
	return m1;
}

Matrix operator*(Matrix const& m, double s)
{
	return Matrix(m) *= s;
}

Matrix operator*(double s, Matrix const& m)
{
	return  m * s;
}

Matrix operator+(Matrix const& m, double s)
{
	return Matrix(m) += s;
}

Matrix operator+(double s, Matrix const& m)
{
	return  m+s;
}

Matrix operator+(Matrix const& m1, Matrix const& m2)
{
	return Matrix(m1) += m2;
}

Matrix operator*(Matrix const& m1, Matrix const& m2)
{
	return Matrix(m1) *= m2;
}

template<class T=double>
Matrix convert(Matrix const& m)
{
	if (!std::holds_alternative<Mat<T>>(m))
	{
		return std::visit([&](auto const& mat) { return Mat<T>(mat); }, m);
	}
	return m;
}

// M = f(M)
void execute(Matrix& m,MonadicFun f)
{
	std::visit([&](auto& mat) { mat.apply(f); }, m);
}

// M1 = f(M1,M2);
void execute(Matrix& m1, Matrix const& m2, DyadicFun f)
{
	std::visit([&](auto& mat1, auto const& mat2) { mat1.apply(mat2,f); }, m1, m2);
}

// M = f(M1,M2);
Matrix eval(Matrix const& m1, Matrix const& m2, DyadicFun f)
{
	Matrix m = m1;
	std::visit([&](auto& mat1, auto const& mat2) { mat1.apply(mat2, f); }, m, m2);
	return m;
}

bool sameType(Matrix const& m1, Matrix const& m2)
{
	return m1.index() == m2.index();
}

bool writeBin(Matrix const& m, std::string const& p)
{
	size_t t = m.index();
	return std::visit([&](auto const& mat)->bool {return writeBin(mat, p,t); }, m);
}

std::optional<Matrix> readBin(std::string const& p)
{
	if (auto os = std::ifstream(p, std::iostream::binary))
	{
		if (auto oh = readHeader(os))
		{
			const auto t = oh->type;
			//TODO
		}
	}
	return {};
}


int main()
{
	auto runTest = [](char const* s, auto&& f) {std::cout << "Testing " << s << "\n"; f(); };

	Col c3(3);
	Row r3(3);

	runTest("Size, Get, Set",[&]
	{
		Matrix M = Mat<int>(5,5);

		std::cout << "sz1=" << size(M) << "\n";

		std::cout << "x1=" << get(M, c3, r3) << "\n";

		set(M, c3, r3, 14);
		std::cout << "x1=" << get(M, c3, r3) << "\n";

		M += 4;
		std::cout << "x1=" << get(M, c3, r3) << "\n";
	});

	runTest("Ops +=, *=, +, *, convert, execute", [&]
	{
		Matrix M1 = Mat<int>(5,5);
		
		M1 += 2;
		std::cout << "x1=" << get(M1, c3, r3) << "\n";

		auto M2 = convert(M1);
		M2 += 1.1;
		std::cout << "x2=" << get(M2, c3, r3) << "\n";
		
		auto M3 = M2;
		
		M3 += M1;
		std::cout << "x3=" << get(M3, c3, r3) << "\n";
		
		auto M4 = M3 + M2;
		std::cout << "x4=" << get(M4, c3, r3) << "\n";
		
		auto M5 = M3 * M2;
		M5 *= 0.1;
		std::cout << "x5=" << get(M5, c3, r3) << "\n";
		
		execute(M5, [](double x)->double {return x * x; });
		std::cout << "x5=" << get(M5, c3, r3) << "\n";
	});

	runTest("Types double + int", [&]
	{
		Matrix M1 = Mat<double>(5,5, 100.1);
		Matrix M2 = Mat<int>(5,5, 99);
		
		auto M3 = M1 + M2;
		std::cout << "m3=" << get(M3, c3, r3) << "\n";
		
		auto M4 = M2 + M1;
		std::cout << "m4=" << get(M4, c3, r3) << "\n";
	});

	runTest("Types double * int", [&]
	{
		Matrix M1 = Mat<double>(5,5, 100.1);
		Matrix M2 = Mat<int>(5, 5,99);
		
		auto M3 = M1 * M2;
		std::cout << "m3=" << get(M3, c3, r3) << "\n";
		
		auto M4 = M2 * M1;
		std::cout << "m4=" << get(M4, c3, r3) << "\n";
	});

	runTest("Types float + int", [&]
	{
		Matrix M1 = Mat<float>(5, 5,100.1f);
		Matrix M2 = Mat<int>(5,5, 99);
		
		auto M3 = M1 + M2;
		std::cout << "m3=" << get(M3, c3, r3) << "\n";
		
		auto M4 = M2 + M1;
		std::cout << "m4=" << get(M4, c3, r3) << "\n";
	});

	runTest("Types float + uint8", [&]
	{
		Matrix M1 = Mat<float>(5, 5, 100.1f);
		Matrix M2 = Mat<uint8_t>(5, 5,254);
		
		auto M3 = M1 + M2;
		std::cout << "m3=" << get(M3, c3, r3) << "\n";

		auto M4 = M2 + M1;
		std::cout << "m4=" << get(M4, c3, r3) << "\n";
	});

	runTest("Types float + uint8 + double", [&]
	{
		Matrix M1 = Mat<float>(5,5, 100.1f);
		Matrix M2 = Mat<uint8_t>(5,5, 254);
		auto M3 = Mat<double>(5,5, 700);
		try
		{
			auto M4 = M3 + M2 + M1;
			std::cout << "m4=" << get(M4, c3, r3) << "\n";

			auto M5 = M3 + (M2 + M1);
			std::cout << "m5=" << get(M5, c3, r3) << "\n";
		}
		catch (std::exception& e)
		{
			std::cout << e.what() << "\n";
		}
	});

	runTest("execute(M1,M2,f)", [&]
	{
		float a = 200.1f;
		uint16_t b = 55, n=5,m=5;

		Matrix M1 = Mat<float>(n,m, a);
		Matrix M2 = Mat<uint16_t>(n,m, b);

		auto f = [](double a, double b)->double { return 5.0 * a + 2.0*b; };
		auto M3 = eval(M1, M2, f);
		std::cout << f(a, b) << "==" << double(get(M3, c3, r3)) << "\n";
		std::cout << std::boolalpha << (float(f(a, b)) == get(M3, c3, r3)) << "\n";
	});

	runTest("writeBin", []
	{
		Matrix M1 = Mat<double>(5, 5, 20);
		writeBin(M1, "mat1.bin");

	});
}
