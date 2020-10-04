#include "MainWindow.h"
#include "SetupWindow.h"
#include "Definitions.h"
#include "MessageManager.h"
#include "SettingsManager.h"
#include "DatabaseManager.h"

#include <QDir>
#include <QMenu>
#include <QDatetime>
#include <QMessageBox>
#include <QTableWidgetItem>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	UpdateDateTimeLabel();

	envanterWnd  = NULL;
	memberWnd    = NULL;
	addMemberWnd = NULL;
	exportWnd    = NULL;
	memberFeeWnd = NULL;
	libraryWnd   = NULL;
	incomeExpenseWnd = NULL;
	sendSMSWnd = NULL;

	messageRefreshTimer = NULL;

	addMemberWnd = new AddMemberWindow(this);
	ui.stackedWidgetMain->insertWidget(PAGE_TYPE_ENUM_ADD_MEMBER, addMemberWnd);

	memberWnd = new MembersWindow(this);
	ui.stackedWidgetMain->insertWidget(PAGE_TYPE_ENUM_MEMBER_INFO, memberWnd);

	exportWnd = new ExportWindow(this);
	ui.stackedWidgetMain->insertWidget(PAGE_TYPE_ENUM_EXPORT, exportWnd);

	memberFeeWnd = new MemberFee(this);
	ui.stackedWidgetMain->insertWidget(PAGE_TYPE_ENUM_MEMBER_FEE, memberFeeWnd);

	libraryWnd = new LibraryWindow(this);
	ui.stackedWidgetMain->insertWidget(PAGE_TYPE_ENUM_LIBRARY, libraryWnd);

	incomeExpenseWnd = new IncomeExpense(this);
	ui.stackedWidgetMain->insertWidget(PAGE_TYPE_ENUM_INCOMEEXPENSE, incomeExpenseWnd);

	sendSMSWnd = new SendSMSWindow(this);
	ui.stackedWidgetMain->insertWidget(PAGE_TYPE_ENUM_SEND_SMS, sendSMSWnd);

	messageRefreshTimer = new QTimer(this);
	messageRefreshTimer->start(TIME_UPDATE_FREQUENCY);

	ui.labelLicence->setText(QString::fromLocal8Bit("Lisans : %1").arg(SettingsManager::organizationSettings.orgName));
	ui.labelFoundationName->setText(QString::fromLocal8Bit("%1 \nBÝLGÝ YÖNETÝMÝ SÝSTEMÝ").arg(SettingsManager::organizationSettings.orgName.toStdString().c_str())); 
		              
	ui.stackedWidgetSubMenu->setCurrentIndex(SUBMENU_TYPE_ENUM_NO_SUBMENU);

	connect(ui.toolButtonOKU,      SIGNAL(clicked()), this, SLOT(OKUClicked()));
	connect(ui.toolButtonKurum,    SIGNAL(clicked()), this, SLOT(KurumClicked()));
    connect(ui.toolButtonMember,   SIGNAL(clicked()), this, SLOT(MemberClicked()));
	connect(ui.toolButtonSettings, SIGNAL(clicked()), this, SLOT(SettingsClicked()));
	connect(ui.toolButtonAnaSayfa, SIGNAL(clicked()), this, SLOT(MainWindowClicked()));

	connect(ui.toolButtonClose,    SIGNAL(clicked()), this, SLOT(CloseClicked()));
	connect(ui.toolButtonCloseApp, SIGNAL(clicked()), this, SLOT(CloseAppClicked()));
	connect(ui.toolButtonClosePC,  SIGNAL(clicked()), this, SLOT(ClosePCClicked()));

	connect(ui.toolButtonAddMember,  SIGNAL(clicked()), this, SLOT(AddMemberClicked()));
	connect(ui.toolButtonMemberInfo, SIGNAL(clicked()), this, SLOT(MemberInfoClicked()));
	connect(ui.toolButtonExport,     SIGNAL(clicked()), this, SLOT(ExportClicked()));
	connect(ui.toolButtonAidat,      SIGNAL(clicked()), this, SLOT(MemberFeeClicked()));
	connect(ui.toolButtonFeeSetup,   SIGNAL(clicked()), this, SLOT(FeeSetupClicked()));
	connect(ui.toolButtonUserSetup,  SIGNAL(clicked()), this, SLOT(UserSetupClicked()));
	connect(ui.toolButtonLogSetup,   SIGNAL(clicked()), this, SLOT(LogSetupClicked()));
	connect(ui.toolButtonEnvanter,   SIGNAL(clicked()), this, SLOT(EnvanterClicked()));
	connect(ui.toolButtonLibrary,    SIGNAL(clicked()), this, SLOT(LibraryClicked()));
	connect(ui.toolButtonIncome,     SIGNAL(clicked()), this, SLOT(IncomeExpenseClicked()));
	connect(ui.toolButtonSendSMS,    SIGNAL(clicked()), this, SLOT(SendSMSClicked()));
	
	connect(messageRefreshTimer,  SIGNAL(timeout()), this, SLOT(UpdateDateTimeLabel()));
	connect(messageRefreshTimer,  SIGNAL(timeout()), this, SLOT(UpdateLogMessages()));

	QIcon icon;
    icon.addFile(QApplication::applicationDirPath() + QString("/Fotos/logo.JPG"), QSize(64, 64), QIcon::Normal, QIcon::Off);
	QApplication::setWindowIcon(icon);
	ui.toolButton->setIcon(icon);

	QString path = QApplication::applicationDirPath() + PHOTO_PATH + "logo.JPG";
	QPixmap logo(path);
	ui.labelFoto->setPixmap(logo);

	IsTimeToLoadAnnualyMemberFee();

	Database::AddLastYear(QDateTime::currentDateTime().date().year());
}


