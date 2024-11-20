#include "infra/sign/signature.h"
#include "infra/log/log.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <array>
#include <csignal>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/rsa.h>
#include <openssl/pkcs7.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

namespace lic
{

std::string read_self_content()
{
    char exe_path[1024];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len == -1) 
    {
        LOG_ERROR("Error: Unable to get executable path");
        return "";
    }
    exe_path[len] = '\0';

  
    std::ifstream exe_file(exe_path, std::ios::binary | std::ios::ate);
    if (!exe_file) 
    {
        LOG_ERROR("Error: Unable to open executable file");
        return "";
    }

    std::streamsize size = exe_file.tellg();
    exe_file.seekg(0, std::ios::beg);

    std::string content(static_cast<std::string::size_type>(size), '\0');
    if (!exe_file.read(&content[0], size))
    {
        return "";
    }
    return content;
}

std::string readFile(const std::string &filePath)
{
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        return "";
    }
    std::streamsize size = file.tellg();
    file.seekg(0);
    std::string content(static_cast<std::string::size_type>(size), '\0');
    if (!file.read(&content[0], size))
    {
        return "";
    }
    return content;
}

std::string computeSha256(const std::string &data)
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

std::string extractHashFromSignature(const std::string &sigPath, const std::string &certPath)
{
    std::string sigContent = readFile(sigPath);
    std::string certContent = readFile(certPath);

    BIO *bioCert = BIO_new_mem_buf(certContent.c_str(), static_cast<int>(certContent.size()));
    if (!bioCert)
    {
        LOG_ERROR("Failed to create BIO for certificate");
        return "";
    }

    X509 *cert = PEM_read_bio_X509(bioCert, nullptr, nullptr, nullptr);
    if (!cert)
    {
        LOG_ERROR("Failed to read certificate");
        BIO_free(bioCert);
        return "";
    }

    EVP_PKEY *pubKey = X509_get_pubkey(cert);
    if (!pubKey)
    {
        LOG_ERROR("Failed to get public key from certificate");
        X509_free(cert);
        BIO_free(bioCert);
        return "";
    }

    RSA *rsaPubKey = EVP_PKEY_get1_RSA(pubKey);
    if (!rsaPubKey)
    {
        LOG_ERROR("Failed to get RSA public key from EVP_PKEY");
        EVP_PKEY_free(pubKey);
        X509_free(cert);
        BIO_free(bioCert);
        return "";
    }

    unsigned char decryptedHash[1024];
    int decryptedLength = RSA_public_decrypt(sigContent.length(),
                                             reinterpret_cast<const unsigned char *>(sigContent.c_str()),
                                             decryptedHash, rsaPubKey, RSA_PKCS1_PADDING);

    if (decryptedLength == -1)
    {
        // std::cerr << "Decryption failed." << std::endl;
        // ERR_print_errors_fp(stderr);
        RSA_free(rsaPubKey);
        EVP_PKEY_free(pubKey);
        X509_free(cert);
        BIO_free(bioCert);
        return "";
    }


    unsigned char *hashStart = &decryptedHash[decryptedLength - SHA256_DIGEST_LENGTH];

    std::string extractedHash;
    std::stringstream hashStream;
    hashStream << std::hex << std::setfill('0');
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
    {
        hashStream << std::setw(2) << static_cast<int>(hashStart[i]);
    }

    RSA_free(rsaPubKey);
    EVP_PKEY_free(pubKey);
    X509_free(cert);
    BIO_free(bioCert);

    return hashStream.str();
}

bool verify_sig(const std::string& sign_file, const std::string& cert_file)
{
    const std::string dataContent = read_self_content();
    std::string calculatedHash = computeSha256(dataContent);
    std::string extractedHash = extractHashFromSignature(sign_file, cert_file);

    if (extractedHash.empty())
    {
        return false;
    }
    return extractedHash == calculatedHash;
}

} // namespace lic
