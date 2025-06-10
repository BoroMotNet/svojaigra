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
    ~SettingsDialog();

    QColor selectedColor() const;
    int volumeLevel() const;
    bool isLanguageChanged() const;

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

#endif // SETTINGSDIALOG_H