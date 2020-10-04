#include "Definitions.h"
#include "SendSMSWindow.h"
#include "SMSManager.h"
#include "DatabaseManager.h"
#include "SettingsManager.h"
#include "MessageManager.h"
#include "SMSPreviewWindow.h"

#include <QFile>
#include <QPainter>
#include <QFileDialog>
#include <QPdfWriter>
#include <qcombobox.h>
#include <QTableWidgetItem>
#include <QLineEdit>
#include <QInputDialog>
#include <QMessageBox>
#include <QNetworkConfigurationManager>

SendSMSWindow::SendSMSWindow(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	
	connect(ui.toolButtonSendSMS,     SIGNAL(clicked()), this, SLOT(SendClicked()));
	connect(ui.toolButtonClear,       SIGNAL(clicked()), this, SLOT(ClearClicked()));
	connect(ui.toolButtonClearSMS,    SIGNAL(clicked()), this, SLOT(ClearSMSClicked()));
	connect(ui.toolButtonMakeGroup,   SIGNAL(clicked()), this, SLOT(MakeGroupClicked()));
	connect(ui.toolButtonDeleteGroup, SIGNAL(clicked()), this, SLOT(DeleteGroupClicked()));
	
	numberList = "";

	qry = NULL;
	model = NULL;

	model = new QSqlQueryModel();
	qry = new QSqlQuery(QSqlDatabase::defaultConnection);

	grqry = NULL;
	grmodel = NULL;

	grmodel = new QSqlTableModel();
	grqry = new QSqlQuery(QSqlDatabase::defaultConnection);

	LoadMembersGroups(true);
	ui.labelTextLengthSMS->setText("0 / 160");

	ui.labelMemberSendSMS->setText(QString::fromLocal8Bit("Mesaj [ 0 ] üyeye gönderilecektir."));
	ui.tableViewSelection->setEnabled(false);
	ui.tableViewSelection->clearSelection();

    ui.tableViewGroups->setEnabled(false);
	ui.tableViewGroups->clearSelection();

	QRegExp smsRegEx("^[A-Za-z1-9]{150}$");
	QValidator* smsText;
	smsText = new QRegExpValidator(smsRegEx, this);
	//ui.textEditSMS->setValidator(smsText);

	connect(ui.comboBoxSelect,     SIGNAL(currentIndexChanged(int)),this, SLOT(ComboClicked(int)));
	connect(ui.tableViewSelection, SIGNAL(clicked(QModelIndex)), this, SLOT(ActivateButtons(QModelIndex)));
	connect(ui.tableViewGroups,    SIGNAL(clicked(QModelIndex)), this, SLOT(ActivateButtonsGr(QModelIndex)));
	connect(ui.textEditSMSText,    SIGNAL(textChanged()), this, SLOT(SMSTextChanged()));
}

void SendSMSWindow::SMSTextChanged()
{
	int count = ui.textEditSMSText->toPlainText().length();

	ui.labelTextLengthSMS->setText(QString("%1 / 160").arg(count));

	if (count > 160)
	{
		 QString text = ui.textEditSMSText->toPlainText();
		 text.chop(text.length() - 160);
		 ui.textEditSMSText->setPlainText(text);
	}
}

void SendSMSWindow::MakeGroupClicked()
{
   bool ok;
   QString groupName = QInputDialog::getText(this, QString("Grup ismi giriniz"),
                                             QString("Grup ismi:"), QLineEdit::Normal,QString(""), &ok);
   if (ok && !groupName.isEmpty())
        MakeNewGroup(groupName);
}

void SendSMSWindow::DeleteGroupClicked()
{
	Database::Connect();
	int selectedRow = selectedIndexGr.row();
	
	QString grName = grmodel->index(selectedRow, 0).data().toString();

	grqry->prepare(QString("DELETE FROM groups WHERE gr_name = '%1' ;").arg(grName));
	
	grqry->exec();

	grmodel->setQuery(*grqry);
	MessageManager::GetInstance()->AddInfoMessage(QString::fromLocal8Bit("[ %1 ] grubu, SMS gruplardan silindi.").arg(grName));
	
	ui.toolButtonDeleteGroup->setEnabled(false);

	LoadMembersGroups(true);
}

