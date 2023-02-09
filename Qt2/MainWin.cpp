#include "MainWin.h"
#include <qlabel>
#include <qboxlayout>
#include <qlistwidget.h>
#include <qtablewidget.h>

MainWin::MainWin(QWidget *parent) : QMainWindow(parent)
{
   auto w = new QWidget();
   w->setMinimumSize(800, 600);
   auto layout = new QHBoxLayout();
   auto makeRBox = [this]
   {
      auto l = new QVBoxLayout();
      l->addLayout(makeBillsView());
      l->addLayout(makeTripsView());
      l->addLayout(makeReportView());
      return l;
   };
   layout->addLayout(makeBillingsView());
   layout->addLayout(makeRBox());

   w->setLayout(layout);
   setCentralWidget(w);
}

QLayout* MainWin::makeBillingsView() 
{ 
   auto w = new QListWidget(this);
   w->setMaximumSize(300, 800);
   w->addItem(new QListWidgetItem("b1"));
   w->addItem("b2");
   auto l = new QHBoxLayout();
   l->addWidget(w);
   return l; 
}

QLayout* MainWin::makeBillsView() 
{
   auto w = new QTableWidget(this);
   w->setColumnCount(2);
   w->setHorizontalHeaderLabels({ "Summe","Fahrer" });
   auto l = new QHBoxLayout();
   l->addWidget(w);
   return l;
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

