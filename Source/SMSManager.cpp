#include "stddef.h"
#include "SMSManager.h"

#include "qurl.h"

#include "SettingsManager.h"
#include <qmap.h>

SMSManager* SMSManager::msgManager = NULL;

SMSManager *SMSManager::GetInstance()
{
	if(!msgManager)
	{
		msgManager = new SMSManager();
	}

	return msgManager;
}

SMSManager::~SMSManager() 
{
	SAFE_DELETE(NAManager);
}

SMSManager::SMSManager() 
{
	NAManager = NULL;
	NAManager = new QNetworkAccessManager();

	request = QNetworkRequest(QUrl(QStringLiteral("http://77.223.141.225/")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("text/html; charset=utf-8"));

	connect(NAManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

void SMSManager::SendSingleSMS(QString& smsText, 
						       QString& telList)
{
	QString smsXmlFormat = QString("<?xml version='1.0' encoding='utf-8'?>"
						           "<SingleTextSMS><UserName>%1-%2</UserName><PassWord>%3</PassWord>"
		                           "<Action>0</Action><Mesgbody>%4</Mesgbody>"
		                           "<Numbers>%5</Numbers><Originator></Originator>"
		                           "<SDate></SDate></SingleTextSMS>").arg(SettingsManager::organizationSettings.smsUser)
		                                                             .arg(SettingsManager::organizationSettings.smscode)
										                             .arg(SettingsManager::organizationSettings.smsPass)
										                             .arg(smsText)
										                             .arg(telList);
	
	QByteArray data(smsXmlFormat.toStdString().c_str());
	NAManager->post(request, data);
}

void SMSManager::SendMultipleSMS(QMap<QString, QString>& smsTelTexts)
{
	QMap<QString, QString>::iterator iter;
	for (iter = smsTelTexts.begin(); iter != smsTelTexts.end(); ++iter)
	{
		QString text = iter.key();
		SendSingleSMS(text, iter.value());

		_sleep(100);
	}
}










