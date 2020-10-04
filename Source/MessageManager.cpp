#include "stddef.h"
#include "MessageManager.h"

MessageManager* MessageManager::msgManager = NULL;

MessageManager *MessageManager::GetInstance()
{
	if(!msgManager)
	{
		msgManager = new MessageManager();
	}

	return msgManager;
}

bool MessageManager::HasErrorMessage()
{
	return errorList.count() > 0;
}

bool MessageManager::HasInfoMessage()
{
	return infoList.count() > 0;
}

void MessageManager::AddErrorMessage(QString &msg)
{
	errorList.append(msg);
}

void MessageManager::AddInfoMessage(QString &msg)
{
	infoList.append(msg);
}






