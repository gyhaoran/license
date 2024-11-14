#ifndef B3021A5F_D620_458E_BE9E_3B17161847B9
#define B3021A5F_D620_458E_BE9E_3B17161847B9

#include "infra/log/log_color.h"
#include "infra/utils/symbol.h"
#include <string>
#include <chrono>
#include <iostream>

namespace lic
{

struct ScopeTimer 
{
    ScopeTimer(const std::string& name) : name_(name), start_(std::chrono::high_resolution_clock::now()) 
    {}

    ~ScopeTimer() 
    {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start_;
        std::cout << to_color_fmt(LogColor::GREEN) << name_ << " took " << duration.count() << " ms" << std::endl;
    }

private:
    std::string name_;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};

} // namespace lic

#define MEASURE_TIME() ::lic::ScopeTimer UNIQUE_NAME(timer_){__func__}

#endif /* B3021A5F_D620_458E_BE9E_3B17161847B9 */
