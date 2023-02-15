#include "MainWin.h"
#include <qlabel>
#include <qboxlayout>
#include <qlistwidget.h>
#include <qtablewidget.h>

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
		auto l = new QHBoxLayout();
		l->addWidget(m_billingsView);
		connect(m_billingsView, &QListWidget::currentRowChanged, this, [this](int) {updateAll(); });
		m_billingsView->setCurrentRow(0);
		updateBillsTable();
		return l;
	}
	return nullptr;
}

QLayout* MainWin::makeBillsView()
{
	if (!m_billTable)
	{
		m_billTable = new QTableWidget(this);
		auto l = new QHBoxLayout();
		l->addWidget(m_billTable);
		updateAll();
		return l;
	}
	return nullptr;
}

QLayout* MainWin::makeTripsView()
{
	if (!m_tripTable)
	{
		m_tripTable = new QTableWidget(this);
		m_tripTable->setColumnCount(3);
		auto l = new QHBoxLayout();
		l->addWidget(m_tripTable);
		return l;
	}
	return nullptr;
}

QLayout* MainWin::makeReportView()
{
	auto l = new QHBoxLayout();
	l->addWidget(new QLabel("D"));
	return l;

}

void MainWin::updateBillsTable()
{
	if (m_billTable && m_billingsView && !m_billings.empty())
	{
		auto n = std::max(0, m_billingsView->currentIndex().row());
		m_work.load(m_billings.at(n).string());
		auto& bills = m_work.bills;
		m_billTable->clear();
		m_billTable->setColumnCount(2);
		m_billTable->setRowCount(bills.size());
		m_billTable->setHorizontalHeaderLabels({ "Summe","Fahrer" });
		for (auto&& [x, i] : jh::zip(bills, jh::Loop(bills.size())))
		{
			auto fmt = [](auto v, QString const& u) {auto l = new QLabel(QString("%1%2").arg(v).arg(u)); l->setAlignment(Qt::AlignCenter); return l; };
			m_billTable->setCellWidget(i, 0, fmt(x.amount, " Euro"));
			m_billTable->setCellWidget(i, 1, fmt(x.driver.name.c_str(), ""));
		}
	}
}

void MainWin::updateTripsTable()
{
	if (m_tripTable && m_billingsView && !m_billings.empty())
	{
		auto n = std::max(0, m_billingsView->currentIndex().row());
		m_work.load(m_billings.at(n).string());
		auto& trips = m_work.trips;
		m_tripTable->clear();
		m_tripTable->setColumnCount(3);
		m_tripTable->setRowCount(trips.size());
		m_tripTable->setHorizontalHeaderLabels({ "Start","Ende","Fahrer" });
		for (auto&& [x, i] : jh::zip(trips, jh::Loop(trips.size())))
		{
			auto fmt = [](auto v, QString const& u) {auto l = new QLabel(QString("%1%2").arg(v).arg(u)); l->setAlignment(Qt::AlignCenter); return l; };
			m_tripTable->setCellWidget(i, 0, fmt(x.start, " km"));
			m_tripTable->setCellWidget(i, 1, fmt(x.end, " km"));
			m_tripTable->setCellWidget(i, 2, fmt(x.driver.name.c_str(), ""));
		}
	}
}

void MainWin::updateAll() 
{
	updateBillsTable();
	updateTripsTable();
}

void MainWin::load()
{
	auto db_path = fs::current_path().append("billing_db");
	fs::create_directory(db_path);
	if (fs::exists(db_path))
	{
		m_billingsView->clear();
		for (auto const& d : fs::directory_iterator{ db_path })
		{
			m_billingsView->setCurrentRow(0);
			if (d.is_directory())
			{
				m_billings.emplace_back(d.path().string());
				m_billingsView->addItem(d.path().filename().string().c_str());
			}
		}
	}
}

