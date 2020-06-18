#include "stdafx.h"
#include "CUser.h"

CStudent::CStudent(char*sname = (char*)"Sname", int sno=0, char*d = (char*)"Dept", int y=0, int ag=0, char*pr=(char*)"PRname")
{
	Sname = new char[40];
	Sno = sno;
	Dept = new char[40];
	Year = y;
	AVG_grade = ag;
	PRname = new char[40];

	strcpy_s(Sname, 40, sname);
	strcpy_s(Dept, 40, d);
	strcpy_s(PRname, 40, pr);
}
CStudent::~CStudent()
{
	delete Sname, Dept, PRname;
}

void CStudent::print()
{
	printf("|  %s   |   %d   |   %s  |   %d   |   %d   |   %s\n", Sname, Sno, Dept, Year, AVG_grade, PRname);
	cout << "+-------------------------------------------------------------------+" << endl;
}

CStudentManager::CStudentManager()
{
	bstate = false;
	maxRow = 0;
}
bool CStudentManager::DBDisconnect()
{
	if(bstate)
	{
		mysql.Disconnect();
		return true;
	}
	return false;
}
bool CStudentManager::DBConnect()
{
	if(mysql.Connect("localhost","root","990127","Student_DB"))
	{
		mysql.SelectDB("student_DB");
		mysql.Query("select *from student");
		mysql.First();
		int i = 0;
		while(!mysql.IsEOF())
		{
			student[i] = new CStudent(mysql["Sname"], atoi(mysql["Sno"]), mysql["Dept"],
								atoi(mysql["Year"]), atoi(mysql["AVG_grade"]), mysql["PRname"]);
			mysql.Next();
			i++;
		}
		maxRow = i;
		DBDisconnect();
		return true;
	}
	return false;
}
void CStudentManager::StudentPrint()
{
	cout << "+------------------------------------------------------------------+" << endl;
	cout << "+  No  |  Sname  |  Sno  |  Dept  |  Year  |  AVG_grade  |  PRname +" << endl;
	cout << "+------------------------------------------------------------------+" << endl;
	for(int i = 0; i<maxRow; i++)
	{
		student[i]->print();
	}
}
bool CStudentManager::searchStudent(char *Sname)
{
	for(int i = 0; i<maxRow; i++)
	{
		if(strncmp(student[i]->getSname(), Sname, strlen(Sname)) == 0)
		{
			cout << "+---------------------------------------+" << endl;
			cout << "+ 학    번 : " << student[i]->getSno() << endl;
			cout << "+ 이    름 : " << student[i]->getSname() << endl;
			cout << "+---------------------------------------+" << endl;
			return true;
		}
	}
	cout << "+ 검색실패 : 찾으시는 학생정보가 없습니다" << endl << endl;
	return false;
}
bool CStudentManager::insertStudent(char* Sname, int Sno, char* Dept, int Year, int AVG_grade, char* PRname)
{
	for(int i = 0; i<maxRow; i++)
	{
		if(student[i]->getSno() == Sno)
		{
			cout << "+ 중복되는 학생이 있어, 입력을 취소합니다." << endl << endl;
			cout << "+------------------------------------------------------------------+" << endl;
			cout << "+  No  |  Sname  |  Sno  |  Dept  |  Year  |  AVG_grade  |  PRname +" << endl;
			cout << "+------------------------------------------------------------------+" << endl;
			student[i]->print();
			return false;
		}
	}
	if(MAX_PATH > maxRow)
	{
		student[maxRow++] = new CStudent(Sname, Sno, Dept, Year, AVG_grade, PRname);
		cout << "+ 학생이 등록되었습니다" << endl << endl;
		return true;
	}
	else
	{
		cout << "+ 자료 입력할 공간이 부족합니다" << endl << endl;
		return false;
	}
}

bool CStudentManager::deleteStudent(int Sno)
{
	for(int i = 0; i<maxRow; i++)
	{
		if(student[i]->getSno() == Sno)
		{
			for(int j = i; j<maxRow-1; j++)
				this->student[j] = this->student[j+1];
			
			cout << "+ 자료가 삭제되었습니다" << endl << endl;
			maxRow--;
			return true;
		}
	}
	cout << "+ 삭제실패 : 찾으시는 학생이 없습니다" << endl << endl;
	return false;
}
bool CStudentManager::AllSaveToDB()
{
	char str[256];
	if(mysql.Connect("localhost","root","1","student_DB"))
	{
		mysql.Query("delete from student");
		for(int i = 0; i<maxRow; i++)
		{
			sprintf(str, "insert into book value(%d,%d,%d,%d,'%s','%s','%s')",i+1, 
				student[i]->getSno(), student[i]->getYear(), student[i]->getAVG_grade(),
				student[i]->getSname(), student[i]->getDept(), student[i]->getPRname());
			mysql.Query(str);
		}
		DBDisconnect();
		return true;;
	}
	else
	{
		cout << "+ DB연결이 되지 않습니다." << endl << endl;
		return false;
	}
}
