// compose.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <ranges>
template <class F, class... Fs>
constexpr auto compose(F&& head, Fs &&...tail)
{
   return[  fun = std::forward<F>(head), ... functions = std::forward<Fs>(tail)] <class... Xs> (Xs &&...xs) mutable requires std::invocable<F, Xs...>
      {
         if constexpr (sizeof...(Fs))
         {
            return compose(std::forward<Fs>(functions)...)(std::invoke(std::forward<F>(fun), std::forward<Xs>(xs)...));
         }
         else
         {
            return std::invoke( std::forward<F>(fun), std::forward<Xs>(xs)...);
         }
      };
}

template <class... Fs>
auto composer(Fs&&... functions)
{
   using std::views::transform;
   return (transform(functions) | ...);
}

int main()
{
   struct pt { double x = 0; double y = 0; };
   auto lenght = [&](pt p){return std::hypot(p.x,p.y); };
   auto diff = [](pt p1, pt p2){ return pt{ p1.x - p2.x, p1.y - p2.y }; };
   auto dist = compose( diff, lenght);

   std::cout << dist(pt{ -0.5,-0.5 }, pt{ 0.5,0.5 }) << "\n";

}
