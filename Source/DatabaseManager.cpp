#include "DatabaseManager.h"
#include "MessageManager.h"
#include "SettingsManager.h"
#include "Definitions.h"

bool Database::bIsConnected = false;

QSqlDatabase Database::database = QSqlDatabase::addDatabase("QPSQL");

bool Database::Connect()
{
	if(database.isOpen())
		return true;

	QStringList drivers = QSqlDatabase::drivers(); 
	
	QString hostName     = SettingsManager::organizationSettings.hostName;
	QString databaseName = SettingsManager::organizationSettings.dbName;
	QString userNm       = SettingsManager::organizationSettings.userName;
	QString passNm       = SettingsManager::organizationSettings.password;
						   
	database.setHostName(hostName);
	database.setDatabaseName(databaseName);
	database.setUserName(userNm);
	database.setPassword(passNm);

	bIsConnected = database.open();
	
	if (!database.isOpen())
	{
		MessageManager::GetInstance()->AddInfoMessage(QString::fromLocal8Bit("Sunucu: %1 - Veri Tabaný: %2 baðlantýsý kurulamadý!").arg(hostName).arg(databaseName));

		QSqlError err = database.lastError();
		if (err.text().trimmed().count())
		{
			MessageManager::GetInstance()->AddInfoMessage(err.text());
		}	
	}

	return bIsConnected;
}

void Database::Disconnect()
{
	if(bIsConnected)
		database.close();
}

bool Database::IsConnected()
{
	return bIsConnected;
}

QSqlQuery Database::DeriveParameters(QString query)
{
	QSqlQuery q(database);
	q.prepare(query);
	return q;
}

QSqlQuery Database::DeriveParameters(QString query, QList<QVariant> params)
{
	QRegExp rx(":\\w+");
	rx.setCaseSensitivity(Qt::CaseInsensitive);
	QSqlQuery q(database);

	q.prepare(query);
	int count = 0;
	int pos = 0;

	while ((pos = rx.indexIn(query, pos)) != -1) {

		QString s = query.mid(pos, rx.matchedLength());

		if(QString(params.at(count).typeName()).compare("QString") == 0)
		{
			QString sv = params.at(count).toString();
			if(!sv.length())
				q.bindValue(s, QString(" "));
			else
				q.bindValue(s, params.at(count));
		}
		else
			q.bindValue(s, params.at(count));
		++count;
		pos += rx.matchedLength();
	}

	return q;
}

bool Database::ExecuteNonQuery(QString query, QList<QVariant> params, bool dont_log)
{
	Connect();
	QSqlQuery q = DeriveParameters(query, params);
	bool isOk = q.exec();

	if(q.lastError().text().trimmed().length())
	{
		qDebug() << "ExecuteNonQuery error" << q.lastError().text();
		qDebug() << "while trying to execute " << q.executedQuery();
	}

	q.finish();

	Disconnect();

	if(!dont_log && isOk)
	{
		//Log(query);
	}

	return isOk;
}

QSqlQuery Database::ExecuteQuery(QString query, QList<QVariant> params)
{
	Connect();
	QSqlQuery q = DeriveParameters(query, params);
	q.exec();
	Disconnect();
	return q;
}

bool Database::ExecuteNonQuery(QString query, bool dont_log)
{
	Connect();
	QSqlQuery q = DeriveParameters(query);
	bool isOk = q.exec();

	if(!dont_log && isOk)
	{
		//Log(query);
	}

	q.finish();

	Disconnect();
	return isOk;
}

int Database::InsertQuery(QString query, QList<QVariant> params, bool dont_log)
{
	int retVal =0;
	Connect();
	QSqlQuery q = DeriveParameters(query, params);
	bool isOk = q.exec();
	if(isOk)
	{
		retVal = q.lastInsertId().toInt();
		if(!dont_log)
		{
			//Log(query);
		}
	}
	q.finish();

	Disconnect();
	return retVal;
}

