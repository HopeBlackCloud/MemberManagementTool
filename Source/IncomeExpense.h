#ifndef INCOMEEXPENSE_H
#define INCOMEEXPENSE_H

#include <QDialog>
#include <QSqlTableModel>
#include "QtSql\qsqlquerymodel.h"

#include "ui_IncomeExpense.h"

class IncomeExpense : public QDialog
{
	Q_OBJECT

	public:
		IncomeExpense(QWidget *parent = 0);
		~IncomeExpense();

	public slots:
		void LoadIncomeExpense();
		void ExportPDFClicked();
		void QueryWithDate();
		void EnterExpenseClicked();
		void FillTableView(int type, QString startDate, QString endDate);

	private:
		Ui::IncomeExpense ui;
		QSqlTableModel *model;
		QSqlTableModel *modelExp;

		void SetHeaderTitle(QSqlQueryModel*);

		QSqlQueryModel *modelQuery;
		QSqlQuery *qry;
};

#endif // INCOMEEXPENSE_H
