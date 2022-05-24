#pragma once

#include <QDataStream>

class Data
{ 
    public:
        Data();

        int luminosity = 0;
        int red = 0;
        int blue = 0;
        int green = 0;
        // ...

        friend QDataStream& operator<<(QDataStream& os, const Data& data)
        {
            os << data.luminosity << data.red << data.blue << data.green;
            return os;
        }

        friend QDataStream& operator>>(QDataStream& is, Data& data)
        {
            is >> data.luminosity >> data.red >> data.blue >> data.green;
            return is;
        }
};