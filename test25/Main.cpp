#include <cstdio>
#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <vector>
#include <iostream>

std::vector<unsigned> GetProcessIds(std::string const cmd)
{
	std::vector<unsigned> pids;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry) == TRUE)
	{
		std::wstring ref_cmd(cmd.cbegin(), cmd.cend());
		while (Process32Next(snapshot, &entry) == TRUE)
		{

			if (ref_cmd == std::wstring(entry.szExeFile))
			{
				pids.push_back(entry.th32ProcessID);
			}
		}
	}

	CloseHandle(snapshot);
	return pids;
}

int main(int, char *[])
{
	auto pids = GetProcessIds("DaVis.exe");
	for (auto && x : pids) std::cout << "Davis(" << x << ")\n";
	return 0;
}