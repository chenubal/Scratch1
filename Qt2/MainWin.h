#pragma once

#include <QtWidgets/QMainWindow>
class Qlayout;

class MainWin : public QMainWindow
{
    Q_OBJECT

public:
   MainWin(QWidget *parent = nullptr);
private:
   QLayout* makeBillingsView();
   QLayout* makeBillsView();
   QLayout* makeTripsView();
   QLayout* makeReportView();
 
};
