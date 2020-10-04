#include "EnvanterWindow.h"
#include "DatabaseManager.h"
#include "Definitions.h"
#include "QtSql\qsqlrecord.h"

EnvanterWindow::EnvanterWindow(QWidget *parent)
	: QDialog(parent, Qt::WindowCloseButtonHint)
{
	ui.setupUi(this);

	Database::Connect();

	model = new QSqlTableModel(this);

	model->setTable("envanter");
	model->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model->select();
	model->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("Seri No"));
	model->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("Marka"));
	model->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("Model"));
	model->setHeaderData(3, Qt::Horizontal, QString::fromLocal8Bit("Giriş Tarihi"));
	model->setHeaderData(4, Qt::Horizontal, QString::fromLocal8Bit("Adı"));
	model->setHeaderData(5, Qt::Horizontal, QString::fromLocal8Bit("Tipi"));

	ui.tableViewEnvanter->setModel(model);
	
	connect(ui.toolButtonAdd,     SIGNAL(clicked()), this, SLOT(AddClicked()));
	connect(ui.toolButtonOK,      SIGNAL(clicked()), this, SLOT(OKClicked()));
	connect(ui.toolButtonDelete,  SIGNAL(clicked()), this, SLOT(DeleteClicked()));
	connect(ui.tableViewEnvanter, SIGNAL(clicked(QModelIndex)), this, SLOT(ActivateButtons(QModelIndex)));
}

EnvanterWindow::~EnvanterWindow()
{
	SAFE_DELETE(model);
	Database::Disconnect();
}

void EnvanterWindow::AddClicked()
{
	int row = model->rowCount();
	model->insertRows(row, 1);

	ui.toolButtonDelete->setEnabled(false);
}

void EnvanterWindow::OKClicked()
{
	model->submitAll();
	ui.toolButtonDelete->setEnabled(false);
	this->close();
}

void EnvanterWindow::DeleteClicked()
{
	int selectedRow = selectedIndex.row();
	model->removeRow(selectedRow);
	
	model->submitAll();
	ui.toolButtonDelete->setEnabled(false);
}

void EnvanterWindow::ActivateButtons(QModelIndex index)
{
	selectedIndex = index;
	ui.toolButtonDelete->setEnabled(true);
}
