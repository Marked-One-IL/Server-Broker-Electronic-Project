#include <General/ThreadSafeCout.hpp>
#include <string>
#include <optional>

namespace General
{
	namespace Helper
	{
		template <typename Type>
		void logInfo(const Type& value);

		template <typename Type>
		void logWarning(const Type& value);

		std::optional<std::string> extractFileContent(const char* filename);

		static constexpr int SLEEP_TIME = 3;
	}
}

template <typename Type>
void General::Helper::logInfo(const Type& value)
{
	General::cout << "[INFO] " << value << General::endl;
}

template <typename Type>
void General::Helper::logWarning(const Type& value)
{
	General::cout << "[WARNING] " << value << General::endl;
}