#ifndef MEMBERSHIPFORM_H
#define MEMBERSHIPFORM_H

#include <QDialog>
#include "ui_MembershipForm.h"

class MembershipForm : public QDialog
{
	Q_OBJECT

	public:
		MembershipForm(QString&, bool, QString&, QWidget *parent = 0);
		~MembershipForm();

	public slots:
		void SavePDFClicked();
		void PrintClicked();

	private:
		void FillControlWithMemberID(QString &, bool, QString& );
		
		QString memberName;
		Ui::MembershipForm ui;
};

#endif // MEMBERSHIPFORM_H
