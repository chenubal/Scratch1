#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <functional>
#include <string>
#include <memory>


struct ILoader
{
	virtual void load(std::string const&) = 0;
	virtual void store(std::string const&) = 0;
	virtual ~ILoader() {}
};

template<class T>
struct Loader : public ILoader{};

template<>
struct Loader<float> : public ILoader
{
	Loader() { std::cout << "Loader float\n"; }
	void load(std::string const& s)
	{
		if (auto ifs = std::ifstream(s))
		{
			int n;
			std::string str;
			ifs >> n >> str;
			std::cout << str << "|" << n << "\n";
		}
	}
	void store(std::string const& s)
	{
		if (auto ofs = std::ofstream(s)) { ofs << "1\nfloat"; }
	}
};

int loadHeader(std::string const& s)
{
	if (auto ifs = std::ifstream(s))
	{
		int n;
		std::string str;
		ifs >> n >> str;
		return n;
	}
	return -1;
}

template<>
struct Loader<double> : public ILoader
{
	Loader() { std::cout << "Loader double\n"; }
	void load(std::string const& s)
	{
		if (auto ifs = std::ifstream(s))
		{
			int n;
			std::string str;
			ifs >> n >> str;
			std::cout << str << "|" << n << "\n";
		}
	}
	void store(std::string const& s)
	{
		if (auto ofs = std::ofstream(s)) { ofs << "2\ndouble"; }
	}
};
template<>
struct Loader<short> : public ILoader
{
	Loader() { std::cout << "Loader short\n"; }
	void load(std::string const& s)
	{
		if (auto ifs = std::ifstream(s))
		{
			int n;
			std::string str;
			ifs >> n >> str;
			std::cout << str << "|" << n << "\n";
		}
	}
	void store(std::string const& s)
	{
		if (auto ofs = std::ofstream(s)) { ofs << "0\nshort"; }
	}
};

struct IBuffer
{
	virtual void load(std::string const&) = 0;
	virtual void store(std::string const&) = 0;
	virtual ~IBuffer() {}
};

template<class T>
struct Buffer : IBuffer
{
	Buffer() {}
	
	void load(std::string const& s) { loader.load(s); }
	void store(std::string const& s) { loader.store(s); }

	Loader<T> loader;
};


using cmap_t = std::map<unsigned, std::function<IBuffer*()>>;
cmap_t createMap()
{
	return{ 
			{ 0,[]() {return new Buffer<short>(); } },
			{ 1,[]() {return new Buffer<float>(); } },
			{ 2,[]() {return new Buffer<double>();} } };
}


IBuffer* createBuffer(unsigned i)
{
	auto map = createMap();
	if (map.count(i))
		return map[i]();
	else
		return new Buffer<short>();
}

int main()
{
	std::cout << "Type n: ";
	unsigned n; std::cin >> n;
	auto f = createBuffer(n);
	f->store("myfile.im7");
	delete f;
	auto m = loadHeader("myfile.im7");
	auto g = createBuffer(m);
	delete g;
}
