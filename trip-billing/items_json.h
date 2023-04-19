#pragma once
#include "items.h"
#include <nlohmann/json.hpp>

namespace jh
{
	using json = nlohmann::json;

	inline void to_json(json& js, const driver_t& d)
	{
		js = json{ {"name", d.name}};
	}

	inline void from_json(const json& js, driver_t& d) 
	{	
		js.at("name").get_to(d.name);
	}

	inline void to_json(json& js, const trip_t& trip) 
	{ 
		js = json
		{ 
			{"start", trip.start}, 
			{"end", trip.end}, 
			{"driver", trip.driver} 
		}; 
	}

	inline void from_json(const json& js, trip_t& trip)
	{ 
		js.at("start").get_to(trip.start); 
		js.at("end").get_to(trip.end); 
		js.at("driver").get_to(trip.driver); 
	}

	inline void to_json(json& js, const bill_t& bill)
	{ 
		js = json
		{ 
			{"amount", bill.amount},
		   {"driver", bill.driver} 
		};
	}

	inline  void from_json(const json& js, bill_t& bill)
	{ 
		js.at("amount").get_to(bill.amount); 
		js.at("driver").get_to(bill.driver); 
	}

	inline  void to_json(json& js, const billing& bill)
	{ 
		js = json
		{ 
			{"name", bill.name},
		   {"trips", bill.trips}, 
		   {"bills", bill.bills} 
		}; 
	}

	inline  void from_json(const json& js, billing& bill)
	{ 
		js.at("name").get_to(bill.name); 
		js.at("trips").get_to(bill.trips); 
		js.at("bills").get_to(bill.bills); 
	}

}

