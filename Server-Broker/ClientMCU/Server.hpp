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
		void addClient(SOCKET clientSocket);
		void freeFinishedThreads(void);

		std::vector<std::pair<SOCKET, std::thread>> m_clients;
		std::thread m_mainThread;
		std::thread m_freeingThread;
		std::mutex m_clientsMutex;
		std::mutex m_getSensorsDataMutex;
		
		SOCKET m_mainSocket = INVALID_SOCKET;
		General::SensorsData m_sensorsData;

	protected:
		virtual void handle(SOCKET socket);
		General::SensorsData getSensorsData(void);
		std::atomic<bool> m_keepRunning = true;
	};
}