#include <iostream>


template<typename Base>
struct AddWhen
{
   template<typename F, typename... T>
   auto& when(bool flag, F f, T&&... params)
   {
      auto& self = static_cast<Base&>(*this);
      if (flag)
      {
         std::invoke(f, self, std::forward<T>(params)...);
      }
      return self;
   }
};

struct point : AddWhen<point>
{ 
   point(double x=0.0, double y=0.0) : x(x), y(y) {}
   double x = 0.0, y = 0.0; 
};

std::ostream& operator<<(std::ostream& os, point const& p)
{
   os << "P=" << p.x << "|" << p.y;
   return os;
}

struct configuration
{
   bool useP0 = true;
   bool addOffset = true;
};

void test(configuration const& cfg, bool uselog = false)
{
   auto setP0 = [](point& p) { p.x = 3.0; p.y = 4.0; };
   auto addOffset = [](point& p, double x, double y = 0.0) { p.x += x; p.y += y; };
   auto p = point{}
      .when(cfg.useP0, setP0)
      .when(uselog, [](point&) { std::cout << "logging: "; })
      .when(cfg.addOffset, addOffset, 7.0, 8.0);

   std::cout << p << "\n";
}

int main()
{
   test({ false, false });
   test({ true, false }, true);
   test({ false, true });
   test({ true, true });
   return 0;
}
