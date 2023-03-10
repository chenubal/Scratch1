#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using S = std::string;
using SIt = S::const_iterator;
using VS = std::vector<S>;
using VSIt = VS::const_iterator;

void split_r(SIt b, SIt e, VS& r, char sep = '|')
{
	auto p = std::find(b, e, sep);
	r.emplace_back(b, p);
	if (p != e) split_r(++p, e, r, sep);
}

VS split_r(S const& s, char sep = '|')
{
	VS r;
	split_r(std::begin(s), std::end(s), r, sep);
	return r;
}

VS split_l(const S& s, char sep ='|')
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

void join_r(VSIt b, VSIt e, S& s, char sep = '|')
{
	if (b != e)
	{
		s += *b++;
		if (b != e) s += sep;
		join_r(b, e, s, sep);
	}
}

S join_r(VS const& vs, char sep = '|')
{
	S r;
	join_r(vs.begin(), vs.end(), r, sep);
	return r;
}

S join_l(VS const& vs, char sep = '|')
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
	auto test = [](S const& s) {std::cout << join_r(split_r(s)) << "\n"; };
	auto test2 = [](S const& s) {std::cout << join_l(split_l(s)) << "\n"; };
	test("||Hello|World|abc|||h|");
	test("|");
	test("");
	test("a");
	test2("||Hello|World|abc|||h|");
	return 0;
}