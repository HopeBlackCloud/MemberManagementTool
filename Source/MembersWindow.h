#ifndef MEMBERSWINDOW_H
#define MEMBERSWINDOW_H

#include <QDialog>
#include <qtimer.h>
#include "QtSql\qsqlquerymodel.h"

#include "ui_MembersWindow.h"
#include "Definitions.h"

class MembersWindow : public QDialog
{
	Q_OBJECT

	public:
		MembersWindow(QWidget *parent = 0);
		~MembersWindow();
	
	public slots:
		void LoadMembers();
		void OpenMembershipForm(QString&, bool);
		void DetailedViewClicked();
		void ActivateButtons(QModelIndex);
		void MemberFormClicked();
		void MemberCardClicked();
		void UpdateClicked();
		void NameQuery();
		void CellPhoneQuery();
		void SurnameQuery();
		void LoadAllMembers();
		void SexQuery();
		void EduLevelQuery();
		void BloodTypeQuery();
		void EduStatusQuery();
		void MemIdQuery();
		void MemTypeQuery();
		void MemStatusQuery();
		void MemAddressQuery();
		void ExportPDFClicked();
		void SendSMSClicked();

	private:
		void ChangeButtonStatus(bool);
		void SetHeaderTitle(QSqlQueryModel*);
		void MakeConnections();
		QString SetSelectedSort();
		void FillTableView(QUERY_TYPE_ENUM queryType, QString& queryValue = QString(""));

		QSqlQueryModel *model;
		QSqlQuery *qry;

		QModelIndex selectedIndex;
		QString photoFilePath;

		Ui::MembersWindow ui;
};

#endif // MEMBERSWINDOW_H
