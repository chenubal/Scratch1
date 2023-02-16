#include "MainWin.h"
#include <qlabel>
#include <qboxlayout>
#include <qlistwidget.h>
#include <qtablewidget.h>
#include <qtextedit.h>
#include <qpushbutton.h>
#include <iostream>
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
}

QLayout* MainWin::makeBillingsView()
{
	if (!m_billingsView)
	{
		m_billingsView = new QListWidget(this);
		m_billingsView->setMaximumSize(300, 800);
		auto vBox = new QVBoxLayout();
		vBox->addWidget(m_billingsView);
		connect(m_billingsView, &QListWidget::currentRowChanged, this, [this](int) {updateAll(); });

		auto addBtn = new QPushButton("Neu");
		connect(addBtn, &QPushButton::pressed, this, [this]{ addBilling();});

		auto delBtn = new QPushButton("Entfernen");
		connect(delBtn, &QPushButton::pressed, this, [this] {delBilling();});

		auto btnBox = new QHBoxLayout();
	   btnBox->addWidget(addBtn);
		btnBox->addWidget(delBtn);
		vBox->addLayout(btnBox);

		m_billingsView->setCurrentRow(0);
		updateBillsTable();
		return vBox;
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

		auto btnBox = new QHBoxLayout();
		auto addBtn = new QPushButton("Neuer Eintrag");
		connect(addBtn, &QPushButton::pressed, this, [this] 
		{
			auto& b = m_work.bills;
			if (b.empty())
				b.emplace_back(jh::bill_t{ 100,{"Josef"} });
			else
			{
				auto& bb = b.back();
				b.emplace_back(jh::bill_t{ bb.amount+1,bb.driver});
			}
			auto n = std::max(0, m_billingsView->currentRow());
			auto path = m_billings.at(n);
			m_work.store(path.string());
			updateAll();

		});
		auto delBtn = new QPushButton("Letzten Eintrag entfernen");
		connect(delBtn, &QPushButton::pressed, this, [this]
		{
			auto& b = m_work.bills;
			if (!b.empty())
			{
				auto d = std::next(b.begin(), b.size() - 1);
				b.erase(d);
			}
			auto n = std::max(0, m_billingsView->currentRow());
			auto path = m_billings.at(n);
			m_work.store(path.string());
			updateAll();

		});
		btnBox->addWidget(addBtn);
		btnBox->addWidget(delBtn);

		auto vBox = new QVBoxLayout();
		vBox->addWidget(m_billTable);
		vBox->addLayout(btnBox);
		updateAll();
		return vBox;
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
		auto l = new QHBoxLayout();
		l->addWidget(m_tripTable);
		return l;
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
		auto n = std::max(0, m_billingsView->currentRow());
		auto path = m_billings.at(n);
		m_work.load(path.string());
		m_work.name = path.filename().string();
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


