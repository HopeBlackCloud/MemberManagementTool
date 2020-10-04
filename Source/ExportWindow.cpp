#include "ExportWindow.h"
#include "Definitions.h"
#include "DatabaseManager.h"
#include "SettingsManager.h"

#include <QFile>
#include <QPainter>
#include <QFileDialog>
#include <QPdfWriter>
#include <QInputDialog>
#include <QTableWidgetItem>

ExportWindow::ExportWindow(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	connect(ui.toolButtonExportPDF,   SIGNAL(clicked()), this, SLOT(ExportPDFClicked()));
	connect(ui.toolButtonExportExcel, SIGNAL(clicked()), this, SLOT(ExportExcelClicked()));
	connect(ui.toolButtonInsert,      SIGNAL(clicked()), this, SLOT(InsertClicked()));
	connect(ui.toolButtonMeeting,      SIGNAL(clicked()), this, SLOT(MeetingDocClicked()));

	connect(ui.toolButtonUp,      SIGNAL(clicked()), this, SLOT(UpClicked()));
	connect(ui.toolButtonDown,    SIGNAL(clicked()), this, SLOT(DownClicked()));
	connect(ui.toolButtonDelete,  SIGNAL(clicked()), this, SLOT(DeleteClicked()));
	connect(ui.toolButtonClear, SIGNAL(clicked()), this, SLOT(ClearClicked()));

	connect(ui.listWidgetSelected,  SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(ListItemClicked(QListWidgetItem*)));
	connect(ui.listWidgetSelection, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(SelectItemClicked(QListWidgetItem*)));
	
	ui.listWidgetSelected->clear();
}

ExportWindow::~ExportWindow()
{

}

void ExportWindow::SelectItemClicked(QListWidgetItem*)
{
	ui.toolButtonInsert->setEnabled(true);
}

void ExportWindow::ListItemClicked(QListWidgetItem* item)
{
	Q_UNUSED(item);

	ui.toolButtonDelete->setEnabled(true);
	ui.toolButtonDown->setEnabled(true);
	ui.toolButtonUp->setEnabled(true);
}

void ExportWindow::ExportExcelClicked()
{
	QString path = QApplication::applicationDirPath() + DOCS_PATH;
	
	QFileDialog fileDlg;
	QString fileName = fileDlg.getSaveFileName(this, QString::fromLocal8Bit("Dosyaya bir isim veriniz ..."), path, "*.csv");
	fileName+=".csv";

	QStringList headers;
	headers = GetSelectedHeadersList();	
	
	QSqlQuery result = Database::ExecuteQuery(PrepareQueryString(headers));

	int rowCount = result.size();

	QFile csvfile(fileName);
    if (csvfile.open(QIODevice::WriteOnly))
	{
		QTextStream strm(&csvfile);

		for (int i = 0; i < headers.count(); i++)
		{	
			 strm<<headers.at(i)<<",";
		}

		strm << "\n";
	
		while (result.next())
		{
			for (int i = 0; i < headers.count(); i++)
			{	
				strm << result.value(i).toString() << ",";
			}

	        strm <<"\n";
		}
	}
}

