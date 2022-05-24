#pragma once

#include <iostream>
using namespace std;

class Data
{ 
    public:
        Data();

        float temperature = 0.f;
        float humidity = 0.f;
        float pressure = 0.f;
        // ...

        friend ostream& operator<<(ostream& os, const Data & data);
        friend istream& operator>>(istream& is, Data & data);
};