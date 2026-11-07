#pragma once

#include <string>
#include <vector>

// namespace Master Passwords - mp
namespace mp {

class Crypt {
 public:
  /**
   * Шифрует входные данные с использованием пароля (ключа).
   * @param plaintext Исходные данные
   * @param password Пароль для генерации ключа шифрования
   * @return Зашифрованные данные (включая соль и IV) или пустой вектор в случае
   * ошибки
   */
  static std::vector<unsigned char> Encrypt(const std::string& plaintext,
                                            const std::string& password);

  /**
   * Дешифрует данные с использованием пароля (ключа).
   * @param ciphertext Зашифрованные данные
   * @param password Пароль, использованный при шифровании
   * @return Расшифрованные данные или пустая строка в случае ошибки
   */
  static std::string Decrypt(const std::vector<unsigned char>& ciphertext,
                             const std::string& password);
};

}  // namespace mp
