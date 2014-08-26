//============================================================================
// Name        : DaoTest.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <list>
#include <iterator>
#include <iostream>
#include <stdio.h>
#include <cstring>
#include "sqlite3.h"
using namespace std;

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
	int i;
//	for(i=0; i<argc; i++){
//			printf("%10s\t", azColName[i]);
//		}
//	printf("\n---------------------------------------------------------------------------------------\n");
	for(i=0; i<argc; i++){
		printf("%10s\t", argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

template <class T1>
class Dao{
public:
	//virtual ~Dao() = 0;
	virtual void create(T1 instance) = 0;
	virtual void read(char const* name) = 0;
	virtual void update(T1 instance,char const*,char const*) = 0;
	virtual void del(T1 instance) = 0;


};
class nameable {
public:
	nameable(char const* name):name(name) {

	}

	char const* getName(){
		return name;
	}

private:
	char const* name;
};

class Installed {
public:
	Installed(long date):date(date){

	}
	long getDate(){
		return date;
	}
private:
	long date;
};

class Software : public nameable,public Installed {
public:
	Software(char const* name,long date):nameable(name),Installed(date){

	}
	friend bool operator<(Software sw1, Software sw2);
	friend bool operator>(Software sw1, Software sw2);
	friend bool operator==(Software sw1, Software sw2);
};

bool operator<(Software sw1, Software sw2){
	return sw1.getDate() < sw2.getDate();
}
bool operator>(Software sw1, Software sw2){
	return sw1.getDate() > sw2.getDate();
}
bool operator==(Software sw1, Software sw2){
	return sw1.getDate() == sw2.getDate();
}

void getDetails(Software& sw) {
        std::cout << "name: " << sw.getName() << endl;
}

void for_each(long begin,long end,void(*getDetails)(Software&)){

}

class SoftwareDao : public Dao <Software> {
public:
	//~SoftwareDao();
	SoftwareDao(){
		//sqlite3 *db; //database file pointer
		//string str;
		recNum=0;
		colNum=6;
		sql = new char[1024]; //sql input
		zErrMsg = 0; //error message buffer

		/* open test.db, create test.db if it doesn't exist. */
		if(sqlite3_open("test.db",&db)) {
			cout << "Could not open database: " << sqlite3_errmsg(db) << endl;
			//exit(0);
		}
		sprintf(sql, "CREATE TABLE INVENTORY("
					"ID INTEGER PRIMARY KEY, "
					"VENDOR TEXT, "
					"NAME TEXT NOT NULL, "
					"VERSION TEXT, "
					"STOCK INT, "
					"UPDATED TEXT );");
		sqlite3_exec(db,sql,callback,0,&zErrMsg);

	}
	void create(Software instance){
		cout<< "Creating " << instance.getName() << endl;
		char** sqlres;

		sprintf(sql,"SELECT * from INVENTORY ;");
		sqlite3_get_table(db,sql,&sqlres,&recNum,&colNum,&zErrMsg);
		sprintf(sql, "INSERT into INVENTORY VALUES ("
					"NULL, 'Microsoft', '%s', 'ver', 1, NULL );",instance.getName() );
		for(int i=2;i<(recNum+1)*colNum;i+=colNum){
			if (sqlres[i]==NULL ) break;
			if(strcmp(sqlres[i],instance.getName()) == 0){
				sprintf(sql, "REPLACE into INVENTORY VALUES ("
							"COALESCE( (SELECT ID FROM INVENTORY WHERE NAME='%s'), 1),"
							"'Microsoft', '%s', 'ver', "
							"COALESCE( (SELECT STOCK FROM INVENTORY WHERE NAME='%s'), 0) + 1, NULL );",
							instance.getName(),instance.getName(),instance.getName());
				//cout <<sqlres[i]<< " foundit " <<instance.getName() <<endl;
				break;
			}// else cout<< sqlres[i]<<" nope "<<instance.getName() <<endl;
		}
		sqlite3_free_table(sqlres);
		sqlite3_exec(db,sql,callback,0,&zErrMsg);
		recNum++;
	}
	void read(char const* name){
		if(strcmp(name,"") != 0){
			cout<< "Reading " << name << endl;
			sprintf(sql, "SELECT * from INVENTORY where NAME='%s';",name );
			sqlite3_exec(db,sql,callback,0,&zErrMsg);
		}
		else {
			cout<< "Reading All" << endl;
			sprintf(sql, "SELECT * from INVENTORY ;" );
			sqlite3_exec(db,sql,callback,0,&zErrMsg);
		}
	}
	void update(Software instance, char const* field, char const* value){
		cout<< "Updating " << instance.getName() << endl;
		sprintf(sql, "UPDATE INVENTORY set %s='%s' where NAME='%s'; ",field,value,instance.getName() );
		sqlite3_exec(db,sql,callback,0,&zErrMsg);
	}
	void del(Software instance){
		cout<< "Deleting " << instance.getName() << endl;
		sprintf(sql, "DELETE from INVENTORY where NAME='%s'; ", instance.getName() );
		sqlite3_exec(db,sql,callback,0,&zErrMsg);
	}
private:
	sqlite3 *db;
	char* sql;
	char *zErrMsg;
	int recNum;
	int colNum;
};


list<Software> find(list<Software> list, long startDate, long endDate){
	list.sort();
	for(int i=0;i<(int)list.size();i++){
		if(list.front().getDate() < startDate){
			list.pop_front();
			i--;
		}
		else if(list.back().getDate() > endDate){
			list.pop_back();
			i--;
		}
	}
	return list;
};

int main() {
	SoftwareDao p;
	Software p1("Office",20141020);
	Software p2("McAfee",20131020);
	Software p3("Outlook",20140220);
	Software p4("Explorer",20140120);
	Software p5("Flash",20101020);
	list<Software> swList;
	swList.push_back(p1);
	swList.push_back(p2);
	swList.push_back(p3);
	swList.push_back(p4);
	swList.push_back(p5);
	swList = find(swList,20101021,20141130);
	for(int i=0;i<(int)swList.size();i++){
		printf("%ld %s \n",swList.front().getDate(),swList.front().getName());
		swList.push_back(swList.front());
		swList.pop_front();
	}
	p.create(p1);
	p.create(p2);
	p.create(p3);
	p.create(p4);
	p.create(p5);
	p.update(p2,"UPDATED","Mon Aug 8 2014 15:47:53");
	p.read("");
	p.del(p2);
	p.del(p1);
	return 0;
}
