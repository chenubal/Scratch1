#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <vector>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <chrono>

namespace Bank
{
	using ctp_t = std::chrono::time_point<std::chrono::system_clock>;

	std::string dateStr(ctp_t const& t)
	{
		auto tt = std::chrono::system_clock::to_time_t(t);
		auto tm = *std::localtime(&tt);

		std::ostringstream oss;
		oss << std::put_time(&tm, "%c %Z");
		return oss.str();
	}

	enum class Gender { male, female };

	std::string genderLabel(Gender g)
	{
		static const std::map<Gender, std::string> GL{ {Gender::male, "male"},{ Gender::female, "female"} };
		return GL.count(g) ? GL.at(g) : "unknown";
	}

	struct Customer
	{
		const std::string surname;
		const std::string forename;
		const Gender geschlecht;
		Customer(const std::string& n, const std::string& v, Gender g, float init = 0) : surname(n), forename(v), geschlecht(g) {}
	};

	std::ostream& operator<<(std::ostream &o, Customer const& k)
	{
		o << "Name: " << k.forename << " " << k.surname << "\n" << "Gender: " << genderLabel(k.geschlecht);
		return o;
	}

	using KundenDB = std::vector<Customer>;


	template<class T = float>
	struct Transactions
	{
		Transactions(T a, std::string const& c = "") : amount(a), comment(c), date(std::chrono::system_clock::now()) { }
		const std::string comment;
		const T amount;
		const ctp_t date;
	};

	template<class T>
	std::ostream& operator<<(std::ostream &o, Transactions<T> const& t)
	{
		o << std::to_string(t.amount) + "\t" + dateStr(t.date) + "\t" + t.comment;
		return o;
	}

	template<class T = float>
	struct Account
	{
		std::vector<Transactions<T>> transactions;
		Account(T init = 0) { deposit(init); }
		void deposit(T betrag, std::string const& c = "") { if (betrag > 0) transactions.emplace_back(betrag, c); }
		void withdraw(T betrag, std::string const& c = "") { if (betrag > 0) transactions.emplace_back(-betrag, c); }
		float balance() const
		{
			T accu(0); for (auto const& x : transactions) accu += x.amount; return accu;
		}
	};

	template<class T>
	std::ostream& operator<<(std::ostream &o, Account<T> const& a)
	{
		for (auto const& x : a.transactions) o << x << "\n";
		return o;
	}

}

int main(int, char**)
{

	using namespace Bank;
	Account<float> k(233.4f);
	k.deposit(100, "T1");
	k.deposit(10, "T2");
	k.deposit(1);
	k.withdraw(33.3f, "T4");
	std::cout << "Balance = " << k.balance() << "Euro\nTransactions:\n" << k;
	return 0;

}

