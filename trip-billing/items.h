#pragma once
#include <string>
#include <vector>
#include <span>
#include <numeric>
#include <compare>
#include <fstream>

namespace jh
{
	template<typename T>
	std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
	{
		auto e = std::end(v);
		if (auto s = std::begin(v); s!=e)
		{
			os << *s;
			for (auto&& v : std::span(std::next(s), e)) os << '\n' << v;
		}
		return os;
	}

	template<class C, class R, class F> // Needs F with R=F(R,C::V,)
   R sum_f(C const c, F f) { return std::accumulate(std::begin(c), std::end(c), R(0), f); }

	//----------------------------------------------------------------
	struct driver_t
	{
		std::string name;
		auto operator<=>(const driver_t&) const = default;
	};

	std::ostream& operator<<(std::ostream& os, driver_t const& driver)
	{
		os << driver.name;
		return os;
	}
	std::istream& operator>>(std::istream& is, driver_t& driver)
	{
		is >> driver.name;
		return is;
	}

	//----------------------------------------------------------------
	struct trip_t
	{
		int start;
		int end;
		driver_t driver;
		int dist() const { return end - start; }
		friend std::istream& operator>>(std::istream& is, trip_t& trip);
		friend std::ostream& operator<<(std::ostream& os, trip_t const& trip);
	};

	std::istream& operator>>(std::istream& is, trip_t& trip)
	{
		is >> trip.start >> trip.end >> trip.driver;
		return is;
	}

	std::ostream& operator<<(std::ostream& os, trip_t const& trip)
	{
		os << trip.start << '\t' << trip.end << '\t' << trip.driver;
		return os;
	}

	using trips_t = std::vector<trip_t>;
	int total(trips_t const trips) 
	{	
		return sum_f<trips_t, int>(trips, [](int s, trip_t const& t) {return s + t.dist(); }); 
	}
	int total(trips_t const trips, driver_t const& d) 
	{ 
		return sum_f<trips_t, int>(trips, [&d](int s, trip_t const& t) {return s + (t.driver==d? t.dist() : 0); }); 
	}

	//----------------------------------------------------------------
	struct bill_t
	{
		double amount;
		driver_t driver;
	};

	std::ostream& operator<<(std::ostream& os, bill_t const& bill)
	{
		os << bill.amount << '\t' << bill.driver;
		return os;
	}

	std::istream& operator>>(std::istream& is, bill_t& bill)
	{
		is >> bill.amount >> bill.driver;
		return is;
	}

	using bills_t = std::vector<bill_t>;
   double total(bills_t const trips) 
	{	
		return sum_f<bills_t, double>(trips, [](double s, bill_t const& t) {return s + t.amount; }); 
	}
	double total(bills_t const trips, driver_t const& d) 
	{
		return sum_f<bills_t, double>(trips, [&d](double s, bill_t const& t) {return s + (t.driver == d ? t.amount : 0.0); });
	}

	//----------------------------------------------------------------
	struct billing
	{
		explicit billing(std::string name) : name(std::move(name)) {}

		std::string name;
		trips_t trips{};
		bills_t bills{};
		void store(std::string folder = "./") const
		{
			if (auto f = std::ofstream(folder + "trips.txt"))	
				f << trips;
			if (auto f = std::ofstream(folder + "bills.txt"))	
				f << bills;
		}
		void load(std::string folder = "./") 
		{
			trips.clear();
			if (auto f = std::ifstream(folder + "trips.txt"))
			{
				while (!f.eof()) { trips.emplace_back(); f >> trips.back(); }
			}
			bills.clear();
			if (auto f = std::ifstream(folder + "bills.txt"))
			{
				bills.clear();
				if (auto f = std::ifstream(folder + "bills.txt"))
				{
					while (!f.eof()) { bills.emplace_back(); f >> bills.back(); }
				}
			}
		}
	};

}

