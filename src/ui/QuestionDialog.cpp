#include "./QuestionDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTemporaryFile>
#include <QStandardPaths>
#include <QDir>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QSlider>
#include <QPixmap>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>
#include <QDebug>
#include <QFile>
#include <QStyle>
#include <QMediaDevices>
#include <QAudioDevice>
#include <QFileInfo>
QuestionDialog::QuestionDialog(const Question& question, QWidget *parent)
    : QDialog(parent),
      m_mediaPlayer(nullptr),
      m_audioOutput(nullptr)
{
    setWindowTitle(question.category + " - " + QString::number(question.points));
    setMinimumSize(800, 600);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);

    QLabel* questionLabel = new QLabel(question.questionText, this);
    questionLabel->setFont(QFont("Arial", 18));
    questionLabel->setWordWrap(true);
    questionLabel->setAlignment(Qt::AlignCenter);

    m_mediaLabel = new QLabel(this);
    m_mediaLabel->setAlignment(Qt::AlignCenter);
    m_mediaLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_playPauseButton = new QPushButton(this);
    m_playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    m_playPauseButton->setFixedSize(40, 40);
    connect(m_playPauseButton, &QPushButton::clicked, this, &QuestionDialog::togglePlayback);

    m_stopButton = new QPushButton(this);
    m_stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    m_stopButton->setFixedSize(40, 40);
    connect(m_stopButton, &QPushButton::clicked, this, &QuestionDialog::stopPlayback);

    m_volumeSlider = new QSlider(Qt::Horizontal, this);
    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setValue(50);
    m_volumeSlider->setToolTip(tr("Громкость"));
    connect(m_volumeSlider, &QSlider::valueChanged, this, &QuestionDialog::setVolume);

    m_volumeLabel = new QLabel(tr("Громкость: 50%"), this);

    QHBoxLayout* playerControlsLayout = new QHBoxLayout();
    playerControlsLayout->addStretch(1);
    playerControlsLayout->addWidget(m_playPauseButton);
    playerControlsLayout->addWidget(m_stopButton);
    playerControlsLayout->addWidget(m_volumeSlider);
    playerControlsLayout->addWidget(m_volumeLabel);
    playerControlsLayout->addStretch(1);

    m_playPauseButton->hide();
    m_stopButton->hide();
    m_volumeSlider->hide();
    m_volumeLabel->hide();

    bool mediaSuccessfullyDisplayed = false;
    QString resourcePath = ":/media/" + question.mediaPath;
    qDebug() << "Сформированный ресурсный путь для QFile::exists:" << resourcePath;
    if (QFile::exists(resourcePath)) {
        qDebug() << "QFile::exists() вернул TRUE для пути:" << resourcePath;
    } else {
        qWarning() << "QFile::exists() вернул FALSE для пути:" << resourcePath;
    }

    if (!QMediaDevices::defaultAudioOutput().isNull()) {
        qDebug() << "Аудиовыход по умолчанию доступен.";
    } else {
        qWarning() << "ОШИБКА: Нет доступного аудиовыхода! Проверьте драйвера или устройства.";
    }

    if (question.isMultimedia() && !question.mediaPath.isEmpty()) {
        if (QFile::exists(resourcePath)) {
            if (question.type == QuestionType::ImageAndText) {
                QPixmap pixmap(resourcePath);
                if (!pixmap.isNull()) {
                    int maxWidth = 700;
                    int maxHeight = 400;
                    QPixmap scaledPixmap = pixmap.scaled(maxWidth, maxHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                    m_mediaLabel->setPixmap(scaledPixmap);
                    qDebug() << "Изображение загружено из ресурсов:" << resourcePath;
                    mediaSuccessfullyDisplayed = true;
                } else {
                    qWarning() << "Ошибка загрузки изображения из ресурсов (pixmap.isNull()):" << resourcePath;
                }
            } else if (question.type == QuestionType::SoundAndText) {
                if (!QMediaDevices::defaultAudioOutput().isNull()) {
                    m_audioOutput = new QAudioOutput(this);
                    m_audioOutput->setVolume(m_volumeSlider->value() / 100.0f);
                    qDebug() << "QAudioOutput создан. Громкость по умолчанию:" << m_audioOutput->volume();

                    m_mediaPlayer = new QMediaPlayer(this);
                    m_mediaPlayer->setAudioOutput(m_audioOutput);

                    QString tempFilePath;
                    QString fileName = QFileInfo(resourcePath).fileName();
                    QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/SvojaIgraTempMedia";

                    QDir dir(tempDir);
                    if (!dir.exists()) {
                        dir.mkpath(".");
                    }

                    tempFilePath = tempDir + "/" + fileName;

                    QFile resourceFile(resourcePath);
                    if (resourceFile.open(QIODevice::ReadOnly)) {
                        QFile tempFile(tempFilePath);
                        if (tempFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                            tempFile.write(resourceFile.readAll());
                            tempFile.close();
                            resourceFile.close();
                            qDebug() << "Медиафайл успешно извлечен в временный файл:" << tempFilePath;
                            m_mediaPlayer->setSource(QUrl::fromLocalFile(tempFilePath));
                            m_tempMediaFilePath = tempFilePath;
                            mediaSuccessfullyDisplayed = true;
                        } else {
                            qWarning() << "Ошибка: Не удалось открыть временный файл для записи:" << tempFilePath;
                        }
                    } else {
                        qWarning() << "Ошибка: Не удалось открыть ресурсный файл для чтения:" << resourcePath;
                    }

                    if (mediaSuccessfullyDisplayed) {
                        m_playPauseButton->show();
                        m_stopButton->show();
                        m_volumeSlider->show();
                        m_volumeLabel->show();
                    } else {
                        qWarning() << "Медиафайл не был успешно извлечен. Плеер не будет работать.";
                    }

                    connect(m_mediaPlayer, &QMediaPlayer::errorOccurred, this, &QuestionDialog::handleMediaPlayerError);
                    connect(m_mediaPlayer, &QMediaPlayer::playbackStateChanged, this, &QuestionDialog::handleMediaPlayerStateChanged);
                    connect(m_mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, [this, resourcePath](QMediaPlayer::MediaStatus status){
                        qDebug() << "Статус медиа изменился на:" << status;
                        if (status == QMediaPlayer::InvalidMedia) {
                            qWarning() << "MediaStatus: InvalidMedia - Невозможно загрузить медиафайл."
                                       << "Проверьте формат файла, его целостность или доступность кодеков."
                                       << "Путь:" << resourcePath;
                        } else if (status == QMediaPlayer::LoadedMedia) {
                            qDebug() << "MediaStatus: LoadedMedia - Длительность:" << m_mediaPlayer->duration() << "ms";
                        }
                    });

                } else {
                    qWarning() << "Аудиовыход недоступен, плеер для аудиовопроса не создан.";
                }
            }
        } else {
            qWarning() << "Медиафайл не найден в ресурсах (QFile::exists false):" << resourcePath;
        }
    }

    if (question.isMultimedia() && !mediaSuccessfullyDisplayed) {
        m_mediaLabel->setText(tr("Нет медиафайлов для этого вопроса."));
        m_mediaLabel->setFont(QFont("Arial", 14, QFont::StyleItalic));
    } else if (!question.isMultimedia()) {
        m_mediaLabel->hide();
        m_playPauseButton->hide();
        m_stopButton->hide();
        m_volumeSlider->hide();
        m_volumeLabel->hide();
    }

    m_answerEdit = new QLineEdit(this);
    m_answerEdit->setFont(QFont("Arial", 14));
    m_answerEdit->setPlaceholderText(tr("Введите ваш ответ здесь..."));

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);

    mainLayout->addWidget(questionLabel, 0);
    mainLayout->addWidget(m_mediaLabel, 1);
    mainLayout->addLayout(playerControlsLayout);
    mainLayout->addWidget(m_answerEdit);
    mainLayout->addWidget(buttonBox);

    m_answerEdit->setFocus();
}

