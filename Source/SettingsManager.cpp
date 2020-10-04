#include "SettingsManager.h"
#include <QSettings>
#include <QRegExp>
#include <QStringList>   
#include <math.h>    
#include <QDir>

#include "Definitions.h"
#include <windows.h>

#include <Qtxml\qdom.h>

OrganizationSettings SettingsManager::organizationSettings;

SettingsManager::SettingsManager()
{

}

SettingsManager::~SettingsManager()
{
	if (m_xmlFile != NULL)
	{
		m_xmlFile->close();
		delete m_xmlFile;
		m_xmlFile = NULL;
	}
}

// Organizasyona özgü ayarlarý dosyadan okur.
void SettingsManager::ReadOrganizationInfo(const QString & orgSettingsPath)
{
	bool isOpen = openFile(orgSettingsPath, Organization);
	QString strSectionDB("Organization/Database, 0");
	organizationSettings.dbInfo   = getXMLValue(strSectionDB, "dbInfo", "");
	organizationSettings.dbName   = getXMLValue(strSectionDB, "dbName", "");
	organizationSettings.hostName = getXMLValue(strSectionDB, "hostName", "");
	organizationSettings.userName = getXMLValue(strSectionDB, "userName", "");
	organizationSettings.password = getXMLValue(strSectionDB, "password", "");
	organizationSettings.port     = getXMLValue(strSectionDB, "port", 0);

	QString strSectionName("Organization/Name, 0");
	organizationSettings.orgName     = getXMLValue(strSectionName, "orgName", "");
	organizationSettings.orgDispName = getXMLValue(strSectionName, "dispName", "");
	organizationSettings.address = getXMLValue(strSectionName, "address", "");
	organizationSettings.telFax = getXMLValue(strSectionName, "telFax", "");
	organizationSettings.email = getXMLValue(strSectionName, "email", "");
	organizationSettings.macAdd = getXMLValue(strSectionName, "code", "");

	QString strSectionSMS("Organization/SMS, 0");
	organizationSettings.smsUser = getXMLValue(strSectionSMS, "user", "");
	organizationSettings.smscode = getXMLValue(strSectionSMS, "code", "");
	organizationSettings.smsPass = getXMLValue(strSectionSMS, "pass", "");
}

// Converts date string to QDateTime
QDate SettingsManager::StringToDate(const QString & rawDate)
{
	if (rawDate.isEmpty())
		return QDate();

	//if rawdate contains chars
	QRegExp letters("[A-Z]", Qt::CaseInsensitive);
	if (letters.indexIn(rawDate) > -1)
		return QDate(); 

	QStringList strListDate = rawDate.split(".");
	QDate finalDate(strListDate.at(2).toInt(), strListDate.at(1).toInt(), strListDate.at(0).toInt());

	//QStringList strListTime = strTime.split(":");
	//QTime finalTime(strListTime.at(0).toInt(), strListTime.at(1).toInt(), strListTime.at(2).toInt() );

	QDate finalDateTime(finalDate);

	return finalDateTime;
}

// XML parser functions. Moved here from Settings.
bool SettingsManager::openFile(const QString strFile, OutputFiles out)
{
    if (out == Organization)
	{
		m_xmlFile = new QFile(strFile + ORGANIZATION_FILE);
	}

	bool res = m_xmlFile->open(QIODevice::ReadOnly | QIODevice::Text);

	return res;
}

QString SettingsManager::getConfigPath()
{
	return CONFIG_PATH;
}

QString SettingsManager::getXMLValue(const QString section, const QString key, const QString defaultValue)
{
	QString strHeader;
	QString strTitle;
	QString strSub;

	int titleIndex = -1;
	int subIndex = -1;

	// min 2 max 3
	QStringList elements = section.split("/");

	// Parse header
	strHeader = elements.at(0);

	// Parse title
	QString strTitleGroup  = elements.at(1);
	QStringList titleList = strTitleGroup.split(",");
	strTitle = titleList.at(0);
	titleIndex = titleList.at(1).toInt();

	// Parse sub title
	if (elements.count() > 2)
	{
		QString strSubGroup = elements.at(2);

		QStringList subList = strSubGroup.split(",");
		strSub = subList.at(0);
		subIndex = subList.at(1).toInt();
	}

	QFile* readFile = m_xmlFile;
	QDomDocument domDoc;
	domDoc.setContent(readFile);
	QDomElement root = domDoc.documentElement();
	readFile->close();

	// QString str = root.tagName();  // DEBUG
	if (root.tagName() == strHeader)  // Session
	{
		QDomElement child = root.firstChildElement();

		int i = 0;
		while(!child.isNull())
		{
			if (child.tagName() == strTitle)  // Record
			{
				if (i == titleIndex)
				{
					if (elements.count() == 2)
					{
						return child.attribute(key);
					}
					else if (elements.count() > 2)
					{
						QDomElement sub = child.firstChildElement();

						int j = 0;
						while (!sub.isNull())
						{
							if (sub.tagName() == strSub)  // Parts
							{
								if (j == subIndex)
								{
									return sub.attribute(key);
								}
								j++;
							}
							sub = sub.nextSiblingElement(strSub);
						}
					}
				}
				i++;
			}
			child = child.nextSiblingElement(strTitle);
		}
	}

	return defaultValue;
}

