#include "DetailUpdateWindow.h"
#include "DatabaseManager.h"
#include "SettingsManager.h"
#include "MessageManager.h"

#include <QMessageBox>
#include <QFileDialog>

DetailUpdateWindow::DetailUpdateWindow(QWidget *parent, bool isUpdate, QString& _mem_ID)
	: QDialog(parent, Qt::WindowCloseButtonHint)
{
	ui.setupUi(this);

	mem_ID = _mem_ID;

	if (isUpdate)
	{
		ui.toolButtonUpdate->show();
		ui.toolButtonAddPhoto->show();
		ui.toolButtonUpdate->setEnabled(true);
		ui.groupBoxDetailed->setEnabled(true);
	}
	else
	{
		ui.toolButtonUpdate->hide();
		ui.toolButtonAddPhoto->hide();
		ui.toolButtonUpdate->setEnabled(false);
		ui.groupBoxDetailed->setEnabled(false);
	}
	
	photoFilePath = "";

	FillControlWithMemberID(mem_ID);

	QRegExp emailRegEx("^[A-Za-z0-9](([_\.\-]?[a-zA-Z0-9]+)*)@([A-Za-z0-9]+)(([\.\-]?[a-zA-Z0-9]+)*)\.([A-Za-z]{2,})$");
	QValidator* emailValidator;
	emailValidator = new QRegExpValidator(emailRegEx, this);
	ui.lineEditMail->setValidator(emailValidator);

	QRegExp cellRegEx("^[0]{1}[5]{1}[0-9]{9}$");
	QValidator* cellValidator;
	cellValidator = new QRegExpValidator(cellRegEx, this);
	//ui.lineEditCellPhone->setValidator(cellValidator);

	QRegExp comRegEx("^[0]{1}[0-9]{10}$");
	QValidator* comValidator;
	comValidator = new QRegExpValidator(comRegEx, this);
	//ui.lineEditWorkPhone->setValidator(comValidator);

	connect(ui.toolButtonUpdate,   SIGNAL(clicked()), this, SLOT(UpdateClicked()));
	connect(ui.toolButtonAddPhoto, SIGNAL(clicked()), this, SLOT(AddPhotoClicked()));
}

DetailUpdateWindow::~DetailUpdateWindow()
{

}

void DetailUpdateWindow::UpdateClicked()
{
	
		if (!IsEmailValidated())
		{
			QMessageBox msg;

			msg.setText(QString::fromLocal8Bit("Geçerli bir e-posta adresi giriniz! Format [xxxx@xxx.com]"));
			msg.setIcon(QMessageBox::Icon::Critical);
			msg.exec();

			return;
		}

		if (!IsCellPhoneValidated())
		{
			QMessageBox msg;

			msg.setText(QString::fromLocal8Bit("Geçerli bir cep telefonu numarasý giriniz! Format [05xxxxxxxxx]"));
			msg.setIcon(QMessageBox::Icon::Critical);
			msg.exec();

			return;
		}

		int memNo = ui.lineEditMemberId->text().toInt();
		if (memNo > 9999)
		{
			QMessageBox msg;

			msg.setText(QString::fromLocal8Bit("Geçerli bir üye numarasý giriniz! [0001] - [9999]"));
			msg.setIcon(QMessageBox::Icon::Critical);
			msg.exec();

			return;
		}

		if (!IsCompanyPhoneValidated())
		{
			QMessageBox msg;

			msg.setText(QString::fromLocal8Bit("Geçerli bir þirket telefonu numarasý giriniz! Format [0xxxxxxxxxx]"));
			msg.setIcon(QMessageBox::Icon::Critical);
			msg.exec();

			return;
		}

		int mem_no = ui.lineEditMemberId->text().toInt();

		if(ui.lineEditMemberId->text() != mem_ID)
		{
			if (Database::IsMemberNoExist(mem_no))
			{
				QMessageBox msg;

				msg.setText(QString::fromLocal8Bit("%1 üye numarasýna sahip üye bulunmaktadýr. Lütfen baþka bir üye numarasý veriniz.").arg(mem_no));
				msg.setIcon(QMessageBox::Icon::Critical);
				msg.exec();
				return;
			}
		}


	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this, QApplication::applicationDisplayName(), QString::fromLocal8Bit("%1 nolu üyenin bilgilerini güncellemek istiyor musunuz?").arg(mem_no),
                                QMessageBox::Yes|QMessageBox::No);
	if (reply == QMessageBox::Yes) 
	{
		if (Database::UpdateMember(mem_no,
								   ui.lineEditName->text(), 
								   ui.lineEditSurname->text(),
								   ui.lineEditNameFathername->text(),
								   ui.lineEditNameBirthPlace->text(),
								   ui.dateTimeEditBirthDate->date().toString("dd.MM.yyyy"),
								   ui.comboBoxSex->currentText(), ui.comboBoxMaritalStatus->currentText(), ui.lineEditCellPhone->text(), 
								   ui.lineEditMail->text(), ui.comboBoxEducationStatus->currentText(), ui.comboBoxEducationLevel->currentText(), ui.lineEditSchoolName->text(),
								   ui.lineEditDepartmentName->text(), ui.lineEditOccup->text(), QString("%1.%2").arg(ui.comboBoxBlood->currentText()).arg(ui.comboBoxRhFactor->currentText()),
								   ui.textEditHomeAddress->toPlainText(), ui.lineEditCompanyName->text(), ui.lineEditWorkPhone->text(), ui.textEditWorkAddress->toPlainText(), ui.dateTimeEditMembershipDate->date().toString("dd.MM.yyyy"),
								   ui.comboBoxMemberType->currentText(), ui.comboBoxMemberStatus->currentText(), photoFilePath))
		{
			QMessageBox msg;

			msg.setText(QString::fromLocal8Bit("%1 %2,\nÜye No: %3 \nÜye baþarýlý bir þekilde güncellendi.").arg(ui.lineEditName->text()).arg(ui.lineEditSurname->text()).arg( mem_no));
			msg.setIcon(QMessageBox::Icon::Information);
			msg.exec();
		}
	}
}

