#pragma once

#ifdef __cplusplus
#include <string>
#include <memory>
#include <algorithm>
#include <utility>
#include <functional>
#include <thread>
#include <array>
#include <tuple>
#include <vector>
#include <chrono>
#else
#include <cmath>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#endif

#include <TlHelp32.h>
#include <WinBase.h>

class API
{
private:
	const char* nAddr = ("127.0.0.1");
	const uint32_t nReqVersion = 2;
	const uint32_t nPort = 54000;

public:
	const char* GetAddress() const
	{
		return nAddr;
	}

	uint32_t GetPort() const
	{
		return nPort;
	}

	uint32_t GetVersion() const
	{
		return nReqVersion;
	}
};