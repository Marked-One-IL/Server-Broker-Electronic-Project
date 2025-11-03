#pragma once
#include <ostream>
#include <WinSock2.h>
#include <Windows.h>

namespace General
{
    struct SensorsData
    {
        SensorsData(void) = default;
        SensorsData(float new_temperature, float new_moisture, float new_soilMoisture);

        static General::SensorsData deserialize(const void* buffer);
        static void serialize(const General::SensorsData& sensorsData, void* buffer);

        bool operator != (const SensorsData& other) const;
        friend std::ostream& operator << (std::ostream& stream, const SensorsData& sensorsData);

        float temperature = 0.0f;
        float moisture = 0.0f;
        float soilMoisture = 0.0f;

    private:
        // Casting.
        static UINT32 floatToNetwork(float value);
        static float networkToFloat(UINT32 value);
    };
}
