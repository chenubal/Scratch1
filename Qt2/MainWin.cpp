#include "MainWin.h"
#include <qlabel>
#include <qboxlayout>
#include <qlistwidget.h>
#include <qtablewidget.h>
#include <qtextedit.h>
#include <qpushbutton.h>
#include <iostream>
#include <qdialog.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <ctime>


namespace fs = std::filesystem;
using path = fs::path;

namespace jh
{
	std::string todayStr()
	{
		auto t = std::time(nullptr);
		auto tm = *std::localtime(&t);

		std::ostringstream oss;
		oss << std::put_time(&tm, "%d-%m-%Y");
		return oss.str();
	}

	path next_free(path p)
	{
		auto fn = p.filename().string();
		auto i = 1u;
		while (fs::exists(p))
		{
			p.replace_filename(fn+ " " + std::to_string(i++));
		}
		return p;
	}

	path db_path()
	{
		auto dir = fs::current_path().append("billing_db");
		fs::create_directory(dir);
		return dir;
	}

	path backup_path()
	{
		auto dir = fs::current_path().append("backup");
		fs::create_directory(dir);
		return dir;
	}

	void toBackup(path const& p)
	{
		fs::rename(p, next_free(backup_path() / p.filename()));
	}

	template<class T>
	QLabel* makeLabel(T v, QString const& u)
	{
		auto lbl = new QLabel(QString("%1%2").arg(v).arg(u));
		lbl->setAlignment(Qt::AlignCenter);
		return lbl;
	};
}

MainWin::MainWin(QWidget* parent) : QMainWindow(parent)
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
	// Select last billing
	if (auto n = m_billingsView->count(); n > 0)
		m_billingsView->setCurrentRow(n - 1);

}

QLayout* MainWin::makeBillingsView()
{
	if (!m_billingsView)
	{
		m_billingsView = new QListWidget(this);
		m_billingsView->setMaximumSize(300, 800);
		connect(m_billingsView, &QListWidget::currentRowChanged, this, [this](int){updateAll();});

		auto addBtn = new QPushButton("Neu");
		connect(addBtn, &QPushButton::pressed, this, [this]{addBilling();});

		auto delBtn = new QPushButton("Entfernen");
		connect(delBtn, &QPushButton::pressed, this, [this]{delBilling();});

		auto btnBox = new QHBoxLayout();
	   btnBox->addWidget(addBtn);
		btnBox->addWidget(delBtn);

		auto mainBox = new QVBoxLayout();
		mainBox->addWidget(m_billingsView);
		mainBox->addLayout(btnBox);

		updateBillsTable();
		return mainBox;
	}
	return nullptr;
}

QLayout* MainWin::makeBillsView()
{
	if (!m_billTable)
	{
		m_billTable = new QTableWidget(this);
		m_billTable->setColumnCount(2);
		m_billTable->setHorizontalHeaderLabels({ "Summe","Fahrer" });

		auto addBtn = new QPushButton("Neuer Eintrag");
		connect(addBtn, &QPushButton::pressed, this, [this] { appendBill();} );
		auto delBtn = new QPushButton("Letzten Eintrag entfernen");
		connect(delBtn, &QPushButton::pressed, this, [this] {	delLastBill(); });

		auto btnBox = new QHBoxLayout();
		btnBox->addWidget(addBtn);
		btnBox->addWidget(delBtn);

		auto mainBox = new QVBoxLayout();
		mainBox->addWidget(m_billTable);
		mainBox->addLayout(btnBox);
		updateAll();
		return mainBox;
	}
	return nullptr;
}

QLayout* MainWin::makeTripsView()
{
	if (!m_tripTable)
	{
		m_tripTable = new QTableWidget(this);
		m_tripTable->setColumnCount(3);
		m_tripTable->setHorizontalHeaderLabels({ "Start","Ende","Fahrer" });

		auto addBtn = new QPushButton("Neuer Eintrag");
		connect(addBtn, &QPushButton::pressed, this, [this] { appendTrip(); });
		auto delBtn = new QPushButton("Letzten Eintrag entfernen");
		connect(delBtn, &QPushButton::pressed, this, [this] { delLastTrip(); });

		auto btnBox = new QHBoxLayout();
		btnBox->addWidget(addBtn);
		btnBox->addWidget(delBtn);

		auto mainBox = new QVBoxLayout();
		mainBox->addWidget(m_tripTable);
		mainBox->addLayout(btnBox);
		return mainBox;
	}
	return nullptr;
}

