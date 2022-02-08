#include <windows.h>
#include <string>
#include <algorithm>
#include <iostream>
#include <map>

// This process opens a handle to a mutex created by another process.
struct namedHandle
{
	const LPCWSTR name = L"";
	HANDLE handle = nullptr;
};

std::map<int, namedHandle> amasHandles =
{
	{0, { L"AUDI_AMAS_BDE_ACTIVE", nullptr}},
	{1, { L"AUDI_AMAS_BDE_IDLE",nullptr}},
	{2,{ L"AUDI_AMAS_BDE_MANU", nullptr}}
};

std::string ws2s(const std::wstring& wide)
{
	std::string str(wide.length(), 0);
	std::transform(wide.begin(), wide.end(), str.begin(), [](wchar_t c) {return (char)c; });
	return str;
}

int main(int argc, char *argv[])
{
	auto key  = argc > 1 ?	std::atoi(argv[1]) : 0;
	if (amasHandles.count(key))
	{
		auto& entry = amasHandles[key];
		if (entry.handle = OpenMutex(MUTEX_ALL_ACCESS, FALSE, entry.name))
		{
			std::cout << "Mutex " << ws2s(entry.name).c_str() << " exists \n";
			CloseHandle(entry.handle);
		}
		else
			std::cout << "Mutex " << ws2s(entry.name).c_str() << " not exists \n";
	}
	else
		std::cout << "Invalid key: " << key << "\n";

	return 0;
}