#include <QApplication>
#include <QSettings>
#include <QTranslator>
#include <QLibraryInfo>
#include <QDebug>
#include <QDir>
#include <QLocale>
#include <QStringLiteral> // Добавляем для QStringLiteral

#include "ui/StartWindow.h"
#include "core/FileManager.h"

int main(int argc, char *argv[])
{
    system("chcp 1251"); // Установка кодовой страницы для консоли
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("Borovik Ivan T-391");
    QCoreApplication::setApplicationName("Своя Игра");

    qDebug() << "--- ПАЧАТАК ДЫЯГНОСТЫКІ ЛАКАЛІЗАЦЫІ ---";

    // --- 1. Якую мову мы хочам загрузіць? ---
    QString lang = FileManager::loadLanguageSetting();
    qDebug() << "[INFO] Мова, загружаная з налад:" << lang;

    // --- 2. Дзе праграма шукае файлы? ---
    // QCoreApplication::applicationDirPath() возвращает путь к директории, где находится исполняемый файл
    QString workingDir = QCoreApplication::applicationDirPath();
    qDebug() << "[INFO] Рабочая дырэкторыя праграмы:" << workingDir;

    // --- 3. Правяраем існаванне папкі `translations` ---
    QString translationsPath = workingDir + "/translations";
    qDebug() << "[CHECK] Поўны шлях да папкі перакладаў:" << translationsPath;

    QDir translationsDir(translationsPath);
    if (translationsDir.exists()) {
        qDebug() << "[OK] Папка 'translations' існуе.";
    } else {
        qWarning() << "[ERROR] Папка 'translations' НЕ ІСНУЕ па ўказаным шляху!";
    }

    // --- 4. Правяраем існаванне канкрэтнага файла `.qm` ---
    QString qmFileName = "svojaigra_" + lang + ".qm";
    QString fullQmPath = translationsPath + "/" + qmFileName;
    qDebug() << "[CHECK] Поўны шлях да файла перакладу:" << fullQmPath;

    if (QFile::exists(fullQmPath)) {
        qDebug() << "[OK] Файл" << qmFileName << "існуе.";
    } else {
        // ИСПРАВЛЕНИЕ: Используем QStringLiteral для строкового литерала перед .arg()
        qWarning() << QStringLiteral("[ERROR] Файл \"%1\" НЕ ЗНОЙДЗЕНЫ па ўказаным шляху!").arg(qmFileName);
    }

    // --- 5. Спрабуем загрузіць пераклад ---
    QTranslator appTranslator;
    // .load() требует имя файла без расширения и путь к папке
    if (appTranslator.load("svojaigra_" + lang, translationsPath)) {
        app.installTranslator(&appTranslator);
        qDebug() << "[OK] Файл перакладу паспяхова загружаны з дыска!";
    } else {
        qWarning() << "[ERROR] Спроба загрузкі файла не атрымалася!";
    }

    qDebug() << "--- КАНЕЦ ДЫЯГНОСТЫКІ ЛАКАЛІЗАЦЫІ ---";

    // Загрузка стандартных перакладаў Qt
    QTranslator qtTranslator;
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        // Note: QLibraryInfo::path(QLibraryInfo::TranslationsPath) points to Qt's installation translations,
        // typically not in your build directory. This part is correct for Qt's own translations.
        if (qtTranslator.load("qtbase_" + lang, QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
            app.installTranslator(&qtTranslator);
        }
    #endif

    StartWindow startWindow;
    startWindow.showFullScreen();

    return app.exec();
}