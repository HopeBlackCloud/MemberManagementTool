#include "MembersWindow.h"
#include "DatabaseManager.h"
#include "MembershipForm.h"
#include "MembershipCard.h"
#include "DetailUpdateWindow.h"
#include "MessageManager.h"
#include "SettingsManager.h"
#include "SMSPreviewWindow.h"
#include "SMSManager.h"

#include <QFile>
#include <QPainter>
#include <QFileDialog>
#include <QPdfWriter>

#include "qmessagebox.h"
#include "qfiledialog.h"
#include <QInputDialog>

MembersWindow::MembersWindow(QWidget *parent)
	: QDialog(parent, Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint)
{
	ui.setupUi(this);

	photoFilePath = "";

	qry = NULL;
	model = NULL;

	model = new QSqlQueryModel();
	qry = new QSqlQuery(QSqlDatabase::defaultConnection);

	LoadMembers();

	MakeConnections();
}

void MembersWindow::MakeConnections()
{
	connect(ui.toolButtonMemberForm,         SIGNAL(clicked()), this, SLOT(MemberFormClicked()));
	connect(ui.toolButtonMemberCard,         SIGNAL(clicked()), this, SLOT(MemberCardClicked()));
	connect(ui.toolButtonView,               SIGNAL(clicked()), this, SLOT(DetailedViewClicked()));
	connect(ui.toolButtonUpdate,             SIGNAL(clicked()), this, SLOT(UpdateClicked()));
	connect(ui.toolButtonShowAll,            SIGNAL(clicked()), this, SLOT(LoadAllMembers()));

	connect(ui.toolButtonQueryWithName,      SIGNAL(clicked()), this, SLOT(NameQuery()));
	connect(ui.toolButtonQueryWithSurname,   SIGNAL(clicked()), this, SLOT(SurnameQuery()));
	connect(ui.toolButtonQueryWithSex,       SIGNAL(clicked()), this, SLOT(SexQuery()));
	connect(ui.toolButtonQueryWithCellPhone, SIGNAL(clicked()), this, SLOT(CellPhoneQuery()));

	connect(ui.toolButtonQueryWithBlood,       SIGNAL(clicked()), this, SLOT(BloodTypeQuery()));
	connect(ui.toolButtonQueryEducationLevel,  SIGNAL(clicked()), this, SLOT(EduLevelQuery()));
	connect(ui.toolButtonQueryEducationStatus, SIGNAL(clicked()), this, SLOT(EduStatusQuery()));
	connect(ui.toolButtonQueryMemberID,        SIGNAL(clicked()), this, SLOT(MemIdQuery()));
	connect(ui.toolButtonQueryMemberType,      SIGNAL(clicked()), this, SLOT(MemTypeQuery()));
	connect(ui.toolButtonQueryMemberStatus,    SIGNAL(clicked()), this, SLOT(MemStatusQuery()));
	connect(ui.toolButtonQueryAdres,           SIGNAL(clicked()), this, SLOT(MemAddressQuery()));

	connect(ui.toolButtonExportPDF, SIGNAL(clicked()), this, SLOT(ExportPDFClicked()));
	connect(ui.toolButtonSendSMS,   SIGNAL(clicked()), this, SLOT(SendSMSClicked()));
	
	connect(ui.tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(ActivateButtons(QModelIndex)));
}
void MembersWindow::SendSMSClicked()
{
	bool ok;

	QInputDialog* editor = new QInputDialog(this);
	editor->setOption(QInputDialog::UsePlainTextEditForTextInput);
	editor->setInputMode(QInputDialog::TextInput);
	editor->setFixedSize(400, 200);
	editor->setWindowTitle(QString::fromLocal8Bit("Mesajýnýzýn içeriðini giriniz."));
	editor->setLabelText(QString("Mesaj Metni:"));
	int result = editor->exec();
   
   QString smsText = editor->textValue();
 
   if (result && !smsText.isEmpty())
   {
		QMap<QString, QString> smsTelTexts;

		QString numberList = "";

		for (int i = 0; i < model->rowCount(); i++)
		{
			numberList.append(QString("%1,").arg( model->index(i, 6).data().toString()));
		}

		int last = numberList.lastIndexOf(',');
		numberList.remove(last, 2);

		SMSPreviewWindow* smsPreviewWindow = new SMSPreviewWindow(QString::fromLocal8Bit("Mesajýnýz [%1] üyeye gönderilecektir.").arg(model->rowCount()), 
																  smsText, this);
		int result = smsPreviewWindow->exec();

		if (result == QDialog::Accepted)
		{
			SMSManager::GetInstance()->SendSingleSMS(smsText, numberList);
		}
   }
}