void SendSMSWindow::MakeNewGroup(QString& grName)
{
	FillNumberList();

	Database::CreateNewGroup(grName, numberList);

	LoadMembersGroups(true);

	numberList.clear();
}

void SendSMSWindow::SetHeaderTitle(QSqlQueryModel *model)
{
	model->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("Adý"));
	model->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("Soyadý"));
	model->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("Baba Adý"));
	model->setHeaderData(3, Qt::Horizontal, QString::fromLocal8Bit("GSM No"));
}

void SendSMSWindow::SetHeaderTitleGr(QSqlQueryModel *model)
{
	model->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("Grup Adý"));
	model->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("Numaralar"));
}

SendSMSWindow::~SendSMSWindow()
{
	SAFE_DELETE(qry);
	SAFE_DELETE(model);
	SAFE_DELETE(grqry);
	SAFE_DELETE(grmodel);
}

bool SendSMSWindow::CheckNetConnection()
{
	QNetworkConfigurationManager mgr;
	QList<QNetworkConfiguration> activeConfigs = mgr.allConfigurations(QNetworkConfiguration::Active);
	if (activeConfigs.count() > 0)
		if (mgr.isOnline())
		{
			ui.labelNet->setText(QString::fromLocal8Bit("Ýnternet baðlantýnýz mevcut! SMS gönderilebilir."));
			ui.labelNet->setStyleSheet("background-color: rgb(0, 255, 0);");
			ui.toolButtonSendSMS->setEnabled(true);
			return true;
		}

		if (!mgr.isOnline())
		{
			ui.labelNet->setText(QString::fromLocal8Bit("Ýnternet baðlantýnýz yok! SMS gönderilemez."));
			ui.labelNet->setStyleSheet("background-color: rgb(255, 0, 0);");
			ui.toolButtonSendSMS->setEnabled(false);

			return false;
		}
	else
		if (!mgr.isOnline())
		{
			ui.labelNet->setText(QString::fromLocal8Bit("Ýnternet baðlantýnýz yok! SMS gönderilemez."));
			ui.labelNet->setStyleSheet("background-color: rgb(255, 0, 0);");
			ui.toolButtonSendSMS->setEnabled(false);

			return false;
		}
}

void SendSMSWindow::ComboClicked(int index)
{
	CheckNetConnection();

	if (index == 1)
	{
		numberList.clear();
		ui.toolButtonMakeGroup->setEnabled(false);
		ui.tableViewSelection->selectAll();
		ui.tableViewGroups->setEnabled(false);
		ui.tableViewGroups->clearSelection();
		ui.tableViewSelection->setEnabled(false);
		ui.labelMemberSendSMS->setText(QString::fromLocal8Bit("Mesajýnýz [ %1 ] üyeye gönderilecektir.").arg(ui.tableViewSelection->selectionModel()->selectedRows().count()));
	}
	else if (index == 2)
	{
		numberList.clear();
		ui.toolButtonMakeGroup->setEnabled(false);
		ui.tableViewSelection->setEnabled(false);
		ui.tableViewSelection->clearSelection();
		ui.tableViewGroups->clearSelection();
		ui.tableViewGroups->setEnabled(true);
	}
	else if(index == 3)
	{
		numberList.clear();
		ui.tableViewGroups->setEnabled(false);
		ui.tableViewSelection->clearSelection();
		ui.tableViewGroups->clearSelection();
		ui.tableViewSelection->setEnabled(true);
		ui.labelMemberSendSMS->setText(QString::fromLocal8Bit("Mesajýnýz [ 0 ] üyeye gönderilecektir."));
	}
}

void SendSMSWindow::LoadMembersGroups(bool isGroup)
{
	Database::Connect();

	qry->prepare("SELECT mem_name, mem_surname, mem_fathername, mem_cellphone FROM membersinfo ORDER BY mem_name ASC");
	
	qry->exec();

	model->setQuery(*qry);

	SetHeaderTitle(model);
	
	ui.tableViewSelection->setModel(model);

	grqry->prepare("SELECT gr_name, gr_numbers FROM groups");
	
	grqry->exec();

	grmodel->setQuery(*grqry);

	SetHeaderTitleGr(grmodel);
	
	ui.tableViewGroups->setModel(grmodel);

	Database::Disconnect();
}

