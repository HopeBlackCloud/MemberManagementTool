#include "Makbuz.h"
#include "MemberFee.h"
#include "SMSManager.h"
#include "DatabaseManager.h"
#include "SettingsManager.h"
#include "PaymentsWindow.h"
#include "Definitions.h"
#include "SMSPreviewWindow.h"

#include <QDateTime>

MemberFee::MemberFee(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	qry = NULL;
	model = NULL;

	model = new QSqlQueryModel();
	qry = new QSqlQuery(QSqlDatabase::defaultConnection);

	ui.labelMemberCount->setText("");

	ChangeButtonStatus(false);
	
	LoadFees();

	MakeConnections();
}

MemberFee::~MemberFee()
{
	SAFE_DELETE(qry);
	SAFE_DELETE(model);
}

void MemberFee::LoadFees()
{
	ChangeButtonStatus(false);

	ui.toolButtonBorclandir->setEnabled(Database::GetFeeLoaded());

	Database::Connect();

	qry->prepare("SELECT f.f_id, m.mem_name, m.mem_surname, m.mem_fathername, m.mem_cellphone, m.mem_membershipdate, f.f_paidyears, f.f_notpaidyears FROM membersfee f, membersinfo m where f.f_id = m.mem_no and m.mem_status = 'Aktif' ORDER BY mem_no DESC");
	
	qry->exec();

	model->setQuery(*qry);

	SetHeaderTitle(model);
	
	ui.tableViewFee->setModel(model);

	ui.labelMemberCount->setText("");

	Database::Disconnect();
}

void MemberFee::BorcluClicked()
{
	Database::Connect();

	qry->prepare("SELECT f.f_id, m.mem_name, m.mem_surname, m.mem_fathername, mem_cellphone, m.mem_membershipdate, f.f_paidyears, f.f_notpaidyears FROM membersfee f, membersinfo m where f.f_id = m.mem_no and f.f_notpaidyears <> ' ' ORDER BY mem_no DESC");
	
	qry->exec();

	model->setQuery(*qry);

	SetHeaderTitle(model);
	
	ui.tableViewFee->setModel(model);

	ui.labelMemberCount->setText("");

	Database::Disconnect();
}

void MemberFee::BorclandirClicked()
{
	Database::AddAnnualyMemberFee(QString("%1").arg(QDateTime::currentDateTime().date().year()));
	ui.toolButtonBorclandir->setEnabled(false);
	Database::AddFeeLoaded(false);

	ui.labelMemberCount->setText("");

	LoadFees();
}

void MemberFee::BorcluSMSClicked()
{
	QString smsText = "";
	short count = model->rowCount();
	QMap<QString, QString> smsTelTexts;
	QString borclu = "";

	QStringList years;

	for (int i = 0; i < count; i++)
	{
		borclu = model->index(i, 7).data().toString();
		
		if (!borclu.trimmed().isEmpty())
		{
			double totalBorc = 0;
			years = model->index(i, 7).data().toString().split(",");
			years.removeLast();

			for (int j = 0; j < years.count(); j++)
			{
				totalBorc += Database::GetFeePerYear(years.at(j).toInt());
			}
	
			if (totalBorc > 0)
			{
				smsText = QString::fromLocal8Bit("Sayin %1 %2, [%3 - %4] yýllarýna ait, toplam %5TL aidat borcunuz bulunmaktadýr. Lütfen ödemenizi yapýnýz.")
												.arg(model->index(i, 1).data().toString())
												.arg(model->index(i, 2).data().toString())
												.arg(years.first())
												.arg(years.last())
												.arg(totalBorc);
			}
			else
			{
				smsText = QString::fromLocal8Bit("Sayin %1 %2, vakfýmýza aidat borcunuz bulunmamaktadýr. Yonetim Kurulu olarak size tesekkür ederiz.").arg(model->index(i, 1).data().toString())
												.arg(model->index(i, 2).data().toString());
			}


			smsTelTexts.insert(smsText, model->index(i, 4).data().toString());
		}
	}

	SMSPreviewWindow* smsPreviewWindow = new SMSPreviewWindow(QString::fromLocal8Bit("Mesajýnýz [%1] üyeye gönderilecektir.").arg(smsTelTexts.count()), 
		                                                      smsText, this);
	int result = smsPreviewWindow->exec();

	if (result == QDialog::Accepted)
	{
		SMSManager::GetInstance()->SendMultipleSMS(smsTelTexts);
	}
}

