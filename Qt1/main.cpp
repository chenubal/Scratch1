#include <QtCore/QCoreApplication>
#include <QtCore/QString>
#include <QtCore/QRegularExpression>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include  <iostream>
#include <functional>

template<typename T0>
static QString qFmt(const QString &fmt, T0 x) {
	return fmt.arg(x);
}

template<typename T0, typename ...T>
static QString qFmt(const QString &fmt, T0 x, T...args) {
	return qFmt(qFmt(fmt, x), args...);
}

void InfoText(QString const& s)
{
	std::cout << s.toStdString() << "\n";
}
template<typename ...T>

void InfoText(const QString &f, T...args)
{
	InfoText(qFmt(f, args...));
}

struct matcher
{
	matcher() : rx("*.im7")
	{
		rx.setPatternSyntax(QRegExp::Wildcard);
	}
	void operator()(QFileInfo const& f)
	{
		if (rx.exactMatch(f.filePath()))
		{
			w << f.absoluteFilePath();
			ts += f.size();
		}
	}
	QStringList w;
	unsigned ts = 0;
	QRegExp rx;
};

using fileVisitor_t = std::function<void(QFileInfo const&)>;
void visitDirectory(QString const& rootDir, fileVisitor_t visit, bool recursive = false)
{
	auto files = QDir(rootDir).entryInfoList(QDir::Files, QDir::Name);
	for (auto &&f : files) visit(f);
	if (recursive)
	{
		for (auto && d : QDir(rootDir).entryList(QDir::Dirs | QDir::NoDotAndDotDot))
			visitDirectory(rootDir + "/" + d, visit, recursive);
	}
};

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	InfoText(QString("jheers; % %3 %2 -> %1"), 55, "meier", 44.7);

	QRegExp rx("*.im7");
	rx.setPatternSyntax(QRegExp::Wildcard);
	InfoText("ok=%1", rx.exactMatch("/xz/abc/www.im7"));

	matcher m0;
	visitDirectory("D:/MyProjects/Jheers-Daten", std::bind(&matcher::operator(), &m0,std::placeholders::_1), true);
	InfoText("\nts=%1mb", float(m0.ts) / (1024 * 1024));
	QFileInfo info("abd\\gdsgfj/meier.jpg.müller");
	InfoText(info.path()+"/"+info.completeBaseName());

	QString ss("xy321fgfdg");
	InfoText(ss.mid(2));
	auto i = std::atoi(ss.mid(2).toLatin1());
	std::cout << "i=" << i << "\n";

	
	return a.exec();
}