MainWindow::~MainWindow()
{
	SAFE_DELETE(envanterWnd);
	SAFE_DELETE(memberWnd);
	SAFE_DELETE(exportWnd);
	SAFE_DELETE(memberFeeWnd);
	SAFE_DELETE(addMemberWnd);
	SAFE_DELETE(libraryWnd);
	SAFE_DELETE(incomeExpenseWnd);
	SAFE_DELETE(sendSMSWnd);
	SAFE_DELETE(messageRefreshTimer);
}

void MainWindow::IsTimeToLoadAnnualyMemberFee()
{
	int previousYear = Database::GetLastYear();
    int thisYear = QDateTime::currentDateTime().date().year();
	short yearDiff = 0;

	if (thisYear > previousYear)
	{
		QString msgText = QString::fromLocal8Bit("[%1] yýlý için aidatlar güncellenmelidir. Yeni yýla girildi.").arg(thisYear);
		MessageManager::GetInstance()->AddInfoMessage(msgText);

		QMessageBox msg;
		msg.setText(msgText);
		msg.setIcon(QMessageBox::Icon::Warning);
		msg.exec();

		Database::AddFeeLoaded(true);
	}
}

void MainWindow::DisableButtons(bool currentUserType)
{
	memberFeeWnd->setEnabled(currentUserType);
	ui.toolButtonAddMember->setEnabled(currentUserType);
	ui.toolButtonExport->setEnabled(currentUserType);
	ui.toolButtonSettings->setEnabled(currentUserType);

	ui.toolButtonAidat->setEnabled(currentUserType);
	ui.toolButtonIncome->setEnabled(currentUserType);
}

void MainWindow::OKUClicked()
{
	ui.toolButtonKurum->setChecked(false);
	ui.toolButtonMember->setChecked(false);
	ui.toolButtonClose->setChecked(false);
	ui.toolButtonSettings->setChecked(false);

	if (ui.toolButtonOKU->isChecked())
		ui.stackedWidgetSubMenu->setCurrentIndex(SUBMENU_TYPE_ENUM_EDUCATION_OPERATIONS);
	else
		ui.stackedWidgetSubMenu->setCurrentIndex(SUBMENU_TYPE_ENUM_NO_SUBMENU);
}

