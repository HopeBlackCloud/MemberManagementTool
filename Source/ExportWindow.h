#ifndef EXPORTWINDOW_H
#define EXPORTWINDOW_H

#include <QDialog>
#include "ui_ExportWindow.h"

class ExportWindow : public QDialog
{
	Q_OBJECT

	public:
		ExportWindow(QWidget *parent = 0);
		~ExportWindow();

	public slots:
		void ExportPDFClicked();
		void ExportExcelClicked();
		void InsertClicked();
		void MeetingDocClicked();
		void UpClicked();
		void DownClicked();
		void DeleteClicked();
		void ClearClicked();
		void ListItemClicked(QListWidgetItem*);
		void SelectItemClicked(QListWidgetItem*);
		
	private:
		QString PrepareQueryString(QStringList &);
		QStringList GetSelectedHeadersList();

		Ui::ExportWindow ui;
};

#endif // EXPORTWINDOW_H
