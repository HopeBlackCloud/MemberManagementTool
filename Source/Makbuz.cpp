#include "Makbuz.h"
#include "DatabaseManager.h"
#include "MessageManager.h"
#include "QDateTime.h"
#include <QListWidget>
#include "qmessagebox.h"

Makbuz::Makbuz(bool isBagis, int mem_ID, QStringList& years, QWidget *parent)
	: QDialog(parent,  Qt::WindowCloseButtonHint)
{
	ui.setupUi(this);

	paymentNo = mem_ID;
	notpaidmonthYears = years;

	isAidat = !isBagis;

	static int row = 0;
	for (int i = 0; i < years.count(); i++)
	{
		QListWidgetItem *newItem = new QListWidgetItem;
		newItem->setText(years.at(i));
		newItem->setCheckState(Qt::Unchecked);
		ui.listWidgetYears->insertItem(row, newItem);

		row++;
	}

	if (isBagis)
	{
		ui.groupBox->hide();
	}
	else
	{
		ui.spinBoxAmount->setEnabled(false);
		ui.spinBoxAmount->setValue(0);
	}

	ui.comboBoxPayType->setCurrentIndex(isBagis);
	ui.toolButtonSave->setEnabled(isBagis);
	
	ui.labelKurum->setText(QApplication::applicationDisplayName());
	ui.labelDate->setText("Tarih: " + QDateTime::currentDateTime().toString("yyyy.MM.dd"));

	connect(ui.toolButtonSave,   SIGNAL(clicked()), this, SLOT(Save()));
	connect(ui.toolButtonCancel, SIGNAL(clicked()), this, SLOT(close()));
	connect(ui.listWidgetYears,  SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(SelectedYear(QListWidgetItem*)));
}

Makbuz::~Makbuz()
{

}

void Makbuz::SelectedYear(QListWidgetItem* item)
{
	double amount = 0.0;
	for (int i = 0; i < ui.listWidgetYears->count(); i++)
	{
		if (ui.listWidgetYears->item(i)->checkState() == Qt::Checked)
		{
			amount = amount + Database::GetFeePerYear(ui.listWidgetYears->item(i)->text().toInt());
		}
	}

	ui.spinBoxAmount->setValue(amount);

	ui.toolButtonSave->setEnabled(amount);
}

bool Makbuz::CheckControls()
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

void Makbuz::Save()
{
	if (CheckControls() == true)
	{
		QString listPay;
		QString listNoyPay;

		for (int i = 0; i < ui.listWidgetYears->count(); i++)
		{
			if (ui.listWidgetYears->item(i)->checkState() == Qt::Checked)
			{

				listPay.append(ui.listWidgetYears->item(i)->text() + ",");
			}
			else
			{
				listNoyPay.append(ui.listWidgetYears->item(i)->text() + ",");
			}
		}
		
		QString paidYears = Database::GetPaidYears(paymentNo);

		paidYears.append(listPay);
	
		Database::InsertPayment(isAidat, 
								paymentNo, 
								QDateTime::currentDateTime().toString("yyyy.MM.dd"), 
								ui.lineEditResponsible->text(),
								listPay, 
								ui.spinBoxAmount->value(), 
								ui.comboBoxPayType->currentText(), 
								ui.lineEditSerial->text(), 
								ui.lineEditCode->text(), 
								ui.lineEditNote->text(),
								paidYears,
								listNoyPay);


		QMessageBox msg;
		QString text = QString::fromLocal8Bit("[Üye No : %1] - %2TL tahsilat baþarýyla gerçekleþti.").arg(paymentNo).arg(ui.spinBoxAmount->value());
		MessageManager::GetInstance()->AddInfoMessage(QString::fromLocal8Bit("[Üye No : %1] - %2TL tahsilat baþarýyla gerçekleþti.").arg(paymentNo).arg(ui.spinBoxAmount->value()));
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


