#ifndef SENDSMSWINDOW_H
#define SENDSMSWINDOW_H

#include <QDialog>
#include "ui_SendSMSWindow.h"
#include "QtSql\qsqlquerymodel.h"

class SendSMSWindow : public QDialog
{
	Q_OBJECT

	public:
		SendSMSWindow(QWidget *parent = 0);
		~SendSMSWindow();

		void LoadMembersGroups(bool isGroup);
		bool CheckNetConnection();
	
	public slots:
		void ActivateButtons(QModelIndex);
		void ActivateButtonsGr(QModelIndex);

		void ClearClicked();
		void SendClicked();
		void ClearSMSClicked();
		void ComboClicked(int index);
		void MakeGroupClicked();
		void DeleteGroupClicked();
		void SMSTextChanged();
		
	private:
		QString PrepareQueryString(QStringList &);

		QSqlQueryModel *model;
		QSqlQuery *qry;

		QSqlQueryModel *grmodel;
		QSqlQuery *grqry;

		QString numberList;

		QModelIndex selectedIndex;
		QModelIndex selectedIndexGr;
		void SetHeaderTitle(QSqlQueryModel*);
		void SetHeaderTitleGr(QSqlQueryModel*);

		void MakeNewGroup(QString& grName);
		void MakeConnections();
		void FillNumberList();

		Ui::SendSMSWindow ui;
};

#endif // SendSMSWindow_H
