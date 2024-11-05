#include "activator.h"
#include "device.h"
#include "hash.h"
#include "license_parser.h"
#include "license_info.h"
#include "json.hpp"

#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

using json = nlohmann::json;

namespace lic
{

bool is_license_valid(const std::string& issue_date, const std::string& expire_date) 
{
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::localtime(&now_time_t);

    std::istringstream issue_stream(issue_date);
    std::istringstream expiration_stream(expire_date);
    std::tm issue_tm = {};
    std::tm expiration_tm = {};

    issue_stream >> std::get_time(&issue_tm, "%Y%m%d %H%M%S");
    if (issue_stream.fail()) 
    {
        return false;
    }

    expiration_stream >> std::get_time(&expiration_tm, "%Y%m%d %H%M%S");
    if (expiration_stream.fail()) 
    {
        return false;
    }

    auto issue_time = std::mktime(&issue_tm);
    auto expiration_time = std::mktime(&expiration_tm);

    return (issue_time <= now_time_t) && (now_time_t <= expiration_time);
}

bool verify_signature(const std::string& signature, const std::string& data, RSA* rsa_pub_key) 
{
    auto hash = computeSha256(data);

    unsigned char* decoded_signature = reinterpret_cast<unsigned char*>(const_cast<char*>(signature.c_str()));
    int signature_length = signature.size();
    
    int result = RSA_verify(NID_sha256, reinterpret_cast<unsigned char*>(hash.data()), SHA256_DIGEST_LENGTH, decoded_signature, signature_length, rsa_pub_key);
    if (result != 1) 
    {
        std::cerr << "Signature verification failed: " << ERR_error_string(ERR_get_error(), nullptr) << std::endl;
        return false;
    }
    return true;
}

std::string decrypt_aes(const std::string& encrypted_data, const std::string& confused_aes_key) 
{
    if (confused_aes_key.size() < 32) 
    {
        return "";
    }

    std::string aes_key = confused_aes_key.substr(8, 32);

    AES_KEY decrypt_key;
    AES_set_decrypt_key(reinterpret_cast<const unsigned char*>(aes_key.data()), 256, &decrypt_key);

    size_t decrypted_size = (encrypted_data.size() / AES_BLOCK_SIZE) * AES_BLOCK_SIZE;
    std::vector<unsigned char> decrypted_data(decrypted_size);

    for (size_t i = 0; i < decrypted_size; i += AES_BLOCK_SIZE) 
    {
        AES_decrypt(reinterpret_cast<const unsigned char*>(encrypted_data.data()) + i, decrypted_data.data() + i, &decrypt_key);
    }

    return std::string(reinterpret_cast<char*>(decrypted_data.data()), decrypted_size);
}

bool validate_license(const LicenseHeader& header, const LicenseData& data)
{
    const unsigned char* pub_key_data = reinterpret_cast<const unsigned char*>(data.publicKey.data());

    RSA* rsa_pub_key = d2i_RSA_PUBKEY(nullptr, &pub_key_data, data.publicKey.size());
    if (!rsa_pub_key) 
    {
        return false;
    }

    std::string license_info_json = decrypt_aes(data.encryptedData, data.confusedAesKey);
    std::string license_data = json::parse(license_info_json.c_str()).dump();
    if (!verify_signature(data.signature, license_data, rsa_pub_key))
    {
        RSA_free(rsa_pub_key);
        return false;
    }

    auto license_info = parse_license_info(license_data);
    bool is_server = license_info.is_server;
    std::string ether_name = license_info.ether_name;
    std::string device_hash = license_info.device_hash;

    std::string actual_hash = get_device_hash(is_server, ether_name);
    if (actual_hash != device_hash) 
    {
        std::cerr << "Device is not authorzed." << std::endl;
        RSA_free(rsa_pub_key);
        return false;
    }

    std::string issue_date = license_info.issue_date;
    std::string expire_date = license_info.expire_date;
    if (!is_license_valid(issue_date, expire_date)) 
    {
        std::cerr << "License has expired." << std::endl;
        RSA_free(rsa_pub_key);
        return false;
    }

    RSA_free(rsa_pub_key);
    return true;
}

bool verify(const std::string& license_file)
{
    LicenseHeader header;
    LicenseData data;
    if (!parse_license_file(license_file, header, data))
    {
        return false;
    }

    return validate_license(header, data);
}

} // namespace lic
