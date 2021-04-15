#pragma once

#include "IncludeMe.hpp"

class WS2VI
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