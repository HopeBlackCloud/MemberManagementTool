#ifndef LIBRARYWINDOW_H
#define LIBRARYWINDOW_H

#include <QDialog>
#include "ui_LibraryWindow.h"
#include "QtSql\qsqltablemodel.h"
#include "QNetworkReply.h"

class LibraryWindow : public QDialog
{
	Q_OBJECT

	public:
		LibraryWindow(QWidget *parent = 0);
		~LibraryWindow();

	public slots:
		void LoadBooks();
		void QueryClicked();
		void AddClicked();
		void OKClicked();
		void ShowAllClicked();
		void DeleteClicked();
		void TakenClicked();
		void ActivateButtons(QModelIndex index);
		void replyFinished(QNetworkReply*);

	private:
		Ui::LibraryWindow ui;

		QSqlTableModel *model;
		QModelIndex selectedIndex;

		void ChilkatSample(void);

		void SetHeaderTitle(QSqlTableModel*);
};

#endif // LIBRARYWINDOW_H
