#include "device.h"
#include "hash.h"
#include <cpuid.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <string.h>
#include <unistd.h>
#include "json.hpp"
#include <iostream>

using json = nlohmann::json;

namespace lic
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
    struct ifaddrs *ifaddr, *ifa;
    int fd;
    struct ifreq ifr;

    if (getifaddrs(&ifaddr) == -1) 
    {
        return "";
    }

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) 
    {
        freeifaddrs(ifaddr);
        return "";
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) 
    {
        if (ifa->ifa_addr == NULL || ifa->ifa_addr->sa_family != AF_PACKET) { continue; }

        if (strcmp(ifa->ifa_name, interface_name.c_str()) != 0) { continue; }

        strncpy(ifr.ifr_name, ifa->ifa_name, IFNAMSIZ - 1);
        if (ioctl(fd, SIOCGIFHWADDR, &ifr) == 0) 
        {
            unsigned char *mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
            std::ostringstream oss;
            oss << std::hex << std::setfill('0')
                << std::setw(2) << (int)mac[0] << ":"
                << std::setw(2) << (int)mac[1] << ":"
                << std::setw(2) << (int)mac[2] << ":"
                << std::setw(2) << (int)mac[3] << ":"
                << std::setw(2) << (int)mac[4] << ":"
                << std::setw(2) << (int)mac[5];

            close(fd);
            freeifaddrs(ifaddr);
            return oss.str();
        } 
        else 
        {
            close(fd);
            freeifaddrs(ifaddr);
            return "";
        }
    }

    close(fd);
    freeifaddrs(ifaddr);
    return "";
}

std::string gen_device_hash(const std::string& cpu_id, const std::string& mac_addr)
{
    json device_info = {    
        {"CPU ID", cpu_id},    
        {"MAC Address", mac_addr}  
    };
    return computeSha256(device_info.dump());
}

std::string get_device_hash(bool is_server, const std::string& ether_name)
{
    auto cpu_id = get_cpuid_info();
    std::string mac_addr = get_mac_address(ether_name);

    return gen_device_hash(cpu_id, mac_addr);
}
    
} // namespace lic
