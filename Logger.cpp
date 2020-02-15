#include "Logger.h"

// static variables
Logger* Logger::m_instance = 0;

// return the Logger instance
Logger* Logger::instance()
{
	if (!m_instance)
	{
		m_instance = new Logger;
	}

	return m_instance;
}

void Logger::log(string str)
{
	m_stream << "Log: " << str << endl;
}

// log a variable number of arguments
void Logger::log( const char* str, va_list list)
{
	char buffer[260];

	vsnprintf(buffer, 260, str, list);

	//m_stream << "Log: " << "[ " << __TIME__ << " ] " << buffer << endl;
	m_stream << "Log: " << buffer << endl;

}
 
Logger::Logger()
{
	m_stream.open("DX_LOG.txt");
	m_stream << "[ " << __DATE__ << "\t" << __TIME__ << " ] " << "log file opened..." << endl;
}

Logger::~Logger()
{
	m_stream << "[ " << __DATE__ << "\t" << __TIME__ << " ] " << "log file closed..." << endl;
	m_stream.close();
}

Logger::Logger(const Logger& l)
{
	// this cannot happen
}

void DXLOG(string str)
{
//#ifdef _DEBUG
	Logger::instance()->log(str.c_str());
//#endif
}

void DXLOG( const char* str, ...)
{
//#ifdef _DEBUG
	va_list args;
	va_start(args, str);
	Logger::instance()->log(str, args);
	va_end(args);
//#endif
}

void PrintLogVectorToFile()
{
	for (int i = 0; i < fLogData.size(); i++)
	{
		DXLOG( "%s ExecTime : %f millisec", fLogData[i].funcName.c_str(), fLogData[i].exectime );
		logLineCounter++;
	}
}

void PrintLogVectorToFileAtShutDown()
{
	for (int i = logLineCounter; i < fLogData.size(); i++)
	{
		DXLOG("%s ExecTime : %f millisec", fLogData[i].funcName.c_str(), fLogData[i].exectime);
	}
}

DxTime::DxTime()
{
	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&StartingTime);
	QueryPerformanceCounter(&EndingTime);
}

DxTime::~DxTime()
{
}

void DxTime::Start()
{
	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&StartingTime);
}

float DxTime::End(const char* funcName)
{
	QueryPerformanceCounter(&EndingTime);
	ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
	//AddToLogVector(funcName);
	return GetExecTime_ms();
}

float DxTime::GetExecTime_ms()
{
	ElapsedMicroseconds.QuadPart *= 1000000;
	ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;

	float deltaTime = (ElapsedMicroseconds.QuadPart) / (1000.f);
	//DXLOG( "deltaTime %f ", deltaTime );
	return (deltaTime);
}

void DxTime::AddToLogVector( const char* funcName)
{
	fLog tmp = {};

	tmp.funcName = string(funcName);
	tmp.exectime = GetExecTime_ms();
	fLogData.push_back(tmp);
}