void MainWindow::KurumClicked()
{
	ui.toolButtonMember->setChecked(false);
	ui.toolButtonOKU->setChecked(false);
	ui.toolButtonClose->setChecked(false);
	ui.toolButtonSettings->setChecked(false);

	if (ui.toolButtonKurum->isChecked())
		ui.stackedWidgetSubMenu->setCurrentIndex(SUBMENU_TYPE_ENUM_ORGANIZATION_OPERATIONS);
	else
		ui.stackedWidgetSubMenu->setCurrentIndex(SUBMENU_TYPE_ENUM_NO_SUBMENU);
}

void MainWindow::SettingsClicked()
{
	ui.toolButtonMember->setChecked(false);
	ui.toolButtonKurum->setChecked(false);
	ui.toolButtonOKU->setChecked(false);
	ui.toolButtonClose->setChecked(false);

	if (ui.toolButtonSettings->isChecked())
		ui.stackedWidgetSubMenu->setCurrentIndex(SUBMENU_TYPE_ENUM_SYSTEM_SETUP);
	else
		ui.stackedWidgetSubMenu->setCurrentIndex(SUBMENU_TYPE_ENUM_NO_SUBMENU);
}

void MainWindow::MainWindowClicked()
{
	ui.stackedWidgetMain->setCurrentWidget(ui.centralWidget);
}

void MainWindow::EnvanterClicked()
{
	EnvanterWindow *envanterWnd = new EnvanterWindow(this);
		envanterWnd->setAttribute(Qt::WA_DeleteOnClose);
	envanterWnd->show();

	ui.stackedWidgetSubMenu->setCurrentIndex(SUBMENU_TYPE_ENUM_NO_SUBMENU);
	ui.toolButtonKurum->setChecked(false);
}

void MainWindow::LibraryClicked()
{
	ui.stackedWidgetMain->setCurrentIndex(PAGE_TYPE_ENUM_LIBRARY);
	
	ui.stackedWidgetSubMenu->setCurrentIndex(SUBMENU_TYPE_ENUM_NO_SUBMENU);
	ui.toolButtonOKU->setChecked(false);
}

void MainWindow::IncomeExpenseClicked()
{
	ui.stackedWidgetMain->setCurrentIndex(PAGE_TYPE_ENUM_INCOMEEXPENSE);
	
	ui.stackedWidgetSubMenu->setCurrentIndex(SUBMENU_TYPE_ENUM_NO_SUBMENU);
	ui.toolButtonKurum->setChecked(false);

	incomeExpenseWnd->LoadIncomeExpense();
}

void MainWindow::SendSMSClicked()
{
	ui.stackedWidgetMain->setCurrentIndex(PAGE_TYPE_ENUM_SEND_SMS);
	
	ui.stackedWidgetSubMenu->setCurrentIndex(SUBMENU_TYPE_ENUM_NO_SUBMENU);
	ui.toolButtonOKU->setChecked(false);

	sendSMSWnd->LoadMembersGroups(true);
	sendSMSWnd->CheckNetConnection();
}

void MainWindow::MemberClicked()
{
	ui.toolButtonKurum->setChecked(false);
	ui.toolButtonOKU->setChecked(false);
	ui.toolButtonClose->setChecked(false);
	ui.toolButtonSettings->setChecked(false);

	if (ui.toolButtonMember->isChecked())
		ui.stackedWidgetSubMenu->setCurrentIndex(SUBMENU_TYPE_ENUM_MEMBER_OPERATIONS);
	else
		ui.stackedWidgetSubMenu->setCurrentIndex(SUBMENU_TYPE_ENUM_NO_SUBMENU);
}

void MainWindow::MemberInfoClicked()
{
	ui.stackedWidgetMain->setCurrentIndex(PAGE_TYPE_ENUM_MEMBER_INFO);

	ui.stackedWidgetSubMenu->setCurrentIndex(SUBMENU_TYPE_ENUM_NO_SUBMENU);
	memberWnd->LoadAllMembers();

	ui.toolButtonMember->setChecked(false);
}

