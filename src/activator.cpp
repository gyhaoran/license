#include "activator.h"
#include "device.h"
#include "license_parser.h"
#include "license_info.h"

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
    if (issue_stream.fail()) {
        std::cerr << "Failed to parse issue_date." << std::endl;
        return false;
    }

    expiration_stream >> std::get_time(&expiration_tm, "%Y%m%d %H%M%S");
    if (expiration_stream.fail()) {
        std::cerr << "Failed to parse expire_date." << std::endl;
        return false;
    }

    auto issue_time = std::mktime(&issue_tm);
    auto expiration_time = std::mktime(&expiration_tm);

    return (issue_time <= now_time_t) && (now_time_t <= expiration_time);
}

// 计算SHA256哈希
std::string sha256(const std::string& data) 
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), hash);
    return std::string(reinterpret_cast<char*>(hash), SHA256_DIGEST_LENGTH);
}

bool verify_signature(const std::string& signature, const std::string& data, RSA* rsa_pub_key) 
{
    std::string hash = sha256(data);

    unsigned char* decoded_signature = reinterpret_cast<unsigned char*>(const_cast<char*>(signature.c_str()));
    int signature_length = signature.size();

    int result = RSA_verify(NID_sha256, reinterpret_cast<const unsigned char*>(hash.c_str()), SHA256_DIGEST_LENGTH,
                            decoded_signature, signature_length, rsa_pub_key);

    if (result != 1) {
        std::cerr << "Signature verification failed: " << ERR_error_string(ERR_get_error(), nullptr) << std::endl;
        return false;
    }
    return true;
}

std::string decrypt_aes(const std::string& encrypted_data, const std::string& confused_aes_key) 
{
    if (confused_aes_key.size() < 32) 
    {
        std::cerr << "Confused AES key must be at least 32 bytes." << std::endl;
        return "";
    }

    std::string aes_key = confused_aes_key.substr(8, 32);

    AES_KEY decrypt_key;
    AES_set_decrypt_key(reinterpret_cast<const unsigned char*>(aes_key.data()), 256, &decrypt_key);

    size_t decrypted_size = (encrypted_data.size() / AES_BLOCK_SIZE) * AES_BLOCK_SIZE;
    std::vector<unsigned char> decrypted_data(decrypted_size);

    for (size_t i = 0; i < decrypted_size; i += AES_BLOCK_SIZE) 
    {
        AES_decrypt(reinterpret_cast<const unsigned char*>(encrypted_data.data()) + i,
                     decrypted_data.data() + i,
                     &decrypt_key);
    }

    return std::string(reinterpret_cast<char*>(decrypted_data.data()), decrypted_size);
}

bool validate_license(const LicenseHeader& header, const LicenseData& data)
{
    BIO* bio = BIO_new_mem_buf(data.publicKey.data(), header.publicKeyLength);
    RSA* rsa_pub_key = PEM_read_bio_RSA_PUBKEY(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);
    if (!rsa_pub_key) {
        std::cerr << "Failed to read public key." << std::endl;
        return false;
    }

    // 2. 验证签名
    std::string license_info_json = decrypt_aes(data.encryptedData, data.confusedAesKey);
    if (!verify_signature(data.signature, license_info_json, rsa_pub_key)) {
        std::cerr << "Signature verification failed." << std::endl;
        RSA_free(rsa_pub_key);
        return false;
    }

    // 3. 解析license_info
    auto license_info = parse_license_info(license_info_json);
    bool is_server = license_info.is_server;
    std::string ether_name = license_info.ether_name;
    std::string device_hash = license_info.device_hash;

    // 4. 计算设备hash并比对
    std::string actual_hash = get_device_hash(is_server, ether_name);
    if (actual_hash != device_hash) {
        std::cerr << "Device hash mismatch." << std::endl;
        RSA_free(rsa_pub_key);
        return false;
    }

    // 5. 检查许可证是否到期
    std::string issue_date = license_info.issue_date;
    std::string expire_date = license_info.expire_date;
    if (is_license_valid(issue_date, expire_date)) {
        std::cout << "License expired." << std::endl;
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
