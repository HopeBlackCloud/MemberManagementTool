#include "Makbuz.h"
#include "Definitions.h"
#include "LibraryWindow.h"
#include "DatabaseManager.h"
#include "MessageManager.h"
#include "QtSql\qsqlrecord.h"
#include "qmessagebox.h"

LibraryWindow::LibraryWindow(QWidget *parent)
	: QDialog(parent, Qt::WindowCloseButtonHint)
{
	ui.setupUi(this);

	model = NULL;

	model = new QSqlTableModel(this);

	connect(ui.toolButtonTaken,   SIGNAL(clicked()),           this, SLOT(TakenClicked()));
	connect(ui.toolButtonQuery,   SIGNAL(clicked()),           this, SLOT(QueryClicked()));
	connect(ui.tableViewLibrary,  SIGNAL(clicked(QModelIndex)), this, SLOT(ActivateButtons(QModelIndex)));
	connect(ui.toolButtonAdd,     SIGNAL(clicked()), this, SLOT(AddClicked()));
	connect(ui.toolButtonOK,      SIGNAL(clicked()), this, SLOT(OKClicked()));
	connect(ui.toolButtonDelete,  SIGNAL(clicked()), this, SLOT(DeleteClicked()));
	connect(ui.toolButtonShowAll, SIGNAL(clicked()), this, SLOT(ShowAllClicked()));

	LoadBooks();
}

LibraryWindow::~LibraryWindow()
{
	SAFE_DELETE(model);
	Database::Disconnect();
}

void LibraryWindow::QueryClicked()
{
	QString filtre = "";

	filtre = QString("b_name='" +ui.lineEditBookName->text()+"' or b_writer='"+ui.lineEditWriterName->text()+ "'or b_type='"+ui.comboBoxBookType->currentText()+"'");

    model->setFilter(filtre);	

	ui.labelTotalBooks->setText(QString::fromLocal8Bit("Kitap Sayýsý: %1").arg(model->rowCount()));
}

void LibraryWindow::TakenClicked()
{
	QString filtre = "";

	filtre = QString::fromLocal8Bit("b_takername<>'Kütüphane'");

    model->setFilter(filtre);	

	ui.labelTotalBooks->setText(QString::fromLocal8Bit("Kitap Sayýsý: %1").arg(model->rowCount()));
}

void LibraryWindow::ShowAllClicked()
{
	LoadBooks();
}

void LibraryWindow::replyFinished(QNetworkReply* reply)
{
	QByteArray arr = reply->readAll();

	QString aa(arr);

	QString bb = reply->errorString();
}

void LibraryWindow::ActivateButtons(QModelIndex index)
{
	selectedIndex = index;
	ui.toolButtonDelete->setEnabled(true);
}

void LibraryWindow::LoadBooks()
{
	Database::Connect();

	SetHeaderTitle(model);
	
	ui.tableViewLibrary->setModel(model);

	ui.labelTotalBooks->setText(QString::fromLocal8Bit("Kitap Sayýsý: %1").arg(model->rowCount()));
}

void LibraryWindow::SetHeaderTitle(QSqlTableModel *model)
{
	model->setTable("book");
	model->setEditStrategy(QSqlTableModel::OnManualSubmit);
	
	model->select();
	model->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("Kitap No"));
	model->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("Kitap Adý"));
	model->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("Kitap Yazarý"));
	model->setHeaderData(3, Qt::Horizontal, QString::fromLocal8Bit("Kitap Türü"));
	model->setHeaderData(4, Qt::Horizontal, QString::fromLocal8Bit("Kitap Yeri"));
}

void LibraryWindow::AddClicked()
{
	int row = model->rowCount();
	model->insertRows(row, 1);
	
	model->setData(model->index(row, 4), QString::fromLocal8Bit("Kütüphane"));

	ui.toolButtonDelete->setEnabled(false);
}

void LibraryWindow::OKClicked()
{
	ui.toolButtonDelete->setEnabled(false);

	int row = model->rowCount();

	if (model->data(model->index(row-1, 0)) == "" ||
		model->data(model->index(row-1, 1)) == "" ||
		model->data(model->index(row-1, 2)) == "" ||
		model->data(model->index(row-1, 3)) == "")
	{
		QMessageBox msg;
		msg.setText(QString::fromLocal8Bit("Tüm alanlarý doldurmanýz gerekmektedir!"));
		msg.setIcon(QMessageBox::Icon::Warning);
		msg.exec();
	} 
	else
	{
		model->submitAll();
		MessageManager::GetInstance()->AddInfoMessage(QString::fromLocal8Bit("[ %1 ] kütüphaneye eklendi.").arg(model->data(model->index(row-1, 1)).toString()));
	}
}

void LibraryWindow::DeleteClicked()
{
	int selectedRow = selectedIndex.row();
	model->removeRow(selectedRow);
	
	MessageManager::GetInstance()->AddInfoMessage(QString::fromLocal8Bit("[ %1 ] kütüphaneden silindi.").arg(model->data(model->index(selectedRow, 1)).toString()));
	model->submitAll();
	ui.labelTotalBooks->setText(QString::fromLocal8Bit("Kitap Sayýsý: %1").arg(model->rowCount()));
	ui.toolButtonDelete->setEnabled(false);
}