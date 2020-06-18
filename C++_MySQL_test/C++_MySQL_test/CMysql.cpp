#include "stdafx.h"
#include "CMysql.h"

//Constructor and Destructor
CMysql::CMysql()
{
    iRowNum = 0;
    iFieldNum = 0;
    iCurrentPos = 0;

    bIsBof = false;
    bIsEof = false;
    bIsConnect = false;
    bHaveResultQuery = false;

    mysql_init(&conn);
}
CMysql::~CMysql()
{
    if (&conn != NULL)
    {
        mysql_close(&conn);
    }
}
bool CMysql::Connect(const char *host,
    const char *username,
    const char *password,
    const char *database,
    const unsigned int port,
    const char *unix_socket,
    const unsigned int client_flag)
{

    if (&conn == NULL)
    {
        cout << "[Error:" << mysql_errno(&conn) << "] " << mysql_error(&conn) << endl;
        exit(1);
    }

    if (mysql_real_connect(&conn, host, username, password, database, port, unix_socket, client_flag) == NULL)
    {
        IsConnect(false);
        return false;
    }
    IsConnect(true);
    return true;
}
void CMysql::Disconnect()
{
    if (&conn != NULL)
    {
        mysql_close(&conn);
        iRowNum = 0;
        iFieldNum = 0;
        iCurrentPos = 0;
        bIsBof = false;
        bIsEof = false;
        bIsConnect = false;
        bHaveResultQuery = false;
    }
}
bool CMysql::SelectDB(const char *szString)
{
    return !mysql_select_db(&conn, szString);
}
bool CMysql::Query(const char *szString)
{
    if (&conn == NULL || !IsConnect())
    {
        cout << "[Error:" << mysql_errno(&conn) << "] " << mysql_error(&conn) << endl;
        exit(1);
    }

    if (mysql_query(&conn, szString)) //raise error
    {
        return false;
    }
    else
    {
        pres = mysql_store_result(&conn); //결과를 저장하고
        if (pres)
        {
            //OK,현재의 오프셋 등을 저장
            iRowNum = mysql_num_rows(pres);
            iFieldNum = mysql_num_fields(pres);
            row = mysql_fetch_row(pres);
            pfield = mysql_fetch_fields(pres); //pfield[iFieldNum].name;
            IsBOF(true);
            IsEOF(false);
            bHaveResultQuery = true;
            return true;
        }
        else
        {
            //Update, Delete, insert query
            if (mysql_field_count(&conn) == 0)
            {
                iRowNum = mysql_affected_rows(&conn);
                bHaveResultQuery = false;
                return true;
            }
            else
            {
                //raise error
                return false;
            }
        }
    }
    return true;
}
char * CMysql::Field(const char *szFieldName)
{
    //결과가 있는 쿼리인지 조사
    if (!bHaveResultQuery)
    {
        cout << "Exception Field() or [], 결과가 없는 쿼리를 실행했습니다" << endl;
        cout << "[Error:" << mysql_errno(&conn) << "] " << mysql_error(&conn) << endl;
        exit(1);
    }
    //error here!!!!!
    if (iRowNum == 0)
    { 
        cout << "Exception Field() or [], Query 에 해당하는 레코드셋이 없습니다" << endl;
        cout << "[Error:" << mysql_errno(&conn) << "] " << mysql_error(&conn) << endl;
        exit(1);
    }
    //필드이름이 있는지 조사 있다면 인덱스를 리턴한다.
    for (unsigned int i = 0; i < iFieldNum; i++)
    {
        if (strcmp(pfield[i].name, szFieldName) == 0)
        {
            return row[i];
        }
    }
    return row[-1];
}
char * CMysql::Field(const my_ulonglong iFieldIndex)
{
    //결과가 있는 쿼리인지 조사
    if (!bHaveResultQuery)
    {
        cout << "Exception Field() or [], 결과가 없는 쿼리를 실행했습니다" << endl;
        cout << "[Error:" << mysql_errno(&conn) << "] " << mysql_error(&conn) << endl;
        exit(1);
    }
    if (iRowNum == 0)
    {
        cout << "Exception Field() or [], Query 에 해당하는 레코드셋이 없습니다" << endl;
        cout << "[Error:" << mysql_errno(&conn) << "] " << mysql_error(&conn) << endl;
        exit(1);
    }
    if (iFieldIndex > iFieldNum)
    {
        cout << "Exception field() or [], 필드의 인덱스가 음수이거나 범위를 넘습니다" << endl;
        cout << "[Error:" << mysql_errno(&conn) << "] " << mysql_error(&conn) << endl;
        exit(1);
    }
    return row[iFieldIndex];
}
void CMysql::First()
{
    IsBOF(true);
    IsEOF(false);
    iCurrentPos = 0;
    mysql_data_seek(pres, iCurrentPos);
    row = mysql_fetch_row(pres);
}
void CMysql::Last()
{
    IsBOF(false);
    IsEOF(true);
    iCurrentPos = iRowNum - 1;  
    mysql_data_seek(pres, iCurrentPos);
    row = mysql_fetch_row(pres);
}
void CMysql::Next()
{
    if (!IsEOF())
    {
        Move(1);
        if (IsBOF())
        {
            IsBOF(false); 
        }
    }
    else
    {
        cout << "[EXCEPTION] Exception Next(), 다음 레코드셋이 없습니다. ";
        exit(1);
    }
}
void CMysql::Prev()
{
    if (!IsBOF())
    {
        Move(-1);
        if (IsEOF())
        {
            IsEOF(false); 
        }
    }
    else
    {
        cout << "Exception Prev(), 이전 레코드셋이 없습니다." << endl;
        cout << "[Error:" << mysql_errno(&conn) << "] " << mysql_error(&conn) << endl;
        exit(1);
    }
}
void CMysql::Move(int n)
{
    //범위를 확인한다.
    my_ulonglong pos = iCurrentPos + n;
    if (pos == -1)
    {
        IsBOF(true);
        IsEOF(false);
    }
    else if (pos == iRowNum)
    {
        IsBOF(false);
        IsEOF(true);
    }
    else if ((pos < 0) || (pos > iRowNum))
    {
        cout << "Exception Prev() or Next() or Move(), 레코드셋범위를 넘습니다" << endl;
        cout << "[Error:" << mysql_errno(&conn) << "] " << mysql_error(&conn) << endl;
        exit(1);
    }
    iCurrentPos = pos;
    mysql_data_seek(pres, iCurrentPos);
    row = mysql_fetch_row(pres);
}
