#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <vector>

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
		float kontostand;
		Kunde(const std::string& n, const std::string& v, Gender g, float init=0) : nachname(n) , vorname(v), geschlecht(g), kontostand(init) {}
		void einzahlen(float betrag) { if (betrag > 0) kontostand += betrag; }
		void auszahlen(float betrag) { if (betrag > 0) kontostand -= betrag; }
		float Kontostand() const { return kontostand; }
	};

	std::ostream& operator<<(std::ostream &o, Kunde const& k)
	{
		std::cout << "Name: " << k.vorname << " " << k.nachname << "\n" << "Geschlecht: " << genderLabel(k.geschlecht);
		return o;
	}

	using KundenDB = std::vector<Kunde>;
}


int main(int, char**)
{

	using namespace Bank;
	KundenDB kunden = { {"Mueller","Klaus",Gender::male, 45000},
	                             { "Mueller","Gaby",Gender::female, 4500 },
								 { "Meier","Tom",Gender::male, 0 } };

	for (auto const& k : kunden) std::cout << k << "\n-------\n";
	return 0;

}

