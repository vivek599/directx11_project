#include "CpuUsageClass.h"



CpuUsageClass::CpuUsageClass()
{
}

CpuUsageClass::~CpuUsageClass()
{
	Shutdown();
}

void CpuUsageClass::Initialize()
{
	PDH_STATUS status;

	// Initialize the flag indicating whether this object can read the system cpu usage or not.
	m_bCanReadCpu = true;

	// Create a query object to poll cpu usage.
	status = PdhOpenQuery(NULL, 0, &m_queryHandle);
	if (status != ERROR_SUCCESS)
	{
		m_bCanReadCpu = false;
	}

	// Set query object to poll all cpus in the system.
	status = PdhAddCounter(m_queryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &m_counterHandle);
	if (status != ERROR_SUCCESS)
	{
		m_bCanReadCpu = false;
	}

	m_lastSampleTime = GetCurrentTime();


	m_cpuUsage = 0;
}

long CpuUsageClass::GetCurrentTime()
{
	LARGE_INTEGER li, freq;
	QueryPerformanceCounter(&li);
	QueryPerformanceFrequency(&freq);
	return 1000.0f * li.QuadPart / freq.QuadPart;
}

void CpuUsageClass::Shutdown()
{
	if (m_bCanReadCpu)
	{
		PdhCloseQuery(m_queryHandle);
	}

}

void CpuUsageClass::Frame()
{
	PDH_FMT_COUNTERVALUE value;

	if (m_bCanReadCpu)
	{
		if ((m_lastSampleTime + 1000) < GetCurrentTime())
		{
			m_lastSampleTime = GetCurrentTime();

			PdhCollectQueryData(m_queryHandle);

			PdhGetFormattedCounterValue(m_counterHandle, PDH_FMT_LONG, NULL, &value);

			m_cpuUsage = value.longValue;
		}
	}
}

int CpuUsageClass::GetCpuPercentage()
{
	int usage;

	if (m_bCanReadCpu)
	{
		usage = (int)m_cpuUsage;
	}
	else
	{
		usage = 0;
	}

	return usage;
}
