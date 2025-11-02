#pragma once
#include "../Global/SensorsData.hpp"

#include <WinSock2.h>
#include <Windows.h>
#include <atomic>
#include <thread>
#include <mutex>

namespace Static
{
    class Server
    {
    public:
        Server(const char* ip, unsigned short port);
        ~Server(void);

        bool getServerStatus(void) const;
        Global::SensorsData getSensorsData(void);
        void stop(void);

    private:
        void handle(void);
        void setSensorsData(const Global::SensorsData& sensorsData);
        static void receiveData(SOCKET socket, char* buffer, int size);

        std::mutex m_getSensorsDataMutex;
        Global::SensorsData m_sensorsData;

        std::atomic<bool> m_keepRunning = true;
        std::thread m_serverThread;
        SOCKET m_mainSocket = INVALID_SOCKET;
        SOCKET m_clientSocket = INVALID_SOCKET;
    };
}