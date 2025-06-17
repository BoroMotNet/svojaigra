#include "AdminEditor.h"
#include <QPushButton>

AdminEditor::AdminEditor(QWidget *parent) : QWidget(parent)
{
    setWindowTitle(tr("Редактор вопросов"));
    resize(800, 600);
}