#include "service/serialization_service.h"
#include "infra/log/log.h"
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <thread>
#include <mutex>
#include <fstream>

namespace lic
{

namespace
{

std::mutex obj_mutex;

void try_to_remove(const std::string& file) 
{
    if (access(file.c_str(), F_OK) == 0) 
    {
        if (remove(file.c_str()) != 0) 
        {
            LOG_ERROR("Error removing file: %s, errno info: %s", file.c_str(), std::strerror(errno));
        }
    } 
    else
    {
        LOG_ERROR("Error removing file: %s, errno info: %s", file.c_str(), std::strerror(errno));
    }
}

} // namespace


SerializationService::SerializationService(const std::string& file, int save_time) 
: save_file_{file}, save_time_{save_time}, stop_flag_{false}
{
}

SerializationService::~SerializationService()
{
    stop();
}

void SerializationService::periodic_save() 
{
    while (true) 
    {
        try 
        {
            if (stop_flag_.load()) 
            { 
                return; 
            }

            for (int i = 0; i < save_time_; ++i) 
            {
                std::unique_lock<std::mutex> lock(mutex_);
                if (cv_.wait_for(lock, std::chrono::seconds(1), [this]() { return stop_flag_.load(); })) 
                {
                    return;
                }                      
            }
            save_to_file(LicenseRepo::get_instance().instances(), save_file_);
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

void SerializationService::stop()
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

void SerializationService::run()
{
    thread_ = std::thread(&SerializationService::periodic_save, this);
}

void save_to_file(const InstanceInfos& infos, const std::string& filename) 
{
    try
    {
        std::lock_guard<std::mutex> lock(obj_mutex);
        std::ofstream ofs(filename, std::ios::binary);
        if (!ofs) { return; }

        boost::archive::binary_oarchive oa(ofs);
        oa << infos;
    }
    catch(const std::exception& e)
    {
        LOG_ERROR("save_to_file error: %s", e.what());
    }
}

bool load_from_file(const std::string& filename, InstanceInfos& infos) 
{
    try
    {
        std::lock_guard<std::mutex> lock(obj_mutex);
        std::ifstream ifs(filename, std::ios::binary);
        if (!ifs) 
        {
            LOG_WARN("load_from_file error");
            return false;
        }
        boost::archive::binary_iarchive ia(ifs);
        ia >> infos;
    }
    catch(const std::exception& e)
    {
        LOG_ERROR("load_from_file exception: %s", e.what());
        return false;
    }
    catch(...)
    {
        LOG_ERROR("load_from_file unexpect exception, errno info: %s", std::strerror(errno));
        return false;
    }

    return true;
}

} // namespace lic
