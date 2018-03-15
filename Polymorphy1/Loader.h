#pragma once
#include <iostream>
#include <fstream>
#include <string>

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

void loaddummy(std::string const& s)
{
	if (auto ifs = std::ifstream(s))
	{
		int n;
		std::string str;
		ifs >> n >> str;
		std::cout << str << "|" << n << "\n";
	}
}

struct ILoader
{
	virtual void load(std::string const&) = 0;
	virtual void store(std::string const&) = 0;
	virtual ~ILoader() {}
};

template<class T>
struct Loader : public ILoader {};

template<>
struct Loader<float> : public ILoader
{
	Loader() { std::cout << "Loader float\n"; }
	void load(std::string const& s)	{loaddummy(s);}
	void store(std::string const& s) {if (auto ofs = std::ofstream(s)) ofs << "1\nfloat"; }
};

template<>
struct Loader<double> : public ILoader
{
	Loader() { std::cout << "Loader double\n"; }
	void load(std::string const& s) { loaddummy(s); }
	void store(std::string const& s) { if (auto ofs = std::ofstream(s)) ofs << "2\ndouble"; }
};

template<>
struct Loader<short> : public ILoader
{
	Loader() { std::cout << "Loader short\n"; }
	void load(std::string const& s) { loaddummy(s); }
	void store(std::string const& s) { if (auto ofs = std::ofstream(s))  ofs << "0\nshort"; }
};