QLayout* MainWin::makeReportView()
{
	m_report = new QTextEdit();
	auto l = new QHBoxLayout();
	l->addWidget(m_report);
	return l;

}

void MainWin::updateBillsTable()
{
	if (m_billTable)
	{
		auto& bills = m_work.bills;
		m_billTable->clearContents();
		m_billTable->setRowCount(bills.size());
		for (auto&& [bill, i] : jh::zip(bills, jh::Loop(bills.size())))
		{
			m_billTable->setCellWidget(i, 0, jh::makeLabel(bill.amount," Euro"));
			m_billTable->setCellWidget(i, 1, jh::makeLabel(bill.driver.name.c_str(), ""));
		}
	}
}

void MainWin::updateTripsTable()
{
	if (m_tripTable)
	{
		auto& trips = m_work.trips;
		m_tripTable->clearContents();
		m_tripTable->setRowCount(trips.size());
		for (auto&& [trip, i] : jh::zip(trips, jh::Loop(trips.size())))
		{
			m_tripTable->setCellWidget(i, 0, jh::makeLabel(trip.start, " km"));
			m_tripTable->setCellWidget(i, 1, jh::makeLabel(trip.end, " km"));
			m_tripTable->setCellWidget(i, 2, jh::makeLabel(trip.driver.name.c_str(), ""));
		}
	}
}

void MainWin::updateAll()
{
	m_work.clear();
	if (m_billingsView && !m_billings.empty())
	{
		auto p = currentPath();
		m_work.load(p.string());
		m_work.name = p.filename().string();
	}
	updateBillsTable();
	updateTripsTable();
	updateReport();
}

void MainWin::updateReport()
{
	if (m_report)
	{
		auto s = m_work.invoiceAll();
		m_report->setText(s.c_str());
	}
}

void MainWin::load()
{
	if (auto const dbPath = jh::db_path(); fs::exists(dbPath))
	{
		m_billingsView->clear();
		m_billings.clear();
		for (auto const& file : fs::directory_iterator{ dbPath })
		{
			m_billingsView->setCurrentRow(0);
			if (file.is_directory())
			{
				auto const& fp = file.path();
				m_billings.emplace_back(fp.string());
				m_billingsView->addItem(fp.filename().string().c_str());
			}
		}
	}
}

void MainWin::addBilling()
{
	if (auto dir = jh::db_path(); fs::exists(dir))
	{
		dir = jh::next_free(dir / path("Abrechnung am " + jh::todayStr()));
		fs::create_directory(dir);
		jh::billing b(dir.filename().string());
		b.store(dir.string());
		load();
		m_billingsView->setCurrentRow(m_billingsView->count() - 1);
		updateAll();
	}
}
void MainWin::delBilling()
{
	if (auto r = m_billingsView->currentRow(); r >=0)
	{
		jh::toBackup(m_billings.at(r));
		load();
		m_billingsView->setCurrentRow(m_billingsView->count() - 1);
		updateAll();
	}
}

void MainWin::delLastBill()
{
	auto& b = m_work.bills;
	if (!b.empty())
	{
		auto d = std::next(b.begin(), b.size() - 1);
		b.erase(d);
	}
	m_work.store(currentPath().string());
	updateAll();
}

void MainWin::delLastTrip()
{
	auto& b = m_work.trips;
	if (!b.empty())
	{
		auto d = std::next(b.begin(), b.size() - 1);
		b.erase(d);
	}
	m_work.store(currentPath().string());
	updateAll();
}

 void MainWin::appendBill()
{
	jh::bill_t nBill{ 30,{ "Josef" } };
	auto& b = m_work.bills;
	if (!b.empty()) nBill.driver = b.back().driver;
	runBillEditor(nBill);
}

 void MainWin::appendTrip()
 {
	 jh::trip_t trip{ 125000, 125201,{ "Josef" } };
	 auto& b = m_work.trips;
	 if (!b.empty())
	 {
		 trip.start = b.back().end;
		 trip.end = trip.start + 15;
		 trip.driver = b.back().driver;
	 }
	 runTripEditor(trip);
 }

