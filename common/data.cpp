#include "data.h"

ostream& operator<<(ostream& os, const Data& data)
{
    os << data.temperature << data.humidity << data.pressure;
    return os;
}

istream& operator>> (istream& is, Data& data)
{
    is >> data.pressure >> data.humidity >> data.temperature;
    return is;
}