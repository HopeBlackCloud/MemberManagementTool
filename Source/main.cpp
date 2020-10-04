#include "UserLoginWindow.h"
#include "SettingsManager.h"

#include <QFile>
#include <qtextcodec.h>
#include <QtWidgets/QApplication>
#include <qnetworkinterface.h>
#include <qmessagebox.h>

QString LoadStyleSheet()
{
    QFile file(":/Style/bys.qss");
    file.open(QFile::ReadOnly);
    return QString::fromLatin1(file.readAll());
}

QString GetMACAddressAsString()
{
    foreach(QNetworkInterface netInterface, QNetworkInterface::allInterfaces())
    {
        // Return only the first non-loopback MAC Address
        if (!(netInterface.flags() & QNetworkInterface::IsLoopBack))
            return netInterface.hardwareAddress();
    }

    return QString();
}

bool CheckActivation(QString & code)
{
	bool activated = false;

	QString macAddress = GetMACAddressAsString();
	macAddress = macAddress.remove(':');

	if (macAddress == code)
		activated = true;
	else
		activated = false;

	return activated;
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	
	SettingsManager::ReadOrganizationInfo(QApplication::applicationDirPath());

	QApplication::setApplicationDisplayName(SettingsManager::organizationSettings.orgName.toStdString().c_str());
	QApplication::setOrganizationName(SettingsManager::organizationSettings.orgDispName.toStdString().c_str());
	
	QIcon icon;
    icon.addFile(QApplication::applicationDirPath() + QString("/Fotos/logo.JPG"), QSize(64, 64), QIcon::Normal, QIcon::Off);
	QApplication::setWindowIcon(icon);

	app.setStyleSheet(LoadStyleSheet());

	if (CheckActivation(SettingsManager::organizationSettings.macAdd))
	{
		UserLoginWindow* userLoginWnd = new UserLoginWindow();
		userLoginWnd->show();

		return app.exec();
	}
	else
	{
		QMessageBox msg;
		msg.setText(QString::fromLocal8Bit("Bu yazýlýmý kullanma hakkýnýz yok! Ýlgili kiþiye ulaþýnýz!"));
		msg.setIcon(QMessageBox::Icon::Information);
		msg.exec();

		app.exit();
	}

	
}
