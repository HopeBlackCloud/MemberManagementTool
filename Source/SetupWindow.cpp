#include "SetupWindow.h"
#include "DatabaseManager.h"
#include "Definitions.h"
#include "QtSql\qsqlrecord.h"

SetupWindow::SetupWindow(QWidget *parent, SETTINGS_TYPE_ENUM isUserSetup)
	: QDialog(parent, Qt::WindowCloseButtonHint)
{
	ui.setupUi(this);

	Database::Connect();

	userSetup = isUserSetup;

	model = new QSqlTableModel(this);
	if (userSetup == SETTINGS_TYPE_USERS)
	{
		model->setTable("userinfo");
		model->setEditStrategy(QSqlTableModel::OnManualSubmit);
		model->select();
		model->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("Kullanýcý Adý"));
		model->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("Kullanýcý Þifresi"));
		model->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("Kullanýcý Türü"));

		HideButtons(false);

		ui.tableViewSetup->setModel(model);
	}
	else if (userSetup == SETTINGS_TYPE_FEES)
	{
		model->setTable("yearfee");
		model->setEditStrategy(QSqlTableModel::OnManualSubmit);
		model->select();
		model->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("Yýl"));
		model->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("Aidat Miktarý"));

		HideButtons(false);

		ui.tableViewSetup->setModel(model);
	}
	else if (userSetup == SETTINGS_TYPE_LOGS)
	{
		model->setTable("settings");
		model->setEditStrategy(QSqlTableModel::OnManualSubmit);
		model->select();
		model->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("Log Aktif"));
		model->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("Son Baþarýlý Giriþ"));
	    model->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("Güncel Yýl"));
		model->setHeaderData(3, Qt::Horizontal, QString::fromLocal8Bit("Yeni Aidatlar Durumu"));

		ui.tableViewSetup->setColumnHidden(2, true);
		ui.tableViewSetup->setColumnHidden(3, true);
		
		HideButtons(true);

		ui.tableViewSetup->setModel(model);
	}
	
	

	connect(ui.toolButtonAdd,    SIGNAL(clicked()), this, SLOT(AddClicked()));
	connect(ui.toolButtonOK,     SIGNAL(clicked()), this, SLOT(OKClicked()));
	connect(ui.toolButtonDelete, SIGNAL(clicked()), this, SLOT(DeleteClicked()));
	connect(ui.tableViewSetup,   SIGNAL(clicked(QModelIndex)), this, SLOT(ActivateButtons(QModelIndex)));
}

SetupWindow::~SetupWindow()
{
	SAFE_DELETE(model);
	Database::Disconnect();
}

void SetupWindow::AddClicked()
{
	int row = model->rowCount();
	model->insertRows(row, 1);

	ui.toolButtonDelete->setEnabled(false);
}

void SetupWindow::OKClicked()
{
	ui.toolButtonDelete->setEnabled(false);
	model->submitAll();
	this->close();
}

void SetupWindow::DeleteClicked()
{
	int selectedRow = selectedIndex.row();
	model->removeRow(selectedRow);
	
	model->submitAll();
	ui.toolButtonDelete->setEnabled(false);
}

void SetupWindow::ActivateButtons(QModelIndex index)
{
	selectedIndex = index;
	ui.toolButtonDelete->setEnabled(true);
}

void SetupWindow::HideButtons(bool hide)
{
	ui.toolButtonDelete->setVisible(!hide);
	ui.toolButtonAdd->setVisible(!hide);
}