QuestionDialog::~QuestionDialog() {
    if (m_mediaPlayer) {
        m_mediaPlayer->stop();
        delete m_mediaPlayer;
        m_mediaPlayer = nullptr;
    }
    m_audioOutput = nullptr;

    if (!m_tempMediaFilePath.isEmpty()) {
        QFile::remove(m_tempMediaFilePath);
        qDebug() << "Временный медиафайл удален:" << m_tempMediaFilePath;

        QDir tempDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/SvojaIgraTempMedia");
        if (tempDir.exists() && tempDir.isEmpty()) {
            tempDir.removeRecursively();
        }
    }
}

QString QuestionDialog::getAnswer() const
{
    return m_answerEdit->text();
}

void QuestionDialog::togglePlayback() {
    if (!m_mediaPlayer) {
        qDebug() << "togglePlayback: MediaPlayer не инициализирован. Нет аудио для воспроизведения.";
        return;
    }

    qDebug() << "togglePlayback: Текущее состояние плеера:" << m_mediaPlayer->playbackState();
    qDebug() << "togglePlayback: Текущий статус медиа:" << m_mediaPlayer->mediaStatus();

    if (m_mediaPlayer->mediaStatus() == QMediaPlayer::InvalidMedia) {
        qWarning() << "Невозможно воспроизвести: медиафайл помечен как InvalidMedia. Проверьте консоль для подробностей.";
        return;
    }

    if (m_mediaPlayer->playbackState() == QMediaPlayer::PlayingState) {
        m_mediaPlayer->pause();
        qDebug() << "togglePlayback: Пауза.";
    } else {
        if (m_mediaPlayer->mediaStatus() == QMediaPlayer::LoadedMedia ||
            m_mediaPlayer->mediaStatus() == QMediaPlayer::BufferedMedia ||
            m_mediaPlayer->mediaStatus() == QMediaPlayer::EndOfMedia)
        {
            m_mediaPlayer->play();
            qDebug() << "togglePlayback: Воспроизведение запущено.";
        } else if (m_mediaPlayer->mediaStatus() == QMediaPlayer::NoMedia) {
            qWarning() << "togglePlayback: Медиа не загружено (NoMedia). Проверьте файл и путь к ресурсу.";
        }
        else {
            qWarning() << "togglePlayback: Медиа не готово к воспроизведению. Текущий статус:" << m_mediaPlayer->mediaStatus();
        }
    }
}

void QuestionDialog::stopPlayback() {
    if (m_mediaPlayer) {
        m_mediaPlayer->stop();
        qDebug() << "stopPlayback: Остановлено.";
    }
}

void QuestionDialog::setVolume(int volume) {
    if (m_audioOutput) {
        m_audioOutput->setVolume(volume / 100.0f);
        m_volumeLabel->setText(QString(tr("Громкость: %1%")).arg(volume));
        qDebug() << "Громкость установлена на:" << volume << "%";
    }
}

void QuestionDialog::handleMediaPlayerError(QMediaPlayer::Error error) {
    qWarning() << "ОШИБКА QMediaPlayer:" << error << "Описание:" << m_mediaPlayer->errorString();
}

void QuestionDialog::handleMediaPlayerStateChanged(QMediaPlayer::PlaybackState state) {
    qDebug() << "Состояние воспроизведения изменилось на:" << state;
    if (state == QMediaPlayer::PlayingState) {
        m_playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    } else {
        m_playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    }
}