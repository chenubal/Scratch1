#include <iostream>
#include <list>
#include <string>
#include <cstdlib>
#include <ctime>
#include <typeinfo>

#define SHOWVAR(variable) std::cout << #variable << " = " << variable << std::endl;

struct Item {};
struct Base : Item {};
struct AbstractDisplay : Base { virtual std::string show() const = 0; };

struct ViewProvider : Base 
{
	virtual struct View* createView() const = 0;
protected:
	template <typename T> 
	static inline View* createViewContainerBase();
private:
	
	template <int, typename> 
	struct AddViews;
	
	template <int, typename> 
	struct GetViewContainer;
	
	template <typename T> 
	static void addView(View* m, int n, int numViews) 
	{
		AddViews<T::numViewTypes - 1, T>()(m, n, numViews);
	}
	template <typename T> 
	static View* getViewContainer(int n) 
	{ 
		return GetViewContainer<T::numViewContainerTypes - 1, T>()(n); 
	}
};

struct View : Item 
{
	std::list<AbstractDisplay*> display;
	template <typename T> 
	void addSpecificDisplay(int amount) 
	{
		for (int i = 0; i < amount; i++)
			display.emplace_back(new T);
	}
	virtual int capacity() const = 0;
	virtual std::string show() const = 0;
};

template <int N, typename T>
struct ViewProvider::GetViewContainer 
{ 
	View* operator() (int n)
	{
		if (n == N)
			return new typename T::template viewContainerType<N>;  // Template disambiguator
		return GetViewContainer<N - 1, T>()(n);
	}
};

template <typename T>
struct ViewProvider::GetViewContainer<-1, T> 
{
	View* operator() (int) { return nullptr; }  // End of recursion
};

template <int N, typename T>
struct ViewProvider::AddViews 
{  
	void operator() (View* m, int n, int numViews) 
	{
		if (n == N) 
		{
			m->template addSpecificDisplay<typename T::template viewType<N>>(numViews);  // Template disambiguator needed in two places!
			return;
		}
		AddViews<N - 1, T>()(m, n, numViews);
	}
};

template <typename T>
struct ViewProvider::AddViews<-1, T> 
{
	void operator() (View*, int, int) {}  // End of recursion
};

template <int, typename ...> struct NthType;  

template <typename Head, typename... Tail>
struct NthType<0, Head, Tail...> {	using type = Head;};

template <int N, typename Head, typename... Tail>
struct NthType<N, Head, Tail...> : NthType<N - 1, Tail...> {};

template <typename...> struct DisplayTypes;

template <typename Head, typename... Tail>
struct DisplayTypes<Head, Tail...> 
{
	using primaryMissile = Head;
	static const int numViewTypes = sizeof...(Tail) + 1;
	
	template <int N>
	using viewType = typename NthType<N, Head, Tail...>::type;
};

template <typename...> struct ViewContainerTypes;

template <typename Head, typename... Tail>
struct ViewContainerTypes<Head, Tail...>
{
	using primaryViewContainer = Head;
	static const int numViewContainerTypes = sizeof...(Tail) + 1;
	
	template <int N> 
	using viewContainerType = typename NthType<N, Head, Tail...>::type;
};

/////////////////////////////// Samples ////////////////////////////////////////////////

struct DisplayA : AbstractDisplay { std::string show() const override { return "DisplayA"; } };
struct DisplayB : AbstractDisplay { std::string show() const override { return "DisplayB"; } };
struct DisplayC : AbstractDisplay { std::string show() const override { return "DisplayC"; } };
struct DisplayD : AbstractDisplay { std::string show() const override { return "DisplayD"; } };
struct DisplayE : AbstractDisplay { std::string show() const override { return "DisplayE"; } };

struct ViewDADB : View, DisplayTypes<DisplayA, DisplayB> 
{  
	int capacity() const override { return 20; }
	std::string show() const override { return "View_DA_DB"; }
};

struct ViewDEDD : View, DisplayTypes<DisplayD, DisplayE>
{  
	int capacity() const override { return 20; }
	std::string show() const override { return "View_DD_DE"; }
};


struct ModelA : ViewProvider, DisplayTypes<DisplayA, DisplayB, DisplayC>, ViewContainerTypes<ViewDADB, ViewDEDD> 
{
	View* createView() const override { return createViewContainerBase<ModelA>(); }
};

struct ModelB : ViewProvider, DisplayTypes<DisplayD, DisplayE>, ViewContainerTypes<ViewDEDD> 
{
	View* createView() const override { return createViewContainerBase<ModelB>(); }
};

template <typename T>
inline View* ViewProvider::createViewContainerBase() 
{
	// Provides random views (as an example)
	const int m = std::rand() % T::numViewContainerTypes;
	auto view = getViewContainer<T>(m);  // Template recursion.
	const int r = std::rand() % T::numViewTypes;
	const int numViews = std::rand() % view->capacity() / 2 + view->capacity() / 2 + 1;
	addView<T>(view, r, numViews);
	std::cout << "View type = " << m << std::endl;  /////
	std::cout << "Model type = " << r << std::endl;  /////
	return view;
}

int main() 
{
	const int srand_seed = static_cast<int>(std::time(nullptr));
	std::srand(srand_seed);

	ViewProvider* model1 = new ModelA;
	View* view1 = model1->createView();

	SHOWVAR(view1->show())  
	SHOWVAR(view1->display.size())  
	SHOWVAR(view1->display.front()->show())  

	ViewProvider* model2 = new ModelB;
	View* view2 = model2->createView();
	SHOWVAR(view2->show())
	SHOWVAR(view2->display.size())
	SHOWVAR(view2->display.front()->show())
}