QSqlQuery Database::ExecuteQuery(QString query)
{
	Connect();
	QSqlQuery q = DeriveParameters(query);
	q.exec();
	Disconnect();
	return q;
}


QString Database::GetUserPassword(QString user_name)
{
	QString r = "";
	QString query = "SELECT u_password FROM userinfo WHERE u_name = :user_name";
	QSqlQuery q = ExecuteQuery(query, QVariantList() << user_name);
	if(q.next())
	{
		r = q.value(0).toString();
	}
	q.finish();
	return r;
}

void Database::CreateNewGroup(QString& groupName, QString& numberList)
{
	QString query = "INSERT INTO groups ( "
		"gr_name, "       
		"gr_numbers " 
		") VALUES ( "
		":gr_name,  "       
		":gr_numbers  " 
		")";

	ExecuteNonQuery(query, QVariantList() << groupName << numberList);	
}

QString Database::GetNumbersInGroup(QString& grname)
{
	QString numbers = "";

	QString query = "SELECT gr_numbers FROM groups WHERE gr_name = :gr_name";
	QSqlQuery q = ExecuteQuery(query, QVariantList() << grname);

	if(q.next())
	{
		numbers = q.value(0).toString();
	}

	q.finish();
	return numbers;
}

bool Database::GetUserType(QString name)
{
	bool type = 0;

	QString query = "SELECT u_type FROM userinfo WHERE u_name = :user_name";
	QSqlQuery q = ExecuteQuery(query, QVariantList() << name);

	if(q.next())
	{
		type = q.value(0).toBool();
	}

	q.finish();
	return type;
}

void Database::AddNewMemberFee(quint32 mem_ID, QString& notpaid, QString& paid)
{
	QString query = "INSERT INTO membersfee ( "
		"f_id, "       
		"f_paidyears, " 
		"f_notpaidyears "
		") VALUES ( "
		":mem_ID,  "       
		":paid,  " 
		":notpaid "
		")";

	ExecuteNonQuery(query, QVariantList() << mem_ID << paid << notpaid);
}

void Database::AddAnnualyMemberFee(QString& notpaid)
{
	QString query1 = QString("UPDATE membersfee SET "
		"f_notpaidyears = CONCAT(f_notpaidyears, '%1,')").arg(notpaid);

	ExecuteNonQuery(query1, QVariantList());
}

bool Database::InsertMember(QString& mem_name,     
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
							QString& mem_photopath)
{
	QString query = "INSERT INTO membersinfo ( "
		"mem_name, "       
		"mem_surname, " 
		"mem_fathername, " 
		"mem_birthplace, "
		"mem_birthdate, "        
		"mem_sex, "            
		"mem_maritialstatus, " 
		"mem_cellphone, "           
		"mem_email, "               
		"mem_educationstatus, "     
		"mem_educationallevel, "    
		"mem_schoolname, "          
		"mem_department, "          
		"mem_occupation, "          
		"mem_bloodtype, "                    
		"mem_homeaddress, "         
		"mem_companyname, "         
		"mem_companyphone, "        
		"mem_companyaddress, "		
		"mem_membershipdate, "		
		"mem_membershiptype, "		
		"mem_no, "
		"mem_status, "
		"mem_photopath "

		") VALUES ( "
		":mem_name,  "       
		":mem_surname,  " 
		":mem_fathername, "
		":mem_birthplace,  "
		":mem_birthdate,  "       
		":mem_sex,  "            
		":mem_maritialstatus,  " 
		":mem_cellphone,  "           
		":mem_email,  "               
		":mem_educationstatus,  "     
		":mem_educationallevel,  "    
		":mem_schoolname,  "          
		":mem_department,  "          
		":mem_occupation,  "          
		":mem_bloodtype,  "                     
		":mem_homeaddress,  "         
		":mem_companyname,  "         
		":mem_companyphone,  "        
		":mem_companyaddress,  "		
		":mem_membershipdate,  "		
		":mem_membershiptype,  "		
		":mem_no,  " 
		":mem_status, "
		":mem_photopath "
		")";

	return ExecuteNonQuery(query, QVariantList() << mem_name << mem_surname << mem_fathername << mem_birthplace<< mem_birthdate
												 << mem_sex << mem_maritialstatus << mem_cellphone << mem_email
												 << mem_educationstatus << mem_educationallevel << mem_schoolname << mem_department << mem_occupation
												 << mem_bloodtype << mem_homeaddress << mem_companyname << mem_companyphone
												 << mem_companyaddress << mem_membershipdate << mem_membershiptype << mem_no << mem_status << mem_photopath);
}


