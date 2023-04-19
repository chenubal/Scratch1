#pragma once
#include "items.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;


namespace jh
{
	inline void to_json(json& j, const driver_t& d) 
	{
		j = json{ {"name", d.name}};
	}

	inline void from_json(const json& j, driver_t& d) 
	{	
		j.at("name").get_to(d.name);
	}

	inline void to_json(json& j, const trip_t& trip) 
	{ 
		j = json{ 
			{"start", trip.start}, 
			{"end", trip.end}, 
			{"driver", trip.driver} 
		}; 
	}

	inline void from_json(const json& j, trip_t& trip)
	{ 
		j.at("start").get_to(trip.start); 
		j.at("end").get_to(trip.end); 
		j.at("driver").get_to(trip.driver); 
	}

	inline void to_json(json& j, const trips_t& trips)
	{ 
		j.clear(); 
		for (auto&& x : trips) j.emplace_back(x); 
	}

	inline void from_json(const json& j, trips_t& trips)
	{ 
		trips.clear(); 
		for (auto&& x : j) trips.emplace_back(x); }

	inline void to_json(json& j, const bill_t& bill)
	{ 
		j = json{ 
			{"amount", bill.amount},
		{"driver", bill.driver} 
		};
	}

	inline  void from_json(const json& j, bill_t& bill)
	{ 
		j.at("amount").get_to(bill.amount); 
		j.at("driver").get_to(bill.driver); 
	}

	inline  void to_json(json& j, const bills_t& bills)
	{ 
		j.clear(); 
		for (auto&& x : bills) j.emplace_back(x); 
	}

	inline  void from_json(const json& j, bills_t& bills)
	{ 
		bills.clear(); 
		for (auto&& x : j) bills.emplace_back(x); 
	}

	inline  void to_json(json& j, const billing& bill)
	{ 
		j = json{ 
			{"name", bill.name},
		   {"trips", bill.trips}, 
		   {"bills", bill.bills} }; 
	}

	inline  void from_json(const json& j, billing& bill)
	{ 
		j.at("name").get_to(bill.name); 
		j.at("trips").get_to(bill.trips); 
		j.at("bills").get_to(bill.bills); 
	}

}

