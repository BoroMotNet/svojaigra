//
// Created by ruthen on 10.06.25.
//

#include "SettingsDialog.h"
#include "./SettingsDialog.h"
#include "../core/FileManager.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QSlider>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QColorDialog>
#include <QLabel>
#include <QSettings>
#include <QMessageBox>

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent)
{
    setupUi();
    loadSettings();
    setWindowTitle(tr("Настройки"));
}

void SettingsDialog::setupUi()
{
    m_languageComboBox = new QComboBox(this);
    m_languageComboBox->addItem(tr("Русский"), "ru");
    m_languageComboBox->addItem(tr("English"), "en");
    m_languageComboBox->addItem(tr("Беларуская"), "be");

    m_volumeSlider = new QSlider(Qt::Horizontal, this);
    m_volumeSlider->setRange(0, 100);
    m_volumeValueLabel = new QLabel("100%", this);
    connect(m_volumeSlider, &QSlider::valueChanged, this, &SettingsDialog::onVolumeSliderChanged);

    m_colorButton = new QPushButton(tr("Выбрать цвет..."), this);
    connect(m_colorButton, &QPushButton::clicked, this, &SettingsDialog::onColorButtonClicked);

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::onAccepted);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &SettingsDialog::reject);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(tr("Язык (требует перезапуска):"), m_languageComboBox);

    QHBoxLayout *volumeLayout = new QHBoxLayout;
    volumeLayout->addWidget(m_volumeSlider);
    volumeLayout->addWidget(m_volumeValueLabel);
    formLayout->addRow(tr("Громкость:"), volumeLayout);
    formLayout->addRow(tr("Цвет фона:"), m_colorButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(m_buttonBox);
}

void SettingsDialog::loadSettings()
{
    QSettings settings;
    m_volumeLevel = settings.value("volume", 100).toInt();
    m_selectedColor.setNamedColor(settings.value("backgroundColor", "#FFFFFF").toString());
    m_initialLanguage = FileManager::loadLanguageSetting();

    m_volumeSlider->setValue(m_volumeLevel);
    m_volumeValueLabel->setText(QString::number(m_volumeLevel) + "%");

    int index = m_languageComboBox->findData(m_initialLanguage);
    if (index != -1) {
        m_languageComboBox->setCurrentIndex(index);
    }

    QString style = QString("background-color: %1;").arg(m_selectedColor.name());
    m_colorButton->setStyleSheet(style);
}

void SettingsDialog::saveSettings()
{
    QSettings settings;
    settings.setValue("volume", m_volumeLevel);
    settings.setValue("backgroundColor", m_selectedColor.name());
    FileManager::saveLanguageSetting(m_languageComboBox->currentData().toString());
}

void SettingsDialog::onAccepted()
{
    saveSettings();
    if (isLanguageChanged()) {
        QMessageBox::information(this, tr("Смена языка"), tr("Язык будет изменен после перезапуска приложения."));
    }
    accept();
}

void SettingsDialog::onColorButtonClicked()
{
    QColor color = QColorDialog::getColor(m_selectedColor, this, tr("Выбор цвет фона"));
    if (color.isValid()) {
        m_selectedColor = color;
        QString style = QString("background-color: %1;").arg(m_selectedColor.name());
        m_colorButton->setStyleSheet(style);
    }
}

void SettingsDialog::onVolumeSliderChanged(int value)
{
    m_volumeLevel = value;
    m_volumeValueLabel->setText(QString::number(value) + "%");
}

QColor SettingsDialog::selectedColor() const { return m_selectedColor; }
int SettingsDialog::volumeLevel() const { return m_volumeLevel; }
bool SettingsDialog::isLanguageChanged() const {
    return m_initialLanguage != m_languageComboBox->currentData().toString();
}