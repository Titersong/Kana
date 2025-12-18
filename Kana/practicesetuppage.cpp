#include "practicesetuppage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

// ---------- Styles ----------
static QString toggleStyle(bool active)
{
    return QString(
               "QPushButton {"
               " background:%1;"
               " color:white;"
               " border-radius:10px;"
               " padding:10px;"
               " font-size:12pt;"
               "}"
               ).arg(active ? "#e68b0e" : "#333");
}

// --------------------------------
PracticeSetupPage::PracticeSetupPage(QWidget *parent)
    : QWidget(parent)
{
    buildUi();
}

// --------------------------------
void PracticeSetupPage::buildUi()
{
    auto *root = new QVBoxLayout(this);
    root->setSpacing(16);
    root->setContentsMargins(24, 24, 24, 24);
    auto *homeRow = new QHBoxLayout();
    QPushButton *btnHome = new QPushButton("← Home");

    btnHome->setStyleSheet(
        "QPushButton { padding:6px 14px; background:#444;"
        "color:white; border-radius:6px; }"
        "QPushButton:hover { background:#555; }"
        );

    connect(btnHome, &QPushButton::clicked, this, [this]() {
        emit goHome();
    });

    homeRow->addWidget(btnHome);
    homeRow->addStretch();

    root->addLayout(homeRow);
    // ---- Title ----
    auto *title = new QLabel("Practice");
    title->setStyleSheet("color:white; font-size:26pt; font-weight:bold;");
    title->setAlignment(Qt::AlignCenter);
    root->addWidget(title);

    // ---- MODE ----
    auto *modeLabel = new QLabel("Mode");
    modeLabel->setStyleSheet("color:#aaa;");
    root->addWidget(modeLabel);

    auto *modeRow = new QHBoxLayout();
    QStringList modes = { "Kana → Romaji", "Romaji → Kana", "Mixed" };

    for (int i = 0; i < 3; ++i) {
        btnMode[i] = new QPushButton(modes[i]);
        modeRow->addWidget(btnMode[i]);

        connect(btnMode[i], &QPushButton::clicked, this, [=]() {
            m_config.mode = static_cast<PracticeConfig::Mode>(i);
            updateButtonStates();
        });
    }
    root->addLayout(modeRow);

    // ---- SCRIPT ----
    auto *scriptLabel = new QLabel("Script");
    scriptLabel->setStyleSheet("color:#aaa;");
    root->addWidget(scriptLabel);

    auto *scriptRow = new QHBoxLayout();
    QStringList scripts = { "Hiragana", "Katakana", "Both" };

    for (int i = 0; i < 3; ++i) {
        btnScript[i] = new QPushButton(scripts[i]);
        scriptRow->addWidget(btnScript[i]);

        connect(btnScript[i], &QPushButton::clicked, this, [=]() {
            m_config.script = static_cast<PracticeConfig::Script>(i);
            updateButtonStates();
        });
    }
    root->addLayout(scriptRow);

    // ---- SOURCE ----
    auto *sourceLabel = new QLabel("Practice set");
    sourceLabel->setStyleSheet("color:#aaa;");
    root->addWidget(sourceLabel);

    auto *sourceRow = new QHBoxLayout();

    btnSourceAll = new QPushButton("All");
    btnSourceMastered = new QPushButton("Mastered");

    sourceRow->addWidget(btnSourceAll);
    sourceRow->addWidget(btnSourceMastered);
    root->addLayout(sourceRow);

    connect(btnSourceAll, &QPushButton::clicked, this, [this]() {
        m_config.source = PracticeConfig::Source::All;
        updateButtonStates();
    });

    connect(btnSourceMastered, &QPushButton::clicked, this, [this]() {
        m_config.source = PracticeConfig::Source::Mastered;
        updateButtonStates();
    });

    // ---- QUESTION COUNT ----
    auto *countLabel = new QLabel("Questions");
    countLabel->setStyleSheet("color:#aaa;");
    root->addWidget(countLabel);

    auto *countRow = new QHBoxLayout();
    QList<int> counts = { 10, 20, 50, -1 };
    QStringList labels = { "10", "20", "50", "Infinite" };

    for (int i = 0; i < 4; ++i) {
        btnCount[i] = new QPushButton(labels[i]);
        countRow->addWidget(btnCount[i]);

        connect(btnCount[i], &QPushButton::clicked, this, [=]() {
            m_config.questionLimit = counts[i];
            updateButtonStates();
        });
    }
    root->addLayout(countRow);

    // ---- START ----
    btnStart = new QPushButton("Start Practice");
    btnStart->setStyleSheet(
        "QPushButton { background:#e68b0e; color:black;"
        " padding:12px; border-radius:12px; font-size:14pt; font-weight:bold; }"
        );

    connect(btnStart, &QPushButton::clicked, this, [this]() {
        emit startPractice(m_config);
    });

    root->addStretch();
    root->addWidget(btnStart);

    updateButtonStates();
}

// --------------------------------
void PracticeSetupPage::updateButtonStates()
{
    for (int i = 0; i < 3; ++i)
        btnMode[i]->setStyleSheet(toggleStyle(i == (int)m_config.mode));

    for (int i = 0; i < 3; ++i)
        btnScript[i]->setStyleSheet(toggleStyle(i == (int)m_config.script));

    QList<int> counts = { 10, 20, 50, -1 };
    for (int i = 0; i < 4; ++i)
        btnCount[i]->setStyleSheet(
            toggleStyle(m_config.questionLimit == counts[i])
            );
    btnSourceAll->setStyleSheet(
        toggleStyle(m_config.source == PracticeConfig::Source::All));

    btnSourceMastered->setStyleSheet(
        toggleStyle(m_config.source == PracticeConfig::Source::Mastered));
}

