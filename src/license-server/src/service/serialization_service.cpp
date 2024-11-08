#include "service/serialization_service.h"
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <thread>
#include <mutex>
#include <fstream>

namespace lic
{

std::mutex obj_mutex;

void periodic_save(const std::string& filename, std::chrono::seconds interval) 
{
    while (true) 
    {
        std::this_thread::sleep_for(interval);
        save_to_file(LicenseRepo::get_instance().devices(), filename);
    }
}

SerializationService::SerializationService(const std::string& file) : save_file_{file}
{
}

void SerializationService::run()
{
    std::thread save_thread(periodic_save, save_file_, std::chrono::seconds(60));
    save_thread.detach();
}

void save_to_file(const DeviceInfos& devices, const std::string& filename) 
{
    std::lock_guard<std::mutex> lock(obj_mutex);
    std::ofstream ofs(filename, std::ios::binary);    
    if (!ofs) { return; }

    boost::archive::binary_oarchive oa(ofs);
    oa << devices;
}

bool load_from_file(const std::string& filename, DeviceInfos& devices) 
{
    std::map<DeviceId, DeviceInfo> d;
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) 
    {
        return false;
    }
    boost::archive::binary_iarchive ia(ifs);
    ia >> d;

    return true;
}

} // namespace lic
