#ifndef PAYMENTSWINDOW_H
#define PAYMENTSWINDOW_H

#include <QDialog>
#include "ui_PaymentsWindow.h"
#include "QtSql\qsqlquerymodel.h"

class PaymentsWindow : public QDialog
{
	Q_OBJECT

	public:
		PaymentsWindow(int memID, QWidget *parent = 0);
		~PaymentsWindow();

	public slots:
		void LoadPayments(int memID);

	private:
		Ui::PaymentsWindow ui;

		QSqlQueryModel *model;
		QSqlQuery *qry;

		QModelIndex selectedIndex;

		void SetHeaderTitle(QSqlQueryModel*);
};

#endif // PAYMENTSWINDOW_H
