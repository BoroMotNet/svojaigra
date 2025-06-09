#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QColor>

class QComboBox;
class QSlider;
class QPushButton;
class QDialogButtonBox;
class QLabel;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

    QColor selectedColor() const;
    int volumeLevel() const;
    bool isLanguageChanged() const;

private slots:
    void onColorButtonClicked();
    void onVolumeSliderChanged(int value);
    void onAccepted();

private:
    void setupUi();
    void loadSettings();
    void saveSettings();

    QComboBox* m_languageComboBox;
    QSlider* m_volumeSlider;
    QPushButton* m_colorButton;
    QDialogButtonBox* m_buttonBox;
    QLabel* m_volumeValueLabel;

    QColor m_selectedColor;
    int m_volumeLevel;
    QString m_initialLanguage;
};

#endif // SETTINGSDIALOG_H