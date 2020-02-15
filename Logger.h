#pragma once
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <stdarg.h>
using namespace std;

void DXLOG( string str );
void DXLOG(const char* str, ...);


class Logger
{
public:

    static Logger* instance();
    
	void log(string str);
	void log(const char* str, va_list list);
    
private:

    Logger();
    Logger(const Logger& l);
	~Logger(); 
    
    // static variables
    static Logger *m_instance;
    ofstream m_stream;
 
};

struct fLog
{
	string funcName;
	float exectime;
};

static vector <fLog> fLogData;
static int logLineCounter = 0;
void PrintLogVectorToFile();
void PrintLogVectorToFileAtShutDown();

class DxTime
{
public:
	DxTime();
	~DxTime();
	void Start();
	float End(const char* funcName);
private:
	float GetExecTime_ms();
	void AddToLogVector(const char* funcName);

	LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
	LARGE_INTEGER Frequency;
};