bool Database::UpdateMember(int mem_no,
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
							QString& mem_photopath)
{
	QString query = "UPDATE membersinfo SET "
		"mem_no = :mem_no, " 
		"mem_name = :mem_name, "       
		"mem_surname = :mem_surname, " 
		"mem_fathername = :mem_fathername, " 
		"mem_birthplace = :mem_birthplace, "
		"mem_birthdate = :mem_birthdate, "        
		"mem_sex = :mem_sex, " 
		"mem_cellphone = :mem_cellphone,"
		"mem_maritialstatus = :mem_maritialstatus, "          
		"mem_email = :mem_email, "               
		"mem_educationstatus = :mem_educationstatus, "     
		"mem_educationallevel = :mem_educationallevel, "    
		"mem_schoolname = :mem_schoolname,"          
		"mem_department = :mem_department, "          
		"mem_occupation = :mem_occupation, "          
		"mem_bloodtype = :mem_bloodtype, "                     
		"mem_homeaddress = :mem_homeaddress, "         
		"mem_companyname = :mem_companyname, "         
		"mem_companyphone = :mem_companyphone, "        
		"mem_companyaddress = :mem_companyaddress, "		
		"mem_membershipdate = :mem_membershipdate, "		
		"mem_membershiptype = :mem_membershiptype, "		
		"mem_status = :mem_status, "
		"mem_photopath = :mem_photopath "
		"WHERE mem_no = :mem_no;";

	return ExecuteNonQuery(query, QVariantList() <<mem_no<< mem_name << mem_surname << mem_fathername << mem_birthplace<< mem_birthdate
												 << mem_sex << mem_cellphone <<mem_maritialstatus << mem_email
												 << mem_educationstatus << mem_educationallevel << mem_schoolname << mem_department << mem_occupation
												 << mem_bloodtype << mem_homeaddress << mem_companyname << mem_companyphone
												 << mem_companyaddress << mem_membershipdate << mem_membershiptype << mem_status << mem_photopath << mem_no );
}

void Database::InsertPayment(bool isAidat, 
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
							 QString f_notpaidyears)
{

	QString query = "INSERT INTO payments ( "
		"pay_no, "       
		"pay_date, "
		"pay_responsible, "       
		"pay_year, "
		"pay_amount, "       
		"pay_type, "
		"pay_serial, "       
		"pay_code, "
		"pay_text "
		") VALUES ( "
		":pay_no, "       
		":pay_date, "
		":pay_responsible, "       
		":pay_year, "
		":pay_amount, "       
		":pay_type, "
		":pay_serial, "       
		":pay_code, "
		":pay_text "
		")";

	ExecuteNonQuery(query, QVariantList() << pay_no << pay_date << pay_res << pay_years << pay_amount << pay_type << pay_serial << pay_code <<  pay_text);
	
	if (isAidat)
	{
		QString query = "UPDATE membersfee SET "
						"f_paidyears = :f_paidyears, "   
						"f_notpaidyears = :f_notpaidyears " 
						"WHERE f_id = :pay_no;";

		ExecuteNonQuery(query, QVariantList() << f_paidyears << f_notpaidyears << pay_no);
	}

}

