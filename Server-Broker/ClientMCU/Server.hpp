#pragma once
#include "../General/SensorsData.hpp"
#include <WinSock2.h>
#include <Windows.h>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <utility>

namespace ClientMCU
{
	class Server
	{
	public:
		Server(unsigned short port);
		~Server(void);

		void setSensorsData(const General::SensorsData& sensorsData);
		bool getServerStatus(void) const;
		void stop(void);

	private:
		void acceptClients(void);
		void handle(SOCKET socket);
		void addClient(SOCKET clientSocket);
		void removeClient(SOCKET clientSocket);
		General::SensorsData getSensorsData(void);
		
		std::atomic<bool> m_keepRunning = true;

		std::mutex m_clientsMutex;
		std::vector<SOCKET> m_clients;
		SOCKET m_mainSocket = INVALID_SOCKET;

		std::mutex m_getSensorsDataMutex;
		General::SensorsData m_sensorsData;
	};
}