#ifndef A8193567_5A4E_4D96_9846_0201A76668C6
#define A8193567_5A4E_4D96_9846_0201A76668C6

#include <vector>
#include <string>
#include <cstdint>

namespace lic
{

struct LicenseHeader 
{
    uint32_t publicKeyLength;
    uint32_t encryptedDataLength;
    uint32_t signatureLength;
    uint32_t confusedAesKeyLength;
    char reserved[16]; // rsv
};

struct LicenseData 
{
    std::string publicKey;        
    std::string encryptedData;
    std::string signature;    
    std::string confusedAesKey;   
};

bool parse_license_file(const std::string& filename, LicenseHeader& header, LicenseData& data);
std::string get_license_info(const std::string& filename);

} // namespace lic

#endif /* A8193567_5A4E_4D96_9846_0201A76668C6 */