void ExportWindow::ExportPDFClicked()
{
	QString path = QApplication::applicationDirPath() + DOCS_PATH;
	
	QFileDialog fileDlg;
	QString fileName = fileDlg.getSaveFileName(this, QString::fromLocal8Bit("Dosyaya bir isim veriniz ..."), path, "*.pdf");
	fileName+=".pdf";

	QStringList headers;
	headers = GetSelectedHeadersList();	

	QSqlQuery result = Database::ExecuteQuery(PrepareQueryString(headers));

	int rowCount = result.size();

	QFile pdfFile(fileName);
    if (pdfFile.open(QIODevice::WriteOnly))
	{
		QPdfWriter* writer = new QPdfWriter(&pdfFile);
		QPainter* painter = new QPainter(writer);
		writer->setPageSize(QPagedPaintDevice::A4);

		QFont font;
		font.setBold(true);
		QPen pen(Qt::black, 15);
		painter->setPen(pen);
		painter->setFont(font);

		int x = 0;

		int width  = painter->window().width();
		int height = painter->window().height();

		QRect rect(0, 0, width, height/20.0);
		
		painter->drawText(rect, Qt::AlignCenter, SettingsManager::organizationSettings.orgName);
		painter->drawLine(0, height/20.0 + 10.0, width, height/20.0 + 10.0);

		int y = height/20.0 + 10.0 + 40.0;

		for (int i = 0; i < headers.count(); i++)
		{	
			QRect rec(x,y,(width/headers.count()),300);
			painter->drawText(rec, Qt::AlignBottom | Qt::AlignLeft | Qt::TextWordWrap, headers.at(i));
			painter->drawRect(rec);
			x += width/headers.count();
		}

		y += 300;

		x = 0;
		while (result.next())
		{
			for (int i = 0; i < headers.count(); i++)
			{	
				QRect rec(x,y,(width/headers.count()),300);
				painter->drawText(rec, Qt::AlignBottom | Qt::AlignLeft | Qt::TextWordWrap, result.value(i).toString());
				painter->drawRect(rec);
				x += width/headers.count();
			}

			x = 0;
			y+= 300;

			if (y +100 >= height)
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

void ExportWindow::InsertClicked()
{
	ui.listWidgetSelected->addItem(ui.listWidgetSelection->currentItem()->text());
	ui.toolButtonInsert->setEnabled(false);
}

void ExportWindow::MeetingDocClicked()
{
	bool ok;

	QInputDialog* editor = new QInputDialog(this);
	editor->setOption(QInputDialog::UsePlainTextEditForTextInput);
	editor->setInputMode(QInputDialog::TextInput);
	editor->setFixedSize(400, 200);
	editor->setWindowTitle(QString::fromLocal8Bit("Toplantýnýn konusunu giriniz."));
	editor->setLabelText(QString("Konusu:"));
	int result = editor->exec();
   
   QString meetingTopic = editor->textValue();
 
   if (result && !meetingTopic.isEmpty())
   {
	   QString path = QApplication::applicationDirPath() + DOCS_PATH;
	
	QFileDialog fileDlg;
	QString fileName = fileDlg.getSaveFileName(this, QString::fromLocal8Bit("Dosyaya bir isim veriniz ..."), path, "*.pdf");
	fileName+=".pdf";

	QStringList headers;
	headers <<QString::fromLocal8Bit("S.No")
		    <<QString::fromLocal8Bit("Adý")
		    <<QString::fromLocal8Bit("Soyadý")
		    <<QString::fromLocal8Bit("Ünvaný")
		    <<QString::fromLocal8Bit("Ýmza");
	
	QFile pdfFile(fileName);
    if (pdfFile.open(QIODevice::WriteOnly))
	{
		QPdfWriter* writer = new QPdfWriter(&pdfFile);
		QPainter* painter = new QPainter(writer);
		writer->setPageSize(QPagedPaintDevice::A4);

		QFont font;
		font.setBold(true);
		font.setWeight(99);
		painter->setFont(font); 

		int x = 0;

		int width  = painter->window().width();
		int height = painter->window().height();

		//ç
		int headerSecondLineHeightLimit = height/20.0 + 100.0;
		int headerFirstLineHeightLimit = height/20.0 - 600.0;
		int dateTopicHeight = height/20.0 - 100.0;
		int pageHeight = height/20.0 - 850.0;


		QRect rect(0, 0, width, height/20.0);
		QRect rectDate(0, dateTopicHeight, width, 200);

		//topic rectangle-ç
		QRect rectTopic(0, dateTopicHeight, width, 200);
		
		//page rectangle
		QRect rectPageNumber(0, pageHeight, width, 200);

		font.setBold(true);
		font.setPointSize(16);
		
		painter->setFont(font);
		painter->drawText(rect, Qt::AlignCenter, SettingsManager::organizationSettings.orgName);
		
		painter->drawLine(0, headerFirstLineHeightLimit, width, headerFirstLineHeightLimit);
		painter->drawLine(0, headerSecondLineHeightLimit, width, headerSecondLineHeightLimit);

		int y = headerSecondLineHeightLimit + 40.0;

		font.setBold(true);
		font.setPointSize(10);
		painter->setFont(font);

		QPen pen(Qt::black, 15);
		painter->setPen(pen);

		for (int i = 0; i < headers.count(); i++)
		{	
			QRect rec;
			if (i == 0 )
			{
				rec = QRect(x, y, 450, 300);
			}
			else
			{
				rec = QRect(x, y, (width/headers.count() + 350), 300);
			}
			
			font.setBold(true);
			painter->drawText(rec, Qt::AlignBottom | Qt::AlignCenter , headers.at(i));
			painter->drawRect(rec);

			if (i ==0)
			{
				x += 450;
			}
			else
			{
				x += width/headers.count() + 350;
			}
		
		}

		y += 300;
		x = 0;
		
		font.setBold(true);
		//font.setBold(false);
		painter->setFont(font);

		//Painter draw topic
		painter->drawText(rectTopic,Qt::AlignCenter,QString::fromLocal8Bit("%1 %2").arg(meetingTopic).arg(QString::fromLocal8Bit("Katýlýmcý Listesi")));
		
		painter->drawText(rectDate, Qt::AlignRight,
			              QString("Tarih: %1").arg(QDateTime::currentDateTime().toString("dd.MM.yyyy")));

		int pageCount= (ui.comboBoxSelectPage->currentIndex()) + 1;
		int rowCount = 42;
		int limit = rowCount + (pageCount -1) * 45;
		int currentPage = 1;

		//page count
		painter -> drawText(rectPageNumber,Qt::AlignRight,QString::fromLocal8Bit("%1 %2 %3 %4")
			.arg("Sayfa:")
			.arg(currentPage)
			.arg("/")
			.arg(pageCount));

		for (int i = 1; i < limit; i++)
		{
			for (int j = 0; j < headers.count(); j++)
			{
				QRect rec;
				if (j == 0 )
				{
					QPen pen(Qt::black, 15);
				painter->setPen(pen);
					rec = QRect(x, y, 450, 300);
					painter->drawText(rec, Qt::AlignBottom | Qt::AlignCenter , QString("%1").arg(i));
				}
				else
				{
					rec = QRect(x, y, (width/headers.count() + 350), 300);
				}
				//QRect rec(x, y, (width/headers.count() + 100), 300);
				
				QPen pen(Qt::black, 15);
				painter->setPen(pen);
				painter->drawRect(rec);
				if (j ==0)
				{
					x += 450;
				}
				else
				{
					x += width/headers.count() + 350;
				}
				
			}

			x = 0;
			y+= 300;

			if (y > height)
			{
				currentPage++;
				writer->newPage();

				painter -> drawText(rectPageNumber,Qt::AlignRight,QString::fromLocal8Bit("%1 %2 %3 %4")
					.arg("Sayfa:")
					.arg(currentPage)
					.arg("/")
					.arg(pageCount));

				y = 50;
			}
		}

		painter->end();

		SAFE_DELETE(painter);
		SAFE_DELETE(writer);
		pdfFile.close();
   }
   }
}

void ExportWindow::UpClicked()
{
	int row = ui.listWidgetSelected->currentRow();

	QListWidgetItem* currentItem = ui.listWidgetSelected->takeItem(row);

	ui.listWidgetSelected->insertItem(row-1, currentItem);
}

void ExportWindow::DownClicked()
{
	int row = ui.listWidgetSelected->currentRow();

	QListWidgetItem* currentItem = ui.listWidgetSelected->takeItem(row);

	ui.listWidgetSelected->insertItem(row+1, currentItem);
}

void ExportWindow::DeleteClicked()
{
	int row = ui.listWidgetSelected->currentRow();		
	ui.listWidgetSelected->takeItem(row);

	if (ui.listWidgetSelected->count() == 0)
	{
		ui.toolButtonDelete->setEnabled(false);
		ui.toolButtonDown->setEnabled(false);
		ui.toolButtonUp->setEnabled(false);
	}
}

void ExportWindow::ClearClicked()
{
	ui.listWidgetSelected->clear();

	ui.toolButtonDelete->setEnabled(false);
	ui.toolButtonDown->setEnabled(false);
	ui.toolButtonUp->setEnabled(false);
}

QString ExportWindow::PrepareQueryString(QStringList& list)
{
	QString query = "select ";
	foreach(QString header, list)
	{
		if (header == QString::fromLocal8Bit("Üye No"))
		{
			query+="mem_no,"; 
		}
		else if (header == QString::fromLocal8Bit("Üye Adý"))
		{
			query+="mem_name,"; 
		}
		else if (header == QString::fromLocal8Bit("Üye Soyadý"))
		{
			query+="mem_surname,"; 
		}
		else if (header == QString::fromLocal8Bit("Üye Kayýt Tarihi"))
		{
			query+="mem_membershipdate,";
		}
		else if (header == QString::fromLocal8Bit("Üye Türü"))
		{
			query+="mem_membershiptype,";
		}
		else if (header == QString::fromLocal8Bit("Üye Adresi"))
		{
			query+="mem_homeaddress,";
		}
		else if (header == QString::fromLocal8Bit("Üye Cep Tel"))
		{
			query+="mem_cellphone,";
		}
		else if (header == QString::fromLocal8Bit("Üye Kan Grubu"))
		{
			query+="mem_bloodtype,";
		}
		else if (header == QString::fromLocal8Bit("Üye E-Posta"))
		{
			query+="mem_email,";
		}
		else if (header == QString::fromLocal8Bit("Üye Doðum Tarihi"))
		{
			query+="mem_birthdate,";
		}
		else if (header == QString::fromLocal8Bit("Üye Eðitim Derecesi"))
		{
			query+="mem_educationallevel,";
		}
		else if (header == QString::fromLocal8Bit("Üye Eðitim Durumu"))
		{
			query+="mem_educationstatus,";
		}
		else if (header == QString::fromLocal8Bit("Üye Cinsiyeti"))
		{
			query+="mem_sex,";
		}
		else if (header == QString::fromLocal8Bit("Üye Medeni Hali"))
		{
			query+="mem_maritialstatus,";
		}
		else if (header == QString::fromLocal8Bit("Üye Mesleði"))
		{
			query+="mem_occupation,";
		}
	}
	
	int last = query.lastIndexOf(',');
	query.remove(last, 2);

	if(ui.comboBox->currentIndex() == 0)
		query+=" from membersinfo ORDER BY mem_name ASC";
	else if(ui.comboBox->currentIndex() == 1)  
		query+=" from membersinfo WHERE mem_status='Aktif' ORDER BY mem_name ASC";
	else if(ui.comboBox->currentIndex() == 2)
		query+=" from membersinfo WHERE mem_status='Pasif' ORDER BY mem_name ASC";
	else if(ui.comboBox->currentIndex() == 3)  
	{
		QString women =  QString::fromLocal8Bit("Kadýn");
		query+=" from membersinfo WHERE mem_sex='" + women+ "' ORDER BY mem_name ASC";
	}
	else if(ui.comboBox->currentIndex() == 4)
		query+=" from membersinfo WHERE mem_sex='Erkek' ORDER BY mem_name ASC";

	return query;
}

QStringList ExportWindow::GetSelectedHeadersList()
{
	QStringList headerList;
	int count = ui.listWidgetSelected->count();

	for (int i = 0; i < count; i++)
	{
		QListWidgetItem *item = ui.listWidgetSelected->item(i);

		headerList.append(item->text());
	}

	return headerList;
}

