#include "device.h"
#include "hash.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <json.hpp>
#include <random>
#include <sstream>

using json = nlohmann::json;
using namespace lic;

std::string bytes_to_hex(const std::vector<uint8_t>& bytes) 
{
    std::stringstream ss;
    for (uint8_t byte : bytes) 
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    return ss.str();
}

std::string generate_random_id(int length) 
{
    std::random_device rd;
    std::uniform_int_distribution<uint8_t> dist(0, 255);

    std::vector<uint8_t> random_bytes(length);
    for (int i = 0; i < length; ++i) {
        random_bytes[i] = dist(rd);
    }

    return bytes_to_hex(random_bytes);
}

std::string generate_aes_key(size_t length) 
{
    std::vector<unsigned char> key(length);
    RAND_bytes(key.data(), length);
    return std::string(reinterpret_cast<char*>(key.data()), length);
}

std::string sign_data(RSA* private_key, const std::string& data) 
{
    auto hash = computeSha256(data);    
    unsigned char signature[RSA_size(private_key)];
    unsigned int signature_length;
    RSA_sign(NID_sha256, reinterpret_cast<unsigned char*>(hash.data()), SHA256_DIGEST_LENGTH, signature, &signature_length, private_key);

    return std::string(reinterpret_cast<char*>(signature), signature_length);
}

std::string generate_public_key(RSA* private_key) 
{
    RSA* rsa_public_key = RSAPublicKey_dup(private_key);
    if (!rsa_public_key) 
    {
        std::cerr << "Failed to generate public key from private key!" << std::endl;
        return "";
    }

    unsigned char* public_key_der = nullptr;
    int public_key_der_len = i2d_RSA_PUBKEY(rsa_public_key, &public_key_der);
    if (public_key_der_len <= 0) 
    {
        std::cerr << "Failed to encode public key to DER format!" << std::endl;
        RSA_free(rsa_public_key);
        return "";
    }

    std::string public_key_der_str(reinterpret_cast<char*>(public_key_der), public_key_der_len);

    return public_key_der_str;
}

void generate_license(const std::string& private_key_path, const std::string& license_filename)
{
    FILE* private_key_file = fopen(private_key_path.c_str(), "rb");
    if (!private_key_file)
    {
        std::cerr << "Unable to open private key file." << std::endl;
        return;
    }

    RSA* private_key = PEM_read_RSAPrivateKey(private_key_file, nullptr, nullptr, nullptr);
    fclose(private_key_file);
    if (!private_key) {
        std::cerr << "Unable to read private key." << std::endl;
        return;
    }

    std::string lic_id = generate_random_id(16);
    std::string aes_key = generate_aes_key(32);
    std::string random_data1 = generate_aes_key(8);
    std::string random_data2 = generate_aes_key(8);
    std::string confused_aes_key = random_data1 + aes_key + random_data2;
    std::string device_hash = get_device_hash(true, "ens33");
    auto device_hash_2 = gen_device_hash("1F8BFBFF00090675", "00:0c:29:e2:43:83");

    json license_info = {
        {"id", lic_id},
        {"is_server", true},
        {"issue_date", "20240101 000000"},
        {"expire_date", "20250101 000000"},
        {"devices", {
            {device_hash, 1},
            {device_hash_2, 2},
        }}
    };

    std::string plaintext = license_info.dump();
    std::cout << plaintext << '\n';

    std::string encrypted_data = encrypt_info(plaintext, aes_key);
    std::string signature = sign_data(private_key, plaintext);

    auto public_key = generate_public_key(private_key);
    uint32_t public_key_length = public_key.size();
    uint32_t encrypted_data_length = encrypted_data.size();
    uint32_t signature_length = signature.size();
    uint32_t confused_aes_key_length = confused_aes_key.size();

    std::vector<char> header(32, 0);
    memcpy(header.data(), &public_key_length, sizeof(public_key_length));
    memcpy(header.data() + 4, &encrypted_data_length, sizeof(encrypted_data_length));
    memcpy(header.data() + 8, &signature_length, sizeof(signature_length));
    memcpy(header.data() + 12, &confused_aes_key_length, sizeof(confused_aes_key_length));


    std::ofstream license_file(license_filename, std::ios::binary);
    if (!license_file) 
    {
        std::cerr << "Unable to open license file for writing." << std::endl;
        return;
    }

    license_file.write(header.data(), header.size());
    license_file.write(public_key.data(), public_key_length);
    license_file.write(encrypted_data.data(), encrypted_data.size());
    license_file.write(signature.data(), signature.size());
    license_file.write(confused_aes_key.data(), confused_aes_key.size());
    
    license_file.close();

    RSA_free(private_key);
    std::cout << "License file generated successfully: " << license_filename << std::endl;
}

int main(int argc, char** argv) 
{
    const std::string private_key_path = "./private_key.pem";
    const std::string license_filename = "license.dat";

    generate_license(private_key_path, license_filename);
    return 0;
}

