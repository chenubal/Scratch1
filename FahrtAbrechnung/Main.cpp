#include <vector>
#include <iostream>
#include <string>
#include <map>
#include <numeric>
#include <fstream>
#include <regex>

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
	for (auto& el : vec) { os << el << '|'; }
	return os;
}

using Strings = std::vector<std::string>;

Strings readFile(const char * filename)
{
	Strings lines;
	if (auto is = std::ifstream(filename))
	{
		while (!is.eof())
		{
			std::string l;
			is >> l;
			lines.push_back(l);
		}
	}
	return lines;
}


using Position = unsigned;
using Driver = unsigned;

const std::map<Driver, std::string> DriverMap = { { 1, "Josef" }, { 2, "Johannes" }, { 3, "Luis" } };

struct Ride
{
	Ride(Position _start, Position _end, Driver _driver) :start(_start), end(_end), driver(_driver) {}
	unsigned distance() const { return end - start; }
	const Position start = 0;
	const Position end = 0;
	const Driver driver = 0;
};

using Rides = std::vector<Ride>;

Position sumDistances(Rides const& rides, Driver driver)
{
	return std::accumulate(rides.begin(), rides.end(), Position(0), [&](unsigned sum, Ride fahrt) 
	{ 
		return sum + (fahrt.driver == driver ? fahrt.distance() : 0); }
	);
}

using distMap = std::map<Driver, Position>;

distMap sumDistances(Rides const& rides)
{
	distMap r;
	for (auto const& item : DriverMap)
		r.insert(std::make_pair(item.first, sumDistances(rides, item.first)));
	return r;
}

std::ostream& operator<<(std::ostream & os, Ride const& ride)
{
	os << DriverMap.at(ride.driver) << ": " << ride.start << "-" << ride.end << " Distanz " << (ride.distance()) << "km";
	return os;
}

struct PitStop
{
	PitStop(double _amount, Driver _driver) : amount(_amount), driver(_driver) {}
	const double amount;
	const Driver driver = 0;

};

using PitStops = std::vector<PitStop>;

double sumAmount(PitStops const& pitstops, Driver driver)
{
	return std::accumulate(pitstops.begin(), pitstops.end(), 0.0, [&](double sum, PitStop pitstop)
	{
		return sum + (pitstop.driver == driver ? pitstop.amount : 0); }
	);
}

using amountMap = std::map<Driver, double>;

amountMap sumAmount(PitStops const& pitstops)
{
	amountMap r;
	for (auto const& item : DriverMap)
		r.insert(std::make_pair(item.first, sumAmount(pitstops, item.first)));
	return r;
}



std::ostream& operator<<(std::ostream & os, PitStop const& pitstop)
{
	os << DriverMap.at(pitstop.driver) << ": " << pitstop.amount << "Eur";
	return os;
}

std::vector<std::string> split(const std::string& input, const std::string& regex="\\s+") 
{
	std::regex rx(regex);
	std::sregex_token_iterator first{ input.begin(), input.end(), rx, -1 },	last;
	return { first, last };
}

struct Abrechnung
{
	Abrechnung(Position pos) :globalStart(pos), globalEnd(pos) {}

	void readRides(Strings const& lines)
	{
		for (auto&& line : lines)
		{
			auto v = split(line, ";");
			if (v.size() == 3)
			{
				Driver driver = std::atoi(v.at(2).c_str());
				auto te =  std::atoi(v.at(1).c_str());
				if (v.at(0).empty())
				{
					addFahrt(globalEnd, te, driver);
				}
				else
				{
					globalStart = std::atoi(v.at(0).c_str());
					addFahrt(globalStart, te, driver);
				}
				globalEnd = te;
			}
		}
	}

	void addFahrt( Position s, Position e, Driver d)
	{
		if (s > e) throw std::exception("Ungueltige Eingabe");
		rides.emplace_back(s, e, d);
	}

	void readPitStops()
	{
		pitstops.clear();
		bool getMore;
		do 
		{
			getMore = false;
			double price=0;
			Driver i=0;
			std::cout << "Kosten: ";
			std::cin >> price;
			if (price > 0.0)
			{
				std::cout << "Fahrer: ";
				std::cin >> i;
				if (DriverMap.count(i))
				{
					pitstops.emplace_back(price, i);
					getMore = true;
				}
			}

		} while (getMore);

	}

	friend std::ostream& operator<<(std::ostream & os, Abrechnung const& A)
	{
		auto totalDist = (A.globalEnd - A.globalStart);
		os << "Gesamt Fahrten: " << A.globalStart << " - " << A.globalEnd << " => " << totalDist <<"km\n";
		//for (auto&& f : A.rides) os << f << "\n";
		//for (auto&& t : A.pitstops) os << t << "\n";
		auto sd = sumDistances(A.rides);
		auto sa = sumAmount(A.pitstops);
		for (auto&& item : sd) os << DriverMap.at(item.first) << ": " << item.second << "km\n";
		for (auto&& item : sa) os << DriverMap.at(item.first) << ": " << item.second << " Euro\n";
		auto totalAmount = std::accumulate(sa.begin(), sa.end(), 0.0, [](double s, amountMap::value_type const& p) { return s + p.second; });
		std::vector<double> ratios(sd.size());
		std::transform(sd.begin(), sd.end(), ratios.begin(), [&](distMap::value_type const &p) {return double(p.second) / totalDist; });
		os << "Sizes " << ratios << "\n";
		os << "Gesamt Kosten: " << totalAmount << "Euro\n";
		return os;
	}

private:
	void printError(std::string const & msg) { std::cout << "ERROR: " << msg << "\n"; }
	Position globalStart;
	Position globalEnd;
	Rides rides;
	PitStops pitstops;
};



int main(int argc, char *argv[])
{
	if (argc <= 1) { std::cout << "No file argument\n"; return 0; }
	auto lines = readFile(argv[1]);
	Abrechnung A(0);
	A.readRides(lines);
	A.readPitStops();
 	std::cout << A << "\n";
	return 0;
}