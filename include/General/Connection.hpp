#include <WinSock2.h>
#include <ws2tcpip.h>
#include <string>

namespace General
{
	class Connection
	{
	public:
		Connection(void);
		~Connection(void);

		// Init.
		static SOCKET createSocket(void);
		static void doBind(SOCKET self, unsigned short port);
		static void doListen(SOCKET self);
		static SOCKET doAccept(SOCKET self);
		static void closeSocket(SOCKET& self);

		// Send/Recv.
		static void receiveData(SOCKET self, void* buffer, int size);
		static void receiveHttp(SOCKET self, std::string& buffer, int maxSize);
		static void sendData(SOCKET self, const void* buffer, int size);
	};
}