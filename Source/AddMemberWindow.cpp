#include "AddMemberWindow.h"
#include "DatabaseManager.h"
#include "MembershipForm.h"
#include "MembershipCard.h"
#include "DetailUpdateWindow.h"
#include "MessageManager.h"

#include "qpainter.h"
#include "qmessagebox.h"
#include "qfiledialog.h"

AddMemberWindow::AddMemberWindow(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	photoFilePath = "";

	ui.labelTextLengthEv->setText("0 / 50");
	ui.labelTextLengthKurum->setText("0 / 50");

	ui.dateTimeEditBirthDate->setDateTime(QDateTime::currentDateTime());
	ui.dateTimeEditMembershipDate->setDateTime(QDateTime::currentDateTime());

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

	MakeConnections();
}

void AddMemberWindow::MakeConnections()
{
    connect(ui.toolButtonAdd,   SIGNAL(clicked()), this, SLOT(AddClicked()));
	connect(ui.toolButtonClear, SIGNAL(clicked()), this, SLOT(ClearControls()));

	connect(ui.toolButtonAddPhoto,       SIGNAL(clicked()), this, SLOT(AddPhotoClicked()));
	connect(ui.plainTextEditHomeAddress, SIGNAL(textChanged()), this, SLOT(OnHomeAddressTextChanged()));
	connect(ui.plainTextEditWorkAddress, SIGNAL(textChanged()), this, SLOT(OnKurumAddressTextChanged()));
}

AddMemberWindow::~AddMemberWindow()
{
}

void AddMemberWindow::OnHomeAddressTextChanged()
{
	int count = ui.plainTextEditHomeAddress->toPlainText().length();

	ui.labelTextLengthEv->setText(QString("%1 / 50").arg(count));

	if (count > 50)
	{
		 QString text = ui.plainTextEditHomeAddress->toPlainText();
		 text.chop(text.length() - 50);
		 ui.plainTextEditHomeAddress->setPlainText(text);
	}
}

void AddMemberWindow::OnKurumAddressTextChanged()
{
	int count = ui.plainTextEditWorkAddress->toPlainText().length();

	ui.labelTextLengthKurum->setText(QString("%1 / 50").arg(count));

	if (count > 50)
	{
		 QString text = ui.plainTextEditWorkAddress->toPlainText();
		 text.chop(text.length() - 50);
		 ui.plainTextEditWorkAddress->setPlainText(text);
	}
}

void AddMemberWindow::AddPhotoClicked()
{
	photoFilePath = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("Üye fotoðrafýný seçiniz ..."), "", "JPEG (*.jpg *.jpeg);; PNG(*.png)");
	
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

		msg.setText(QString::fromLocal8Bit("Geçerli bir üye fotoðrafý seçemediniz"));
		msg.setIcon(QMessageBox::Icon::Critical);
		msg.exec();
	}
}

void AddMemberWindow::OpenMembershipForm(QString& mem_ID, bool isNewlyAdded)								 
{
	MembershipForm *memberform = new MembershipForm(mem_ID, isNewlyAdded, ui.dateTimeEditMembershipDate->date().toString("dd.MM.yyyy"), this);

	memberform->setAttribute(Qt::WA_DeleteOnClose);
	memberform->exec();
}

bool AddMemberWindow::IsEmailValidated()
{
	return ui.lineEditMail->hasAcceptableInput();
}

bool AddMemberWindow::IsCellPhoneValidated()
{
	return ui.lineEditCellPhone->hasAcceptableInput();
}

bool AddMemberWindow::IsMemNoValidated()
{
	return ui.lineEditMemberNo->hasAcceptableInput();
}

bool AddMemberWindow::IsCompanyPhoneValidated()
{
	return ui.lineEditWorkPhone->hasAcceptableInput();
}

