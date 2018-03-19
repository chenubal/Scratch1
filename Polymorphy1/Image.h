#pragma once
#include <vector>
#include <algorithm>

namespace detail
{
	struct Size2 { size_t width, height; };
	struct Size3 { size_t width, height, depth; };
}

template<class T>
struct Image
{
	using Data = std::vector<T>;
	using Iter = typename Data::iterator;
	using CIter = typename Data::const_iterator;
	// C'tors
	Image() = delete;
	Image(size_t width, size_t height) : width(width), data(width*height, 0) { if (width*height == 0) throw; }
	Image(detail::Size2 const& s) : Image<T>(s.width,s.height) {}
	Image(Image const& o) : data(o.data), width(o.width) {}
	// STL access
	Iter begin() { return data.begin(); }
	Iter end() { return data.end(); }
	CIter cbegin() const { return data.cbegin(); }
	CIter cend() const { return data.cend(); }
	size_t size() const { return data.size(); }
	// Single access
	T operator()(size_t i, size_t j) const { return data[imap(i, j)]; }
	T& operator()(size_t i, size_t j) { return data[imap(i, j)]; }
	T operator[](size_t k) const { return data[k]; }
	T& operator[](size_t k) { return data[k]; }
	T at(size_t k) const { return data[k]; }
	// Helpers
	detail::Size2 ImageSize() const { return{ width, size() / width }; }
	template<class S=T>
	Image<S> clone() 
	{ 
		Image<S> tmp(ImageSize());	
		std::transform(cbegin(), cend(), tmp.begin(), [](T x)->S {return static_cast<S>(x); }); 
		return tmp;	
	}
private:
	size_t imap(size_t i, size_t j) { return i + j*width; }
	size_t width;
	Data data;
};

template<typename T>
std::ostream& operator<<(std::ostream& os, Image<T>& img)
{
	for (auto i = 0; i < img.size(); i++) { if (i > 0) os << ", "; os << img.at(i); }
	return os;
}

template<typename T, typename S=T>
Image<S> apply(Image<T> const& l, std::function<S(T)> f)
{
	Image<S> t(l.ImageSize());
	std::transform(l.cbegin(), l.cend(), t.begin(), f);
	return t;
}

template<typename T, typename S>
Image<T> combine(Image<T> const& l, Image<S> const& r, std::function<T(T,S)> f)
{
	Image<T> t(l);
	std::transform(l.cbegin(), l.cend(), r.cbegin(), t.begin(), f);
	return t;
}

template<typename T, typename S>
Image<T> operator+(Image<T> const& l, Image<S> const& r)
{
	return combine<T,S>(l,r, [](T x, S y)->T {return x + S(y); });
}

template<typename T, typename S>
Image<T> operator-(Image<T> const& l, Image<S> const& r)
{
	return combine<T, S>(l, r, [](T x, S y)->T {return x - S(y); });
}

template<typename T, typename S>
Image<T> operator*(Image<T> const& l, Image<S> const& r)
{
	return combine<T, S>(l, r, [](T x, S y)->T {return x * S(y); });
}
template<typename T, typename S>
Image<T> operator/(Image<T> const& l, Image<S> const& r)
{
	return combine<T, S>(l, r, [](T x, S y)->T {return x / S(y); });
}

template<class T>
struct Image3D
{
	using Data = std::vector<T>;
	using Iter = typename Data::iterator;
	using CIter = typename Data::const_iterator;
	// C'tors
	Image3D() = delete;
	Image3D(size_t width, size_t height, size_t depth) : width(width), plane(width*height), data(width*height*depth, 0) { if (width*height*depth == 0) throw; }
	Image3D(detail::Size3 const& s) : Image3D<T>(s.width, s.height, s.depth) {}
	Image3D(Image3D const& o) :  width(o.width), plane(o.plane), data(o.data) {}
	// STL access
	size_t size() const { return data.size(); }
	Iter begin() { return data.begin(); }
	Iter end() { return data.end(); }
	CIter cbegin() const { return data.cbegin(); }
	CIter cend() const { return data.cend(); }
	// Single access
	T operator()(size_t i, size_t j, size_t k) const { return data[imap(i, j, k)]; }
	T& operator()(size_t i, size_t j, size_t k) { return data[imap(i, j, k)]; }
	T operator[](size_t k) const { return data[k]; }
	T& operator[](size_t k) { return data[k]; }
	T at(size_t k) const { return data[k]; }
	// Helpers
	detail::Size3 ImageSize() const { return{ width, plane / width, size() / plane }; }
	template<class S = T>
	Image3D<S> clone()
	{
		Image3D<S> tmp(ImageSize());
		std::transform(cbegin(), cend(), tmp.begin(), [](T x)->S{return static_cast<S>(x); });
		return tmp;
	}

private:
	size_t imap(size_t i, size_t j, size_t k) { return i + j*width+k*plane; }
	size_t width,plane;
	Data data;
};

