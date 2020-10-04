#include "MembershipForm.h"
#include "qdatetime.h"
#include "qpainter.h"
#include "qpdfwriter.h"
#include "qfile.h"
#include "qrect.h"
#include "qmessagebox.h"
#include "QtPrintSupport\qprinter.h"
#include "QtPrintSupport\qprintdialog.h"

#include "Definitions.h"
#include "DatabaseManager.h"
#include "SettingsManager.h"

#include <QFileDialog>


MembershipForm::MembershipForm(QString& mem_ID, bool isNewlyAdded, QString& memDate, QWidget *parent)
	: QDialog(parent,  Qt::FramelessWindowHint)
{
	ui.setupUi(this);

	connect(ui.toolButtonSavePDF, SIGNAL(clicked()), this, SLOT(SavePDFClicked()));
	connect(ui.toolButtonPrint, SIGNAL(clicked()), this, SLOT(PrintClicked()));

	memberName = "";
	FillControlWithMemberID(mem_ID, isNewlyAdded, memDate);
}

MembershipForm::~MembershipForm()
{

}

void MembershipForm::SavePDFClicked()
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
		painter->setRenderHint( QPainter::Antialiasing);
		writer->setPageSize(QPagedPaintDevice::A4);
		QPixmap pm = this->grab();
		painter->drawPixmap(QRect(0, 0, writer->logicalDpiX() * 8.0, writer->logicalDpiY() * 12.5), pm);
		painter->end();

		SAFE_DELETE(painter);
		SAFE_DELETE(writer);
		pdfFile.close();

		QMessageBox::information(this, QApplication::applicationDisplayName(), QString::fromLocal8Bit("Üye kayýt formu %1 altýna kaydedildi.").arg(path), QMessageBox::Ok);

		this->close();
	}
	else
	{
		QMessageBox::critical(this, QApplication::applicationDisplayName(), QString::fromLocal8Bit("Üye kayýt formu %1 altýna kaydedilemedi.").arg(path), QMessageBox::Ok);
	}
}

void MembershipForm::PrintClicked()
{
    QPrinter printer;
    printer.setPaperSize(QPrinter::A4);
	printer.setCopyCount(2);

	QPixmap pm = this->grab();

    QPrintDialog *dlg = new QPrintDialog(&printer, 0);
    if (dlg->exec() == QDialog::Accepted)
    {
        QPainter painter(&printer);
        painter.drawPixmap(QRect(0, 0, printer.logicalDpiX() * 8.0, printer.logicalDpiY() * 12.5), pm);
        painter.end();
    }  
}

void MembershipForm::FillControlWithMemberID(QString& mem_ID, bool isNewlyAdded, QString& memDate)
{
	QString photoPath = "";

	ui.labelOrgName->setText(QApplication::applicationDisplayName().toUpper());
	ui.labelDate->setText(memDate);
	ui.labelOrgEMail->setText(SettingsManager::organizationSettings.email);
	ui.labelOrgAdres->setText(SettingsManager::organizationSettings.address);
	ui.labelOrgTel->setText(SettingsManager::organizationSettings.telFax);

	QString path = QApplication::applicationDirPath() + PHOTO_PATH + "logo.JPG";
	QPixmap logo(path);
	ui.labelOrgLogo->setPixmap(logo);

	ui.labelMessage->setText(ui.labelMessage->text().prepend(SettingsManager::organizationSettings.orgName));

	if (!isNewlyAdded)
	{
		ui.labelHeaderMemberRecord->setText(QString::fromLocal8Bit("ÜYE BÝLGÝ FORMU"));
		ui.labelYon->setVisible(false);
		ui.labelMessage->setText(QString::fromLocal8Bit("NOT: Bu belge üyenin isteði üzerine düzenlenmiþtir.\n\n         %1 Yönetim Kurulu").arg(SettingsManager::organizationSettings.orgName));
	}

	Database::Connect();

	QSqlQuery *qry = new QSqlQuery(QSqlDatabase::defaultConnection);

	qry->prepare("select mem_no, mem_name, mem_surname, mem_birthplace, mem_birthdate, mem_cellphone, mem_email, mem_occupation,  mem_homeaddress, mem_bloodtype, mem_fathername, mem_photopath from membersinfo where mem_no = " + mem_ID);
	qry->exec();

	if (qry->next())
	{
		memberName = qry->value(1).toString() + qry->value(2).toString();

		ui.labelMemebershipNo->setText(qry->value(0).toString());
		ui.labelNameSurname->setText(qry->value(1).toString() + " " + qry->value(2).toString());
		ui.labelBirthDate->setText(qry->value(4).toString());
		ui.labelBirthPlace->setText(qry->value(3).toString());
		ui.labelCellPhone->setText(qry->value(5).toString());
		ui.labelKanGrup->setText(qry->value(9).toString());
		ui.labelOccupation->setText(qry->value(7).toString());
		ui.labelHomeAddress->setText(qry->value(8).toString());
		ui.labelFatherName->setText(qry->value(10).toString());
		photoPath = qry->value(11).toString();
	}

	qry->finish();

	Database::Disconnect();

	QPixmap picture(photoPath);
	ui.labelMemberPhoto->setPixmap(picture);
}

