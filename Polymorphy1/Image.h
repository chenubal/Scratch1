#pragma once
#include <vector>
#include <algorithm>
#include <iterator>

namespace computer_vision
{
	namespace D2
	{
		struct Size { size_t width, height; };

		template<class T, class S=T>
		using mfun_t = std::function<S(T)>;
		template<class T,class S=T>
		using dfun_t = typename std::function<T(T, S)>;

		template<class T>
		struct Image
		{
			using Data = std::vector<T>;
			using Iter = typename Data::iterator;
			using CIter = typename Data::const_iterator;
			// C'tors
			Image() = delete;
			Image(size_t width, size_t height) : width(width), data(width*height, 0) { if (width*height == 0) throw; }
			Image(Size const& s) : Image(s.width, s.height) {}
			template<class S=T>
			Image(Image<S> const& I) : Image<T>(I.ImageSize()) {apply<S>(I, [](T, S x){return T(x);});}
			// STL access
			Iter begin() { return data.begin(); }
			Iter end() { return data.end(); }
			CIter cbegin() const { return data.cbegin(); }
			CIter cend() const { return data.cend(); }
			size_t size() const { return data.size(); }
			// Single access
			T operator()(size_t i, size_t j) const { return data[kmap(i, j)]; }
			T& operator()(size_t i, size_t j) { return data[kmap(i, j)]; }
			T operator[](size_t k) const { return data[k]; }
			T& operator[](size_t k) { return data[k]; }
			T at(size_t k) const { return data[k]; }
			// Helpers
			Size ImageSize() const { return{ width, size() / width }; }

			template<typename S = T>
			Image<T>& apply( Image<S> const& I, dfun_t<T,S> f) {std::transform(cbegin(), cend(), I.cbegin(), begin(), f); return *this;	}

			Image<T>& apply(mfun_t<T> f) { return apply<T>(*this, [=](T x, T){return f(x); }); }

		private:
			size_t kmap(size_t i, size_t j) { return i + j*width; }
			size_t width;
			Data data;
		};

		template<typename T, typename S = T>
		Image<S> apply(Image<T> const& I, mfun_t<T,S> f) {return Image<S>(I).apply([=](S x) {return f(T(x)); }); }
		template<typename T, typename S>
		Image<T> apply(Image<T> const& l, Image<S> const& r, dfun_t<T, S> f) { return Image<T>(l).apply<S>(r, f);	}

		template<typename T, typename S>
		Image<T> operator+(Image<T> const& l, Image<S> const& r) { return apply<T, S>(l, r, [](T x, S y)->T {return x + T(y); }); }
		template<typename T, typename S>
		Image<T> operator-(Image<T> const& l, Image<S> const& r) { return apply<T, S>(l, r, [](T x, S y)->T {return x - T(y); }); }
		template<typename T, typename S>
		Image<T> operator*(Image<T> const& l, Image<S> const& r) { return apply<T, S>(l, r, [](T x, S y)->T {return x * T(y); }); }
		template<typename T, typename S>
		Image<T> operator/(Image<T> const& l, Image<S> const& r) { return apply<T, S>(l, r, [](T x, S y)->T {return x / T(y); }); }
		template<typename T>
		std::ostream& operator<<(std::ostream& os, Image<T>& I)	{std::copy(I.cbegin(), I.cend(), std::ostream_iterator<T>(std::cout, ", ")); return os;	}

	}
	namespace D3
	{
		struct Size { size_t width, height, depth; };

		template<class T>
		struct Image3D
		{
			using Data = std::vector<T>;
			using Iter = typename Data::iterator;
			using CIter = typename Data::const_iterator;
			// C'tors
			Image3D() = delete;
			Image3D(size_t width, size_t height, size_t depth) : width(width), plane(width*height), data(width*height*depth, 0) { if (width*height*depth == 0) throw; }
			Image3D(Size const& s) : Image3D<T>(s.width, s.height, s.depth) {}
			Image3D(Image3D const& o) : width(o.width), plane(o.plane), data(o.data) {}
			// STL access
			size_t size() const { return data.size(); }
			Iter begin() { return data.begin(); }
			Iter end() { return data.end(); }
			CIter cbegin() const { return data.cbegin(); }
			CIter cend() const { return data.cend(); }
			// Single access
			T operator()(size_t i, size_t j, size_t k) const { return data[kmap(i, j, k)]; }
			T& operator()(size_t i, size_t j, size_t k) { return data[kmap(i, j, k)]; }
			T operator[](size_t k) const { return data[k]; }
			T& operator[](size_t k) { return data[k]; }
			T at(size_t k) const { return data[k]; }
			// Helpers
			Size ImageSize() const { return{ width, plane / width, size() / plane }; }
			template<class S = T>
			Image3D<S> clone()
			{
				Image3D<S> tmp(ImageSize());
				std::transform(cbegin(), cend(), tmp.begin(), [](T x)->S {return static_cast<S>(x); });
				return tmp;
			}

		private:
			size_t kmap(size_t i, size_t j, size_t k) { return i + j*width + k*plane; }
			size_t width, plane;
			Data data;
		};
	}
}

namespace cv2 = computer_vision::D2;

