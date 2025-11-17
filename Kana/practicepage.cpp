#include "practicepage.h"
#include <QLabel>
#include <QVBoxLayout>

PracticePage::PracticePage(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Practice Screen (WIP)", this));
}
