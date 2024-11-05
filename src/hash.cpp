#include "hash.h"
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

namespace lic
{

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
    
} // namespace lic
