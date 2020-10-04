#ifndef USERLOGINWINDOW_H
#define USERLOGINWINDOW_H

#include <QDialog>
#include "ui_UserLoginWindow.h"

class UserLoginWindow : public QDialog
{
	Q_OBJECT

public:
	UserLoginWindow(QWidget *parent = 0);
	~UserLoginWindow();

	public slots:
		void OKClicked();
		void CancelClicked();

private:
	void SetOrganizationLogo();
	
	Ui::UserLoginWindow ui;
};

#endif // USERLOGINWINDOW_H
