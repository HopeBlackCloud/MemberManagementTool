#include "Makbuz.h"
#include "Definitions.h"
#include "PaymentsWindow.h"
#include "DatabaseManager.h"

PaymentsWindow::PaymentsWindow(int memID, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	qry = NULL;
	model = NULL;

	model = new QSqlQueryModel();
	qry = new QSqlQuery(QSqlDatabase::defaultConnection);

	LoadPayments(memID);
}

PaymentsWindow::~PaymentsWindow()
{
	SAFE_DELETE(qry);
	SAFE_DELETE(model);
}

void PaymentsWindow::LoadPayments(int memID)
{
	Database::Connect();

	qry->prepare(QString("SELECT pay_no, pay_serial, pay_code, pay_type, pay_text, pay_date, pay_responsible, pay_amount, pay_year FROM payments where pay_no = %1").arg(memID));
	
	qry->exec();

	model->setQuery(*qry);

	SetHeaderTitle(model);
	
	ui.tableViewPayment->setModel(model);

	int toplam = 0;
	for (int i = 0; i < model->rowCount(); i++)
	{
		toplam += model->index(i, 7).data().toInt();
	}

	ui.labelTotalPayments->setText(QString::fromLocal8Bit("Üyenin toplam ödeme tutarý : %1 TL").arg(toplam));

	Database::Disconnect();
}

void PaymentsWindow::SetHeaderTitle(QSqlQueryModel *model)
{
	model->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("Üye No"));
	model->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("Makbuz No"));
	model->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("Makbuz Kodu"));
	model->setHeaderData(3, Qt::Horizontal, QString::fromLocal8Bit("Ödeme Türü"));
	model->setHeaderData(4, Qt::Horizontal, QString::fromLocal8Bit("Açýklamasý"));
	model->setHeaderData(5, Qt::Horizontal, QString::fromLocal8Bit("Makbuz Tarihi"));
	model->setHeaderData(6, Qt::Horizontal, QString::fromLocal8Bit("Makbuz Sorumlusu"));
	model->setHeaderData(7, Qt::Horizontal, QString::fromLocal8Bit("Ödeme Miktarý"));
	model->setHeaderData(8, Qt::Horizontal, QString::fromLocal8Bit("Ödenen Yýllar"));
}
