#include <iostream>
#include <vector>
#include <string>
#include <map>

namespace Bank
{
	enum class Gender { male, female};
	static const std::map<Gender, std::string> GenderLabel{ {Gender::male, "mäaennlich"},{ Gender::female, "weiblich"} };

	struct Kunde
	{
		const std::string nachname;
		const std::string vorname;
		const Gender geschlecht;
		Kunde(const std::string& n, const std::string& v, Gender g) : nachname(n) , vorname(v), geschlecht(g) {}
	};

	std::ostream& operator<<(std::ostream &o, Kunde const& k)
	{
		std::cout << "Name: " << k.vorname << " " << k.nachname << "\n" << "Geschlecht: " << GenderLabel.at(k.geschlecht);
		return o;
	}
}


int main(int, char**)
{

	using namespace Bank;
	Kunde k("Meier","Theo", Gender::male);
	std::cout << k << "\n";
	return 0;

}

