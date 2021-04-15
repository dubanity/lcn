#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NONSTDC_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#pragma comment(lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>

#include "WS2VersionInfo.hpp"
#include <ctime>

#include <fstream>
#include <sstream>
#include <iostream>

#include "mysql.h"

using namespace std::literals::chrono_literals;

class MySQL
{
protected:
	const char* hostname = "localhost";
	const char* username = "root";
	const char* passwd = "<enter password here>";
	const char* dbName = "<enter database name here>";
	uint32_t port = 3306;
public:
	MySQL() {};
	~MySQL() {};
public:
	static void QueryCompleted(const char* sQuery)
	{
		auto n_syscl = std::chrono::system_clock::now();
		std::time_t end_time = std::chrono::system_clock::to_time_t(n_syscl);
		std::cout << "------------------------------\n";
		std::cout << "--| " << "QUERY COMPLETED" << " |---------\n";
		std::cout << "------------------------------\n";
		std::cout << "[QUERY]\n";
		std::cout << sQuery << "\n";
		std::cout << "------------------------------\n";
		std::cout << "[TIMESTAMP]\n";
		std::cout << std::ctime(&end_time);
		std::cout << "------------------------------\n";
		std::cout << std::endl;
	}

	static void QueryFailed(MYSQL* hConn)
	{
		std::cout << "------------------------------\n";
		std::cout << "--| " << "QUERY FAILED" << " |------------\n";
		std::cout << "------------------------------\n";
		std::cout << "[ERR_MSG]\n";
		std::cout << mysql_error(hConn) << "\n";
		std::cout << "------------------------------\n";
		std::cout << std::endl;
	}
	void CreateTable(const std::string& tbName, const std::string& colName) const
	{
		int qState;
		MYSQL* conn;
		conn = mysql_init(0);
		conn = mysql_real_connect(conn, hostname, username, passwd, dbName, port, NULL, 0);
		if (conn)
		{
			std::stringstream ss;

			ss << "CREATE TABLE " << tbName << "(" << colName << " varchar(255));";

			std::string query = ss.str();
			const char* q = query.c_str();
			qState = mysql_query(conn, q);
			if (!qState)
			{
				MySQL::QueryCompleted(q);
			}
			else
			{
				MySQL::QueryFailed(conn);
			}
		}
	}
	void DropTable(const std::string& tbName) const
	{
		int qState;
		MYSQL* conn;
		conn = mysql_init(0);
		conn = mysql_real_connect(conn, hostname, username, passwd, dbName, port, NULL, 0);
		if (conn)
		{
			std::stringstream ss;

			ss << "DROP TABLE " << tbName << ";";
			std::string query = ss.str();
			const char* q = query.c_str();
			qState = mysql_query(conn, q);
			if (!qState)
			{
				MySQL::QueryCompleted(q);
			}
			else
			{
				MySQL::QueryFailed(conn);
			}
		}
	}

	void InsertKey(const std::string& tbName, const std::string& colName, const std::string& idxData) const
	{
		int qState;
		MYSQL* conn;
		conn = mysql_init(0);
		conn = mysql_real_connect(conn, hostname, username, passwd, dbName, port, NULL, 0);
		if (conn)
		{
			std::stringstream ss;

			ss << "INSERT INTO " << tbName << "(" << colName << ") VALUES" << "('" << idxData << "');";
			std::string query = ss.str();
			const char* q = query.c_str();
			qState = mysql_query(conn, q);
			if (!qState)
			{
				MySQL::QueryCompleted(q);
			}
			else
			{
				MySQL::QueryFailed(conn);
			}
		}
	}

	void QueryTable(const std::string& tbName, const int& idxPos)
	{
		int qState;
		MYSQL* conn;
		MYSQL_ROW row;
		MYSQL_RES* res;
		conn = mysql_init(0);
		conn = mysql_real_connect(conn, hostname, username, passwd, dbName, port, NULL, 0);
		if (conn)
		{
			std::stringstream ss;

			ss << "SELECT * FROM " << tbName << ";";
			std::string query = ss.str();
			const char* q = query.c_str();
			qState = mysql_query(conn, q);
			if (!qState)
			{
				res = mysql_store_result(conn);
				std::cout << "------------------------------\n";
				std::cout << "--| " << "QUERY RESULT" << " |------------\n";
				std::cout << "------------------------------\n";
				while (row = mysql_fetch_row(res))
				{
					std::cout << row[idxPos] << "\n";
				}
				std::cout << "------------------------------\n";
				std::cout << std::endl;
			}
			else
			{
				MySQL::QueryFailed(conn);
			}
		}
	}
};

API api;
MySQL db;

uint32_t nReqVersion = api.GetVersion();
uint32_t nPort = api.GetPort();
const char* nAddr = api.GetAddress();

int main(int argc, const char* argv[])
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(nReqVersion, 0x00), &wsaData) == NULL)
	{
		if (LOBYTE(wsaData.wVersion >= nReqVersion))
		{
			SOCKET fdServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			switch (fdServer)
			{
			case INVALID_SOCKET:
				std::cerr << "[WS2]> Failed initialize socket for local server." << std::endl;
				WSACleanup();
				break;
			default:
				sockaddr_in saServer;
				saServer.sin_family = AF_INET;
				saServer.sin_port = htons(nPort);
				saServer.sin_addr.s_addr = inet_addr(nAddr);

				int bResult = bind(fdServer, reinterpret_cast<sockaddr*>(&saServer), sizeof(saServer));
				int lResult = listen(fdServer, SOMAXCONN);

				std::cout << "[WS2]> Server started successfully, no errors to report." << std::endl;

				sockaddr_in rClService;
				int rClSize = sizeof(rClService);

				SOCKET hRemoteSocket = accept(fdServer, reinterpret_cast<sockaddr*>(&rClService), &rClSize);
				switch (hRemoteSocket)
				{
				case SOCKET_ERROR:
					break;
				default:
					break;
				}

				char host[NI_MAXHOST];
				char service[NI_MAXSERV];

				memset(&host, 0, sizeof(host));
				memset(&service, 0, sizeof(service));

				if (getnameinfo(reinterpret_cast<sockaddr*>(&rClService), rClSize, host, NI_MAXHOST, service, NI_MAXSERV, 0) == NULL)
				{
					std::cout << host << " connected on port " << service << std::endl;
				}
				else
				{
					inet_ntop(AF_INET, &rClService.sin_addr, host, NI_MAXHOST);
					std::cout << host << " connected on port " << ntohs(rClService.sin_port) << std::endl;
				}

				closesocket(fdServer);

				char buffer[4096];

				while (true)
				{
					memset(&buffer, 0, 4096);

					int rBytes = recv(hRemoteSocket, buffer, 4096, 0);
					if (rBytes == 0)
					{
						WSACleanup();
						ExitProcess(EXIT_SUCCESS);
					}

					if (send(hRemoteSocket, buffer, rBytes, 0) == SOCKET_ERROR)
					{
						std::cerr << host << " has disconnected." << std::endl;
						WSACleanup();
						std::cin.get();
						break;
					}
				}

				closesocket(hRemoteSocket);
			}
		}
		else
		{
			std::cerr << "[WS2]> Requested version is not supported." << std::endl;
		}
	}
	else
	{
		std::cerr << "[WS2]> WinSock2 initial startup failed. Exiting program..." << std::endl;
	}
	ExitProcess(EXIT_SUCCESS);
}