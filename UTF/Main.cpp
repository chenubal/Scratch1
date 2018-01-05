#include <cstdio>
#include <clocale>
#include <fstream>
#include <iostream>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

int main()
{

	fs::path p = fs::u8path(u8"π.txt");

	
	if (auto f = std::ofstream(p)) {
		f << u8"要らない.txt";
	}

	// multibyte and wide representation can be used for output
	std::cout.imbue(std::locale());
	std::cout << "\nFile name in narrow multibyte encoding: "
		<< p.string() << '\n';

	std::wcerr.imbue(std::locale());
	std::wcerr << "File name in wide encoding: "
		<< p.wstring() << '\n';

	//fs::remove(p);
}