#pragma once
#include <vector>
#include <set>
#include <numeric>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "../Tools/zipper.h"

namespace jh
{

	//----------------------------------------------------------------
	struct driver_t
	{
		std::string name;

		bool valid() const { return !name.empty(); }
		auto operator==(const driver_t& o) const { return name == o.name; }
		auto operator<(const driver_t& o) const { return name < o.name; }
	};

	inline std::ostream& operator<<(std::ostream& os, driver_t const& driver) {os << driver.name; return os; }
	inline std::istream& operator>>(std::istream& is, driver_t& driver) { is >> driver.name;	return is; }

	//----------------------------------------------------------------
	struct trip_t
	{
		int start;
		int end;
		driver_t driver;

		bool valid() const { return end > start && driver.valid(); }

		int dist() const { return end - start; }
	};

	inline std::istream& operator>>(std::istream& is, trip_t& trip) 
	{ 
		is >> trip.start >> trip.end >> trip.driver; 
		return is; 
	}
	inline std::ostream& operator<<(std::ostream& os, trip_t const& trip) { os << trip.start << '\t' << trip.end << '\t' << trip.driver;	return os; }

	using trips_t = std::vector<trip_t>;

	inline int total(trips_t const trips)
	{	
		auto accu = [](int s, trip_t const& trip) {return s + trip.dist(); };
		return sum_f<trips_t, int>(trips, accu); 
	}
	inline int total(trips_t const trips, driver_t const& driver)
	{
		auto accu = [&driver](int s, trip_t const& trip) {return trip.driver == driver ? s + trip.dist() : s; };
		return sum_f<trips_t, int>(trips, accu); 
	}

	//----------------------------------------------------------------
	struct bill_t
	{
		double amount;
		driver_t driver;
		bool valid() const { return amount > 0.0 && driver.valid(); }
	};

	inline std::ostream& operator<<(std::ostream& os, bill_t const& bill)	{ os << bill.amount << '\t' << bill.driver; return os;}
	inline std::istream& operator>>(std::istream& is, bill_t& bill)	{ is >> bill.amount >> bill.driver;	return is; }

	using bills_t = std::vector<bill_t>;

	inline double total(bills_t const bills)
	{	
		auto accu = [](double s, bill_t const& bill) {return s + bill.amount; };
		return sum_f<bills_t, double>(bills,accu); 
	}

	inline double total(bills_t const bills, driver_t const& driver)
	{
		auto accu = [&driver](double s, bill_t const& bill) {return bill.driver == driver ? s + bill.amount : s; };
		return sum_f<bills_t, double>(bills, accu);
	}

	template<class T>
	void transfer(std::ifstream&& in, T& out)
	{
		out.clear();
		if (in)
		{
			while (!in.eof())
			{
				out.emplace_back(); in >> out.back();
				if (!out.back().valid())
					out.pop_back();
			}
		}
	};

	//----------------------------------------------------------------
	struct billing
	{
		explicit billing(std::string const& name="") : name(name) {}

		std::string name{};
		trips_t trips{};
		bills_t bills{};
		const double insurance = 0.05; // €/km

		void store(std::string folder = ".") const
		{
			auto transfer = [&](auto& in, auto&& out) {if (out) out << in; } ;
			transfer( trips, std::ofstream(folder + "/trips.txt"));
			transfer( bills, std::ofstream(folder + "/bills.txt"));
		}
		void load(std::string folder = ".") 
		{
	
			transfer(std::ifstream(folder + "/trips.txt"), trips);
			transfer(std::ifstream(folder + "/bills.txt"), bills);
		}

		std::vector<driver_t> driver() const
		{
			std::set<driver_t> u;
			for (auto&& x : bills) u.insert(x.driver);
			for (auto&& x : trips) u.insert(x.driver);
			return std::vector<driver_t>(u.begin(),u.end());
		}

		std::string invoice(std::vector<driver_t> const& drivers) const
		{
			if (!trips.empty())
			{
				std::stringstream ss;
				ss << "----------------- " << name << " -----------------------\n";
				if (auto totalTrack = total(trips))
				{
					auto totalI = (totalTrack * insurance);
					auto totalAmount = total(bills) + totalI;
					auto evaluate = [&](driver_t driver)
					{
						auto track = double(total(trips, driver));
						auto ratio = (track / totalTrack);
						auto credit = total(bills, driver);
						auto debit = totalAmount * ratio;
						ss << std::setprecision(1) << std::left;
						ss << "\n----------------- " << driver.name << " -----------------------\n";
						ss << "Strecke:   " << track << " km  Anteil: " << (100 * ratio) << "%\n";
						ss << std::setprecision(2);
						ss << "Haben:\t" << credit << "€\n";
						ss << "Soll:\t" << debit << "€\n";
						ss << "Ausgleich:\t" << (credit - debit) << "€\n";
					};
					ss << std::fixed << std::setprecision(2);
					ss << "Gefahren gesamt:  " << totalTrack << " km\n";
					ss << "Bezahlt  gesamt:  " << totalAmount << "€\n";
					ss << "Quote:  " << (100 * totalAmount / totalTrack) << " ct/km\n";
					ss << "Vesicherung:  " << (insurance*100) << " ct/km gesamt: "<< totalI << "€\n";
					for (auto&& driver : drivers)
						evaluate(driver);
				}
				return ss.str();
			}
			return "No trips found!";
		}

		std::string invoiceAll() const { return invoice(driver()); }
			
		void clear()
		{
			bills.clear();
			trips.clear();
			name = "";
		}
	};

}

