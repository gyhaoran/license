#ifndef C77FCD60_AE3E_4920_929B_69952A72006D
#define C77FCD60_AE3E_4920_929B_69952A72006D

namespace lic
{

struct KeepAliveService
{
    explicit KeepAliveService(int timeout_seconds);
    
    void run();
private:
    int timeout_seconds_;
};

} // namespace lic

#endif /* C77FCD60_AE3E_4920_929B_69952A72006D */