void Database::InsertExpense(QString &expense_no, 
							 QString &expense_date, 
							 QString &expense_res, 
				 		     float expense_amount, 
						     QString &expense_commercename, 
						     QString &expense_serial, 
						     QString &expense_text)
{

	QString query = "INSERT INTO expenses ( "
		"expense_no, "       
		"expense_date, "
		"expense_responsible, "       
		"expense_amount, "       
		"expense_commercename, "
		"expense_serial, "       
		"expense_text "
		") VALUES ( "
		":expense_no, "       
		":expense_date, "
		":expense_responsible, "       
		":expense_amount, "       
		":expense_commercename, "
		":expense_serial, "       
		":expense_text "
		")";

	ExecuteNonQuery(query, QVariantList() << expense_no << expense_date << expense_res << expense_amount << expense_commercename<< expense_serial <<  expense_text);
}

bool Database::MakeMemberPasif(int mem_no)
{
	QString mem_status = QString("Pasif");
	QString query = "UPDATE membersinfo SET "
		            "mem_status = :mem_status "   
			        "WHERE mem_no = :mem_no;";

   return ExecuteNonQuery(query, QVariantList() << mem_status << mem_no);
}

qint32 Database::GetLastMemberID()
{
	qint32 r = -1;
	QSqlQuery q = ExecuteQuery("SELECT mem_no FROM membersinfo ORDER BY mem_no DESC");
	if(q.next())
		r = q.value(0).toInt();

	q.finish();

	return r;
}

bool Database::IsMemberFreezed(int mem_no)
{
	QString r = "";
	bool freeze = false;
	QString query = QString("SELECT mem_status FROM membersinfo WHERE mem_no =%1").arg(mem_no);
	QSqlQuery q = ExecuteQuery(query);
	if(q.next())
		r = q.value(0).toString();

	q.finish();

	return r == "Pasif";
}

bool Database::IsMemberNoExist(int mem_no)
{
	bool r = false;

	QString query = QString("SELECT mem_status FROM membersinfo WHERE mem_no =%1").arg(mem_no);
	QSqlQuery q = ExecuteQuery(query);
	if(q.size() > 0)
		r = true;

	q.finish();

	return r;
}

QString Database::GetPaidYears(int payNo)
{
	QString r = "";
	
	QString query = "SELECT f_paidyears FROM membersfee WHERE f_id = :payNo";
	QSqlQuery q = ExecuteQuery(query, QVariantList() << payNo);
	if(q.next())
	{
		r = q.value(0).toString();
	}
	q.finish();

	return r;
}

int Database::GetLastYear()
{
	qint32 r = -1;
	
	QString query = "SELECT set_lastyear FROM settings";
	QSqlQuery q = ExecuteQuery(query, QVariantList());
	if(q.next())
	{
		r = q.value(0).toInt();
	}
	q.finish();

	return r;
}

bool Database::GetFeeLoaded()
{
	bool r = false;
	
	QString query = "SELECT set_loadfee FROM settings";
	QSqlQuery q = ExecuteQuery(query, QVariantList());
	if(q.next())
	{
		r = q.value(0).toBool();
	}
	q.finish();

	return r;
}

qint32 Database::GetFeePerYear(int year)
{
	qint32 r = -1;
	
	QString query = "SELECT y_fee FROM yearfee WHERE y_year = :year";
	QSqlQuery q = ExecuteQuery(query, QVariantList() << year);
	if(q.next())
	{
		r = q.value(0).toInt();
	}
	q.finish();

	return r;
}

bool Database::AddLastLogin(QString lastLoginInfo)
{
	QString query = "UPDATE settings SET "
		            "set_lastlogin = :set_lastlogin ";

   return ExecuteNonQuery(query, QVariantList() << lastLoginInfo);
}

bool Database::AddLastYear(int year)
{
	QString query = "UPDATE settings SET "
		            "set_lastyear = :set_lastyear ";

   return ExecuteNonQuery(query, QVariantList() << year);
}

bool Database::AddFeeLoaded(bool loaded)
{
	QString query = "UPDATE settings SET "
		            "set_loadfee = :set_loadfee ";

	return ExecuteNonQuery(query, QVariantList() << loaded);
}



