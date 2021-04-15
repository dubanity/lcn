#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NONSTDC_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN 

#pragma comment(lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>
#include "IncludeMe.hpp"
#include "WS2VersionInfo.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

WS2VI ws2vi;

uint32_t nReqVersion = ws2vi.GetVersion();
uint32_t nPort = ws2vi.GetPort();
const char* nAddr = ws2vi.GetAddress();

class CL
{
public:
	CL() {};
	~CL() {};

public:
	void log(char* user, char* resBuffer, int rBytes)
	{
		std::cout << "[" << user << "]> " << std::string(resBuffer, NULL, rBytes) << std::endl;
	}
};

using namespace std::literals::chrono_literals;

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	int wsOk = WSAStartup(MAKEWORD(nReqVersion, 0x00), &wsaData);
	if (wsOk == NULL)
	{
		if (LOBYTE(wsaData.wVersion >= nReqVersion))
		{
			SOCKET fdServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			switch (fdServer)
			{
			case INVALID_SOCKET:
				std::cerr << "Failed to fetch local server file descriptor. Code #" << WSAGetLastError() << std::endl;
				WSACleanup();
				break;
			default:
				sockaddr_in clService;
				clService.sin_family = AF_INET;
				clService.sin_port = htons(nPort);
				clService.sin_addr.s_addr = inet_addr(nAddr);

				int clSize = sizeof(clService);

				int connResult = connect(fdServer, reinterpret_cast<sockaddr*>(&clService), clSize);
				switch (connResult)
				{
				case SOCKET_ERROR:
					std::cerr << "Could not connect to local server on specified port. Code #" << WSAGetLastError() << std::endl;
					WSACleanup();
					break;
				default:
					break;
				}

				char host[NI_MAXHOST];
				memset(&host, 0, sizeof(host));
				char service[NI_MAXSERV];
				memset(&service, 0, sizeof(service));

				getnameinfo(reinterpret_cast<sockaddr*>(&clService), clSize, host, NI_MAXHOST, service, NI_MAXSERV, 0);

				char responseBuffer[4096];
				std::string uInput;
				CL cl;

				std::cout << "init some stuff" << std::endl;
				std::this_thread::sleep_for(8s);
				std::cout << "done waiting" << std::endl;

				do
				{
					std::cout << "> ";
					std::getline(std::cin, uInput);
					if (uInput.size() > 0)
					{
						int sendResult = send(fdServer, uInput.c_str(), uInput.size() + 1, NULL);
						if (sendResult != SOCKET_ERROR)
						{
							memset(&responseBuffer, 0, sizeof(responseBuffer));
							int rBytes = recv(fdServer, responseBuffer, 4096, NULL);
							if (rBytes > 0)
							{
								cl.log(host, responseBuffer, rBytes);
							}
						}
					}
				} while (uInput.size() > 0);

				closesocket(fdServer);
			}
		}
		else
		{
			std::cerr << "Requested version is not supported." << std::endl;
		}
	}
	else
	{
		std::cerr << "WinSock2 initial startup failed. Exiting program..." << std::endl;
	}
	WSACleanup();
	ExitProcess(EXIT_SUCCESS);
}