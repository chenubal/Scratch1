#include "MainWin.h"
#include <qlabel>
#include <qboxlayout>
#include <qlistwidget.h>
#include <qtablewidget.h>
#include "../trip-billing/items.h"


MainWin::MainWin(QWidget *parent) : QMainWindow(parent)
{
   auto makeBillViews = [this]
   {
      auto l = new QVBoxLayout();
      l->addLayout(makeBillsView());
      l->addLayout(makeTripsView());
      l->addLayout(makeReportView());
      return l;
   };
   auto mainLayout = [&, this]
   {
      auto l = new QHBoxLayout();
      l->addLayout(makeBillingsView());
      l->addLayout(makeBillViews());
      return l;
   };
   auto makeMainWidget = [&, this]
   {
      auto w = new QWidget();
      w->setMinimumSize(800, 600);
      w->setLayout(mainLayout());
      return w;
   };

   setCentralWidget(makeMainWidget());
   load();
}
QLayout* MainWin::makeBillingsView() 
{ 
   if (!billingsView)
   {
      billingsView = new QListWidget(this);
      billingsView->setMaximumSize(300, 800);
      auto l = new QHBoxLayout();
      l->addWidget(billingsView);
      return l;
   }
   return nullptr; 
}

QLayout* MainWin::makeBillsView() 
{
   if (!billTable)
   {
      billTable = new QTableWidget(this);
      billTable->setColumnCount(2);
      billTable->setHorizontalHeaderLabels({ "Summe","Fahrer" });
      auto l = new QHBoxLayout();
      l->addWidget(billTable);
      return l;
   }
   return nullptr;
}

 QLayout* MainWin::makeTripsView() 
 { 
    auto w = new QTableWidget(this);
    w->setColumnCount(3);
    w->setHorizontalHeaderLabels({ "Start","Ende","Fahrer" });
    auto l = new QHBoxLayout();
    l->addWidget(w);
    return l;

 }

 QLayout* MainWin::makeReportView()
 {
    auto l = new QHBoxLayout();
    l->addWidget(new QLabel("D"));
    return l;

 }

 void MainWin::load() 
 {
    auto db_path = fs::current_path().append("billing_db");
    fs::create_directory(db_path);
    if (fs::exists(db_path))
    {
       billingsView->clear();
       for (auto const& d :  fs::directory_iterator{ db_path })
       {
          if (d.is_directory())
          {
             billings.emplace_back(d.path().string());
             billingsView->addItem(d.path().filename().string().c_str());
          }
       }
    }
 }

