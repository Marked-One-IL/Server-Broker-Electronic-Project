#include <WinSock2.h>
#include <Windows.h>

#define LOG_INFO(message) General::cout << "[INFO] " << message << General::endl
#define LOG_ERROR(message) General::cout << "[ERROR] " << message << General::endl
#define LOG_WARNING(message) General::cout << "[WARNING] " << message << General::endl

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
		static void sendData(SOCKET self, void* buffer, int size);
	};
}