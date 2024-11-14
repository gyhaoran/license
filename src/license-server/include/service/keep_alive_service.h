#ifndef C77FCD60_AE3E_4920_929B_69952A72006D
#define C77FCD60_AE3E_4920_929B_69952A72006D

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace lic
{

struct KeepAliveService
{
    explicit KeepAliveService(int inactive_time);
    ~KeepAliveService();

    void run();
    void stop();

private:
    void evict_inactive_instances();

private:
    int inactive_time_;
    std::atomic<bool> stop_flag_;
    std::thread thread_;
    std::condition_variable cv_;
    std::mutex mutex_;
};

} // namespace lic

#endif /* C77FCD60_AE3E_4920_929B_69952A72006D */
