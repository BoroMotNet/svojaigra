#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QColor>

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog() override;

    [[nodiscard]] QColor selectedColor() const;
    [[nodiscard]] int volumeLevel() const;
    [[nodiscard]] bool isLanguageChanged() const;

private slots:
    void onColorButtonClicked();
    void onVolumeSliderChanged(int value);
    void onAccepted();
    void onResetButtonClicked();

private:
    void loadSettings();
    void saveSettings();

    Ui::SettingsDialog *ui;

    QColor m_selectedColor;
    int m_volumeLevel;
    QString m_initialLanguage;
};

#endif