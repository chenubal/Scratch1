#include <fstream>

/////////////////// DELETE THIS LOGGER ///////////////////////////////////
#include <chrono>
#include <iomanip>
#include <iostream>
struct Logger
{
	using T = std::chrono::microseconds;
	using Clk = std::chrono::system_clock;
	explicit Logger(const char* path) : str(path, std::ofstream::out), start(Clk::now()){}
	std::ofstream str;
	Clk::time_point start;
	long long count() {
		return std::chrono::duration_cast<T>(Clk::now() - start).count();}
	void log0(const char* msg, const char* end="\n") { if (str)	str << msg << end; }
	void log(const char* msg) { log0(msg); }
	void log(std::string const& msg) { log(msg.c_str()); }
	void log_t(const char* msg) {	str << std::setw(10) << count() << "us: "; log(msg);}
	void log_t(std::string const& msg) { log_t(msg.c_str()); }
};


//#include <iostream>
#include <chrono>

template<class  T = std::chrono::microseconds>
struct stopwatch
{
    using sc_t = std::chrono::system_clock;
    stopwatch() {reset();}
    void reset() {start =sc_t::now();}
    T duration() {auto t = sc_t::now(); return std::chrono::duration_cast<T>(t - start);}
    typename T::rep count() { return duration().count();}
    private:
    sc_t::time_point start;
};


int main(int, char**)
{
	Logger l("log.txt");
	stopwatch<> sw;
	l.log_t("Test1");
	l.str << sw.count() << "ms\n";
	l.log_t("Test2");
	return 0;

}

