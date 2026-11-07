#include "src/crypt.h"

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <cstring>
#include <vector>

namespace mp {

// Параметры для формирования ключа
constexpr int SALT_SIZE = 8;
constexpr int KEY_SIZE = 32;  // Для AES-256
constexpr int IV_SIZE = 16;   // Размер блока для AES

std::vector<unsigned char> Crypt::Encrypt(const std::string& plaintext,
                                          const std::string& password) {
  std::vector<unsigned char> output;

  // Создаем контекст шифрования
  EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
  if (!ctx) return output;

  // Генерируем случайную соль
  unsigned char salt[SALT_SIZE];
  if (RAND_bytes(salt, SALT_SIZE) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return output;
  }

  // Генерируем ключ и IV из пароля и соли с помощью EVP_BytesToKey (SHA-256)
  unsigned char key[KEY_SIZE];
  unsigned char iv[IV_SIZE];
  if (!EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha256(), salt,
                      reinterpret_cast<const unsigned char*>(password.data()),
                      password.size(), 1, key, iv)) {
    EVP_CIPHER_CTX_free(ctx);
    return output;
  }

  // Инициализируем шифрование AES-256-CBC
  if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return output;
  }

  // Выделяем буфер под зашифрованные данные (с запасом на паддинг)
  int max_len = plaintext.size() + EVP_MAX_BLOCK_LENGTH;
  std::vector<unsigned char> ciphertext(max_len);
  int len = 0;
  int ciphertext_len = 0;

  // Шифруем тело сообщения
  if (EVP_EncryptUpdate(
          ctx, ciphertext.data(), &len,
          reinterpret_cast<const unsigned char*>(plaintext.data()),
          plaintext.size()) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return output;
  }
  ciphertext_len = len;

  // Финализируем шифрование (добавляем паддинг PKCS#7)
  if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return output;
  }
  ciphertext_len += len;
  EVP_CIPHER_CTX_free(ctx);

  // Формируем итоговый пакет: префикс "Salted__" (8 байт) + соль (8 байт) +
  // ciphertext
  std::string salted_magic = "Salted__";
  output.insert(output.end(), salted_magic.begin(), salted_magic.end());
  output.insert(output.end(), salt, salt + SALT_SIZE);
  output.insert(output.end(), ciphertext.begin(),
                ciphertext.begin() + ciphertext_len);

  return output;
}

std::string Crypt::Decrypt(const std::vector<unsigned char>& ciphertext,
                           const std::string& password) {
  // Проверяем минимальный размер пакета (магическая строка "Salted__" + соль +
  // хотя бы один блок данных)
  if (ciphertext.size() < 16 + IV_SIZE) return "";

  // Проверяем заголовок "Salted__" (совместимо с форматом OpenSSL CLI)
  if (std::memcmp(ciphertext.data(), "Salted__", 8) != 0) return "";

  const unsigned char* salt = ciphertext.data() + 8;
  const unsigned char* actual_ciphertext = ciphertext.data() + 16;
  size_t actual_ciphertext_len = ciphertext.size() - 16;

  // Генерируем ключ и IV по паролю и соли
  unsigned char key[KEY_SIZE];
  unsigned char iv[IV_SIZE];
  if (!EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha256(), salt,
                      reinterpret_cast<const unsigned char*>(password.data()),
                      password.size(), 1, key, iv)) {
    return "";
  }

  EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
  if (!ctx) return "";

  if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return "";
  }

  std::vector<unsigned char> plaintext(actual_ciphertext_len +
                                       EVP_MAX_BLOCK_LENGTH);
  int len = 0;
  int plaintext_len = 0;

  if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, actual_ciphertext,
                        actual_ciphertext_len) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return "";
  }
  plaintext_len = len;

  if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return "";  // Ошибка расшифровки (неверный пароль или поврежденные данные)
  }
  plaintext_len += len;
  EVP_CIPHER_CTX_free(ctx);

  return std::string(reinterpret_cast<char*>(plaintext.data()), plaintext_len);
}

}  // namespace mp