bool DetailUpdateWindow::IsEmailValidated()
{
	return ui.lineEditMail->hasAcceptableInput();
}

bool DetailUpdateWindow::IsCellPhoneValidated()
{
	return ui.lineEditCellPhone->hasAcceptableInput();
}

bool DetailUpdateWindow::IsMemNoValidated()
{
	return ui.lineEditMemberId->hasAcceptableInput();
}

bool DetailUpdateWindow::IsCompanyPhoneValidated()
{
	return ui.lineEditWorkPhone->hasAcceptableInput();
}

void DetailUpdateWindow::AddPhotoClicked()
{
	photoFilePath = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("Fotoðraf seçiniz ..."), "", "JPEG (*.jpg *.jpeg);; PNG(*.png)");
	
	if (photoFilePath != "")
	{
		QImage image = QImage(photoFilePath);
		QPixmap pixmap = pixmap.fromImage(image.scaled(ui.labelMemberFoto->width(),
													   ui.labelMemberFoto->height(),
													   Qt::IgnoreAspectRatio,
													   Qt::SmoothTransformation));
		ui.labelMemberFoto->setPixmap(pixmap);
	}
	else
	{
		QMessageBox msg;

		msg.setText(QString::fromLocal8Bit("Geçerli bir üye fotoðrafý seçmediniz!"));
		msg.setIcon(QMessageBox::Icon::Critical);
		msg.exec();
	}
}

void DetailUpdateWindow::FillControlWithMemberID(QString& mem_ID)
{
	Database::Connect();

	QSqlQuery *qry = new QSqlQuery(QSqlDatabase::defaultConnection);

	qry->prepare("select * from membersinfo where mem_no = " + mem_ID);
	qry->exec();

	if (qry->next())
	{
		QDate dateBirth  = SettingsManager::StringToDate(qry->value(4).toString());
        QDate dateMember = SettingsManager::StringToDate(qry->value(19).toString());

		ui.lineEditMemberId->setText(qry->value(0).toString());
		ui.lineEditName->setText(qry->value(1).toString());
		ui.lineEditSurname->setText(qry->value(2).toString());
		ui.dateTimeEditBirthDate->setDate(dateBirth);
		ui.lineEditNameBirthPlace->setText(qry->value(3).toString());
		ui.comboBoxSex->setCurrentText(qry->value(5).toString());
		ui.comboBoxMaritalStatus->setCurrentText(qry->value(6).toString());
		ui.lineEditCellPhone->setText(qry->value(7).toString());
		ui.lineEditMail->setText(qry->value(8).toString());
		ui.comboBoxEducationStatus->setCurrentText(qry->value(9).toString());
		ui.comboBoxEducationLevel->setCurrentText(qry->value(10).toString());
		ui.lineEditSchoolName->setText(qry->value(11).toString());
		ui.lineEditDepartmentName->setText(qry->value(12).toString());
		ui.lineEditOccup->setText(qry->value(13).toString());
		ui.comboBoxBlood->setCurrentText(qry->value(14).toString().split(".").at(0));
		ui.comboBoxRhFactor->setCurrentText(qry->value(14).toString().split(".").at(1));
		ui.textEditHomeAddress->setText(qry->value(15).toString());
		ui.lineEditCompanyName->setText(qry->value(16).toString());
		ui.lineEditWorkPhone->setText(qry->value(17).toString());
		ui.textEditWorkAddress->setText(qry->value(18).toString());
		ui.dateTimeEditMembershipDate->setDate(dateMember);
		ui.comboBoxMemberType->setCurrentText(qry->value(20).toString());
		ui.comboBoxMemberStatus->setCurrentText(qry->value(21).toString());
		photoFilePath = qry->value(22).toString();
		ui.lineEditNameFathername->setText(qry->value(23).toString());
	}

	qry->finish();

	Database::Disconnect();

	QImage image = QImage(photoFilePath);
	QPixmap pixmap = pixmap.fromImage(image.scaled(ui.labelMemberFoto->width(),
													ui.labelMemberFoto->height(),
													Qt::IgnoreAspectRatio,
													Qt::SmoothTransformation));
	ui.labelMemberFoto->setPixmap(pixmap);
}
