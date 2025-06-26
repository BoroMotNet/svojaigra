#include "HelpDialog.h"
#include "../core/FileManager.h"
#include <QVBoxLayout>
#include <QTextBrowser>
#include <QPushButton>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QDebug>

HelpDialog::HelpDialog(QWidget *parent) : QDialog(parent)
{
    setupUi();
    loadHelpContent();
    setWindowTitle(tr("Справка"));
    setMinimumSize(600, 500);
}

void HelpDialog::setupUi()
{
    auto* layout = new QVBoxLayout(this);
    m_textBrowser = new QTextBrowser(this);
    m_textBrowser->setOpenExternalLinks(true);
    
    auto* closeButton = new QPushButton(tr("Закрыть"), this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

    layout->addWidget(m_textBrowser);
    layout->addWidget(closeButton);
}

void HelpDialog::loadHelpContent()
{
    QString lang = FileManager::loadLanguageSetting();
    QString filePath = QCoreApplication::applicationDirPath() + "/help/help_" + lang + ".md";

    if (!QFile::exists(filePath)) {
        qWarning() << "Help file for lang" << lang << "not found, falling back to ru";
        filePath = QCoreApplication::applicationDirPath() + "/help/help_ru.md";
    }

    QFile helpFile(filePath);
    if (helpFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&helpFile);
        QString markdownContent = in.readAll();
        m_textBrowser->setMarkdown(markdownContent);
        helpFile.close();
    } else {
        m_textBrowser->setHtml(tr("<h2>Ошибка</h2><p>Не удалось загрузить файл справки: %1</p>").arg(filePath));
        qCritical() << "Could not open help file:" << filePath;
    }
}