cmake_minimum_required(VERSION 3.16)

project(SvojaIgra LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON) # Оставляем ON для автоматической обработки main resources.qrc

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia LinguistTools)

# Определяем список исходных файлов вашего приложения
set(SVOJA_IGRA_SOURCES
        src/main.cpp
        src/ui/startwindow.cpp
        src/ui/MainWindow.cpp
        src/ui/AdminEditor.cpp
        src/core/Question.cpp
        src/core/Player.cpp
        src/core/GameManager.cpp
        src/core/FileManager.cpp
        src/ui/QuestionDialog.cpp
        src/ui/PlayerNameDialog.cpp
        src/ui/settingsdialog.cpp
)

# --- ИЗМЕНЕНИЯ ЗДЕСЬ: qt_add_translations ---
# Определяем директорию для сохранения .qm файлов
set(TRANSLATIONS_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/translations")

# Убедимся, что эта директория существует
file(MAKE_DIRECTORY ${TRANSLATIONS_OUTPUT_DIR})

qt_add_translations(SvojaIgra # Имя вашей основной цели
        TS_FILES
        resources/translations/svojaigra_en.ts
        resources/translations/svojaigra_be.ts
        resources/translations/svojaigra_ru.ts
        # Используем QM_OUTPUT_DIRECTORY для указания ВЫХОДНОЙ ДИРЕКТОРИИ для .qm файлов
        # Это должно заставить lrelease положить их именно туда.
        QM_OUTPUT_DIRECTORY ${TRANSLATIONS_OUTPUT_DIR}
        # Больше никаких QM_FILES_BASE_DIR, QM_FILES_OUTPUT_VAR, GENERATE_QM_RESOURCE_PREFIX
)

add_executable(SvojaIgra
        ${SVOJA_IGRA_SOURCES}
        resources/resources.qrc # Ваш основной QRC файл (для картинок, звуков, JSON)
        # Важно: Не добавляем сюда TRANSLATION_QM_FILES или подобные,
        # так как мы хотим, чтобы QM-файлы были на диске, а не в ресурсах исполняемого файла.
)

target_link_libraries(SvojaIgra PRIVATE
        Qt6::Widgets
        Qt6::Multimedia
)