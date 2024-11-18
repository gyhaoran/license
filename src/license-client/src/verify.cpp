#include "verify.h"
#include "encoder.h"
#include "json.hpp"
#include <hv/requests.h>
#include <hv/HttpClient.h>
#include <cpuid.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <linux/if.h>
#include <unistd.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

using InstanceId = std::string;
using json = nlohmann::json;

namespace lic
{
namespace
{

std::string get_cpuid_info() 
{
    unsigned int eax, ebx, ecx, edx;
    __cpuid(1, eax, ebx, ecx, edx);

    std::ostringstream oss;
    oss << std::hex << std::setfill('0') << std::uppercase
        << std::setw(8) << edx
        << std::setw(8) << eax;

    return oss.str();
}

bool is_virtual_interface(const char *iface_name) 
{
    return (strncmp(iface_name, "lo", 2) == 0 ||
            strncmp(iface_name, "docker", 6) == 0 ||
            strncmp(iface_name, "veth", 4) == 0 ||
            strncmp(iface_name, "virbr", 5) == 0 ||
            strncmp(iface_name, "vmnet", 5) == 0);
}

// Get all MAC addresses and return as a vector of strings
std::vector<std::string> get_all_mac_addresses() 
{
    struct ifaddrs *ifaddr, *ifa;
    int fd;
    struct ifreq ifr;
    std::vector<std::string> mac_addresses;

    if (getifaddrs(&ifaddr) == -1)
    {
        return mac_addresses;
    }

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1)
    {
        freeifaddrs(ifaddr);
        return mac_addresses;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) 
    {
        if (ifa->ifa_addr == NULL || ifa->ifa_addr->sa_family != AF_PACKET) 
        {
            continue;  
        }
        if (is_virtual_interface(ifa->ifa_name)) 
        {
            continue;
        }

        snprintf(ifr.ifr_name, IFNAMSIZ, "%s", ifa->ifa_name);
        if (ioctl(fd, SIOCGIFHWADDR, &ifr) == 0) 
        {
            unsigned char *mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
            char mac_str[18];
            snprintf(mac_str, sizeof(mac_str), "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            mac_addresses.emplace_back(mac_str);
        }
    }

    close(fd);
    freeifaddrs(ifaddr);
    return mac_addresses;
}

struct LicenseValidator
{

    LicenseValidator(const std::string& ip, int port, int period) : ip_{ip}, port_{port}, period_{period}
    {}

    ~LicenseValidator()
    {
        std::cout << "LicenseValidator destory\n";
        stop();
    }

    void stop()
    {
        if (!inst_id_.empty())
        {
            stop_heart_beat_msg();
            send_instance_rel_http_msg();
            inst_id_ = "";
        }
    }

    void run()
    {
        thread_ = std::thread(&LicenseValidator::start_heart_beat_msg, this);
    }

    bool send_auth_req_http_msg(const std::string& cpuid, const std::vector<std::string>& mac_addrs)
    {
        json req = {
            {"cpuid", cpuid},
            {"mac", mac_addrs},
        };

        auto rsp = send_auth_req_http_msg(req);
        if (!rsp)
        {
            std::cerr << "License validate failure: may be license server network can not reach\n";
            return false;
        }

        return handle_auth_rsp(rsp);
    }

private:
    bool handle_success_rsp(const json& msg)
    {
        if (!msg.contains("uuid"))
        {
            std::cerr << "License validate failure: rcv error msg from license server\n";
            return false;
        }
        std::cout << "License validate success\n";

        inst_id_ = msg["uuid"].get<std::string>();
        return true;
    }

    bool handle_failed_rsp(const json& msg)
    {
        if (!msg.contains("message") && !msg["message"].is_string())
        {
            std::cerr << "License validate failure: rcv error msg from license server\n";
        }
        else
        {
            std::cerr << "License validate failure: " << msg["message"].get<std::string>() << '\n';
        }
        return false;
    }

    bool handle_auth_rsp(const requests::Response& rsp)
    {
        try
        {
            auto msg = json::parse(decrypt_info(rsp->body));
            if (msg.is_discarded())
            {
                std::cerr << "License validate failure: rcv error msg from license server\n";
                return false;
            }
            if (!msg.contains("status"))
            {
                std::cerr << "License validate failure: rcv error msg from license server\n";
                return false;
            }

            if (msg["status"] == "success")
            {
                return handle_success_rsp(msg);
            }
            else
            {
                return handle_failed_rsp(msg);
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    requests::Response send_auth_req_http_msg(const json& req)
    {
        auto msg = encrypt_info(req.dump());
        http_headers headers;
        headers["Content-Type"] = "application/octet-stream";
        auto auth_url = std::string("http://") + ip_ + ":" + std::to_string(port_) + "/auth/license";
        auto rsp = requests::post(auth_url.c_str(), msg, headers);
        return rsp;
    }

    requests::Response send_instance_rel_http_msg()
    {
        json inst_rel_msg = {
            {"uuid", inst_id_}
        };

        auto msg = encrypt_info(inst_rel_msg.dump());
        http_headers headers;
        headers["Content-Type"] = "application/octet-stream";
        auto rel_url = std::string("http://") + ip_ + ":" + std::to_string(port_) + "/inst/rel";
        auto rsp = requests::post(rel_url.c_str(), msg, headers);

        if (rsp && rsp->status_code == HTTP_STATUS_OK)
        {
            std::cout << "send_instance_rel_http_msg success\n";
        }

        return rsp;
    }

    requests::Response send_inst_echo_http_msg()
    {
        json inst_echo_msg = {
            {"uuid", inst_id_}
        };

        auto msg = encrypt_info(inst_echo_msg.dump());
        http_headers headers;
        headers["Content-Type"] = "application/octet-stream";
        auto echo_url = std::string("http://") + ip_ + ":" + std::to_string(port_) + "/inst/echo";
        auto rsp = requests::post(echo_url.c_str(), msg, headers);

        return rsp;
    }

    void start_heart_beat_msg()
    {
        while (true) 
        {
            try 
            {
                if (stop_flag_.load()) 
                { 
                    return; 
                }
                for (int i = 0; i < period_; ++i) 
                {
                    std::unique_lock<std::mutex> lock(mutex_);
                    if (cv_.wait_for(lock, std::chrono::seconds(1), [this]() { return stop_flag_.load(); })) 
                    {
                        return;
                    }
                }
                
                std::cout << "client send heart_beat_msg, period: " << period_ << "s\n";
                send_inst_echo_http_msg();
            }
            catch (const std::exception& e) 
            {
                return;
            }
        }
    }

    void stop_heart_beat_msg()
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

private:
    std::string ip_{"localhost"};
    int port_{0};
    int period_{30};
    InstanceId inst_id_{""};

    std::atomic<bool> stop_flag_{false};
    std::thread thread_;
    std::condition_variable cv_;
    std::mutex mutex_;
};

LicenseValidator* validate_{nullptr};

} // namespace

bool verify(const std::string& ip, int port, int period)
{
    if (!validate_) 
    {
        validate_ = new LicenseValidator(ip, port, period);
    }

    auto cpu_id = get_cpuid_info();
    auto mac_addrs = get_all_mac_addresses();
    auto result = validate_->send_auth_req_http_msg(cpu_id, mac_addrs);

    if (result)
    {
        validate_->run();
    }
    
    return result;
}

void cleanup()
{
    delete validate_;
    validate_ = nullptr;
}

} // namespace lic
