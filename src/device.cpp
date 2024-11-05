#include "device.h"
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include <cpuid.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <string.h>
#include <unistd.h>
#include "json.hpp"

using json = nlohmann::json;

namespace lic
{

std::string get_cpuid_info() 
{
    unsigned int eax, ebx, ecx, edx;
    __cpuid(1, eax, ebx, ecx, edx);

    std::ostringstream oss;
    oss << std::hex << std::setfill('0')
        << std::setw(8) << edx
        << std::setw(8) << eax;

    return oss.str();
}

int is_virtual_interface(const char *iface_name) 
{
  return (strncmp(iface_name, "lo", 2) == 0 ||
          strncmp(iface_name, "docker", 6) == 0 ||
          strncmp(iface_name, "veth", 4) == 0 ||
          strncmp(iface_name, "virbr", 5) == 0 ||
          strncmp(iface_name, "vmnet", 5) == 0);
}

std::string get_mac_address(const std::string& interface_name) 
{
    struct ifaddrs* ifaddr;
    if (getifaddrs(&ifaddr) == -1) 
    {
        return "";
    }

    std::string mac_address;

    for (struct ifaddrs* addr = ifaddr; addr != nullptr; addr = addr->ifa_next) 
    {
        if (addr->ifa_name == interface_name && addr->ifa_addr->sa_family == AF_PACKET) 
        {
            unsigned char* mac = reinterpret_cast<unsigned char*>(addr->ifa_addr->sa_data);
            std::ostringstream oss;
            oss << std::hex << std::setfill('0');
            for (int i = 0; i < 6; ++i) 
            {
                oss << std::setw(2) << static_cast<int>(mac[i]);
                if (i < 5) oss << ":";
            }
            mac_address = oss.str();
            break;
        }
    }

    freeifaddrs(ifaddr);

    return mac_address.empty() ? "MAC Address not found" : mac_address;
}

std::string computeSha256(const std::string& data)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256Context;
    SHA256_Init(&sha256Context);
    SHA256_Update(&sha256Context, data.c_str(), data.size());
    SHA256_Final(hash, &sha256Context);

    std::stringstream hashStream;
    hashStream << std::hex << std::setfill('0');
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
    {
        hashStream << std::setw(2) << static_cast<int>(hash[i]);
    }

    return hashStream.str();
}

std::string get_device_hash(bool is_server, const std::string& ether_name)
{
  auto cpu_id = get_cpuid_info();
  std::string mac_addr = get_mac_address(ether_name);
  json device_info = {
    {"CPU ID", cpu_id},
    {"MAC Address", mac_addr}
  };

  return computeSha256(device_info.dump());
}
    
} // namespace lic

