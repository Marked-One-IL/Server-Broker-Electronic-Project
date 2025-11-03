#include "../General/SensorsData.hpp"
#include <cstring>
#include <cmath>

General::SensorsData::SensorsData(float new_temperature, float new_moisture, float new_soilMoisture) :
    temperature(new_temperature), moisture(new_moisture), soilMoisture(new_soilMoisture)
{
}

General::SensorsData General::SensorsData::deserialize(const void* buffer)
{
    General::SensorsData sensorsData;
    const UINT32* buffer32 = reinterpret_cast<const UINT32*>(buffer);
 
    // I gotta do this because different CPUs handle numbers differently (AKA big/small endian).
    // This make sending numbers portable no matter what device is used.
    sensorsData.temperature = General::SensorsData::networkToFloat(buffer32[0]);
    sensorsData.moisture = General::SensorsData::networkToFloat(buffer32[1]);
    sensorsData.soilMoisture = General::SensorsData::networkToFloat(buffer32[2]);

    return sensorsData;
}
void General::SensorsData::serialize(const General::SensorsData& sensorsData, void* buffer)
{
    UINT32* buffer32 = reinterpret_cast<UINT32*>(buffer);

    // I gotta do this because different CPUs handle numbers differently (AKA big/small endian).
    // This make sending numbers portable no matter what device is used.
    buffer32[0] = General::SensorsData::floatToNetwork(sensorsData.temperature);
    buffer32[1] = General::SensorsData::floatToNetwork(sensorsData.moisture);
    buffer32[2] = General::SensorsData::floatToNetwork(sensorsData.soilMoisture);
}

bool General::SensorsData::operator != (const SensorsData& other) const
{
    // Float comparisons are not very accurate so we approximate them.
    const float EPSILON = 0.0001f;
    return (std::fabs(this->temperature - other.temperature)   > EPSILON) or
           (std::fabs(this->moisture - other.moisture)         > EPSILON) or
           (std::fabs(this->soilMoisture - other.soilMoisture) > EPSILON);
}
std::ostream& General::operator << (std::ostream& stream, const SensorsData& sensorsData)
{
    stream << "Temperature: "   << sensorsData.temperature  << std::endl; 
    stream << "Moisture: "      << sensorsData.moisture     << std::endl;
    stream << "Soil Moisture: " << sensorsData.soilMoisture << std::endl;

    return stream;
}

UINT32 General::SensorsData::floatToNetwork(float value)
{
    UINT32 temp;
    std::memcpy(&temp, &value, sizeof(value));
    return htonl(temp);
}
float General::SensorsData::networkToFloat(UINT32 value)
{
    UINT32 temp = ntohl(value);
    float temp2;
    std::memcpy(&temp2, &temp, sizeof(temp2));
    return temp2;
}