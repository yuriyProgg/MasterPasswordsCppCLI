# Руководство по стилю C++23 и кроссплатформенной разработке (Debian / Windows)

Данный документ фиксирует единые стандарты оформления кода и правила совместной работы для нашей команды. Цель — обеспечить полную идентичность форматирования кода в **Neovim (Debian)** и **Visual Studio (Windows)**, а также избежать ошибок компиляции, связанных с различиями файловых систем.

---

## 1. Базовый стиль кода (Google C++ Style Guide)

Мы используем адаптированный **Google Style** с поддержкой возможностей **C++23**.

### Ключевые правила именования:
*   **Классы и структуры:** `PascalCase` (например, `ColoredCircle`).
*   **Интерфейсы (абстрактные классы):** `PascalCase` с префиксом `I` по желанию или в общем стиле (например, `Shape` или `IShape`).
*   **Функции и обычные методы:** `PascalCase` (например, `CalculateArea()`, `Draw()`).
*   **Геттеры и сеттеры (Исключение Google):** `snake_case` (например, `radius()`, `set_radius()`).
*   **Приватные поля класса:** `snake_case` с обязательным подчеркиванием на конце (например, `radius_`, `color_`).
*   **Константы (static constexpr / глобальные):** Префикс `k` + `PascalCase` (например, `kPi`).

### Правила форматирования:
*   **Отступы:** Строго **2 пробела**. Табуляции запрещены.
*   **Фигурные скобки:** Перенос скобки на ту же строку (`Attach` / стиль K&R).
*   **Длина строки:** Ограничение в **100 символов**.

---

## 2. Именование файлов и критические правила для Debian и Windows

Разница между файловыми системами **Linux (ext4 — чувствительна к регистру)** и **Windows (NTFS — нечувствительна к регистру)** может приводить к поломке сборки. 

### Жесткие правила для команды:
1.  **Все имена файлов — строго в нижнем регистре (lowercase) через подчеркивание `_`.**
    *   *Правильно:* `database_connection.cc`, `shapes.h`
    *   *Неправильно:* `DatabaseConnection.cpp`, `shapes.H`, `database-connection.cc`
2.  **Расширения файлов:**
    *   Исходный код (реализация): **`.cc`**
    *   Заголовочные файлы (интерфейсы): **`.h`**
3.  **Регистр в `#include`:** Пути внутри инклюдов должны **символ в символ** совпадать с именем файла на диске. Если написать `#include "Some_File.h"` вместо `some_file.h`, код соберется на Windows, но упадет с ошибкой на Debian!

---

## 3. Конфигурационные файлы проекта

### Файл `.editorconfig` (Должен лежать в корне проекта)
Visual Studio автоматически считывает этот файл и принудительно заставляет сохранять файлы в формате **LF** (Linux-перенос строки) и кодировке **UTF-8**, защищая Git от конфликтов.

```ini
root = true

[*]
charset = utf-8
end_of_line = lf
insert_final_newline = true
indent_style = space
indent_size = 2

[*.{cc,h}]
indent_style = space
indent_size = 2
```

### Файл `.gitattributes` (В корень проекта)
Блокирует автоматическую конвертацию переносов строк со стороны Git на Windows.
```text
* text=auto eol=lf
```

### Файл `.clang-format` (В корень проекта)
```yaml
---
Language: Cpp
BasedOnStyle: Google
Standard: Latest
UseTab: Never
IndentWidth: 2
BreakBeforeBraces: Attach
AlignAfterOpenBracket: Align
ColumnLimit: 100
SortIncludes: CaseSensitive
...
```

---

## 4. Настройка CMake и автоматизация (`CMakeLists.txt`)

Этот кусок CMake находит `clang-format` как на Debian, так и внутри установленной Visual Studio 2022 на Windows, создавая общую цель сборки `format`:

```cmake
cmake_minimum_required(VERSION 3.20)
project(TeamProject CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Рекурсивный сбор исходников
file(GLOB_RECURSE ALL_SOURCE_FILES 
    "${SOURCE_DIR}/*.cc"
    "${SOURCE_DIR}/*.h"
)

# Поиск clang-format ( Debian / Windows )
find_program(CLANG_FORMAT_EXE NAMES clang-format)

if(NOT CLANG_FORMAT_EXE AND WIN32)
    file(GLOB VS_CLANG_FORMAT_EDITIONS
        "C:/Program Files/Microsoft Visual Studio/2022/*/VC/Tools/Llvm/x64/bin/clang-format.exe"
    )
    if(VS_CLANG_FORMAT_EDITIONS)
        list(GET VS_CLANG_FORMAT_EDITIONS 0 CLANG_FORMAT_EXE)
    endif()
endif()

if(CLANG_FORMAT_EXE)
    message(STATUS "Clang-format tool found: ${CLANG_FORMAT_EXE}")
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXE} -i -style=file ${ALL_SOURCE_FILES}
        COMMENT "Formatting code with Google Style..."
    )
else()
    message(WARNING "clang-format not found! Target 'format' is disabled.")
endif()

add_executable(app ${ALL_SOURCE_FILES})
```

---

## 5. Как с этим работать разработчикам

### Разработчикам на Windows (Visual Studio 2022):
1.  **Открытие проекта:** Открывать проект строго через **«Открыть локальную папку» (Open Folder)**, выбирая корень репозитория с `CMakeLists.txt` (НЕ создавать `.sln` файл).
2.  **Создание модулей:** Переключите Обозреватель решений в режим **Folder View** (Представление папок). Нажмите правой кнопкой мыши по папке `src` -> **Добавить -> Создать файл**. В имени файла *вручную* напишите `name.cc` и `name.h` (строго строчными буквами). VS запомнит этот выбор.
3.  **Автоформатирование:** Зайдите в *Сервис -> Параметры -> Текстовый редактор -> C/C++ -> Форматирование* и включите **«Запускать ClangFormat»**, выбрав пункт *«Использовать файл .clang-format в текущей папке»*. Код будет форматироваться сам по горячим клавишам `Ctrl+K, Ctrl+D`.

### Разработчикам на Debian (Neovim):
*   Файл `.clang-format` в корне автоматически подхватится вашим языковым сервером `clangd` или плагинами форматирования (`conform.nvim` / `formatter.nvim`).
*   Для принудительного форматирования всего проекта перед коммитом выполните в терминале сборки:
    ```bash
    cmake --build build --target format
    ```
