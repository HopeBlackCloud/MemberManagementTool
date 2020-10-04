#include "IncomeExpense.h"
#include "DatabaseManager.h"
#include "Expense.h"
#include "Definitions.h"

#include <QFile>
#include <QPainter>
#include <QFileDialog>
#include <QPdfWriter>

#include "SettingsManager.h"

IncomeExpense::IncomeExpense(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	LoadIncomeExpense();

	modelQuery = new QSqlQueryModel();
	qry = new QSqlQuery(QSqlDatabase::defaultConnection);

	
	connect(ui.toolButtonShowAll,      SIGNAL(clicked()), this, SLOT(LoadIncomeExpense()));
	connect(ui.toolButtonQueryWithDate, SIGNAL(clicked()), this, SLOT(QueryWithDate()));
	connect(ui.toolButtonExportPDF,     SIGNAL(clicked()), this, SLOT(ExportPDFClicked()));
	connect(ui.toolButtonEnterExpense,  SIGNAL(clicked()), this, SLOT(EnterExpenseClicked()));	
}

void IncomeExpense::QueryWithDate()
{
	FillTableView(3, ui.dateTimeEditDateStart->text(), ui.dateTimeEditDateEnd->text());
}

void IncomeExpense::EnterExpenseClicked()
{
	Expense *expense = new Expense(this);

	expense->setAttribute(Qt::WA_DeleteOnClose);
	expense->exec();

	LoadIncomeExpense();
}

void IncomeExpense::LoadIncomeExpense()
{
	ui.dateTimeEditDateStart->setDate(QDateTime::currentDateTime().date());
	ui.dateTimeEditDateEnd->setDate(QDateTime::currentDateTime().date());

	Database::Connect();

	model = new QSqlTableModel(this);
	model->setTable("payments");
	model->select();

	modelExp  = new QSqlTableModel(this);
	modelExp->setTable("expenses");
	modelExp->select();
	modelExp->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("Fatura No"));
	modelExp->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("Fatura Tarihi"));
	modelExp->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("Fatura Sorumlusu"));
	modelExp->setHeaderData(3, Qt::Horizontal, QString::fromLocal8Bit("Fatura Miktarý"));
	modelExp->setHeaderData(4, Qt::Horizontal, QString::fromLocal8Bit("Fatura Seri/Sýra"));
	modelExp->setHeaderData(5, Qt::Horizontal, QString::fromLocal8Bit("Fatura Açýklamasý"));
	modelExp->setHeaderData(6, Qt::Horizontal, QString::fromLocal8Bit("Ticari Ünvaný"));
	
	ui.tableViewExpenses->setModel(modelExp);

	model->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("Üye No"));
	model->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("Gelir Tarihi"));
	model->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("Gelir Sorumlusu"));
	model->setHeaderData(3, Qt::Horizontal, QString::fromLocal8Bit("Gelir Miktarý"));
	model->setHeaderData(4, Qt::Horizontal, QString::fromLocal8Bit("Gelir Türü"));
	model->setHeaderData(5, Qt::Horizontal, QString::fromLocal8Bit("Makbuz No"));
	model->setHeaderData(6, Qt::Horizontal, QString::fromLocal8Bit("Makbuz Kodu"));
	model->setHeaderData(7, Qt::Horizontal, QString::fromLocal8Bit("Gelir Açýklamasý"));
	model->setHeaderData(8, Qt::Horizontal, QString::fromLocal8Bit("Gelir Yýllarý"));

	ui.tableViewIncome->setModel(model);
	ui.tableViewIncome->setColumnHidden(6, true);
	ui.tableViewIncome->setColumnHidden(8, true);

	int toplamIncome = 0;
	for (int i = 0; i < model->rowCount(); i++)
	{
		toplamIncome += model->index(i, 3).data().toInt();
	}

	ui.labelTotalIncomes->setText(QString::fromLocal8Bit(" %1 TL").arg(toplamIncome));

	int toplamExpense = 0;
	for (int i = 0; i < modelExp->rowCount(); i++)
	{
		toplamExpense += modelExp->index(i, 3).data().toInt();
	}

	ui.labelTotalExpenses->setText(QString::fromLocal8Bit(" %1 TL").arg(toplamExpense));

	ui.labelTotalKasa->setText(QString::fromLocal8Bit(" %1 TL").arg(toplamIncome - toplamExpense));

	Database::Disconnect();
}

IncomeExpense::~IncomeExpense()
{

}

void IncomeExpense::SetHeaderTitle(QSqlQueryModel *model)
{
	model->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("Üye No"));
	model->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("Gelir Tarihi"));
	model->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("Gelir Sorumlusu"));
	model->setHeaderData(3, Qt::Horizontal, QString::fromLocal8Bit("Gelir Miktarý"));
	model->setHeaderData(4, Qt::Horizontal, QString::fromLocal8Bit("Gelir Türü"));
	model->setHeaderData(5, Qt::Horizontal, QString::fromLocal8Bit("Makbuz No"));
	model->setHeaderData(6, Qt::Horizontal, QString::fromLocal8Bit("Makbuz Kodu"));
	model->setHeaderData(7, Qt::Horizontal, QString::fromLocal8Bit("Gelir Açýklamasý"));
	model->setHeaderData(8, Qt::Horizontal, QString::fromLocal8Bit("Gelir Yýllarý"));
}