void MembersWindow::BloodTypeQuery()
{
	QString bloodType = QString("%1.%2").arg(ui.comboBoxQueryBlood->currentText()).arg(ui.comboBoxQueryBloodRh->currentText());
	
	FillTableView(QUERY_TYPE_ENUM_MEMBER_BLOOD_TYPE, bloodType);	
}

void MembersWindow::EduLevelQuery()
{
	QString eduLevel = ui.comboBoxQueryEducationLevel->currentText();
	
	FillTableView(QUERY_TYPE_ENUM_EDU_LEVEL, eduLevel);	
}

void MembersWindow::EduStatusQuery()
{
	QString eduStatus = ui.comboBoxQueryEducationStatus->currentText();

	FillTableView(QUERY_TYPE_ENUM_EDU_STATUS, eduStatus);	
}

void MembersWindow::MemIdQuery()
{
	QString memId = ui.lineEditQueryMemberId->text();

	FillTableView(QUERY_TYPE_ENUM_MEMBER_ID, memId);	
}

void MembersWindow::MemTypeQuery()
{
	QString memType = ui.comboBoxQueryMemberType->currentText();

	FillTableView(QUERY_TYPE_ENUM_MEMBER_TYPE, memType);	
}

void MembersWindow::MemStatusQuery()
{
	QString memStatus = ui.comboBoxQueryMemberStatus->currentText();

	FillTableView(QUERY_TYPE_ENUM_MEMBER_STATUS, memStatus);	
}

void MembersWindow::MemAddressQuery()
{
	QString memAdres = ui.lineEditQueryAdres->text();

	FillTableView(QUERY_TYPE_ENUM_MEMBER_ADDRESS, memAdres);	
}

void MembersWindow::LoadAllMembers()
{
	LoadMembers();
}

void MembersWindow::CellPhoneQuery()
{
	QString cellPhone = ui.lineEditQueryCellPhone->text();

	FillTableView(QUERY_TYPE_ENUM_CELLPHONE, cellPhone);		
}

void MembersWindow::NameQuery()
{
	QString name = ui.lineEditQueryName->text();

	FillTableView(QUERY_TYPE_ENUM_NAME, name);
}

void MembersWindow::SexQuery()
{
	QString sex = ui.comboBoxQuerySex->currentText();

	FillTableView(QUERY_TYPE_ENUM_SEX, sex);
}

void MembersWindow::SurnameQuery()
{
	QString surname = ui.lineEditQuerySurname->text();

	FillTableView(QUERY_TYPE_ENUM_SURNAME, surname);
}

void MembersWindow::UpdateClicked()
{
	int selectedRow = selectedIndex.row();
	QString mem_id = model->index(selectedRow, 0).data().toString();

	DetailUpdateWindow *detailUpdateWindow = new DetailUpdateWindow(this, true, mem_id);

	detailUpdateWindow->setAttribute(Qt::WA_DeleteOnClose);
	detailUpdateWindow->exec();

	LoadMembers();
}

void MembersWindow::DetailedViewClicked()
{
	int selectedRow = selectedIndex.row();
	QString mem_id = model->index(selectedRow, 0).data().toString();

	DetailUpdateWindow *detailUpdateWindow = new DetailUpdateWindow(this, false, mem_id);

	detailUpdateWindow->setAttribute(Qt::WA_DeleteOnClose);
	detailUpdateWindow->exec();
}

void MembersWindow::MemberCardClicked()
{
	int selectedRow = selectedIndex.row();

	MembershipCard *memberCard = new MembershipCard(model->index(selectedRow, 0).data().toString(), this);

	memberCard->setAttribute(Qt::WA_DeleteOnClose);
	memberCard->exec();
}

void MembersWindow::MemberFormClicked()
{
	int selectedRow = selectedIndex.row();

	OpenMembershipForm(model->index(selectedRow, 0).data().toString(), false);  
}

void MembersWindow::ActivateButtons(QModelIndex index)
{
	QString photoPath;

	selectedIndex = index;

	int selectedRow = selectedIndex.row();
	int mem_ID = model->index(selectedRow, 0).data().toInt();
	QString queryStr = QString("select mem_photopath from membersinfo where mem_no = %1").arg(mem_ID);
	Database::Connect();

	QSqlQuery *qry = new QSqlQuery(QSqlDatabase::defaultConnection);

	qry->prepare(queryStr);
	qry->exec();

	if (qry->next())
	{
		photoPath = qry->value(0).toString();
	}

	qry->finish();

	Database::Disconnect();

	QPixmap picture(photoPath);
	ui.labelMemberPhoto->setPixmap(picture);

	ChangeButtonStatus(true);
}

MembersWindow::~MembersWindow()
{
	SAFE_DELETE(qry);
	SAFE_DELETE(model);
}

void MembersWindow::ChangeButtonStatus(bool status)
{
	ui.toolButtonMemberForm->setEnabled(status);
	ui.toolButtonMemberCard->setEnabled(status);
	ui.toolButtonUpdate->setEnabled(status);
	ui.toolButtonView->setEnabled(status);

	if (!status)
	{
		ui.labelMemberPhoto->setText(" ");
	}
}

