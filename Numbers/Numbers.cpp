#include <iostream>

template<bool Increase = true>
struct Numbers
{
   using value_type = unsigned long;

   struct Iterator
   {
      Iterator(value_type n = 0) : counter(n) {}
      auto operator*() const { return counter; }
      auto& operator++() { inc(); return *this; }
      auto operator!=(Iterator const& o) const { return o.counter != counter; }
   protected:
      value_type inc() { return counter += (Increase ? 1 : -1); }
      value_type counter;
   };

   Numbers(value_type n) : b(Increase ? 0 : (n- 1)), e(Increase ? n : -1) {}
   auto begin() { return b; }
   auto end() { return e; }
private:
   Iterator b, e;
};


int main()
{
   auto k = 17;
   for (auto x : Numbers<>(k))
      std::cout << "x= " << x << " ";
   std::cout << std::endl;
   for (auto x : Numbers<false>(k))
      std::cout << "x= " << x << " ";
   std::cout << std::endl;
   return 0;
}