void IncomeExpense::FillTableView(int type, QString startDate, QString endDate)
{
	Database::Connect();

	if (type == 3)
	{
		if(ui.comboBoxPayType->currentText() == "Hepsi")
			qry->prepare("select pay_no, pay_date, pay_responsible, pay_amount, pay_type, pay_serial, pay_code, pay_text, pay_year from payments WHERE pay_date >= '" + startDate + "' and pay_date <= '" + endDate + "'");
		else
			qry->prepare("select pay_no, pay_date, pay_responsible, pay_amount, pay_type, pay_serial, pay_code, pay_text, pay_year from payments WHERE pay_date >= '" + startDate + "' and pay_date <= '" + endDate + "' and pay_type ='"+ui.comboBoxPayType->currentText()+"'");
	}

	qry->exec();

	modelQuery->setQuery(*qry);

	SetHeaderTitle(modelQuery);

	ui.tableViewIncome->setModel(modelQuery);
	ui.tableViewIncome->setColumnHidden(6, true);
	ui.tableViewIncome->setColumnHidden(8, true);

	int toplamIncome = 0;
	for (int i = 0; i < modelQuery->rowCount(); i++)
	{
		toplamIncome += modelQuery->index(i, 3).data().toInt();
	}

	ui.labelTotalIncomes->setText(QString::fromLocal8Bit(" %1 TL").arg(toplamIncome));

	int toplamExpense = 0;
	for (int i = 0; i < modelQuery->rowCount(); i++)
	{
		toplamExpense += modelQuery->index(i, 3).data().toInt();
	}

	ui.labelTotalExpenses->setText(QString::fromLocal8Bit(" %1 TL").arg(toplamExpense));

	ui.labelTotalKasa->setText(QString::fromLocal8Bit(" %1 TL").arg(toplamIncome - toplamExpense));

	Database::Disconnect();
}

void IncomeExpense::ExportPDFClicked()
{
	QString path = QApplication::applicationDirPath() ;
	
	QFileDialog fileDlg;
	QString fileName = fileDlg.getSaveFileName(this, QString::fromLocal8Bit("Dosyaya bir isim veriniz ..."), path, "*.pdf");
	fileName+=".pdf";

	QStringList headers;
	headers <<QString::fromLocal8Bit("Gelir Tarihi")
		    <<QString::fromLocal8Bit("Gelir Sorumlusu")
		    <<QString::fromLocal8Bit("Gelir Miktarý")
			<<QString::fromLocal8Bit("Gelir Türü")
		    <<QString::fromLocal8Bit("Makbuz No");

	QFile pdfFile(fileName);
    if (pdfFile.open(QIODevice::WriteOnly))
	{
		QPdfWriter* writer = new QPdfWriter(&pdfFile);
		QPainter* painter = new QPainter(writer);
		writer->setPageSize(QPagedPaintDevice::A4);

		int x = 0;

		int width  = painter->window().width();
		int height = painter->window().height();

		QRect rect(0, 0, width, height/20.0);
		QRect rectDate(0, height/20.0 - 200.0, width, 200);
		
		QFont font;
		font.setBold(true);
		font.setPointSize(16);
		painter->setFont(font);
		painter->drawText(rect, Qt::AlignCenter,SettingsManager::organizationSettings.orgName);
		
		painter->drawLine(0, height/20 - 600.0, width, height/20.0 - 600.0);
		
		painter->drawLine(0, height/20.0 + 10.0, width, height/20.0 + 10.0);

		int y = height/20.0 + 10.0 + 40.0;

		font.setPointSize(10);
		painter->setFont(font);

		for (int i = 0; i < headers.count(); i++)
		{	
			QRect rec;
			rec = QRect(x, y, (width/headers.count()), 300);
			painter->drawText(rec, Qt::AlignBottom | Qt::AlignCenter , headers.at(i));
			painter->drawRect(rec);
			x += width/headers.count();
		}

		y += 300;

		x = 0;
		
		font.setBold(false);
		painter->setFont(font);
		painter->drawText(rectDate, Qt::AlignRight,
			              QString("Tarih: %1").arg(QDateTime::currentDateTime().toString("dd.MM.yyyy")));
		for (int i = 0; i < model->rowCount(); i++)
		{
			for (int j = 0; j < 8; j++)
			{	
				if (j == 0 || j==6 || j==7)
				continue;

				QRect rec(x,y,(width/headers.count()),300);
				painter->drawText(rec, Qt::AlignBottom | Qt::AlignCenter | Qt::TextWordWrap, model->index(i, j).data().toString());
				painter->drawRect(rec);
				x += width/headers.count();
			}

			x = 0;
			y+= 300;

			if (y > height)
			{
				writer->newPage();
				y = 0;
			}
		}

		painter->end();

		SAFE_DELETE(painter);
		SAFE_DELETE(writer);
		pdfFile.close();
	}
}
