#ifndef C4DA2CDB_A193_488C_A495_1F75DC08A60E
#define C4DA2CDB_A193_488C_A495_1F75DC08A60E

namespace hv { class HttpService; }

namespace lic
{

struct LicenseServer
{
    LicenseServer(int port=8442);
    ~LicenseServer();

    void run();

private:
    int port_;
    hv::HttpService* service_{nullptr};
};


} // namespace lic


#endif /* C4DA2CDB_A193_488C_A495_1F75DC08A60E */
