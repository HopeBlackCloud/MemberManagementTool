#include "Expense.h"
#include "DatabaseManager.h"
#include "MessageManager.h"
#include "QDateTime.h"
#include <QListWidget>
#include "qmessagebox.h"

Expense::Expense(QWidget *parent)
	: QDialog(parent,  Qt::WindowCloseButtonHint)
{
	ui.setupUi(this);

	ui.toolButtonSave->setEnabled(true);
	
	ui.labelKurum->setText(QApplication::applicationDisplayName());
	ui.labelDate->setText("Tarih: " + QDateTime::currentDateTime().toString("dd.MM.yyyy"));

	connect(ui.toolButtonSave, SIGNAL(clicked()), this, SLOT(Save()));
	connect(ui.toolButtonCancel, SIGNAL(clicked()), this, SLOT(close()));
}

Expense::~Expense()
{

}


bool Expense::CheckControls()
{
	if (ui.lineEditCode->text() == "" || ui.lineEditNote->text() == "" || 
		ui.lineEditResponsible->text() == "" || ui.lineEditSerial->text() == "" )
		
	{
		return false;
	}
	else
	{
		return true;
	}
}

void Expense::Save()
{
	if (CheckControls() == true)
	{
		Database::InsertExpense(ui.lineEditSerial->text(),
								ui.dateTimeEdit->text(),
								ui.lineEditResponsible->text(),
								ui.spinBoxAmount->value(),
								ui.lineEditCommerceName->text(),
								ui.lineEditCode->text(),
								ui.lineEditNote->text());

		QMessageBox msg;
		QString text = QString::fromLocal8Bit("%1TL masraf baþarýyla girildi.").arg(ui.spinBoxAmount->value());
		MessageManager::GetInstance()->AddInfoMessage(QString::fromLocal8Bit("%1TL masraf baþarýyla girildi.").arg(ui.spinBoxAmount->value()));
		msg.setText(text);
		msg.setIcon(QMessageBox::Icon::Information);
		msg.exec();

		this->close();
	}	
	else
	{
		QMessageBox msg;

		msg.setText(QString::fromLocal8Bit("Tüm alanlarý doldurmanýz gerekmektedir!"));
		msg.setIcon(QMessageBox::Icon::Critical);
		msg.exec();
	}
}


