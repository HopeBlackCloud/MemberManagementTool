#pragma once

#include <qobject.h>
#include <qstringlist.h>
#include "Definitions.h"
#include "QNetworkRequest.h"
#include "QNetworkAccessManager.h"

class SMSManager : public QObject
{
	Q_OBJECT

	public:
		static SMSManager *GetInstance();

		virtual ~SMSManager();

		void SendSingleSMS(QString& smsText, QString& telList);
		void SendMultipleSMS(QMap<QString, QString>& smsTelTexts);

	private:
		SMSManager();

		QNetworkRequest request;
		QNetworkAccessManager *NAManager;

		static SMSManager *msgManager;
};