QString MembersWindow::SetSelectedSort()
{
	QString selection = "mem_no";

	if(ui.radioButtonNo->isChecked())
	{
		selection = "mem_no";
	}
	else if(ui.radioButtonName->isChecked())
	{
		selection = "mem_name";
	}

	return selection;
}

void MembersWindow::LoadMembers()
{
	ChangeButtonStatus(false);

	FillTableView(QUERY_TYPE_ENUM_ALL);
}

void MembersWindow::FillTableView(QUERY_TYPE_ENUM queryType, QString& queryVal)
{
	Database::Connect();

	QString queryValue = queryVal.trimmed();

	QString sorting = SetSelectedSort();

	if (queryType == QUERY_TYPE_ENUM_ALL)
	{
		qry->prepare("select mem_no, mem_name, mem_surname, mem_fathername, mem_birthplace, mem_birthdate, mem_cellphone from membersinfo ORDER BY  " + sorting + "  ASC");
	}
	else if (queryType == QUERY_TYPE_ENUM_NAME)
	{
		qry->prepare("select mem_no, mem_name, mem_surname, mem_fathername, mem_birthplace, mem_birthdate, mem_cellphone from membersinfo where mem_name = '" + queryValue + "' ORDER BY " + sorting + "  ASC");
	}
	else if (queryType == QUERY_TYPE_ENUM_SURNAME)
	{
		qry->prepare("select mem_no, mem_name, mem_surname, mem_fathername, mem_birthplace, mem_birthdate, mem_cellphone from membersinfo where mem_surname = '" + queryValue + "' ORDER BY " + sorting + "  ASC");
	}
	else if (queryType == QUERY_TYPE_ENUM_SEX)
	{
		qry->prepare("select mem_no, mem_name, mem_surname, mem_fathername, mem_birthplace, mem_birthdate, mem_cellphone from membersinfo where mem_sex = '" + queryValue + "' ORDER BY " + sorting + "  ASC");
	}
	else if (queryType == QUERY_TYPE_ENUM_CELLPHONE)
	{
		qry->prepare("select mem_no, mem_name, mem_surname, mem_fathername, mem_birthplace, mem_birthdate, mem_cellphone from membersinfo where mem_cellphone = '" + queryValue + "' ORDER BY " + sorting + "  ASC");
	}
	else if (queryType == QUERY_TYPE_ENUM_TCNO)
	{
		qry->prepare("select mem_no, mem_name, mem_surname, mem_fathername, mem_birthplace, mem_birthdate, mem_cellphone from membersinfo where mem_tcno = '" + queryValue + "' ORDER BY " + sorting + "  ASC");
	}
	else if (queryType == QUERY_TYPE_ENUM_EDU_LEVEL)
	{
		qry->prepare("select mem_no, mem_name, mem_surname, mem_fathername, mem_birthplace, mem_birthdate, mem_cellphone from membersinfo where mem_educationallevel = '" + queryValue + "' ORDER BY " + sorting + "  ASC");
	}
	else if (queryType == QUERY_TYPE_ENUM_EDU_STATUS)
	{
		qry->prepare("select mem_no, mem_name, mem_surname, mem_fathername, mem_birthplace, mem_birthdate, mem_cellphone from membersinfo where mem_educationstatus = '" + queryValue + "' ORDER BY " + sorting + "  ASC");
	}
	else if (queryType == QUERY_TYPE_ENUM_MEMBER_ID)
	{
		qry->prepare("select mem_no, mem_name, mem_surname, mem_fathername, mem_birthplace, mem_birthdate, mem_cellphone from membersinfo where mem_no = '" + queryValue + "' ORDER BY " + sorting + "  ASC");
	}
	else if (queryType == QUERY_TYPE_ENUM_MEMBER_STATUS)
	{
		qry->prepare("select mem_no, mem_name, mem_surname, mem_fathername, mem_birthplace, mem_birthdate, mem_cellphone from membersinfo where mem_status = '" + queryValue + "' ORDER BY " + sorting + "  ASC"  );
	}
	else if (queryType == QUERY_TYPE_ENUM_MEMBER_TYPE)
	{
		qry->prepare("select mem_no, mem_name, mem_surname, mem_fathername, mem_birthplace, mem_birthdate, mem_cellphone from membersinfo where mem_membershiptype = '" + queryValue + "' ORDER BY " + sorting + "  ASC");
	}
	else if (queryType == QUERY_TYPE_ENUM_MEMBER_BLOOD_TYPE)
	{
		qry->prepare("select mem_no, mem_name, mem_surname, mem_fathername, mem_birthplace, mem_birthdate, mem_cellphone from membersinfo where mem_bloodtype = '" + queryValue + "' ORDER BY " + sorting + "  ASC");
	}
	else if (queryType == QUERY_TYPE_ENUM_MEMBER_ADDRESS)
	{
		qry->prepare("select mem_no, mem_name, mem_surname, mem_fathername, mem_birthplace, mem_birthdate, mem_cellphone from membersinfo where mem_homeaddress LIKE '%" + queryValue + "%' ORDER BY " + sorting + "  ASC");
	}

	qry->exec();

	model->setQuery(*qry);

	SetHeaderTitle(model);
	
	ui.labelMemberCount->setText(QString::fromLocal8Bit("Üye sayýsý : %1").arg(model->rowCount()));
	ui.tableView->setModel(model);

	ChangeButtonStatus(false);

	Database::Disconnect();
}

