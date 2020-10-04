#pragma once

#include <qstringlist.h>

class MessageManager
{
	public:
		static MessageManager *GetInstance();

		bool HasErrorMessage();
		bool HasInfoMessage();

		void AddErrorMessage(QString &);
		void AddInfoMessage(QString &);

		QStringList & GetErrorList() { errorList.removeDuplicates();  return errorList; }
		QStringList & GetInfoList()  { infoList.removeDuplicates();   return infoList;  }

	private:
		MessageManager() {}

		static MessageManager *msgManager;

		QStringList errorList;
		QStringList infoList;
};