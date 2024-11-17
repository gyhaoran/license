#include "infra/utils/uuid.h"
#include <chrono>
#include <random>
#include <sstream>
#include <iomanip>

namespace lic
{

std::string uuid_1()
{
    // Get current time in 100-nanosecond intervals since UUID epoch (1582-10-15)
    const uint64_t UUID_EPOCH = 0x01B21DD213814000; // Difference between UUID and Unix epoch in 100-ns units
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    uint64_t time_100ns = (std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() / 100) + UUID_EPOCH;

    // Extract components of the timestamp
    uint16_t time_low = static_cast<uint16_t>(time_100ns & 0xFFFF);
    uint16_t time_mid = static_cast<uint16_t>((time_100ns >> 16) & 0xFFFF);
    uint16_t time_hi_and_version = static_cast<uint16_t>((time_100ns >> 32) & 0x0FFF) | (1 << 12); // Version 1

    // Generate a random clock sequence (14 bits)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 0x3FFF);
    uint16_t clock_seq = static_cast<uint16_t>(dis(gen)) | 0x8000; // Set the variant (10xxxxxx)

    std::uniform_int_distribution<uint64_t> dis_node(0, 0xFFFFFFFFFFFF);
    uint64_t node = dis_node(gen);

    // Format UUID v1 as a string
    std::ostringstream oss;
    oss << std::hex << std::setfill('0')
        << std::setw(8) << time_low << '-'
        << std::setw(4) << time_mid << '-'
        << std::setw(4) << time_hi_and_version << '-'
        << std::setw(4) << clock_seq << '-'
        << std::setw(12) << node;

    return oss.str();
}

} // namespace lic