void SendSMSWindow::ClearClicked()
{
	ui.tableViewSelection->clearSelection();
	ui.tableViewGroups->clearSelection();
	ui.tableViewSelection->setEnabled(false);
	ui.tableViewGroups->setEnabled(false);
	numberList.clear();
	ui.toolButtonMakeGroup->setEnabled(false);
	ui.labelMemberSendSMS->setText(QString::fromLocal8Bit("Mesajýnýz [ 0 ] üyeye gönderilecektir."));
	ui.comboBoxSelect->setCurrentIndex(0);
}

void SendSMSWindow::ClearSMSClicked()
{
	ui.textEditSMSText->setText("");
}

void SendSMSWindow::FillNumberList()
{
	QModelIndexList list = ui.tableViewSelection->selectionModel()->selectedIndexes();

	foreach(const QModelIndex &index, list)
	{
		QString cell = index.data(0).toString();
		if (cell.at(0) == '0')
			numberList.append(QString("%1,").arg(cell));
	}

	int last = numberList.lastIndexOf(',');
	numberList.remove(last, 2);
}

void SendSMSWindow::SendClicked()
{
	FillNumberList();

	QMessageBox msg;
	
	msg.setIcon(QMessageBox::Icon::Warning);

	QString mesaj = QString::fromStdWString(ui.textEditSMSText->toPlainText().toStdWString());

	if (mesaj == "")
	{
		msg.setText(QString::fromLocal8Bit("Mesajýnýzýn içeriðini giriniz!"));
		ui.textEditSMSText->setFocus();
		msg.exec();
	}
	else if(numberList.isEmpty())
	{
		msg.setText(QString::fromLocal8Bit("Mesaj gönderilecek kiþileri seçiniz!"));
		ui.textEditSMSText->setFocus();
		msg.exec();
	}
	else if(!CheckNetConnection())
	{
		msg.setText(QString::fromLocal8Bit("Ýnternet baðlantýnýzý kontrol ediniz!"));
		msg.exec();
	}
	else
	{
		SMSPreviewWindow* smsPreviewWindow = new SMSPreviewWindow(ui.labelMemberSendSMS->text(), mesaj, this);
		
		int result = smsPreviewWindow->exec();

		if (result == QDialog::Accepted)
		{
			SMSManager::GetInstance()->SendSingleSMS(mesaj, numberList);

			if (!numberList.isEmpty())
			{
				numberList.clear();
			}
		
			ui.tableViewGroups->clearSelection();
			ui.tableViewSelection->clearSelection();

			ui.comboBoxSelect->setCurrentIndex(0);
			ui.textEditSMSText->setText("");
			ui.labelMemberSendSMS->setText(QString::fromLocal8Bit("Mesajýnýz [ 0 ] üyeye gönderilecektir."));
		}
	}
}

void SendSMSWindow::ActivateButtons(QModelIndex)
{
	ui.toolButtonMakeGroup->setEnabled(true);

	ui.labelMemberSendSMS->setText(QString::fromLocal8Bit("Mesajýnýz [ %1 ] üyeye gönderilecektir.").arg(ui.tableViewSelection->selectionModel()->selectedRows().count()));
}

void SendSMSWindow::ActivateButtonsGr(QModelIndex index)
{
	QStringList numList;
	numberList.clear();

	selectedIndexGr = index;
	ui.toolButtonDeleteGroup->setEnabled(true);

	QModelIndexList list = ui.tableViewGroups->selectionModel()->selectedRows(1);
	numList = index.data(0).toString().split(',');
    
	foreach (const QString &telNO, numList)
	{    
		numberList.append(telNO + ",");
    }

	int last = numberList.lastIndexOf(',');
	numberList.remove(last, 2);

	ui.labelMemberSendSMS->setText(QString::fromLocal8Bit("Mesajýnýz [ %1 ] üyeye gönderilecektir.").arg(numList.count()));
}


