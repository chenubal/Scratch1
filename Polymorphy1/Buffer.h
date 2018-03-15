#pragma once
#include <map>
#include <functional>
#include <string>
#include <memory>
#include "Loader.h"

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
	 
using bcmap_t = std::map<unsigned, std::function<IBuffer*()>>;

bcmap_t bufferCreatorMap()
{
	return{
			{ 0,[]() {return new Buffer<short>(); } },
			{ 1,[]() {return new Buffer<float>(); } },
			{ 2,[]() {return new Buffer<double>(); } } };
}

IBuffer* createBuffer(unsigned i)
{
	auto map = bufferCreatorMap();
	if (map.count(i))
		return map[i]();
	else
		return new Buffer<short>();
}