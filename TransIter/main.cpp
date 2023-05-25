#include <iostream>
#include <type_traits>
#include <array>
#include <vector>
#include <functional>


namespace jh
{
   template<typename Iterator>
   struct span
   {
      span(Iterator begin, Iterator end) : begin_(begin), end_(end) {}
      span(Iterator begin, size_t count) : span(begin, std::next(begin, count)) {}
      Iterator begin() { return begin_; }
      Iterator end() { return end_; }
      const Iterator begin() const { return begin_; }
      const Iterator end() const { return end_; }
   private:
      Iterator begin_, end_;
   };

   template<typename Iterator, typename Transformer>
   class transform_iterator 
   {
      Iterator m_it;
      Transformer m_trafo;

   public:
      transform_iterator(Iterator const& it, Transformer const& trafo) : m_it(it), m_trafo(trafo) {}

      using difference_type = typename std::iterator_traits<Iterator>::difference_type;
      using value_type = typename std::invoke_result<Transformer, Iterator>;
      using pointer = void;
      using reference = void;
      using iterator_category = std::input_iterator_tag;

      auto operator==(transform_iterator const& other) { return m_it == other.m_it; }
      auto operator!=(transform_iterator const& other) { return !(*this == other); }
      auto operator*() const { return m_trafo(*m_it); }
      auto operator++() { ++m_it; return *this; }
      auto operator++(int) { auto t = *this; ++m_it; return t; }
   };

   // For C++14 (no CTAD)
   template<typename Iterator, typename Transformer>
   auto make_transform_iterator(Iterator const& it, Transformer const& tf) { return transform_iterator<Iterator, Transformer>(it, tf); }

   template<typename C, typename Function>
   auto makeTrafoView(C&& c, Function f)
   {
      return jh::span(transform_iterator(c.begin(), f), transform_iterator(c.end(), f));
   }
}

int main()
{
   std::array<int, 4> a = { 99, 88, 2001, 42 };
   for (auto&& x : a)
      std::cout << x << "\n";

   auto t1 = [](auto v) { return v + 1; };
    std::vector<int> v( jh::transform_iterator(a.begin(), t1),  jh::transform_iterator(a.end(), t1));

   for (auto&& x : v)
      std::cout << x << "\n";
   for (auto&& x : jh::makeTrafoView(a, [w=20](auto v) { return v + w; }))
      std::cout << x << "\n";
   return 0;
}