void MainWindow::AddMemberClicked()
{
	ui.stackedWidgetMain->setCurrentIndex(PAGE_TYPE_ENUM_ADD_MEMBER);
	
	ui.stackedWidgetSubMenu->setCurrentIndex(SUBMENU_TYPE_ENUM_NO_SUBMENU);
	ui.toolButtonMember->setChecked(false);
}

void MainWindow::ExportClicked()
{
	ui.stackedWidgetMain->setCurrentIndex(PAGE_TYPE_ENUM_EXPORT);
	
	ui.stackedWidgetSubMenu->setCurrentIndex(SUBMENU_TYPE_ENUM_NO_SUBMENU);
	ui.toolButtonMember->setChecked(false);
}

void MainWindow::OpenSetupWindow(SETTINGS_TYPE_ENUM settingType)
{
	SetupWindow *setupWnd = new SetupWindow(this, settingType);
	setupWnd->setAttribute(Qt::WA_DeleteOnClose);
	setupWnd->show();

	ui.stackedWidgetSubMenu->setCurrentIndex(SUBMENU_TYPE_ENUM_NO_SUBMENU);
	ui.toolButtonSettings->setChecked(false);

}

void MainWindow::FeeSetupClicked()
{
	OpenSetupWindow(SETTINGS_TYPE_FEES);
}

void MainWindow::UserSetupClicked()
{
	OpenSetupWindow(SETTINGS_TYPE_USERS);
}

void MainWindow::LogSetupClicked()
{
	OpenSetupWindow(SETTINGS_TYPE_LOGS);
}

void MainWindow::MemberFeeClicked()
{
	ui.stackedWidgetMain->setCurrentIndex(PAGE_TYPE_ENUM_MEMBER_FEE);
	
	ui.stackedWidgetSubMenu->setCurrentIndex(SUBMENU_TYPE_ENUM_NO_SUBMENU);
	ui.toolButtonKurum->setChecked(false);

	memberFeeWnd->LoadFees();
}

void MainWindow::CloseAppClicked()
{
	WriteLogsToFile();

	this->close();
}

void MainWindow::WriteLogsToFile()
{
	
}

void MainWindow::ClosePCClicked()
{
	WriteLogsToFile();

	SettingsManager::systemShutdown();
}

void MainWindow::CloseClicked()
{
	ui.toolButtonMember->setChecked(false);
	ui.toolButtonKurum->setChecked(false);
	ui.toolButtonOKU->setChecked(false);
	ui.toolButtonSettings->setChecked(false);

	if (ui.toolButtonClose->isChecked())
		ui.stackedWidgetSubMenu->setCurrentIndex(SUBMENU_TYPE_ENUM_SHUTDOWN);
	else
		ui.stackedWidgetSubMenu->setCurrentIndex(SUBMENU_TYPE_ENUM_NO_SUBMENU);
}

void MainWindow::UpdateDateTimeLabel()
{
	QString currentDate = QDateTime::currentDateTime().toString("dd.MM.yyyy");
	QString currentTime = QDateTime::currentDateTime().toString("hh:mm:ss");

	currentDate = "Tarih : " + currentDate;
	currentTime = "Saat : " + currentTime;

	ui.labelDateTime->setText(currentDate +"   "+ currentTime);
}

void MainWindow::UpdateLogMessages()
{
	static int rowcount = 0;
	rowcount = ui.tableWidgetMessages->rowCount();

	QStringList &infoList = MessageManager::GetInstance()->GetInfoList();
	
	QMutableStringListIterator iter(infoList);
	while(iter.hasNext())
	{
		QString msg = iter.next();
		AppendTimeTagToMessage(msg);

		QTableWidgetItem *newItem = new QTableWidgetItem(msg);
		
		ui.tableWidgetMessages->insertRow(rowcount);
		ui.tableWidgetMessages->setItem(rowcount, 0, newItem);

		iter.remove();

		rowcount++;
	}
}

void MainWindow::AppendTimeTagToMessage(QString &msg)
{
	QString currentTime = QDateTime::currentDateTime().toString("dd.MM.yyyy  hh:mm:ss");
	
	msg = currentTime + " --> " + msg;
}


