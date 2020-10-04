#ifndef SMSPREVIEWWINDOW_H
#define SMSPREVIEWWINDOW_H

#include <QDialog>
#include "ui_SMSPreviewWindow.h"

class SMSPreviewWindow : public QDialog
{
	Q_OBJECT

public:
	SMSPreviewWindow(QString& info, QString& msgText, QWidget *parent = 0);
	~SMSPreviewWindow();

private:
	Ui::SMSPreviewWindow ui;
};

#endif // SMSPREVIEWWINDOW_H
