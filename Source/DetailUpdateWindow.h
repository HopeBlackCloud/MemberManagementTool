#ifndef DETAILUPDATEWINDOW_H
#define DETAILUPDATEWINDOW_H

#include <QDialog>
#include "ui_DetailUpdateWindow.h"

class DetailUpdateWindow : public QDialog
{
	Q_OBJECT

	public:
		DetailUpdateWindow(QWidget *parent, bool isUpdate, QString& _mem_ID);
		~DetailUpdateWindow();

	public slots:
		void UpdateClicked();
		void AddPhotoClicked();

	private:
		Ui::DetailUpdateWindow ui;
		void FillControlWithMemberID(QString& mem_ID);

		bool IsEmailValidated();
		bool IsCellPhoneValidated();
		bool IsCompanyPhoneValidated();
		bool IsMemNoValidated();

		QString mem_ID;

		QString photoFilePath;
};

#endif // DETAILUPDATEWINDOW_H
