#ifndef C4DA2CDB_A193_488C_A495_1F75DC08A60E
#define C4DA2CDB_A193_488C_A495_1F75DC08A60E

namespace hv 
{

class HttpService; 
class HttpServer;

}

namespace lic
{

struct LicenseServer
{
    LicenseServer(int port=8442);
    ~LicenseServer();

    void run(bool wait=true);
    void stop();

private:
    int port_;
    hv::HttpService* service_{nullptr};
    hv::HttpServer* server_{nullptr};
};


} // namespace lic


#endif /* C4DA2CDB_A193_488C_A495_1F75DC08A60E */
