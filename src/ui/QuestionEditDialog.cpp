#include "QuestionEditDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QCoreApplication>
#include <QTabWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

QuestionEditDialog::QuestionEditDialog(MultilingualQuestionData &questionData, QWidget *parent)
    : QDialog(parent), m_questionData(questionData)
{
    setupUi();
    loadData();
    setWindowTitle(m_questionData.category.isEmpty() ? tr("Создание нового вопроса") : tr("Редактирование вопроса"));
}

bool QuestionEditDialog::isMarkedForDeletion() const {
    return m_markedForDeletion;
}

void QuestionEditDialog::setupUi() {
    setMinimumSize(700, 500);
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Вкладки для языков
    QTabWidget* langTabs = new QTabWidget(this);
    QStringList languages = {"ru", "en", "be"}; // Ваши языки

    for(const QString& lang : languages) {
        QWidget* tabWidget = new QWidget();
        QFormLayout* tabLayout = new QFormLayout(tabWidget);
        tabLayout->setSpacing(10);

        QLabel* questionLabel = new QLabel(tr("Текст вопроса:"), tabWidget);
        m_questionEdits[lang] = new QTextEdit(tabWidget);
        m_questionEdits[lang]->setMinimumHeight(100);

        QLabel* answerLabel = new QLabel(tr("Ответ:"), tabWidget);
        m_answerEdits[lang] = new QLineEdit(tabWidget);

        tabLayout->addRow(questionLabel, m_questionEdits[lang]);
        tabLayout->addRow(answerLabel, m_answerEdits[lang]);
        langTabs->addTab(tabWidget, lang.toUpper());
    }

    mainLayout->addWidget(langTabs);

    // Общие настройки вопроса
    QFormLayout* generalLayout = new QFormLayout();
    generalLayout->setSpacing(10);

    m_typeComboBox = new QComboBox(this);
    m_typeComboBox->addItem(tr("Текст"), QVariant::fromValue(Question::TextOnly));
    m_typeComboBox->addItem(tr("Текст и Изображение"), QVariant::fromValue(Question::ImageAndText));
    m_typeComboBox->addItem(tr("Текст и Аудио"), QVariant::fromValue(Question::SoundAndText));

    QHBoxLayout* mediaLayout = new QHBoxLayout();
    m_uploadMediaButton = new QPushButton(tr("Загрузить..."), this);
    m_mediaPathLabel = new QLabel(tr("Файл не выбран"), this);
    mediaLayout->addWidget(m_uploadMediaButton);
    mediaLayout->addWidget(m_mediaPathLabel);
    mediaLayout->addStretch();

    generalLayout->addRow(tr("Тип вопроса:"), m_typeComboBox);
    generalLayout->addRow(tr("Медиафайл:"), mediaLayout);

    mainLayout->addLayout(generalLayout);

    // Кнопки управления
    QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
    QPushButton* saveButton = buttonBox->addButton(tr("Сохранить"), QDialogButtonBox::AcceptRole);
    QPushButton* cancelButton = buttonBox->addButton(tr("Отмена"), QDialogButtonBox::RejectRole);
    QPushButton* deleteButton = new QPushButton(tr("Удалить вопрос"), this);
    deleteButton->setStyleSheet("background-color: #ff4d4d; color: white;");

    // Если это новый вопрос (очки = 0), кнопку удаления не показываем
    if (m_questionData.points == 0) {
        deleteButton->hide();
    }

    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(deleteButton);
    bottomLayout->addStretch();
    bottomLayout->addWidget(buttonBox);
    mainLayout->addLayout(bottomLayout);

    // Подключения
    connect(saveButton, &QPushButton::clicked, this, &QuestionEditDialog::saveChanges);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(deleteButton, &QPushButton::clicked, this, &QuestionEditDialog::markForDeletion);
    connect(m_uploadMediaButton, &QPushButton::clicked, this, &QuestionEditDialog::uploadMedia);
    connect(m_typeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &QuestionEditDialog::handleTypeChanged);

    // Инициализация состояния кнопки загрузки
    handleTypeChanged(m_typeComboBox->currentIndex());
}

void QuestionEditDialog::loadData() {
    for (auto it = m_questionEdits.constBegin(); it != m_questionEdits.constEnd(); ++it) {
        it.value()->setText(m_questionData.questionTexts.value(it.key()));
    }
    for (auto it = m_answerEdits.constBegin(); it != m_answerEdits.constEnd(); ++it) {
        it.value()->setText(m_questionData.answerTexts.value(it.key()));
    }

    int typeIndex = m_typeComboBox->findData(QVariant::fromValue(m_questionData.type));
    if (typeIndex != -1) {
        m_typeComboBox->setCurrentIndex(typeIndex);
    }
    m_mediaPathLabel->setText(m_questionData.mediaPath.isEmpty() ? tr("Файл не выбран") : m_questionData.mediaPath);
}

void QuestionEditDialog::saveChanges() {
    // Сохраняем данные из полей ввода обратно в структуру
    for (auto it = m_questionEdits.constBegin(); it != m_questionEdits.constEnd(); ++it) {
        m_questionData.questionTexts[it.key()] = it.value()->toPlainText();
    }
    for (auto it = m_answerEdits.constBegin(); it != m_answerEdits.constEnd(); ++it) {
        m_questionData.answerTexts[it.key()] = it.value()->text();
    }

    m_questionData.type = m_typeComboBox->currentData().value<Question::QuestionType>();
    m_questionData.mediaPath = m_mediaPathLabel->text();
    if (m_questionData.mediaPath == tr("Файл не выбран")) {
        m_questionData.mediaPath = "";
    }

    // Проверка на заполненность
    if (m_questionData.questionTexts["ru"].isEmpty() || m_questionData.answerTexts["ru"].isEmpty()) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Текст вопроса и ответ на основном языке (RU) должны быть заполнены."));
        return;
    }

    accept(); // Закрываем диалог с флагом "Принято"
}

void QuestionEditDialog::uploadMedia() {
    Question::QuestionType type = m_typeComboBox->currentData().value<Question::QuestionType>();
    QString filter;
    if (type == Question::ImageAndText) {
        filter = tr("Изображения (*.png *.jpg *.jpeg *.bmp)");
    } else if (type == Question::SoundAndText) {
        filter = tr("Аудиофайлы (*.mp3 *.wav *.ogg)");
    }

    // Путь к исполняемому файлу, чтобы от него искать папку resources
    QString resourcesDir = QCoreApplication::applicationDirPath() + "/../resources/media/";
    QString filePath = QFileDialog::getOpenFileName(this, tr("Выбрать медиафайл"), resourcesDir, filter);

    if (!filePath.isEmpty()) {
        // Мы хотим хранить относительный путь
        QDir resourcesDirObj(resourcesDir);
        QString relativePath = resourcesDirObj.relativeFilePath(filePath);
        m_mediaPathLabel->setText(relativePath);
        qDebug() << "Выбран файл:" << filePath;
        qDebug() << "Сохранен относительный путь:" << relativePath;
    }
}

void QuestionEditDialog::handleTypeChanged(int index) {
    Question::QuestionType type = m_typeComboBox->itemData(index).value<Question::QuestionType>();
    m_uploadMediaButton->setEnabled(type != Question::TextOnly);
}

void QuestionEditDialog::markForDeletion() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Удаление вопроса"),
                                  tr("Вы уверены, что хотите безвозвратно удалить этот вопрос?"),
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        m_markedForDeletion = true;
        accept();
    }
}