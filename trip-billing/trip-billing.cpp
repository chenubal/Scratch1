#include <iostream>
#include "items_json.h"
//#include "items.h"
#include <sstream>

using json = nlohmann::json;

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
       std::cout << b.trips << "\n-----------------------------\n" << b.bills << "\n";
       std::cout << b.invoice({ {std::string("Josef")},{std::string("Jannes")},{std::string("Luis")} }) << "\n";
          
       if (true)
       {
          jh::trips_t trips{ jh::trip_t{0,200,std::string("Josef")}, jh::trip_t{200,500,std::string("Luis")} };
          b.trips = json(trips); // trips_t -> json ->trips_t
          b.bills = json{ jh::bill_t{255.33,std::string("Josef")} };
          std::cout << json(b).dump(1) << "\n";
       }

   }
}

