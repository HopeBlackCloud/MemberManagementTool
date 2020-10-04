#ifndef MAKBUZ_H
#define MAKBUZ_H

#include <QDialog>
#include "ui_Makbuz.h"

class Makbuz : public QDialog
{
	Q_OBJECT

	public:
		Makbuz(bool isBagis, int mem_ID, QStringList& years, QWidget *parent = 0);
		~Makbuz();

	public slots:
		void Save();
		void SelectedYear(QListWidgetItem*);

	private:
		Ui::Makbuz ui;

		bool isAidat;
		int paymentNo;

		QStringList notpaidmonthYears;

		bool CheckControls();

		QVector<int> payYearList;
};

#endif // MAKBUZ_H
