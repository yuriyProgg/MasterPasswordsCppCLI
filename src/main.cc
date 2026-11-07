#include <print>

#include "src/crypt.h"

int main() {
  std::string original = "Секретное сообщение для Debian и Windows!";
  std::string password = "StrongPassword123";

  // Шифрование
  auto encrypted = mp::Crypt::Encrypt(original, password);

  // Дешифрование
  std::string decrypted = mp::Crypt::Decrypt(encrypted, password);

  std::println("Decrypted: {}", decrypted);
  return 0;
}