void MembersWindow::SetHeaderTitle(QSqlQueryModel *model)
{
	model->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("Üye No"));
	model->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("Adý"));
	model->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("Soyadý"));
	model->setHeaderData(3, Qt::Horizontal, QString::fromLocal8Bit("Baba Adý"));
	model->setHeaderData(4, Qt::Horizontal, QString::fromLocal8Bit("Doðum Yeri"));
	model->setHeaderData(5, Qt::Horizontal, QString::fromLocal8Bit("Doðum Tarihi"));
	model->setHeaderData(6, Qt::Horizontal, "Cep Tel");
}

void MembersWindow::OpenMembershipForm(QString& mem_ID, bool isNewlyAdded)								 
{
	MembershipForm *memberform = new MembershipForm(mem_ID, isNewlyAdded, QString(""), this);

	memberform->setAttribute(Qt::WA_DeleteOnClose);
	memberform->exec();
}

void MembersWindow::ExportPDFClicked()
{
	QString path = QApplication::applicationDirPath() + DOCS_PATH;
	
	QFileDialog fileDlg;
	QString fileName = fileDlg.getSaveFileName(this, QString::fromLocal8Bit("Dosyaya bir isim veriniz ..."), path, "*.pdf");
	fileName+=".pdf";

	QStringList headers;
	headers <<QString::fromLocal8Bit("Üye No")
		    <<QString::fromLocal8Bit("Adý")
		    <<QString::fromLocal8Bit("Soyadý")
		    <<QString::fromLocal8Bit("Baba Adý")
		    <<QString::fromLocal8Bit("Doðum Tarihi")
		    <<QString::fromLocal8Bit("GSM No");

	QFile pdfFile(fileName);
    if (pdfFile.open(QIODevice::WriteOnly))
	{
		QPdfWriter* writer = new QPdfWriter(&pdfFile);
		QPainter* painter = new QPainter(writer);
		writer->setPageSize(QPagedPaintDevice::A4);

		int x = 0;

		int width  = painter->window().width();
		int height = painter->window().height();

		QRect rect(0, 0, width, height/20.0);
		QRect rectDate(0, height/20.0 - 200.0, width, 200);
		
		QFont font;
		font.setBold(true);
		font.setPointSize(16);
		painter->setFont(font);
		painter->drawText(rect, Qt::AlignCenter,SettingsManager::organizationSettings.orgName);
		
		painter->drawLine(0, height/20 - 600.0, width, height/20.0 - 600.0);
		
		painter->drawLine(0, height/20.0 + 10.0, width, height/20.0 + 10.0);

		int y = height/20.0 + 10.0 + 40.0;

		font.setPointSize(10);
		painter->setFont(font);

		for (int i = 0; i < headers.count(); i++)
		{	
			QRect rec;
			rec = QRect(x, y, (width/headers.count()), 300);
			painter->drawText(rec, Qt::AlignBottom | Qt::AlignCenter , headers.at(i));
			painter->drawRect(rec);
			x += width/headers.count();
		}

		y += 300;

		x = 0;
		
		font.setBold(true);
		painter->setFont(font);
		painter->drawText(rectDate, Qt::AlignRight,
			              QString("Tarih: %1").arg(QDateTime::currentDateTime().toString("dd.MM.yyyy")));
		for (int i = 0; i < model->rowCount(); i++)
		{
			for (int j = 0; j < 8; j++)
			{	
				if (j == 4)
				continue;

				QString aa = model->index(i, j).data().toString();

				QRect rec(x,y,(width/headers.count()),300);
				painter->drawText(rec, Qt::AlignBottom | Qt::AlignCenter | Qt::TextWordWrap, model->index(i, j).data().toString());
				painter->drawRect(rec);
				x += width/headers.count();
			}

			x = 0;
			y+= 300;

			if (y > height)
			{
				writer->newPage();
				y = 0;
			}
		}

		painter->end();

		SAFE_DELETE(painter);
		SAFE_DELETE(writer);
		pdfFile.close();
	}
}



