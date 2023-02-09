#pragma once

#include <QtWidgets/QMainWindow>
#include <filesystem>

namespace fs = std::filesystem;
using path = fs::path;

class Qlayout;
class QListWidget;
class QTableWidget;

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
   void load();
   std::vector<path> billings;
   QListWidget* billingsView = nullptr;
   QTableWidget* billTable = nullptr;
};
