#pragma once
#include "factory.h"
#include <iostream>

struct PrintC
{
	void operator()(Element const& e) { std::cout << "C: " << e << "\n"; }
};

