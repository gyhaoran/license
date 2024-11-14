#include "service/keep_alive_service.h"
#include "domain/repo/license_repo.h"
#include "infra/log/log.h"
#include <chrono>
#include <thread>
#include <iostream>

namespace lic
{

KeepAliveService::KeepAliveService(int inactive_time) : inactive_time_{inactive_time}, stop_flag_{false}
{
}

KeepAliveService::~KeepAliveService()
{
    stop();
}

void KeepAliveService::evict_inactive_instances() 
{
    while (true) 
    {
        try 
        {
            if (stop_flag_.load()) 
            { 
                return; 
            }

            for (int i = 0; i < inactive_time_; ++i) 
            {
                std::unique_lock<std::mutex> lock(mutex_);
                if (cv_.wait_for(lock, std::chrono::seconds(1), [this]() { return stop_flag_.load(); })) 
                {
                    return;
                }                      
            }

            LicenseRepo::get_instance().remove_inactive_inst(inactive_time_);
        }
        catch (const std::exception& e) 
        {
            LOG_ERROR("Exception in evict_inactive_instances: %s", e.what());
        }
        catch (...) 
        {
            LOG_FATAL("Unknown exception in evict_inactive_instances, errno info: %s", std::strerror(errno));
        }
    }
}

void KeepAliveService::stop()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        stop_flag_ = true;
    }

    cv_.notify_all();
    if (thread_.joinable()) 
    {
        thread_.join();
    }
}

void KeepAliveService::run()
{
    thread_ = std::thread(&KeepAliveService::evict_inactive_instances, this);
}

} // namespace lic
