#pragma once

namespace Global
{
    struct SensorsData
    {
        SensorsData(void) = default;
        SensorsData(float new_temperature, float new_moisture, float new_soilMoisture);

        float temperature = 0.0f;
        float moisture = 0.0f;
        float soilMoisture = 0.0f;
    };
}
