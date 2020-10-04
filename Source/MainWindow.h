#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_MainWindow.h"

#include "MemberFee.h"
#include "ExportWindow.h"
#include "LibraryWindow.h"
#include "MembersWindow.h"
#include "EnvanterWindow.h"
#include "AddMemberWindow.h"
#include "IncomeExpense.h"
#include "SendSMSWindow.h"

#include <QTimer>
#include <QtWidgets/QMainWindow>

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow(QWidget *parent = 0);
		~MainWindow();

		void DisableButtons(bool currentUserType);

	public slots:
		void OKUClicked();
		void KurumClicked();
		void MemberClicked();
		void CloseClicked();
		void UpdateDateTimeLabel();
		void UpdateLogMessages();
		void AddMemberClicked();
		void MemberInfoClicked();
	    void ClosePCClicked();
		void CloseAppClicked();
		void ExportClicked();
		void MemberFeeClicked();
		void SettingsClicked();
		void MainWindowClicked();
		void FeeSetupClicked();
		void UserSetupClicked();
		void LogSetupClicked();
		void EnvanterClicked();
		void LibraryClicked();
		void IncomeExpenseClicked();
		void SendSMSClicked();

	private:
		QTimer *messageRefreshTimer;

		AddMemberWindow *addMemberWnd;
		MembersWindow   *memberWnd;
		ExportWindow    *exportWnd;
		EnvanterWindow  *envanterWnd;
		MemberFee       *memberFeeWnd; 
		LibraryWindow   *libraryWnd; 
		IncomeExpense   *incomeExpenseWnd;
		SendSMSWindow   *sendSMSWnd;

		void AppendTimeTagToMessage(QString &msg);
		void OpenSetupWindow(SETTINGS_TYPE_ENUM settingType);

		void WriteLogsToFile();
		void IsTimeToLoadAnnualyMemberFee();

		Ui::MainWindowClass ui;
};

#endif // MAINWINDOW_H
