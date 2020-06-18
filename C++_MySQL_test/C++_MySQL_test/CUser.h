#pragma once

#include <iostream>
#include "stdlib.h"
#include "CMysql.h"

class CStudent
{
    int Sno;
    int Year;
    int AVG_grade;
    char* Sname;
    char* Dept;
    char* PRname;
public:
    CStudent(char* Sname, int sno, char* d, int y, int ag, char* pr);
    virtual ~CStudent();
    int getSno() { return Sno; }
    int getYear() { return Year; }
    int getAVG_grade() { return AVG_grade; }
    char* getSname() { return Sname; }
    char* getDept() { return Dept; }
    char* getPRname() { return PRname; }
    void print();
};

class CStudentManager
{
    CStudent *student[MAX_PATH];
    CMysql mysql;
    bool bstate;
    int maxRow;
public:
    CStudentManager();
    virtual ~CStudentManager() {}
    void StudentPrint();
    bool searchStudent(char* Sname);
    bool insertStudent(char* Sname, int sno, char* d, int y, int ag, char* pr);
    bool deleteStudent(int Sno);
    bool AllSaveToDB();
    bool DBDisconnect();
    bool DBConnect();
};
