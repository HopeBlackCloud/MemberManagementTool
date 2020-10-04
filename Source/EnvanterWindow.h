#ifndef ENVANTERWINDOW_H
#define ENVANTERWINDOW_H

#include <QDialog>
#include "ui_EnvanterWindow.h"
#include "QtSql\qsqltablemodel.h"

class EnvanterWindow : public QDialog
{
	Q_OBJECT

	public:
		EnvanterWindow(QWidget *parent);
		~EnvanterWindow();

	public slots:
		void AddClicked();
		void OKClicked();
		void DeleteClicked();
		void ActivateButtons(QModelIndex);

	private:
		Ui::EnvanterWindow ui;

		QSqlTableModel *model;
		QModelIndex selectedIndex;

		bool userSetup;
};

#endif // ENVANTERWINDOW_H
