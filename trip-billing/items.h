#pragma once
#include <vector>
#include <set>
#include <numeric>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "../Tools/zipper.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;



namespace jh
{

	//----------------------------------------------------------------
	struct driver_t
	{
		std::string name{};
		driver_t(std::string const& s="") : name(s) {}

		bool valid() const { return !name.empty(); }
		auto operator==(const driver_t& o) const { return name == o.name; }
		auto operator<(const driver_t& o) const { return name < o.name; }
	};

	void to_json(json& j, const driver_t& d) {j = json{ {"name", d.name}};}
	void from_json(const json& j, driver_t& d) {	j.at("name").get_to(d.name);}

	inline std::ostream& operator<<(std::ostream& os, driver_t const& driver) {os << driver.name; return os; }
	inline std::istream& operator>>(std::istream& is, driver_t& driver) { is >> driver.name;	return is; }

	//----------------------------------------------------------------
	struct trip_t
	{
		int start = 0;
		int end = 0;
		driver_t driver{};

		trip_t() = default;
		trip_t(int s, int e, driver_t const& d) : start(s), end(e), driver(d) {}

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

	void to_json(json& j, const trip_t& trip) { j = json{ {"start", trip.start}, {"end", trip.end}, {"driver", trip.driver} }; }
	void from_json(const json& j, trip_t& trip) { j.at("start").get_to(trip.start); j.at("end").get_to(trip.end); j.at("driver").get_to(trip.driver); }
	void to_json(json& j, const trips_t& trips) { j.clear(); for (auto&& x : trips) j.emplace_back(x); }
	void from_json(const json& j, trips_t& trips) { trips.clear(); for (auto&& x : j) trips.emplace_back(x); }

	//----------------------------------------------------------------
	struct bill_t
	{
		double amount;
		driver_t driver;
		bool valid() const { return amount > 0.0 && driver.valid(); }
	};

	inline std::ostream& operator<<(std::ostream& os, bill_t const& bill) { os << bill.amount << '\t' << bill.driver; return os;}
	inline std::istream& operator>>(std::istream& is, bill_t& bill) { is >> bill.amount >> bill.driver;	return is; }

	using bills_t = std::vector<bill_t>;

	inline double total(bills_t const bills)
	{	
		auto accu = [](double s, bill_t const& bill) {return s + bill.amount; };
		return sum_f<bills_t, double>(bills,accu); 
	}

	inline double total(bills_t const bills, driver_t const& driver)
	{
		auto accu = [&driver](double s, bill_t const& bill) {return s + (bill.driver == driver ? bill.amount : 0); };
		return sum_f<bills_t, double>(bills, accu);
	}

	template<class T>
	void read(std::ifstream&& in, T& out)
	{
		while (in && !in.eof())
		{
			auto& back = out.emplace_back(); in >> back;
			if (!back.valid())
				out.pop_back();
		}
	};

	template<class T>
	void write(T& in, std::ofstream&& out) 
	{
		if (out) out << in; 
	};

	void to_json(json& j, const bill_t& bill) { j = json{ {"amount", bill.amount},{"driver", bill.driver} }; }
	void from_json(const json& j, bill_t& bill) { j.at("amount").get_to(bill.amount); j.at("driver").get_to(bill.driver); }
	void to_json(json& j, const bills_t& bills) { j.clear(); for (auto&& x : bills) j.emplace_back(x); }
	void from_json(const json& j, bills_t& bills) { bills.clear(); for (auto&& x : j) bills.emplace_back(x); }

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
			write( trips, std::ofstream(folder + "/trips.txt"));
			write( bills, std::ofstream(folder + "/bills.txt"));
		}
		void load(std::string folder = ".") 
		{
			trips.clear(); read(std::ifstream(folder + "/trips.txt"), trips);
			bills.clear(); read(std::ifstream(folder + "/bills.txt"), bills);
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
				std::stringstream out;
				out << "----------------- " << name << " -----------------------\n";
				if (auto totalTrack = total(trips))
				{
					auto totalI = (totalTrack * insurance);
					auto totalBills = total(bills);
					auto totalAmount = totalBills + totalI;
					out << std::fixed << std::setprecision(2);
					out << "Reisen gesamt:  " << totalTrack << " km\n";
					out << "Tanken gesamt:  " << totalBills << "€\n";
					out << "Versich. gesamt:  " << totalI << "€\n";
					out << "Kosten gesamt:  " << totalAmount << "€\n";
					out << "Quote (R):  " << (100 * totalBills / totalTrack) << " ct/km\n";
					out << "Quote (T):  " << (100 * totalAmount / totalTrack) << " ct/km\n";
					out << "Versicherung:  " << (insurance*100) << " ct/km\n";

					auto evalDriver = [&](driver_t driver)
					{
						auto track = double(total(trips, driver));
						auto ratio = (track / totalTrack);
						auto credit = total(bills, driver);
						auto debit = totalAmount * ratio;
						out << std::setprecision(1) << std::left;
						out << "\n----------------- " << driver.name << " -----------------------\n";
						out << "Strecke:   " << track << " km  Anteil: " << (100 * ratio) << "%\n";
						out << std::setprecision(2);
						out << "Haben:\t" << credit << "€\n";
						out << "Soll:\t" << debit << "€\n";
						out << "Ausgleich:\t" << (credit - debit) << "€\n";
					};
					for (auto&& driver : drivers)
						evalDriver(driver);
				}
				return out.str();
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

	void to_json(json& j, const billing& bill) { j = json{ {"name", bill.name},{"trips", bill.trips}, {"bills", bill.bills} }; }
	void from_json(const json& j, billing& bill) { j.at("name").get_to(bill.name); j.at("trips").get_to(bill.trips); j.at("bills").get_to(bill.bills);
	}

}

