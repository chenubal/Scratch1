#include "MainWin.h"
#include <qlabel>
#include <qboxlayout>
#include <qlistwidget.h>
#include <qtablewidget.h>
#include <qtextedit.h>
#include <qpushbutton.h>

template<class T>
QLabel* makeLabel(T v, QString const& u) 
{
	auto lbl = new QLabel(QString("%1%2").arg(v).arg(u)); 
	lbl->setAlignment(Qt::AlignCenter); 
	return lbl; 
};

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
		auto l = new QVBoxLayout();
		l->addWidget(m_billingsView);
		connect(m_billingsView, &QListWidget::currentRowChanged, this, [this](int) {updateAll(); });
		auto button = new QPushButton("Add billing");
		l->addWidget(button);
		connect(button, &QPushButton::pressed, this, [this]() 
		{
			auto path = fs::current_path().append("billing_db");
			fs::create_directory(path);
			path.append("New billing");
			fs::create_directory(path);

			jh::billing b("New billing");
			b.store(path.string());
			load();
			m_billingsView->setCurrentRow(m_billingsView->count() - 1);
			updateAll();
		});

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
		m_billTable->setColumnCount(2);
		m_billTable->setHorizontalHeaderLabels({ "Summe","Fahrer" });
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
			m_billTable->setCellWidget(i, 0, makeLabel(bill.amount, " Euro"));
			m_billTable->setCellWidget(i, 1, makeLabel(bill.driver.name.c_str(), ""));
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
			m_tripTable->setCellWidget(i, 0, makeLabel(trip.start, " km"));
			m_tripTable->setCellWidget(i, 1, makeLabel(trip.end, " km"));
			m_tripTable->setCellWidget(i, 2, makeLabel(trip.driver.name.c_str(), ""));
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
	auto db_path = fs::current_path().append("billing_db");
	fs::create_directory(db_path);
	if (fs::exists(db_path))
	{
		m_billingsView->clear();
		m_billings.clear();
		for (auto const& file : fs::directory_iterator{ db_path })
		{
			m_billingsView->setCurrentRow(0);
			if (file.is_directory())
			{
				m_billings.emplace_back(file.path().string());
				m_billingsView->addItem(file.path().filename().string().c_str());
			}
		}
	}
}

