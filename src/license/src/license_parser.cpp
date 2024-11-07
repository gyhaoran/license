#include "license_parser.h"
#include <iostream>
#include <fstream>

namespace lic
{

bool parse_license_file(const std::string& filename, LicenseHeader& header, LicenseData& data) 
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) 
    {
        std::cerr << "Cannot open license file." << std::endl;
        return false;
    }

    file.read(reinterpret_cast<char*>(&header), sizeof(LicenseHeader));

    data.publicKey.resize(header.publicKeyLength);
    file.read(data.publicKey.data(), header.publicKeyLength);

    data.encryptedData.resize(header.encryptedDataLength);
    file.read(data.encryptedData.data(), header.encryptedDataLength);

    data.signature.resize(header.signatureLength);
    file.read(data.signature.data(), header.signatureLength);

    data.confusedAesKey.resize(header.confusedAesKeyLength);
    file.read(data.confusedAesKey.data(), header.confusedAesKeyLength);

    file.close();
    return true;
}


} // namespace lic

