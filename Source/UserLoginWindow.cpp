#include "UserLoginWindow.h"
#include "DatabaseManager.h"
#include "MessageManager.h"
#include "MainWindow.h"
#include "qmessagebox.h"

UserLoginWindow::UserLoginWindow(QWidget *parent)
	: QDialog(parent, Qt::WindowCloseButtonHint)
{
	ui.setupUi(this);

	this->setAttribute(Qt::WA_DeleteOnClose);
	
	SetOrganizationLogo();

	if (Database::Connect())
	{
		MessageManager::GetInstance()->AddInfoMessage(QString::fromLocal8Bit("Veritabaný baðlantýsý baþarýyla kuruldu."));	
		ui.labelDBConStatus->setStyleSheet("background-color: rgb(0, 255, 0);");
		ui.labelDBConStatus->setText(QString::fromLocal8Bit("Baðlandý"));
	}
	else
	{
		MessageManager::GetInstance()->AddInfoMessage(QString::fromLocal8Bit("Veritabaný baðlantýsý kurulamadý."));	
		ui.labelDBConStatus->setStyleSheet("background-color: rgb(255, 0, 0);");
		ui.labelDBConStatus->setText(QString::fromLocal8Bit("Baðlanamadý"));
	}
	
	connect(ui.toolButtonOK,     SIGNAL(clicked()), this, SLOT(OKClicked()));
	connect(ui.toolButtonCancel, SIGNAL(clicked()), this, SLOT(CancelClicked()));
}

UserLoginWindow::~UserLoginWindow()
{

}

void UserLoginWindow::SetOrganizationLogo()
{
	QString fileName = QApplication::applicationDirPath() + PHOTO_PATH + "logo.png";
	QPixmap pix(fileName);

	//ui.labelLogo->setPixmap(pix);
}

void UserLoginWindow::OKClicked()
{
	QString pass = Database::GetUserPassword(ui.lineEditUsername->text());	
	bool userType = Database::GetUserType(ui.lineEditUsername->text());
	pass = pass.trimmed();

	if (pass == ui.lineEditUserPassword->text())
	{
		MainWindow* mainWnd = new MainWindow();
		mainWnd->DisableButtons(userType);
		mainWnd->setAttribute(Qt::WA_DeleteOnClose);
		mainWnd->move(1920,0);
		mainWnd->showMaximized();

		MessageManager::GetInstance()->AddInfoMessage(QString::fromLocal8Bit("%1 kullanýcýsý ile giriþ yapýldý.").
			                                          arg(ui.lineEditUsername->text()));	

		if (!userType)
		{
			Database::AddLastLogin(QString("[%1] - [%2]").arg(ui.lineEditUsername->text()).
				                   arg(QDateTime::currentDateTime().toString("dd.MM.yyyy - hh:mm:ss")));
		}
		
		this->close();
	}
	else
	{
		QMessageBox msg;
		msg.setText(QString::fromLocal8Bit("Kullanýcý adý veya kullanýcý parolasý yanlýþ girildi.\nLütfen tekrar deneyiniz!"));
		msg.setIcon(QMessageBox::Icon::Warning);
		msg.exec();
	}
}

void UserLoginWindow::CancelClicked()
{
	close();
}