void MainWin::runBillEditor(jh::bill_t& bill)
{
	QStringList drvNames{ "Josef","Jannis","Luis" };
	auto dialog = new QDialog();
	auto amountBox = new QDoubleSpinBox();
	amountBox->setFixedWidth(80);
	amountBox->setRange(0, 100000);
	amountBox->setValue(bill.amount);
	connect(amountBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double v) {bill.amount = v; });

	auto acceptBtn = new QPushButton("Okay");
	connect(acceptBtn, &QPushButton::clicked, [dialog] {dialog->accept(); });
	auto cancelBtn = new QPushButton("Abbrechen");
	connect(cancelBtn, &QPushButton::clicked, [dialog] {dialog->reject(); });

	auto drvSelection = new QComboBox();
	drvSelection->addItems(drvNames);
	drvSelection->setCurrentIndex(0);
	connect(drvSelection, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int i) {bill.driver.name = drvNames.at(i).toStdString(); });

	auto hBox = new QHBoxLayout();
	hBox->addWidget(new QLabel("Betrag: "));
	hBox->addWidget(amountBox);
	hBox->addWidget(new QLabel("  Fahrer: "));
	hBox->addWidget(drvSelection);
	hBox->addSpacing(60);
	hBox->addWidget(acceptBtn);
	hBox->addWidget(cancelBtn);
	dialog->setLayout(hBox);
	if (dialog->exec() == QDialog::Accepted)
	{
		m_work.bills.emplace_back(bill);
		m_work.store(currentPath().string());
		updateAll();
	}
}

void MainWin::runTripEditor(jh::trip_t& trip)
{
	QStringList drvNames{ "Josef","Jannis","Luis" };
	auto dialog = new QDialog();
	auto startBox = new QSpinBox();
	startBox->setFixedWidth(80);
	startBox->setRange(0, 10'000'000);
	startBox->setValue(trip.start);
	startBox->setEnabled(false);
	//connect(startBox, QOverload<int>::of(&QSpinBox::valueChanged), [&](int v) {trip.start = v; });

   auto endBox = new QSpinBox();
	endBox->setFixedWidth(80);
	endBox->setRange(trip.start+1, 10'000'000);
	endBox->setValue(trip.end);
	connect(endBox, QOverload<int>::of(&QSpinBox::valueChanged), [&](int v) {trip.end = v; });

	auto acceptBtn = new QPushButton("Okay");
	connect(acceptBtn, &QPushButton::clicked, [dialog] {dialog->accept(); });
	auto cancelBtn = new QPushButton("Abbrechen");
	connect(cancelBtn, &QPushButton::clicked, [dialog] {dialog->reject(); });

	auto drvSelection = new QComboBox();
	drvSelection->addItems(drvNames);
	drvSelection->setCurrentIndex(0);
	connect(drvSelection, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int i) {trip.driver.name = drvNames.at(i).toStdString(); });

	auto hBox = new QHBoxLayout();
	hBox->addWidget(new QLabel("Start: "));
	hBox->addWidget(startBox);
	hBox->addWidget(new QLabel("Ende: "));
	hBox->addWidget(endBox);
	hBox->addWidget(new QLabel("  Fahrer: "));
	hBox->addWidget(drvSelection);
	hBox->addSpacing(60);
	hBox->addWidget(acceptBtn);
	hBox->addWidget(cancelBtn);
	dialog->setLayout(hBox);
	if (dialog->exec() == QDialog::Accepted)
	{
		m_work.trips.emplace_back(trip);
		m_work.store(currentPath().string());
		updateAll();
	}
}

path MainWin::currentPath() const
{
	auto n = std::max(0, m_billingsView->currentRow());
	return m_billings.at(n);
}


