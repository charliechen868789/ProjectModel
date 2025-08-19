#include "VirtualSensor.h"
#include <memory>

int main()
{
    try
    {
        auto sensorApp = std::make_unique<sensor::VirtualSensor>();
        sensorApp->start();
    } 
    catch (const std::exception& e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}