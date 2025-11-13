#include "../ClientHttp/Server.hpp"
#include "../General/Connection.hpp"
#include "../General/ThreadSafeCout.hpp"
#include "../General/Helper.hpp"
#include <format>
#include <sstream>

ClientHttp::Server::Server(const char* htmlFilename) :
	ClientMCU::Server(80)
{
    if (auto opt = General::Helper::extractFileContent(htmlFilename)) this->m_htmlContent = opt.value();
    else throw std::runtime_error("Could not load html file");
}

void ClientHttp::Server::handle(SOCKET socket)
{
    try
    {
        while (this->m_keepRunning)
        {
            std::string trashBuffer;
            General::Connection::receiveHttp(socket, trashBuffer, 4096);

            General::SensorsData sensorsData = this->getSensorsData();

            std::string html = std::vformat(
                this->m_htmlContent,
                std::make_format_args(sensorsData.temperature, sensorsData.moisture, sensorsData.soilMoisture));

            General::Connection::sendData(socket, html.c_str(), html.size());
        }
    }
    catch (const std::exception& error)
    {
        // If server is closing some methods from General::Connection may and probably will throw.
        if (this->m_keepRunning)
        {
            LOG_WARNING(error.what());
        }
    }
}