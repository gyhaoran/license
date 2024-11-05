#include "utils.h"
#include <iostream>
#include <cstring>
#include <iomanip>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/err.h>

namespace lic
{

// PKCS7 padding and unpadding functions
std::string pkcs7_pad(const std::string& data, int block_size) 
{
    int padding_length = block_size - (data.size() % block_size);
    return data + std::string(padding_length, static_cast<char>(padding_length));
}

std::string pkcs7_unpad(const std::string& data, int block_size) 
{
    int padding_length = static_cast<int>(data.back());
    if (padding_length > block_size || padding_length == 0) 
    {
        return "";
    }
    for (int i = 1; i < padding_length; ++i) 
    {
        if (data[data.size() - i - 1] != padding_length) 
        {
            return "Invalid padding";
        }
    }
    return data.substr(0, data.size() - padding_length);
}

// AES encryption function
std::string encrypt_info(const std::string& data, const std::string& key) 
{
    unsigned char iv[AES_BLOCK_SIZE];
    if (!RAND_bytes(iv, AES_BLOCK_SIZE)) 
    {
        return "Failed to generate IV";
    }

    std::string padded_data = pkcs7_pad(data, AES_BLOCK_SIZE);
    std::string ciphertext(padded_data.size() + AES_BLOCK_SIZE, 0);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, reinterpret_cast<const unsigned char*>(key.c_str()), iv)) 
    {
        throw std::runtime_error("EVP_EncryptInit_ex failed");
    }

    int len;
    if (!EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(ciphertext.data()), &len, reinterpret_cast<const unsigned char*>(padded_data.c_str()), padded_data.size())) 
    {
        throw std::runtime_error("EVP_EncryptUpdate failed");
    }
    int ciphertext_len = len;

    if (!EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(ciphertext.data() + len), &len)) 
    {
        throw std::runtime_error("EVP_EncryptFinal_ex failed");
    }
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    std::string result;
    result.append(reinterpret_cast<const char*>(iv), AES_BLOCK_SIZE);
    result.append(ciphertext.begin(), ciphertext.begin() + ciphertext_len);

    return result;
}

// AES decryption function
std::string decrypt_info(const std::string& data, const std::string& key) 
{
    if (data.size() < AES_BLOCK_SIZE) 
    {
        throw std::runtime_error("Data too short");
    }

    unsigned char iv[AES_BLOCK_SIZE];
    std::memcpy(iv, data.c_str(), AES_BLOCK_SIZE);

    std::string ciphertext = data.substr(AES_BLOCK_SIZE);
    std::string plaintext(ciphertext.size(), 0);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, reinterpret_cast<const unsigned char*>(key.c_str()), iv)) 
    {
        throw std::runtime_error("EVP_DecryptInit_ex failed");
    }

    int len;
    if (!EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(plaintext.data()), &len, reinterpret_cast<const unsigned char*>(ciphertext.c_str()), ciphertext.size())) 
    {
        throw std::runtime_error("EVP_DecryptUpdate failed");
    }
    int plaintext_len = len;

    if (!EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(plaintext.data() + len), &len)) 
    {
        throw std::runtime_error("EVP_DecryptFinal_ex failed");
    }
    plaintext_len += len;
    EVP_CIPHER_CTX_free(ctx);
    
    return pkcs7_unpad(plaintext.substr(0, plaintext_len), AES_BLOCK_SIZE);
}

} // namespace lic
