#ifndef C044B3E2_719B_4AE9_9F90_8DB6E8D5D007
#define C044B3E2_719B_4AE9_9F90_8DB6E8D5D007

#include "domain/repo/license_repo.h"
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace lic
{

struct SerializationService
{
    explicit SerializationService(const std::string& file, int save_time=300);
    
    ~SerializationService();

    void run();
    void stop();

private:
    void periodic_save();

private:
    std::string save_file_;
    int save_time_;
    std::atomic<bool> stop_flag_;
    std::thread thread_;
    std::condition_variable cv_;
    std::mutex mutex_;
};

void save_to_file(const InstanceInfos&, const std::string&);
bool load_from_file(const std::string&, InstanceInfos&);

} // namespace lic

#endif /* C044B3E2_719B_4AE9_9F90_8DB6E8D5D007 */
