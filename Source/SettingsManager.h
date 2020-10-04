#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QtSql\qsqldatabase>
#include <QtSql\qsqlquery>
#include <qdatetime>
#include <QFile>

#include <string.h>

using namespace std;


enum OutputFiles
{
	Organization,	
};

static QFile *m_xmlFile;

typedef struct OrganizationSettings
{
	QString name;
	QString dbInfo; 
	QString dbName; 
	QString hostName; 
	QString userName; 
	QString password; 
	quint32 port;
	QString orgName;
	QString orgDispName;
	QString email;
	QString address;
	QString telFax;
	QString smsUser;
	QString smscode;
	QString smsPass; 
	QString macAdd;
} OrganizationSettings;


class SettingsManager 
{
	public:

		SettingsManager();
		virtual ~SettingsManager();

		static void ReadOrganizationInfo(const QString & orgSettingsPath);

		static QDate StringToDate(const QString & rawDate);

		// Gets the record's folder of the computer.
		static QString takeRecordsPath(string & recordsPath);
		// Get the remote computer record's path from the settings.
		static QString takeRemoteFolder();

		// MT 27.9.2011 Tue. Returns the value of a given key in the predefined output files.
		// Returns default value if the key is not found.
		// OutputFiles out: Tells from which file to read the values.
		// QString title:  XML Sentence title  
		// int index:  The index of the key amongst the same titles.
		// QString key: The name of the key
		// QString defaultValue: The return value incase the key is not found.
		//>QString getXMLValue(OutputFiles out, QString title, int index, QString key, QString defaultValue);
		// Overloaded function. return value = integer
		//>int getXMLValue(OutputFiles out, QString title, int index, QString key, int defaultValue);
		// Overloaded function. return value = double
		//>double getXMLValue(OutputFiles out, QString title, int index, QString key, double defaultValue);

		// MT 28.9.2011 Wed. Parses XML up to 3 levels and retrieves the value of the key
		// QString section: The titles of the XML document. 
		// QString key: The name of the key
		// QString defaultValue: The return value incase the key is not found.
		// Example:	 section = "header/group,2/sub,0", key = "info" return the value = "RMation"
		// 		
		// 		 <header>
		// 		     <group/>
		// 		     <group/>
		// 		     <group>
		// 		         <sub info = RMation></subsub>
		// 		     </group>
		// 		 </header>
		//
		static QString getXMLValue(const QString section, const QString key, const QString defaultValue);
		// Overloaded function. return value = integer
		static int getXMLValue(const QString section, const QString key, const int defaultValue);
		// Overloaded function. return value = double
		static double getXMLValue(const QString section, const QString key, const double defaultValue);

		static bool openFile(const QString strFile, OutputFiles out);

		//static QString getRecordsPath();

		static QString getConfigPath();
	
		static QString getVersionFile();

		static QString getRecordInfoFile();

		static bool systemShutdown();
		static bool systemReboot();

		static OrganizationSettings organizationSettings;
};

#endif
