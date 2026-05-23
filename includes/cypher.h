#include <openssl/evp.h>
#include <openssl/rand.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <iomanip>

void deriveKey(const std::string& password, const unsigned char* salt, unsigned char* key, unsigned char* iv) {
    EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha256(), salt,
        (unsigned char*)password.c_str(), password.size(), 1, key, iv);
}

std::vector<unsigned char> encrypt(const std::string& plaintext, const std::string& password) {
    unsigned char key[32], iv[16];
    unsigned char salt[8];
    RAND_bytes(salt, sizeof(salt));  // generate random salt

    deriveKey(password, salt, key, iv);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    std::vector<unsigned char> ciphertext(plaintext.size() + 16);
    int len = 0, total = 0;

    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv);
    EVP_EncryptUpdate(ctx, ciphertext.data(), &len,
        (unsigned char*)plaintext.c_str(), plaintext.size());
    total += len;
    EVP_EncryptFinal_ex(ctx, ciphertext.data() + total, &len);
    total += len;
    EVP_CIPHER_CTX_free(ctx);

    ciphertext.resize(total);

    // prepend salt to ciphertext so we have it for decryption
    std::vector<unsigned char> output(salt, salt + 8);
    output.insert(output.end(), ciphertext.begin(), ciphertext.end());

    return output;
}

std::string decrypt(const std::string& input, const std::string& password) {
    unsigned char key[32], iv[16];

    // extract salt from the first 8 bytes
    unsigned char salt[8];
    std::copy(input.begin(), input.begin() + 8, salt);

    deriveKey(password, salt, key, iv);

    // rest is the actual ciphertext
    std::vector<unsigned char> ciphertext(input.begin() + 8, input.end());

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    std::vector<unsigned char> plaintext(ciphertext.size());
    int len = 0, total = 0;

    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv);
    EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(), ciphertext.size());
    total += len;
    EVP_DecryptFinal_ex(ctx, plaintext.data() + total, &len);
    total += len;

    EVP_CIPHER_CTX_free(ctx);
    return std::string(plaintext.begin(), plaintext.begin() + total);
}

std::string sha256(const std::string& input) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLen = 0;

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);
    EVP_DigestUpdate(ctx, input.c_str(), input.size());
    EVP_DigestFinal_ex(ctx, hash, &hashLen);
    EVP_MD_CTX_free(ctx);

    std::stringstream ss;
    for (unsigned int i = 0; i < hashLen; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return ss.str();
}