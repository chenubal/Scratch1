#include <iostream>
#include "items.h"
#include <sstream>

int main()
{
    //jh::bill_t x;
    //std::stringstream ss("20012\tjosef");
    //ss >> x;
    //std::cout << x << "\n";

    //jh::trips_t  trips{ {200,311,"Luis"},{300,322,"Jannes"} ,{400,500, "Josef"} ,{20,88,"Luis"} };
    //std::cout << trips << "\n";

    //jh::bills_t  bills{ {200,"Luis"},{300,"Jannes"} ,{400,"Josef"} ,{20,"Luis"} };
    //std::cout << bills << "\n";

    //std::cout << jh::total(trips) << "\n";
    //std::cout << jh::total(trips, { "Josef" }) << "\n";
    //std::cout << jh::total(bills) << "\n";
    //std::cout << jh::total(bills, { "Luis" }) << "\n";

    //{
    //   jh::billing b("test");
    //   b.trips = { {200,311,"Luis"},{300,322,"Jannes"} ,{400,500, "Josef"} ,{20,88,"Luis"} };
    //   b.bills = { {200,"Luis"},{300,"Jannes"} ,{400,"Josef"} ,{20,"Luis"} };
    //   std::cout << b.name << "\n";
    //   b.store();
    //}

    {
       jh::billing b("Abrechnung vom 06.02.2023");
       b.load();
       std::cout << b.trips << "\n";
       std::cout << b.bills << "\n";
       std::cout << b.invoice({ {"Josef"},{"Jannes"},{"Luis"} }) << "\n";
   }
}