void MemberFee::QueryNameClicked()
{
	Database::Connect();
	                                                                                                                                                                                                  
	if (ui.lineEditQueryName->text() != "" && ui.lineEditQuerySurname->text() != "")
	{
		qry->prepare("SELECT f.f_id, m.mem_name, m.mem_surname, m.mem_fathername, mem_cellphone, m.mem_membershipdate, f.f_paidyears, f.f_notpaidyears FROM membersfee f, membersinfo m where f.f_id = m.mem_no and  m.mem_name = '"+ ui.lineEditQueryName->text() +"' and m.mem_surname = '"+ ui.lineEditQuerySurname->text() +"'  ORDER BY mem_no DESC");
	}
	else if (ui.lineEditQueryName->text() != "")
	{
		qry->prepare("SELECT f.f_id, m.mem_name, m.mem_surname, m.mem_fathername, mem_cellphone, m.mem_membershipdate, f.f_paidyears, f.f_notpaidyears FROM membersfee f, membersinfo m where f.f_id = m.mem_no and  m.mem_name = '"+ ui.lineEditQueryName->text() +"' ORDER BY mem_no DESC");
	}
	else if (ui.lineEditQuerySurname->text() != "")
	{
		qry->prepare("SELECT f.f_id, m.mem_name, m.mem_surname, m.mem_fathername, mem_cellphone, m.mem_membershipdate, f.f_paidyears, f.f_notpaidyears FROM membersfee f, membersinfo m where f.f_id = m.mem_no and  m.mem_surname = '"+ ui.lineEditQuerySurname->text() +"' ORDER BY mem_no DESC");
	}
	
	qry->exec();

	model->setQuery(*qry);

	SetHeaderTitle(model);
	
	ui.tableViewFee->setModel(model);

	ui.labelMemberCount->setText("");

	Database::Disconnect();
}

void MemberFee::SetHeaderTitle(QSqlQueryModel *model)
{
	model->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("Üye No"));
	model->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("Adý"));
	model->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("Soyadý"));
	model->setHeaderData(3, Qt::Horizontal, QString::fromLocal8Bit("Baba Adý"));
	model->setHeaderData(4, Qt::Horizontal, QString::fromLocal8Bit("GSM No"));
	model->setHeaderData(5, Qt::Horizontal, QString::fromLocal8Bit("Üyelik Tarihi"));
	model->setHeaderData(6, Qt::Horizontal, QString::fromLocal8Bit("Ödenen Aidat"));
	model->setHeaderData(7, Qt::Horizontal, QString::fromLocal8Bit("Ödenmeyen Aidat"));

	ui.tableViewFee->setColumnHidden(0, true);
}

void MemberFee::MakeConnections()
{
	connect(ui.toolButtonShowAll,        SIGNAL(clicked()), this, SLOT(LoadFees()));
	connect(ui.toolButtonPayFeeAidat,    SIGNAL(clicked()), this, SLOT(PayFeeAidatClicked()));
	connect(ui.toolButtonPayFeeBagis,    SIGNAL(clicked()), this, SLOT(PayFeeBagisClicked()));
	connect(ui.toolButtonPaymentDetail,  SIGNAL(clicked()), this, SLOT(PaymentDetailClicked()));
	connect(ui.toolButtonBorclu,         SIGNAL(clicked()), this, SLOT(BorcluClicked()));
	connect(ui.toolButtonQuery,          SIGNAL(clicked()), this, SLOT(QueryNameClicked()));
	connect(ui.toolButtonBorclandir,     SIGNAL(clicked()), this, SLOT(BorclandirClicked()));
	connect(ui.toolButtonBorcluSMS,      SIGNAL(clicked()), this, SLOT(BorcluSMSClicked()));
	
	connect(ui.tableViewFee, SIGNAL(clicked(QModelIndex)), this, SLOT(ActivateButtons(QModelIndex)));
}

void MemberFee::PayFeeAidatClicked()
{
	int selectedRow = selectedIndex.row();

	QStringList years = model->index(selectedRow, 7).data().toString().split(",");
	years.removeLast();
	
	Makbuz *makbuz= new Makbuz(false, model->index(selectedRow, 0).data().toInt(),
							   years, this);

	makbuz->setAttribute(Qt::WA_DeleteOnClose);
	makbuz->exec();

	ui.labelMemberCount->setText("");

	LoadFees();
}

void MemberFee::PayFeeBagisClicked()
{
	QStringList dummy;
	int selectedRow = selectedIndex.row();

	Makbuz *makbuz= new Makbuz(true, model->index(selectedRow, 0).data().toInt(),
							   dummy, this);

	makbuz->setAttribute(Qt::WA_DeleteOnClose);
	makbuz->exec();

	ui.labelMemberCount->setText("");

	LoadFees();
}


void MemberFee::PaymentDetailClicked()
{
	int selectedRow = selectedIndex.row();

	PaymentsWindow *paymentsWnd= new PaymentsWindow(model->index(selectedRow, 0).data().toInt(), this);

	paymentsWnd->setAttribute(Qt::WA_DeleteOnClose);

	paymentsWnd->exec();
}

void MemberFee::ActivateButtons(QModelIndex index)
{
	selectedIndex = index;
	int selectedRow = selectedIndex.row();
	double totalBorc = 0;
	QStringList years = model->index(selectedRow, 7).data().toString().split(",");
	years.removeLast();

	for (int i = 0; i < years.count(); i++)
	{
		totalBorc += Database::GetFeePerYear(years.at(i).toInt());
	}
	
	ui.labelMemberCount->setText(QString::fromLocal8Bit("Seçili Üyenin Aidat Borcu : [ %1 TL ] - Ödeme Yapýlmayan Yýl Sayýsý : [ %2 ]").
		                         arg(totalBorc).arg(years.count()));

	ChangeButtonStatus(true);
}

void MemberFee::ChangeButtonStatus(bool status)
{
	ui.toolButtonPayFeeBagis->setEnabled(status);
	ui.toolButtonPayFeeAidat->setEnabled(status);
	ui.toolButtonPaymentDetail->setEnabled(status);
}