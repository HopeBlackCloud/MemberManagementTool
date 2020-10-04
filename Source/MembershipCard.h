#ifndef MEMBERSHIPCARD_H
#define MEMBERSHIPCARD_H

#include <QDialog>
#include "ui_MembershipCard.h"

class MembershipCard : public QDialog
{
	Q_OBJECT

	public:
		MembershipCard(QString &, QWidget *parent = 0);
		~MembershipCard();

	public slots:
		void SavePDFClicked();
		void PrintClicked();

	private:
		void FillControlWithMemberID(QString &);
		
		QString memberName;
		Ui::MembershipCard ui;
};

#endif // MEMBERSHIPCARD_H
