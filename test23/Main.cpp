#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <vector>
#include <numeric>
#include <ctime>
#include <sstream>
#include <iomanip>

namespace Bank
{
	enum class Gender { male, female};

	std::string genderLabel(Gender g)
	{
		static const std::map<Gender, std::string> GL{ {Gender::male, "maennlich"},{ Gender::female, "weiblich"} };
		return GL.count(g)? GL.at(g) :"unknown";
	}

	struct Kunde
	{
		const std::string nachname;
		const std::string vorname;
		const Gender geschlecht;
		Kunde(const std::string& n, const std::string& v, Gender g, float init = 0) : nachname(n), vorname(v), geschlecht(g) {}
	};

	std::ostream& operator<<(std::ostream &o, Kunde const& k)
	{
		std::cout << "Name: " << k.vorname << " " << k.nachname << "\n" << "Geschlecht: " << genderLabel(k.geschlecht);
		return o;
	}

	using KundenDB = std::vector<Kunde>;

	std::string now()
	{
		auto t = std::time(nullptr);
		auto tm = *std::localtime(&t);

		std::ostringstream oss;
		oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
		return oss.str();
	}
	template<class T = float>
	struct Transactions
	{
		Transactions(T a, std::string const& c = "") : amount(a), comment(c), date(now()) { }
		const std::string comment;
		const T amount;
		const std::string date;
		std::string print() const { return std::to_string(amount) + ";" +date + ";" + comment; }
	};

	template<class T = float>
	struct Konto
	{
		std::vector<Transactions<T>> transactions;
		Konto( T init = 0)  {einzahlen(init); }
		void einzahlen(T betrag, std::string const& c="") { if (betrag > 0) transactions.emplace_back(betrag,c); }
		void auszahlen(T betrag, std::string const& c="") { if (betrag > 0) transactions.emplace_back(-betrag,c); }
		float Kontostand() const
		{
			T accu(0); for (auto const& x : transactions) accu += x.amount; return accu;
			//return std::accumulate(transactions.cbegin(), transactions.cend(), 0.0f, [](float s, Transactions const& x) {return s + x.amount; });
		}
		std::string print() const {	std::string s; for (auto const& x : transactions) s += x.print()+"\n";	return s;}
	};

}

int main(int, char**)
{

	using namespace Bank;
	Konto<float> k(233.4f);
	k.einzahlen(100,"T1");
	k.einzahlen(10, "T2");
	k.einzahlen(1);
	k.auszahlen(33.3f, "T4");
	std::cout << "Stand =" << k.Kontostand() << "Euro\n" << k.print() ;
	return 0;

}

