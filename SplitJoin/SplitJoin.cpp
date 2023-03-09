#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using S = std::string;
using SIt = S::const_iterator;
using VS = std::vector<S>;
using VSIt = VS::const_iterator;

void split(SIt b, SIt e, VS& r, char sep = '|')
{
	auto p = std::find(b, e, sep);
	r.emplace_back(b, p);
	if (p != e) split(++p, e, r, sep);
}

VS split(S const& s, char sep = '|')
{
	VS r;
	split(std::begin(s), std::end(s), r, sep);
	return r;
}

VS split2(const S& s, char sep ='|')
{
	VS result;
	size_t colon = s.find(sep);
	size_t from = 0u;
	while (colon != S::npos)
	{
		result.push_back(s.substr(from, colon - from));
		from = colon + 1;
		colon = s.find(sep, from);
	}
	result.push_back(s.substr(from));
	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

void join(VSIt b, VSIt e, S& s, char sep = '|')
{
	if (b != e)
	{
		s += *b++;
		if (b != e) s += sep;
		join(b, e, s, sep);
	}
}

S join(VS const& vs, char sep = '|')
{
	S r;
	join(vs.begin(), vs.end(), r, sep);
	return r;
}

S join2(VS const& vs, char sep = '|')
{
	S r;
	if (auto b = vs.begin(), e = vs.end(); b != e)
	{
		r += *b++;
		while (b != e) { r += sep; r += *b++; }
	}
	return r;
}


int main()
{
	auto test = [](S const& s) {std::cout << join(split(s)) << "\n"; };
	auto test2 = [](S const& s) {std::cout << join2(split2(s)) << "\n"; };
	test("||Hello|World|abc|||h|");
	test("|");
	test("");
	test("a");
	test2("||Hello|World|abc|||h|");
	return 0;
}