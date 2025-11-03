#pragma once
#include "../General/SensorsData.hpp"

#include <WinSock2.h>
#include <Windows.h>
#include <atomic>
#include <thread>
#include <mutex>

namespace MainMCU
{
    class Server
    {
    public:
        Server(unsigned short port);
        ~Server(void);

        bool getServerStatus(void) const;
        General::SensorsData getSensorsData(void);
        void stop(void);

    private:
        void handle(void);
        void setSensorsData(const General::SensorsData& sensorsData);

        std::mutex m_getSensorsDataMutex;
        General::SensorsData m_sensorsData;

        std::atomic<bool> m_keepRunning = true;
        SOCKET m_mainSocket = INVALID_SOCKET;
        SOCKET m_clientSocket = INVALID_SOCKET;
    };
}