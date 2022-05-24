#pragma once

#include <QDataStream>

class Data
{ 
    public:
        Data();

        float temperature = 0.f;
        float humidity = 0.f;
        float pressure = 0.f;
        // ...

        friend QDataStream& operator<<(QDataStream& os, const Data& data)
        {
            os << data.temperature << data.humidity << data.pressure;
            return os;
        }

        friend QDataStream& operator>>(QDataStream& is, Data& data)
        {
            is >> data.pressure >> data.humidity >> data.temperature;
            return is;
        }
};