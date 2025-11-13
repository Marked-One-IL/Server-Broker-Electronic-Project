#include "../ClientMCU/Server.hpp"

namespace ClientHttp
{
	class Server : public ClientMCU::Server
	{
	public:
		Server(const char* htmlFilename);

	private:
		virtual void handle(SOCKET socket) override;
		std::string m_htmlContent;
	};
}