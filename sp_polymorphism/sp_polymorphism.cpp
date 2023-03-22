// See http://sean-parent.stlab.cc/presentations/2016-10-10-runtime-polymorphism/2016-10-10-runtime-polymorphism.pdf
// or an earlier version, "Inheritance Is The Base Class of Evil"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std;

// Print anything iostreams knows how to:
template <typename T>
void draw(const T& x, ostream& out, size_t indent)
{
   out << string(indent, ' ') << x << endl;
}

// We can insert arbitrary things too, so long as they have a draw()
class Foo {
public:
   Foo() = default;
   Foo(int x_) : x(x_) { }

   int x = 42;
};

void draw(const Foo& f, ostream& out, size_t indent)
{
   out << string(indent, ' ') << "I am a foo with value " << f.x << endl;
}


// An object holds any type value_type which has a corresponding function
// `void draw(const value_type&, ostream& out, size_t indent)`.
// Iterator uses polymorphism to dispatch the correct draw function for each instance
// at runtime, but that polymorphism is an implementation detail,
// **not** a mandatory part of the client code/interface.
class Drawable {
public:
   template <typename T>
   Drawable(T x) : self(make_shared<Model<T>>(move(x))) { }

   // Make this a freestanding friend function (instead of a member)
   // so that it shares the _exact_ same interface as other draw functions.
   // We can now embed Objects in Objects.
   friend void draw(const Drawable& x, ostream& out, size_t indent)
   {
      x.self->apply(out, indent);
   }

private:
   // Here we define our interface.
   struct Concept 
   {
      virtual ~Concept() = default; 
      virtual void apply(ostream&, size_t) const = 0;
   };

   // Here we define the model for a particular type value_type.
   template <typename T>
   struct Model final : Concept 
   {
      Model(T x) : data(move(x)) { }

      void apply(ostream& out, size_t indent) const override
      {
         draw(data, out, indent);
      }

      T data;
   };

   // Making this a shared_ptr to const data has a few really nice properties:
   // 1. We get automatic copy-on-write (CoW) semantics.
   //    Making a copy of ourselves just bumps the reference count.
   //
   // 2. The default special functions (move, assignment, etc.) work great.
   //    If this was a unique_ptr instead, we'd have to implement our own
   //    copy operators for Object, and have a virtual copy() function for
   //    Drawable.
   shared_ptr<const Concept> self;
};

// Let's make a "document" a vector of Objects, and say how to print it.
using Document = vector<Drawable>;

// Note how composable this design is. We can use the exact same interface
// for things like std::vector. Had this design demanded inheritance,
// we would have to make our own wrapper classes around vector, etc.
// (Mantis does this unfortunately often.)
void draw(const Document& x, ostream& out, size_t indent)
{
   const auto indentString = string(indent, ' ');

   out << indentString << "{" << endl;

   for (const auto& e : x) draw(e, out, indent + 4);

   out << indentString << "}" << endl;
}

// Actual use:
int main()
{
   Document d;

   d.push_back(42);
   d.push_back("O hi!");
   d.push_back(Foo(29));
   draw(d, cout, 0);

   cout << "-------------" << endl;

   d.push_back(d); // CoW!
   d.push_back("Ta da!");
   draw(d, cout, 0);

   return 0;
}
