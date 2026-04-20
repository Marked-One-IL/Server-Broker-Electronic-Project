#include <General/SensorsData.hpp>
#include <cstring>
#include <cmath>

General::SensorsData General::SensorsData::deserialize(const void* buffer)
{
    General::SensorsData sensorsData;
    const uint32_t* buffer32 = reinterpret_cast<const uint32_t*>(buffer);
 
    // I gotta do this because different CPUs handle numbers differently (AKA big/small endian).
    // This make sending numbers portable no matter what device is used.
    sensorsData.temperature = General::SensorsData::networkToFloat(buffer32[0]);
    sensorsData.moisture = General::SensorsData::networkToFloat(buffer32[1]);
    sensorsData.soilMoisture = General::SensorsData::networkToFloat(buffer32[2]);
    sensorsData.light = General::SensorsData::networkToFloat(buffer32[3]);

    return sensorsData;
}
void General::SensorsData::serialize(const General::SensorsData& sensorsData, void* buffer)
{
    uint32_t* buffer32 = reinterpret_cast<uint32_t*>(buffer);

    // I gotta do this because different CPUs handle numbers differently (AKA big/small endian).
    // This make sending numbers portable no matter what device is used.
    buffer32[0] = General::SensorsData::floatToNetwork(sensorsData.temperature);
    buffer32[1] = General::SensorsData::floatToNetwork(sensorsData.moisture);
    buffer32[2] = General::SensorsData::floatToNetwork(sensorsData.soilMoisture);
    buffer32[3] = General::SensorsData::floatToNetwork(sensorsData.light);
}

bool General::SensorsData::operator != (const SensorsData& other) const
{
    // Float comparisons are not very accurate so we approximate them.
    const float EPSILON = 1e-4f;
    return (std::fabs(this->temperature  - other.temperature)  > EPSILON) or
           (std::fabs(this->moisture     - other.moisture)     > EPSILON) or
           (std::fabs(this->soilMoisture - other.soilMoisture) > EPSILON);
}
namespace General
{
    std::ostream& operator << (std::ostream& stream, const SensorsData& sensorsData)
    {
        stream << "Temperature: " << sensorsData.temperature << '\n';
        stream << "Moisture: " << sensorsData.moisture << '\n';
        stream << "Soil Moisture: " << sensorsData.soilMoisture << '\n';
        stream << "Light: " << sensorsData.light << '\n';

        return stream;
    }
}

uint32_t General::SensorsData::floatToNetwork(float value)
{
    uint32_t temp;
    std::memcpy(&temp, &value, sizeof(value));
    return htonl(temp);
}
float General::SensorsData::networkToFloat(uint32_t value)
{
    uint32_t temp = ntohl(value);
    float temp2;
    std::memcpy(&temp2, &temp, sizeof(temp2));
    return temp2;
}