void AddMemberWindow::AddClicked()
{
	if (CheckControls() == true)
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

		int mem_no = ui.lineEditMemberNo->text().toInt();

		if (mem_no > 9999)
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

		if (Database::IsMemberNoExist(mem_no))
		{
			QMessageBox msg;

			msg.setText(QString::fromLocal8Bit("%1 üye numarasýna sahip üye bulunmaktadýr. Lütfen baþka bir üye numarasý veriniz.").arg(mem_no));
			msg.setIcon(QMessageBox::Icon::Critical);
			msg.exec();
			return;
		}


		if (Database::InsertMember(ui.lineEditName->text().trimmed(), 
								   ui.lineEditSurname->text().trimmed(),
								   ui.lineEditNameFathername->text().trimmed(),
								   ui.lineEditNameBirthPlace->text().trimmed(),
								   ui.dateTimeEditBirthDate->date().toString("dd.MM.yyyy"),
								   ui.comboBoxSex->currentText(), ui.comboBoxMaritalStatus->currentText(), ui.lineEditCellPhone->text().trimmed(), 
								   ui.lineEditMail->text(), ui.comboBoxEducationStatus->currentText(), ui.comboBoxEducationLevel->currentText(), ui.lineEditSchoolName->text(),
								   ui.lineEditDepartmentName->text(), ui.lineEditOccup->text(), QString("%1.%2").arg(ui.comboBoxBlood->currentText()).arg(ui.comboBoxRhFactor->currentText()),
								   ui.plainTextEditHomeAddress->toPlainText(), ui.lineEditCompanyName->text(), ui.lineEditWorkPhone->text(), ui.plainTextEditWorkAddress->toPlainText(), ui.dateTimeEditMembershipDate->date().toString("dd.MM.yyyy"),
								   ui.comboBoxMemberType->currentText(), mem_no , QString("Aktif"), photoFilePath))
		{
			QMessageBox msg;

			msg.setText(QString::fromLocal8Bit("%1 %2,\nÜye No: %3 \nÜye baþarýlý bir þekilde eklendi. Üyelik formu hazýrlandý.").arg(ui.lineEditName->text()).arg(ui.lineEditSurname->text()).arg(mem_no));
			msg.setIcon(QMessageBox::Icon::Information);
			msg.exec();

			MessageManager::GetInstance()->AddInfoMessage(QString::fromLocal8Bit("%1 %2, Üye No: %3 Üye baþarýlý bir þekilde eklendi.").arg(ui.lineEditName->text()).arg(ui.lineEditSurname->text()).arg(mem_no));
			
			// Ýlk üyelik formu basýlýyor.
			OpenMembershipForm(QString("%1").arg(mem_no), true);

			int membershipYear = ui.dateTimeEditMembershipDate->date().year();
			int thisYear = QDateTime::currentDateTime().date().year();

			QString notPaidYears = "";
			for (int i = membershipYear; i < thisYear + 1; i++)
			{
				notPaidYears.append(QString("%1,").arg(i));
			}

			if (ui.comboBoxMemberType->currentText() != "Fahri")
			{
				Database::AddNewMemberFee(mem_no, notPaidYears, QString(""));
			}

			if(membershipYear >= thisYear)
			{
				QString smsText = QString("Sayin %1 %2, Davulalan Koyu SYKES Vakfi ailesine hosgeldiniz.Toplanti ve etkinliklerimize katilmaniz dilegiyle.Esenlikler dileriz.")
								.arg(ui.lineEditName->text().trimmed())
								.arg(ui.lineEditSurname->text().trimmed());

				SMSManager::GetInstance()->SendSingleSMS(smsText, ui.lineEditCellPhone->text().trimmed());
			}
		}	
		else
		{
			QMessageBox msg;

			msg.setText(QString::fromLocal8Bit("Üye ekleme iþlemi baþarýsýz, tekrar deneyiniz!"));
			msg.setIcon(QMessageBox::Icon::Critical);
			msg.exec();
		}
	}
	else
	{
		QMessageBox msg;

		msg.setText(QString::fromLocal8Bit("Tüm alanlarý doldurmanýz gerekmektedir!"));
		msg.setIcon(QMessageBox::Icon::Critical);
		msg.exec();
	}
}

bool AddMemberWindow::CheckControls()
{
	if (ui.lineEditCellPhone->text() == "" || ui.lineEditName->text() == "" || ui.lineEditSurname->text() == "" ||  ui.lineEditNameFathername->text() == "" ||
		ui.lineEditCompanyName->text() == "" || ui.lineEditWorkPhone->text() == "" || ui.lineEditSchoolName->text() == "" ||
		ui.lineEditMemberNo->text() == "" || ui.lineEditNameBirthPlace->text() == "" || ui.lineEditMail->text() == "" || ui.lineEditDepartmentName->text() == "" ||
		ui.plainTextEditHomeAddress->toPlainText() == "" || ui.plainTextEditWorkAddress->toPlainText() == "" || ui.lineEditOccup->text() == "")
	{
		return false;
	}
	else
	{
		return true;
	}
}

void AddMemberWindow::ClearControls()
{
	ui.lineEditCellPhone->setText(""); ui.lineEditName->setText(""); ui.lineEditSurname->setText(""); ui.lineEditNameFathername->setText("");
	ui.lineEditCompanyName->setText(""); ui.lineEditWorkPhone->setText(""); ui.lineEditSchoolName->setText("");
	ui.lineEditNameBirthPlace->setText(""); ui.lineEditMail->setText(""); ui.lineEditDepartmentName->setText("");
	ui.plainTextEditHomeAddress->setPlainText(""); ui.plainTextEditWorkAddress->setPlainText(""); ui.lineEditOccup->setText(""); ui.lineEditMemberNo->setText("");
	ui.labelMemberFoto->clear();

	ui.labelMemberFoto->setText("Foto");
}
