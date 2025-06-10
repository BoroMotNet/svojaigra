#include "./SettingsDialog.h"

#include <iostream>

#include "ui_SettingsDialog.h"

#include "../core/FileManager.h"
#include <QColorDialog>
#include <QSettings>
#include <QMessageBox>
#include <QSlider>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QDialogButtonBox>

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent),
                                                  ui(new Ui::SettingsDialog) {
    ui->setupUi(this);

    ui->languageComboBox->addItem(tr("Русский"), "ru");
    ui->languageComboBox->addItem(tr("English"), "en");
    ui->languageComboBox->addItem(tr("Беларуская"), "be");

    QPushButton *resetButton = ui->buttonBox->button(QDialogButtonBox::RestoreDefaults);
    if (resetButton) {
        connect(resetButton, &QPushButton::clicked, this, &SettingsDialog::onResetButtonClicked);
    }

    connect(ui->volumeSlider, &QSlider::valueChanged, this, &SettingsDialog::onVolumeSliderChanged);
    connect(ui->colorButton, &QPushButton::clicked, this, &SettingsDialog::onColorButtonClicked);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::onAccepted);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &SettingsDialog::reject);

    loadSettings();
    setWindowTitle(tr("Настройки"));
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}

void SettingsDialog::loadSettings() {
    QSettings settings;
    m_volumeLevel = settings.value("volume", 50).toInt();
    m_selectedColor = QColor(settings.value("backgroundColor", "#0825ed").toString());
    m_initialLanguage = FileManager::loadLanguageSetting();

    ui->volumeSlider->setValue(m_volumeLevel);
    if (ui->volumeValueLabel) {
        ui->volumeValueLabel->setText(QString::number(m_volumeLevel) + "%");
    }

    int index = ui->languageComboBox->findData(m_initialLanguage);
    if (index != -1) {
        ui->languageComboBox->setCurrentIndex(index);
    }

    QString style = QString("background-color: %1;").arg(m_selectedColor.name());
    ui->colorButton->setStyleSheet(style);
}

void SettingsDialog::saveSettings() {
    QSettings settings;
    settings.setValue("volume", m_volumeLevel);
    settings.setValue("backgroundColor", m_selectedColor.name());
    FileManager::saveLanguageSetting(ui->languageComboBox->currentData().toString());
}

void SettingsDialog::onAccepted() {
    saveSettings();
    if (isLanguageChanged()) {
        QMessageBox::warning(this, tr("Смена языка"), tr("Язык будет изменен после перезапуска приложения."));
    }
    accept();
}

void SettingsDialog::onColorButtonClicked() {
    QColor color = QColorDialog::getColor(m_selectedColor, this, tr("Выбор цвет фона"));
    if (color.isValid()) {
        m_selectedColor = color;
        QString style = QString("background-color: %1;").arg(m_selectedColor.name());
        ui->colorButton->setStyleSheet(style);
    }
}

void SettingsDialog::onVolumeSliderChanged(int value) {
    m_volumeLevel = value;
    if (ui->volumeValueLabel) {
        ui->volumeValueLabel->setText(QString::number(value) + "%");
    }
}

void SettingsDialog::onResetButtonClicked() {
    const int defaultVolume = 50;
    const QColor defaultColor("#0825ed");
    const QString defaultLanguage = QLocale::languageToCode(QLocale::system().language());

    m_volumeLevel = defaultVolume;
    m_selectedColor = defaultColor;

    ui->volumeSlider->setValue(defaultVolume);
    if (ui->volumeValueLabel) {
        ui->volumeValueLabel->setText(QString::number(defaultVolume) + "%");
    }

    int index = ui->languageComboBox->findData(defaultLanguage);
    if (index != -1) {
        ui->languageComboBox->setCurrentIndex(index);
    }

    QString style = QString("background-color: %1;").arg(defaultColor.name());
    ui->colorButton->setStyleSheet(style);
}

QColor SettingsDialog::selectedColor() const { return m_selectedColor; }
int SettingsDialog::volumeLevel() const { return m_volumeLevel; }

bool SettingsDialog::isLanguageChanged() const {
    return m_initialLanguage != ui->languageComboBox->currentData().toString();
}
