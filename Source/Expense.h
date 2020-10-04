#ifndef EXPENSE_H
#define EXPENSE_H

#include <QDialog>
#include "ui_Expense.h"

class Expense : public QDialog
{
	Q_OBJECT

	public:
		Expense(QWidget *parent = 0);
		~Expense();

	public slots:
		void Save();

	private:
		Ui::Expense ui;

		bool CheckControls();
};

#endif // Expense_H
