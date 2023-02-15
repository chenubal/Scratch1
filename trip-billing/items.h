#pragma once
#include <vector>
#include <numeric>
#include <fstream>
#include <sstream>
#include "../Tools/zipper.h"

namespace jh
{

	//----------------------------------------------------------------
	struct driver_t
	{
		std::string name;

		friend std::istream& operator>>(std::istream& is, driver_t& trip);
		friend std::ostream& operator<<(std::ostream& os, driver_t const& trip);
		auto operator==(const driver_t& o) const { return name == o.name; }
	};

	inline std::ostream& operator<<(std::ostream& os, driver_t const& d) {os << d.name; return os; }
	inline std::istream& operator>>(std::istream& is, driver_t& d) { is >> d.name;	return is; }

	//----------------------------------------------------------------
	struct trip_t
	{
		int start;
		int end;
		driver_t driver;

		int dist() const { return end - start; }
		friend std::istream& operator>>(std::istream& is, trip_t& trip);
		friend std::ostream& operator<<(std::ostream& os, trip_t const& trip);
		auto operator==(const trip_t& o) const { return std::tie(start, end, driver) == std::tie(o.start, o.end, o.driver); }
	};

	inline std::istream& operator>>(std::istream& is, trip_t& t)	{ is >> t.start >> t.end >> t.driver; return is; }
	inline std::ostream& operator<<(std::ostream& os, trip_t const& t)	{ os << t.start << '\t' << t.end << '\t' << t.driver;	return os; }

	using trips_t = std::vector<trip_t>;

	inline int total(trips_t const t)
	{	
		auto f = [](int s, trip_t const& tt) {return s + tt.dist(); };
		return sum_f<trips_t, int>(t, f); 
	}
	inline int total(trips_t const t, driver_t const& d)
	{
		auto f = [&d](int s, trip_t const& tt) {return s + (tt.driver == d ? tt.dist() : 0); };
		return sum_f<trips_t, int>(t, f); 
	}

	//----------------------------------------------------------------
	struct bill_t
	{
		double amount;
		driver_t driver;

		friend std::istream& operator>>(std::istream& is, bill_t& trip);
		friend std::ostream& operator<<(std::ostream& os, bill_t const& trip);
		auto operator==(const bill_t& o) const { return std::tie(amount, driver) == std::tie(o.amount, o.driver); }
	};

	inline std::ostream& operator<<(std::ostream& os, bill_t const& b)	{ os << b.amount << '\t' << b.driver; return os;}
	inline std::istream& operator>>(std::istream& is, bill_t& b)	{ is >> b.amount >> b.driver;	return is; }

	using bills_t = std::vector<bill_t>;

	inline double total(bills_t const b)
	{	
		auto f = [](double s, bill_t const& bb) {return s + bb.amount; };
		return sum_f<bills_t, double>(b,f); 
	}

	inline double total(bills_t const b, driver_t const& d)
	{
		auto f = [&d](double s, bill_t const& bb) {return s + (bb.driver == d ? bb.amount : 0.0); };
		return sum_f<bills_t, double>(b, f);
	}

	//----------------------------------------------------------------
	struct billing
	{
		explicit billing(std::string const& name="") : name(name) {}

		std::string name{};
		trips_t trips{};
		bills_t bills{};

		void store(std::string folder = ".") const
		{
			auto write = [&](auto& in, auto&& out) {if (out) out << in; } ;
			write( trips, std::ofstream(folder + "/trips.txt"));
			write( bills, std::ofstream(folder + "/bills.txt"));
		}
		void load(std::string folder = ".") 
		{
			auto read = [](auto&& in, auto& out) 
			{
				out.clear(); 
				if (in) 
				{ 
					while (!in.eof()) 
					{ 
						out.emplace_back(); 
						in >> out.back(); 
					} 
				} 
			};
			read(std::ifstream(folder + "/trips.txt"), trips);
			read(std::ifstream(folder + "/bills.txt"), bills);
		}

		std::string invoice(std::vector<driver_t> const& drivers) const
		{
			std::stringstream ss;
			ss << "----------------- " << name << " -----------------------\n";
			if (!trips.empty())
			{
				auto completeTrack = total(trips);
				auto completeAmount = total(bills);
				auto evaluate = [&](driver_t driver)
				{
					auto track = double(total(trips, driver)); 
					auto ratio = (track / completeTrack);
					auto credit = total(bills, driver);
					auto debit = completeAmount * ratio;
					ss << "\n----------------- " <<driver.name << " -----------------------\n";
				   ss << "Strecke:   " << track << "km  Anteil: " << 0.1*int(1000*ratio) << "%\n";
					ss << "Soll:      " << debit << " Euro\n";
					ss << "Haben:     " << credit << " Euro\n";
					ss << "Ausgleich: " << (credit-debit) << " Euro\n";
				};

				ss << "Gefahren gesamt: " << completeTrack << "km\n";
				ss << "Bezahlt gesamt: " << completeAmount << " Euro\n";
				for( auto&& driver : drivers) 
					evaluate(driver);
			}
			return ss.str();
		}
	};

}

