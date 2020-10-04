#ifndef SETUPWINDOW_H
#define SETUPWINDOW_H

#include <QDialog>
#include "ui_SetupWindow.h"
#include "Definitions.h"
#include "QtSql\qsqltablemodel.h"

class SetupWindow : public QDialog
{
	Q_OBJECT

	public:
		SetupWindow(QWidget *parent, SETTINGS_TYPE_ENUM isUserSetup);
		~SetupWindow();

	public slots:
		void AddClicked();
		void OKClicked();
		void DeleteClicked();
		void ActivateButtons(QModelIndex);

	private:
		Ui::SetupWindow ui;

		QSqlTableModel *model;
		QModelIndex selectedIndex;

		SETTINGS_TYPE_ENUM userSetup;

		void HideButtons(bool hide);
};

#endif // SETUPWINDOW_H
