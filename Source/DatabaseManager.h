#pragma once

#include <QDate>
#include <QTime>
#include <QDebug>
#include <QRegExp>
#include <QString>
#include <QVariantList>
#include <QtSql\QSqlQuery>
#include <QtSql\QSqlError>
#include <QtSql\QSqlRecord>
#include <QCryptographicHash>
#include <QtSql\QSqlDatabase>
#include <QtSql\QSqlTableModel>

	class Database
	{
		public:
			static bool bIsConnected;
		public:
			static bool Connect();
			static void Disconnect();
			static bool IsConnected();

			static QSqlQuery DeriveParameters(QString query);

			static bool ExecuteNonQuery(QString query, QList<QVariant> params, bool dont_log=true);
			static QSqlQuery ExecuteQuery(QString query, QList<QVariant> params);
			static bool ExecuteNonQuery(QString query, bool dont_log=true);
			static QSqlQuery ExecuteQuery(QString query);
			static int InsertQuery(QString query, QList<QVariant> params, bool dont_log=true);

		private:
			static QSqlDatabase database;
			static QSqlQuery DeriveParameters(QString query, QList<QVariant> params);
			
		public:						
			static void AddNewMemberFee(quint32, QString&, QString&);
			static bool MakeMemberPasif(int mem_no);
			static bool AddLastLogin(QString lastLoginInfo);
			static bool AddLastYear(int year);
			static bool AddFeeLoaded(bool loaded);
			static QString GetPaidYears(int payNo);
			static qint32 GetFeePerYear(int year);
			static int GetLastYear();
			static bool GetFeeLoaded();
			static void AddAnnualyMemberFee(QString& notpaid);
			static void CreateNewGroup(QString& groupName, QString& numberList);
			static QString GetNumbersInGroup(QString& grname);

			static bool GetLogStatus();
			static void SetLogStatus(bool status);
			static bool IsMemberNoExist(int mem_no);
														
			static bool InsertMember(QString& mem_name,     
									 QString& mem_surname,
									 QString& mem_fathername,
									 QString& mem_birthplace,
									 QString& mem_birthdate,
									 QString& mem_sex,
									 QString& mem_maritialstatus,
									 QString& mem_cellphone,
									 QString& mem_email,
									 QString& mem_educationstatus,
									 QString& mem_educationallevel,
									 QString& mem_schoolname,
									 QString& mem_department,
									 QString& mem_occupation,
									 QString& mem_bloodtype,
									 QString& mem_homeaddress,
									 QString& mem_companyname,
									 QString& mem_companyphone,
									 QString& mem_companyaddress,
									 QString& mem_membershipdate,
									 QString& mem_membershiptype,
									 quint32  mem_no,
									 QString& mem_status,
									 QString& mem_photopath);

			static bool UpdateMember(int mem_no,
									QString& mem_name,     
									QString& mem_surname,
									QString& mem_fathername,
									QString& mem_birthplace,
									QString& mem_birthdate,
									QString& mem_sex,
									QString& mem_maritialstatus,
									QString& mem_cellphone,
									QString& mem_email,
									QString& mem_educationstatus,
									QString& mem_educationallevel,
									QString& mem_schoolname,
									QString& mem_department,
									QString& mem_occupation,
									QString& mem_bloodtype,
									QString& mem_homeaddress,
									QString& mem_companyname,
									QString& mem_companyphone,
									QString& mem_companyaddress,
									QString& mem_membershipdate,
									QString& mem_membershiptype,
									QString& mem_status,
									QString& mem_photopath);

			static void InsertPayment(bool isAidat, 
				                      int pay_no, 
									  QString &pay_date, 
									  QString &pay_res, 
									  QString &pay_years, 
									  int pay_amount, 
									  QString &pay_type, 
									  QString &pay_serial, 
									  QString &pay_code, 
									  QString &pay_text,
									  QString f_paidyears,
							          QString f_notpaidyears);

			static void InsertExpense(QString &expense_no, 
									  QString &expense_date, 
									  QString &expense_res, 
									  float expense_amount, 
									  QString &expense_commercename, 
									  QString &expense_serial, 
									  QString &expense_text);

			static QString GetUserPassword(QString user_id);
			static bool GetUserType(QString name);
			static qint32 GetLastMemberID();
			static bool IsMemberFreezed(int mem_no);
};


