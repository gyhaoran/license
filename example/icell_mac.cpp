#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <cpuid.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <stdlib.h>

void get_username() {
  char *username = getenv("USER");
  if (username != NULL) {
    printf("Username: %s\n", username);
  } else {
    printf("Failed to get the username.\n");
  }
}

void get_cpuid_info() {
  unsigned int eax, ebx, ecx, edx;
  __cpuid(1, eax, ebx, ecx, edx);

  printf("CPU ID: %08X%08X\n", edx, eax);
}

int is_virtual_interface(const char *iface_name) {
  return (strncmp(iface_name, "lo", 2) == 0 ||
          strncmp(iface_name, "docker", 6) == 0 ||
          strncmp(iface_name, "veth", 4) == 0 ||
          strncmp(iface_name, "virbr", 5) == 0 ||
          strncmp(iface_name, "vmnet", 5) == 0);
}

void get_all_mac_addresses() {
  struct ifaddrs *ifaddr, *ifa;
  int fd;
  struct ifreq ifr;

  if (getifaddrs(&ifaddr) == -1) {
    perror("getifaddrs");
    return;
  }

  fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd == -1) {
    perror("Socket creation failed");
    freeifaddrs(ifaddr);
    return;
  }

  for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr == NULL || ifa->ifa_addr->sa_family != AF_PACKET) {
      continue;  
    }

    if (is_virtual_interface(ifa->ifa_name)) continue;

    strncpy(ifr.ifr_name, ifa->ifa_name, IFNAMSIZ - 1);
    if (ioctl(fd, SIOCGIFHWADDR, &ifr) == 0) {
      unsigned char *mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
      printf("MAC Address of %s: %02x:%02x:%02x:%02x:%02x:%02x\n",
             ifa->ifa_name, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    } else {
      perror("Failed to get MAC address");
    }
  }

  close(fd);
  freeifaddrs(ifaddr);
}

int main() {
  // 获取当前用户名
  get_username();

  // 获取 EAX 和 EDX 的值
  get_cpuid_info();

  // 获取本机所有非虚拟接口的 MAC 地址
  get_all_mac_addresses();

  return 0;
}
