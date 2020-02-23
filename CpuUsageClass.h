#pragma once


#pragma comment( lib, "pdh.lib" )

#include <Pdh.h>

class CpuUsageClass
{
public:
	CpuUsageClass();
	~CpuUsageClass();

	void Initialize();


	void Shutdown();
	void Frame();
	int GetCpuPercentage();
	long GetCurrentTime();

private:
	bool m_bCanReadCpu;
	HQUERY m_queryHandle;
	HCOUNTER m_counterHandle;
	unsigned long m_lastSampleTime;
	long m_cpuUsage;
};

