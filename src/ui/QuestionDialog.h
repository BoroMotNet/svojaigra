#ifndef QUESTIONDIALOG_H
#define QUESTIONDIALOG_H

#include <QDialog>
#include "../core/Question.h"
#include <QMediaPlayer>

class QLabel;
class QLineEdit;
class QMediaPlayer;
class QAudioOutput;
class QPushButton;
class QSlider;
class QHBoxLayout;

class QuestionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QuestionDialog(const Question& question, QWidget *parent = nullptr);
    ~QuestionDialog();
    QString getAnswer() const;

private slots:
    void togglePlayback();
    void stopPlayback();
    void setVolume(int volume);
    void handleMediaPlayerError(QMediaPlayer::Error error);
    void handleMediaPlayerStateChanged(QMediaPlayer::PlaybackState state);

private:
    QLineEdit* m_answerEdit;
    QLabel* m_mediaLabel;

    QPushButton* m_playPauseButton;
    QPushButton* m_stopButton;
    QSlider* m_volumeSlider;
    QLabel* m_volumeLabel;

    QMediaPlayer* m_mediaPlayer;
    QAudioOutput* m_audioOutput;
};

#endif