int SettingsManager::getXMLValue(const QString section, const QString key, const int defaultValue)
{
	QString strHeader;
	QString strTitle;
	QString strSub;

	int titleIndex = -1;
	int subIndex = -1;

	// min 2 max 3
	QStringList elements = section.split("/");

	// Parse header
	strHeader = elements.at(0);

	// Parse title
	QString strTitleGroup  = elements.at(1);
	QStringList titleList = strTitleGroup.split(",");
	strTitle = titleList.at(0);
	titleIndex = titleList.at(1).toInt();

	// Parse sub title
	if (elements.count() > 2)
	{
		QString strSubGroup = elements.at(2);

		QStringList subList = strSubGroup.split(",");
		strSub = subList.at(0);
		subIndex = subList.at(1).toInt();
	}

	QFile* readFile = m_xmlFile;
	QDomDocument domDoc;
	domDoc.setContent(readFile);
	QDomElement root = domDoc.documentElement();
	readFile->close();

	QString str = root.tagName();
	if (root.tagName() == strHeader)  
	{
		QDomElement child = root.firstChildElement();

		int i = 0;
		while(!child.isNull())
		{
			if (child.tagName() == strTitle)  
			{
				if (i == titleIndex)
				{
					if (elements.count() == 2)
					{
						QString strResult = child.attribute(key);
						return strResult.toInt();
					}
					else if (elements.count() > 2)
					{
						QDomElement sub = child.firstChildElement();

						int j = 0;
						while (!sub.isNull())
						{
							if (sub.tagName() == strSub)  
							{
								if (j == subIndex)
								{
									QString strResult = sub.attribute(key);
									return strResult.toInt();
								}
								j++;
							}
							sub = sub.nextSiblingElement(strSub);
						}
					}
				}
				i++;
			}
			child = child.nextSiblingElement(strTitle);
		}
	}

	return defaultValue;
}

double SettingsManager::getXMLValue(const QString section, const QString key, const double defaultValue)
{
	QString strHeader;
	QString strTitle;
	QString strSub;

	int titleIndex = -1;
	int subIndex = -1;

	// min 2 max 3
	QStringList elements = section.split("/");

	// Parse header
	strHeader = elements.at(0);

	// Parse title
	QString strTitleGroup  = elements.at(1);
	QStringList titleList = strTitleGroup.split(",");
	strTitle = titleList.at(0);
	titleIndex = titleList.at(1).toInt();

	// Parse sub title
	if (elements.count() > 2)
	{
		QString strSubGroup = elements.at(2);

		QStringList subList = strSubGroup.split(",");
		strSub = subList.at(0);
		subIndex = subList.at(1).toInt();
	}

	QFile* readFile = m_xmlFile;
	QDomDocument domDoc;
	domDoc.setContent(readFile);
	QDomElement root = domDoc.documentElement();
	readFile->close();

	if (root.tagName() == strHeader)  
	{
		QDomElement child = root.firstChildElement();

		int i = 0;
		while(!child.isNull())
		{
			if (child.tagName() == strTitle)  
			{
				if (i == titleIndex)
				{
					if (elements.count() == 2)
					{
						QString strResult = child.attribute(key);
						return strResult.toDouble();
					}
					else if (elements.count() > 2)
					{
						QDomElement sub = child.firstChildElement();

						int j = 0;
						while (!sub.isNull())
						{
							if (sub.tagName() == strSub)  
							{
								if (j == subIndex)
								{
									QString strResult = sub.attribute(key);
									return strResult.toDouble();
								}
								j++;
							}
							sub = sub.nextSiblingElement(strSub);
						}
					}
				}
				i++;
			}
			child = child.nextSiblingElement(strTitle);
		}
	}

	return defaultValue;
}

bool SettingsManager::systemShutdown() {
	HANDLE hToken; 
	TOKEN_PRIVILEGES tkp; 

	// Get a token for this process. 
	if (!OpenProcessToken(GetCurrentProcess(), 
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
		return( FALSE ); 

	// Get the LUID for the shutdown privilege. 
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, 
		&tkp.Privileges[0].Luid); 

	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

	// Get the shutdown privilege for this process. 
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
		(PTOKEN_PRIVILEGES)NULL, 0); 

	if (GetLastError() != ERROR_SUCCESS) 
		return false; 

	// Shut down the system and force all applications to close. 
	if (!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 
		SHTDN_REASON_MAJOR_OPERATINGSYSTEM |
		SHTDN_REASON_MINOR_UPGRADE |
		SHTDN_REASON_FLAG_PLANNED)) 
		return false; 

	//shutdown was successful
	return true;
}

// MT 18.5.2012 Fri. msdn shutdown example
bool SettingsManager::systemReboot() {
	HANDLE hToken; 
	TOKEN_PRIVILEGES tkp; 

	// Get a token for this process. 
	if (!OpenProcessToken(GetCurrentProcess(), 
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
		return( FALSE ); 

	// Get the LUID for the reboot privilege. 
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, 
		&tkp.Privileges[0].Luid); 

	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

	// Get the reboot privilege for this process. 
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
		(PTOKEN_PRIVILEGES)NULL, 0); 

	if (GetLastError() != ERROR_SUCCESS) 
		return false; 

	// Reboot the system and force all applications to close. 
	if (!ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 
		SHTDN_REASON_MAJOR_OPERATINGSYSTEM |
		SHTDN_REASON_MINOR_UPGRADE |
		SHTDN_REASON_FLAG_PLANNED)) 
		return false; 

	// Reboot was successful
	return true;
}
