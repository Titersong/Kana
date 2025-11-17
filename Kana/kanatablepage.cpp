#include "kanatablepage.h"
#include <QLabel>
#include <QVBoxLayout>

KanaTablePage::KanaTablePage(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Kana Table Screen (WIP)", this));
}
