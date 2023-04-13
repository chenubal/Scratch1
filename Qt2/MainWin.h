#pragma once

#include <QtWidgets/QMainWindow>
#include <filesystem>
#include "../trip-billing/items.h"


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
   void addBilling();
   void delBilling();
   void delLastBill();
   void appendBill();
   void delLastTrip();
   void appendTrip();
   void runBillEditor(jh::bill_t& bill);
   void runTripEditor(jh::trip_t& trip, bool enable);
   std::filesystem::path currentPath() const;
   std::vector<std::filesystem::path> m_billings;
   QListWidget* m_billingsView = nullptr;
   QTableWidget* m_billTable = nullptr;
   QTableWidget* m_tripTable = nullptr;
   QTextEdit* m_report;
   jh::billing m_work;
   int lastEnd = 125'000;
   QStringList drvNames{ "Josef","Jannis","Luis" };
};
