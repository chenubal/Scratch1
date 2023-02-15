#pragma once

#include <QtWidgets/QMainWindow>
#include <filesystem>
#include "../trip-billing/items.h"

namespace fs = std::filesystem;
using path = fs::path;

class Qlayout;
class QListWidget;
class QTableWidget;
class QTextEdit;

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
   void updateBillsTable();
   void updateTripsTable();
   void updateAll();
   void updateReport();
   void load();
   std::vector<path> m_billings;
   QListWidget* m_billingsView = nullptr;
   QTableWidget* m_billTable = nullptr;
   QTableWidget* m_tripTable = nullptr;
   QTextEdit* m_report;
   jh::billing m_work;
};
