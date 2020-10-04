#include "MembershipCard.h"
#include "qdatetime.h"
#include "qpainter.h"
#include "qpdfwriter.h"
#include "qfile.h"
#include "qrect.h"
#include <QFileDialog>
#include "qmessagebox.h"
#include "QtPrintSupport\qprinter.h"
#include "QtPrintSupport\qprintdialog.h"

#include "Definitions.h"
#include "DatabaseManager.h"
#include "SettingsManager.h"

MembershipCard::MembershipCard(QString& mem_ID, QWidget *parent)
	: QDialog(parent,  Qt::FramelessWindowHint)
{
	ui.setupUi(this);

	connect(ui.toolButtonSavePDF, SIGNAL(clicked()), this, SLOT(SavePDFClicked()));
	connect(ui.toolButtonPrint, SIGNAL(clicked()), this, SLOT(PrintClicked()));

	memberName = "";
	FillControlWithMemberID(mem_ID);
}

MembershipCard::~MembershipCard()
{

}

void MembershipCard::SavePDFClicked()
{
	QString path = QApplication::applicationDirPath() + DOCS_PATH;

	QFileDialog fileDlg;
	QString fileName = fileDlg.getSaveFileName(this, QString::fromLocal8Bit("Dosyaya bir isim veriniz ..."), path, "*.pdf");
	fileName+=".pdf";

    QFile pdfFile(fileName);
    if (pdfFile.open(QIODevice::WriteOnly))
	{
		QPdfWriter* writer = new QPdfWriter(&pdfFile);
		QPainter* painter = new QPainter(writer);
		writer->setPageSize(QPagedPaintDevice::A7);
		
		QPixmap pm = this->grab();
		painter->drawPixmap(QRect(0, 0, writer->logicalDpiX() * 8.0, writer->logicalDpiY() * 12.5), pm);
		painter->end();

		SAFE_DELETE(painter);
		SAFE_DELETE(writer);
		pdfFile.close();

		//QMessageBox::information(this, QApplication::applicationDisplayName(), QString::fromLocal8Bit("Üye kayýt formu %1 altýna kaydedildi.").arg(path), QMessageBox::Ok);

		this->close();
	}
	else
	{
		QMessageBox::critical(this, QApplication::applicationDisplayName(), QString::fromLocal8Bit("Üye kayýt formu %1 altýna kaydedilemedi.").arg(path), QMessageBox::Ok);
	}
}

void MembershipCard::PrintClicked()
{
    QPrinter printer;
    printer.setPaperSize(QPrinter::A7);
	//printer.setOrientation(QPrinter::Landscape);
	printer.setCopyCount(2);

	QPixmap pm = this->grab();

    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (dlg->exec() == QDialog::Accepted)
    {
        QPainter painter(&printer);
        painter.drawPixmap(QRect(0, 0, printer.logicalDpiX() * 20.0, printer.logicalDpiY() * 8.5), pm);
        painter.end();
    }  
}

void MembershipCard::FillControlWithMemberID(QString& mem_ID)
{
	QString photoPath = "";

	ui.labelOrgName->setText(QApplication::applicationDisplayName().toUpper());
	ui.labelMemeberRecordNo->setText(QString::fromLocal8Bit("ÜYE NO : "));

	ui.labelOrgEMail->setText(SettingsManager::organizationSettings.email);
	ui.labelOrgAdres->setText(SettingsManager::organizationSettings.address);
	ui.labelOrgTel->setText(SettingsManager::organizationSettings.telFax);

	Database::Connect();

	QSqlQuery *qry = new QSqlQuery(QSqlDatabase::defaultConnection);

	qry->prepare("select mem_no, mem_name, mem_surname, mem_photopath from membersinfo where mem_no = " + mem_ID);
	qry->exec();

	if (qry->next())
	{
		memberName = qry->value(1).toString() + qry->value(2).toString();

		ui.labelMemebershipNo->setText(qry->value(0).toString());
		ui.labelNameSurname->setText(qry->value(1).toString() + " " + qry->value(2).toString());

		photoPath = qry->value(3).toString();
	}

	qry->finish();

	Database::Disconnect();

	QPixmap picture(photoPath);
	ui.labelMemberPhoto->setPixmap(picture);

	QString path = QApplication::applicationDirPath() + PHOTO_PATH + "logo.JPG";
	QPixmap logo(path);
	ui.labelOrgLogo->setPixmap(logo);
}

