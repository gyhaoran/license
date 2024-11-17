#include "encoder.h"
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

namespace
{

unsigned char DEFAULT_YEK[] = {
    0xb5, 0xbf, 0x90, 0x7b, 0x97, 0x6f, 0xe5, 0xc8, 0x01, 0x62, 0x31, 0x4f, 0xe4, 0xb3, 0xbe, 0x02,
    0x17, 0x7e, 0x2d, 0xfd, 0x0d, 0x48, 0x42, 0x47, 0x80, 0x63, 0xfc, 0xc0, 0x61, 0xfc, 0x2e, 0x93
};

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

std::string base64_encode(const unsigned char* input, size_t length)
{
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);
    BIO_write(bio, input, length);
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    std::string result(bufferPtr->data, bufferPtr->length);
    BIO_free_all(bio);
    return result;
}


std::string base64_decode(const std::string& input)
{
    BIO *bio, *b64;
    char* buffer = (char*)malloc(input.length());
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_mem_buf(input.data(), input.length());
    bio = BIO_push(b64, bio);
    int decoded_length = BIO_read(bio, buffer, input.length());
    BIO_free_all(bio);
    std::string result(buffer, decoded_length);
    free(buffer);
    return result;
}

} // namespace

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

    const unsigned char* aes_key = nullptr;
    if (key.empty())
    {
        aes_key = DEFAULT_YEK;
    }
    else
    {
        aes_key = reinterpret_cast<const unsigned char*>(key.c_str());
    }

    if (!EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, aes_key, iv)) 
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

    if (key.empty())
    {
        return base64_encode(reinterpret_cast<const unsigned char*>(result.c_str()), result.size());
    }

    return result;
}

// AES decryption function
std::string decrypt_info(const std::string& data, const std::string& key) 
{
    std::string decoded_data = data;
    if (key.empty())
    {
        decoded_data = base64_decode(data);
    }

    if (decoded_data.size() < AES_BLOCK_SIZE) 
    {
        throw std::runtime_error("Data too short");
    }

    unsigned char iv[AES_BLOCK_SIZE];
    std::memcpy(iv, decoded_data.c_str(), AES_BLOCK_SIZE);

    std::string ciphertext = decoded_data.substr(AES_BLOCK_SIZE);
    std::string plaintext(ciphertext.size(), 0);

    const unsigned char* aes_key = nullptr;
    if (key.empty())
    {
        aes_key = DEFAULT_YEK;
    }
    else
    {
        aes_key = reinterpret_cast<const unsigned char*>(key.c_str());
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, aes_key, iv)) 
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
