#ifndef ADDMEMBERSWINDOW_H
#define ADDMEMBERSWINDOW_H

#include <QDialog>
#include "SMSManager.h"
#include "ui_AddMemberWindow.h"
#include "QtPrintSupport\qprinter.h"
#include "QtPrintSupport\qprintdialog.h"

class AddMemberWindow : public QDialog
{
	Q_OBJECT

	public:
		AddMemberWindow(QWidget *parent = 0);
		~AddMemberWindow();
	
	public slots:
		void AddClicked();
		void ClearControls();
		void AddPhotoClicked();
		void OnHomeAddressTextChanged();
		void OnKurumAddressTextChanged();
		void OpenMembershipForm(QString&, bool);

	private:
		bool CheckControls();
		void MakeConnections();
		bool IsEmailValidated();
		bool IsCellPhoneValidated();
		bool IsCompanyPhoneValidated();
		bool IsMemNoValidated();

		QString photoFilePath;

		Ui::AddMemberWindow ui;
};

#endif // ADDMEMBERSWINDOW_H
