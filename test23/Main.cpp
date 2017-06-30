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

	struct Customer
	{
		const std::string surname;
		const std::string forename;
		Customer(const std::string& n, const std::string& v) : surname(n), forename(v) {}
	};

	std::ostream& operator<<(std::ostream &os, Customer const& c)
	{
		os << "Name: " << c.forename << " " << c.surname << "\n";
		return os;
	}

	template<class T = float>
	struct Transaction
	{
		Transaction(T a, std::string const& c = "") : amount(a), comment(c), date(std::chrono::system_clock::now()) { }
		const std::string comment;
		const T amount;
		const ctp_t date;
		T operator()() const { return amount; }
	};

	template<class T>
	std::ostream& operator<<(std::ostream &os, Transaction<T> const& t)
	{
		os << std::to_string(t.amount) + "\t" + dateStr(t.date) + "\t" + t.comment;
		return os;
	}

	template<class T = float>
	struct Account
	{
		std::vector<Transaction<T>> transactions;
		Account(T start = 0) { deposit(start,"Start"); }
		void deposit(T amount, std::string const& comment = "") { if (amount > 0) transactions.emplace_back(amount, comment); }
		void withdraw(T amount, std::string const& comment = "") { if (amount > 0) transactions.emplace_back(-amount, comment); }
		float balance() const
		{
			T s(0); for (auto const& t : transactions) s += t(); return s;
		}
	};

	template<class T>
	std::ostream& operator<<(std::ostream &os, Account<T> const& account)
	{
		for (auto const& t : account.transactions) 
			os << t << "\n";
		return os;
	}

	using KundenDB = std::vector<Customer>;

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

