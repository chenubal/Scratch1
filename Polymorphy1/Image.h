#pragma once
#include <vector>
#include <algorithm>

namespace detail
{
	struct Size { size_t width, height; };
}

template<class T>
struct Image
{
	using iter_t = typename std::vector<T>::iterator;
	using citer_t = typename std::vector<T>::const_iterator;

	Image() = delete;
	Image(size_t width, size_t height) : width(width), data(width*height, 0) { if (width*height == 0) throw; }
	Image(detail::Size const& s) : Image<T>(s.width,s.height) {}
	Image(Image const& o) : data(o.data), width(o.width) {}
	detail::Size ImageSize() { return{ width, size() / width }; }
	// Simple ccess
	T operator()(size_t i, size_t j) const { return data[imap(i, j)]; }
	T& operator()(size_t i, size_t j) { return data[imap(i, j)]; }
	T operator[](size_t k) const { return data[k]; }
	T& operator[](size_t k) { return data[k]; }
	T at(size_t k) const { return data[k]; }
	// STL access
	size_t size() const { return data.size(); }
	iter_t begin() { return data.begin(); }
	iter_t end() { return data.end(); }
	citer_t cbegin() const { return data.cbegin(); }
	citer_t cend() const { return data.cend(); }
	// Helpers
	template<class S=T>
	Image<S> clone() 
	{ 
		Image<S> tmp(ImageSize());	
		std::transform(cbegin(), cend(), tmp.begin(), [](T x) {return S(x); }); 
		return tmp;	
	}

private:
	size_t imap(size_t i, size_t j) { return i + j*width; }
	size_t width;
	std::vector<T> data;
};

template<typename T>
std::ostream& operator<<(std::ostream& os, Image<T>& img)
{
	for (auto i = 0; i < img.size(); i++) { if (i > 0) os << ", "; os << img.at(i); }
	return os;
}
