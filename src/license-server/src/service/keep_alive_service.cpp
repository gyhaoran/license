#include  "service/keep_alive_service.h"
#include "domain/repo/license_repo.h"
#include "infra/log/log.h"
#include <chrono>
#include <thread>
#include <iostream>

namespace lic
{

namespace
{

void evict_inactive_instances(int timeout_seconds, std::chrono::seconds interval) 
{
    while (true) 
    {
        try 
        {
            LOG_INFO("Before clear inactive inst:");
            LicenseRepo::get_instance().dump();
            
            std::this_thread::sleep_for(interval);            
            LicenseRepo::get_instance().remove_inactive_inst(timeout_seconds);

            LOG_INFO("After clear inactive inst:");
            LicenseRepo::get_instance().dump();
        }
        catch (const std::exception& e) 
        {
            LOG_FATAL("Exception in evict_inactive_instances: %s", e.what());
        }
        catch (...) 
        {
            LOG_FATAL("Unknown exception in evict_inactive_instances.");
        }
    }
}

} // namespace

KeepAliveService::KeepAliveService(int timeout_seconds) : timeout_seconds_{timeout_seconds}
{
}

void KeepAliveService::run()
{
    // evict_inactive_instances(timeout_seconds_, std::chrono::seconds(60));
    std::thread save_thread(evict_inactive_instances, timeout_seconds_, std::chrono::seconds(60));
    save_thread.detach();
}

} // namespace lic
