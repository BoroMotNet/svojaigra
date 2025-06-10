#include <QApplication>
#include <QSettings>
#include <QTranslator>
#include <QLibraryInfo>
#include <QDebug>
#include <QDir>

#include "ui/StartWindow.h"
#include "core/FileManager.h"

int main(int argc, char *argv[])
{
    system("chcp 1251");
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("Borovik Ivan T-391");
    QCoreApplication::setApplicationName("Своя Игра");

    qDebug() << "--- ПАЧАТАК ДЫЯГНОСТЫКІ ЛАКАЛІЗАЦЫІ ---";

    // --- 1. Якую мову мы хочам загрузіць? ---
    QString lang = FileManager::loadLanguageSetting();
    qDebug() << "[INFO] Мова, загружаная з налад:" << lang;

    // --- 2. Дзе праграма шукае файлы? ---
    QString workingDir = QDir::currentPath();
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
        qWarning() << "[ERROR] Файл" << qmFileName << "НЕ ЗНОЙДЗЕНЫ па ўказаным шляху!";
    }

    // --- 5. Спрабуем загрузіць пераклад ---
    QTranslator appTranslator;
    // .load() патрабуе назву файла без пашырэння і шлях да папкі
    if (appTranslator.load("svojaigra_" + lang, translationsPath)) {
        app.installTranslator(&appTranslator);
        qDebug() << "[OK] Файл перакладу паспяхова загружаны!";
    } else {
        qWarning() << "[ERROR] Спроба загрузкі файла не атрымалася!";
    }

    qDebug() << "--- КАНЕЦ ДЫЯГНОСТЫКІ ЛАКАЛІЗАЦЫІ ---";

    // Загрузка стандартных перакладаў Qt
    QTranslator qtTranslator;
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        if (qtTranslator.load("qtbase_" + lang, QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
            app.installTranslator(&qtTranslator);
        }
    #endif

    StartWindow startWindow;
    startWindow.showFullScreen();

    return app.exec();
}