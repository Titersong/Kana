#include "homepage.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

HomePage::HomePage(QWidget *parent)
    : QWidget(parent)
{
    buildUi();
}

void HomePage::buildUi()
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(60, 60, 60, 60);
    root->setSpacing(32);
    root->addStretch();

    // ---------- TITLE ----------
    lblTitle = new QLabel("Kana");
    lblTitle->setAlignment(Qt::AlignCenter);
    lblTitle->setStyleSheet(
        "color:white;"
        "font-size:42pt;"
        "font-weight:bold;"
        );
    root->addWidget(lblTitle);

    lblSubtitle = new QLabel("Learn Hiragana & Katakana");
    lblSubtitle->setAlignment(Qt::AlignCenter);
    lblSubtitle->setStyleSheet(
        "color:#bbbbbb;"
        "font-size:14pt;"
        );
    root->addWidget(lblSubtitle);

    root->addSpacing(40);

    // ---------- BUTTON STYLE ----------
    auto btnStyle = QString(
        "QPushButton {"
        "  background:#2f2f2f;"
        "  color:white;"
        "  border-radius:16px;"
        "  padding:18px;"
        "  font-size:16pt;"
        "}"
        "QPushButton:hover { background:#3a3a3a; }"
        "QPushButton:pressed { background:#f7a027; color:black; }"
        );

    // ---------- BUTTONS ----------
    btnKana = new QPushButton("Kana Table");
    btnPractice = new QPushButton("Practice");
    btnStats = new QPushButton("Statistics");

    for (auto b : {btnKana, btnPractice, btnStats}) {
        b->setMinimumHeight(70);
        b->setStyleSheet(btnStyle);
        b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        root->addWidget(b);
    }

    root->addStretch();

    // ---------- SIGNALS ----------
    connect(btnKana, &QPushButton::clicked, this, &HomePage::openKanaTable);
    connect(btnPractice, &QPushButton::clicked, this, &HomePage::openPractice);
    connect(btnStats, &QPushButton::clicked, this, &HomePage::openStatistics);
}
