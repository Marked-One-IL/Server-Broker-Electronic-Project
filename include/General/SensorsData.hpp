#pragma once
#include <ostream>
#include <WinSock2.h>
#include <cstdint>

namespace General
{
    struct SensorsData
    {
        SensorsData(void) = default;

        static General::SensorsData deserialize(const void* buffer);
        static void serialize(const General::SensorsData& sensorsData, void* buffer);

        bool operator != (const SensorsData& other) const;
        friend std::ostream& operator << (std::ostream& stream, const SensorsData& sensorsData);

        float temperature = 0.0f;
        float moisture = 0.0f;
        float soilMoisture = 0.0f;
        float light = 0.0f;

        inline static constexpr size_t SIZE_AS_ARRAY = (sizeof(float) * 4) / sizeof(float);

    private:
        // Casting.
        static uint32_t floatToNetwork(float value);
        static float networkToFloat(uint32_t value);
    };
}