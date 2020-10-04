#ifndef MEMBERFEE_H
#define MEMBERFEE_H

#include <QDialog>
#include "ui_MemberFee.h"
#include "QtSql\qsqlquerymodel.h"

class MemberFee : public QDialog
{
	Q_OBJECT

	public:
		MemberFee(QWidget *parent = 0);
		~MemberFee();

	public slots:
		void LoadFees();
		void PayFeeBagisClicked();
		void PayFeeAidatClicked();
		void ActivateButtons(QModelIndex);
		void BorcluClicked();
		void QueryNameClicked();
		void PaymentDetailClicked();
		void BorclandirClicked();
		void BorcluSMSClicked();

	private:
		Ui::MemberFee ui;

		QSqlQueryModel *model;
		QSqlQuery *qry;

		QModelIndex selectedIndex;
		void SetHeaderTitle(QSqlQueryModel*);
		void MakeConnections();
		void ChangeButtonStatus(bool status);
};

#endif // MEMBERFEE_H
