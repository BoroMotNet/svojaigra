#ifndef ADMINEDITOR_H
#define ADMINEDITOR_H

#include <QDialog>
#include <QMap>
#include "QuestionEditDialog.h"

#include <QCloseEvent>

namespace Ui {
    class AdminEditor;
}

class QGridLayout;
class QPushButton;
class QScrollArea;

class AdminEditor : public QDialog
{
    Q_OBJECT

public:
    explicit AdminEditor(QWidget *parent = nullptr);
    ~AdminEditor() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void openQuestionEditor(const QString& category, int points);
    void addNewQuestion(const QString& category);
    void saveAllChanges();
    void handleExit();

private:
    void setupUi();
    void loadAllQuestions();
    void populateBoard();
    QWidget* createQuestionWidget(const QString& category, int points);

    Ui::AdminEditor *ui;

    QScrollArea* m_scrollArea;
    QPushButton* m_saveButton;
    QPushButton* m_exitButton;

    QMap<QString, QMap<int, MultilingualQuestionData>> m_allQuestions;

    bool m_isDirty;
};

#endif