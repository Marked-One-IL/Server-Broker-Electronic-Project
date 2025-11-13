#include <iostream>
#include "MainMCU/Server.hpp"
#include "ClientMCU/Server.hpp"
#include "ClientHttp/Server.hpp"
#include "General/ThreadSafeCout.hpp"

int main(int argc, char** argv)
{
    while (true)
    {
        try
        {
            /*
            This is a Broker server.

            MainMCU:
            MainMCU is the main and only source data.
            We wait for connection from it and we move on to ClientMCU.

            ClientMCU:
            ClientMCU are the clients and they get the data from MainMCU.
            We handle an infinite amount of clients (But in practice it will be only 2).
            And we send them the data from MainMCU constantly.
            */
            MainMCU::Server mainMCU(4600);
            ClientMCU::Server clientMCU(4601);
            // Location will be fixed later.
            ClientHttp::Server clientHttp("../../../ClientHttp/index.html");

            General::SensorsData tempSensorsData;
            while (mainMCU.getServerStatus() and clientMCU.getServerStatus())
            {
                General::SensorsData sensorsData = mainMCU.getSensorsData();

                if (sensorsData != tempSensorsData)
                {
                    clientMCU.setSensorsData(sensorsData);
                    clientHttp.setSensorsData(sensorsData);
                    General::cout << sensorsData << General::endl; // Avoid printing the same message (Less bloated and easy to debug).
                }

                tempSensorsData = sensorsData;
            }
        }
        catch (const std::exception& error)
        {
            std::cout << error.what() << std::endl;
        }
    }

    return 0;
}
