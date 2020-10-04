#include "SMSPreviewWindow.h"
#include "SettingsManager.h"

SMSPreviewWindow::SMSPreviewWindow( QString& info, QString& msgText, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	move(0, 0); 

	setWindowFlags(Qt::FramelessWindowHint);

	ui.labelInfo->setText(info);
	ui.labelSMSMesaj->setText(msgText);
	ui.labelOriginator->setText(SettingsManager::organizationSettings.orgName);

	connect(ui.toolButtonSend, SIGNAL(clicked()), this, SLOT(accept()));
	connect(ui.toolButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));
}

SMSPreviewWindow::~SMSPreviewWindow()
{

}
