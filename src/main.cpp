// src/main.cpp

#include <QApplication> // Основной класс для управления GUI-приложением
#include <QSettings>    // Для чтения и записи настроек (например, выбранного языка)
#include <QTranslator>  // Для загрузки файлов перевода
#include <QLibraryInfo> // Для получения пути к стандартным переводам Qt

#include "ui/StartWindow.h"

int main(int argc, char *argv[])
{
    // 1. Создание экземпляра QApplication.
    // Это должно быть сделано в первую очередь в любом Qt GUI приложении.
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("Borovik Ivan T-391"); // Укажите название вашей "компании"
    QCoreApplication::setApplicationName("Своя Игра"); // Название вашего приложения

    // 3. Логика локализации (переключения языков).
    QSettings settings;
    // Читаем сохраненный язык из настроек. Если его нет, по умолчанию будет "ru".
    QString lang = settings.value("language", "ru").toString();

    // Загрузчик для наших переводов
    QTranslator appTranslator;
    // Загружаем файл перевода в зависимости от выбранного языка.
    // Файлы .qm должны находиться в подпапке translations.
    if (appTranslator.load("svojaigra_" + lang, "translations")) {
        app.installTranslator(&appTranslator);
    }

    // Загрузчик для стандартных переводов Qt (кнопки "OK", "Cancel" и т.д.)
    QTranslator qtTranslator;
    // Путь к стандартным переводам зависит от системы
    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        // Для Qt5
        if (qtTranslator.load("qt_" + lang, QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
            app.installTranslator(&qtTranslator);
        }
    #else
        // Для Qt6
        if (qtTranslator.load("qtbase_" + lang, QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
            app.installTranslator(&qtTranslator);
        }
    #endif


    // 4. Создание и отображение главного окна.
    // В нашем случае это StartWindow, так как с него начинается работа.
    StartWindow startWindow;
    startWindow.show(); // Показываем окно пользователю


    // 5. Запуск цикла обработки событий.
    // Программа будет "жить" на этой строке, пока пользователь ее не закроет.
    // app.exec() вернет код завершения (обычно 0).
    return app.exec();
}