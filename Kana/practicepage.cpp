#include "practicepage.h"
#include "progressmanager.h"

#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QEasingCurve>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QRandomGenerator>
#include <QMap>

// ========== Вспомогательный стиль ==========
static QString optionStyleNormal()
{
    return QStringLiteral(
        "QPushButton {"
        "  background:#333333;"
        "  color:white;"
        "  border-radius:10px;"
        "  padding:8px 12px;"
        "  font-size:13pt;"
        "}"
        "QPushButton:disabled {"
        "  color:#888888;"
        "}"
        );
}

static QString optionStyleCorrect()
{
    return QStringLiteral(
        "QPushButton {"
        "  background:#2e7d32;"
        "  color:white;"
        "  border-radius:10px;"
        "  padding:8px 12px;"
        "  font-size:13pt;"
        "}"
        );
}

static QString optionStyleWrong()
{
    return QStringLiteral(
        "QPushButton {"
        "  background:#c62828;"
        "  color:white;"
        "  border-radius:10px;"
        "  padding:8px 12px;"
        "  font-size:13pt;"
        "}"
        );
}

// ===============================================
//               PracticePage
// ===============================================

PracticePage::PracticePage(QWidget *parent)
    : QWidget(parent)
{
    progress = new ProgressManager(this);

    buildUi();
    buildKanaPool();
}

// ---------------------------------
//              UI
// ---------------------------------
void PracticePage::buildUi()
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(12);

    // ---- Top bar ----
    auto *topBar = new QHBoxLayout();

    btnHome = new QPushButton("← Home");
    btnHome->setStyleSheet(
        "QPushButton { padding:6px 14px; background:#444;"
        "color:white; border-radius:6px; }"
        "QPushButton:hover { background:#555; }");
    connect(btnHome, &QPushButton::clicked, this, &PracticePage::goHome);
    topBar->addWidget(btnHome);

    topBar->addStretch();

    cmbMode = new QComboBox();
    cmbMode->addItems({
        "Kana → Romaji",
        "Romaji → Kana",
        "Mixed"
    });
    topBar->addWidget(cmbMode);

    cmbScript = new QComboBox();
    cmbScript->addItems({
        "Hiragana",
        "Katakana",
        "Both"
    });
    topBar->addWidget(cmbScript);

    btnStart = new QPushButton("Start");
    btnStart->setStyleSheet(
        "QPushButton { padding:6px 18px; background:#e68b0e;"
        "color:black; border-radius:8px; font-weight:bold; }"
        "QPushButton:hover { background:#f2a63a; }");
    connect(btnStart, &QPushButton::clicked, this, &PracticePage::startQuiz);
    topBar->addWidget(btnStart);

    root->addLayout(topBar);

    // ---- Question ----
    lblQuestion = new QLabel("Press Start");
    QFont f; f.setPointSize(42); f.setBold(true);
    lblQuestion->setFont(f);
    lblQuestion->setAlignment(Qt::AlignCenter);
    lblQuestion->setStyleSheet("color:white;");
    root->addWidget(lblQuestion);

    lblSubtitle = new QLabel("");
    lblSubtitle->setAlignment(Qt::AlignCenter);
    lblSubtitle->setStyleSheet("color:#bbbbbb; font-size:11pt;");
    root->addWidget(lblSubtitle);

    // ---- Opacity animation for question ----
    questionOpacity = new QGraphicsOpacityEffect(this);
    lblQuestion->setGraphicsEffect(questionOpacity);

    fadeOutAnim = new QPropertyAnimation(questionOpacity, "opacity", this);
    fadeOutAnim->setDuration(120);
    fadeOutAnim->setStartValue(1.0);
    fadeOutAnim->setEndValue(0.0);
    fadeOutAnim->setEasingCurve(QEasingCurve::OutQuad);

    fadeInAnim = new QPropertyAnimation(questionOpacity, "opacity", this);
    fadeInAnim->setDuration(160);
    fadeInAnim->setStartValue(0.0);
    fadeInAnim->setEndValue(1.0);
    fadeInAnim->setEasingCurve(QEasingCurve::OutQuad);


    connect(fadeOutAnim, &QPropertyAnimation::finished, this, [this]() {
        askQuestion();
        fadeInAnim->start();
    });

    // ---- Options ----
    auto *grid = new QGridLayout();
    grid->setSpacing(12);

    for (int i = 0; i < 4; ++i) {
        opt[i] = new QPushButton("…");
        opt[i]->setEnabled(false);
        opt[i]->setStyleSheet(optionStyleNormal());
        opt[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    }

    connect(opt[0], &QPushButton::clicked, this, &PracticePage::answer1);
    connect(opt[1], &QPushButton::clicked, this, &PracticePage::answer2);
    connect(opt[2], &QPushButton::clicked, this, &PracticePage::answer3);
    connect(opt[3], &QPushButton::clicked, this, &PracticePage::answer4);

    grid->addWidget(opt[0], 0, 0);
    grid->addWidget(opt[1], 0, 1);
    grid->addWidget(opt[2], 1, 0);
    grid->addWidget(opt[3], 1, 1);

    root->addLayout(grid);

    // ---- Bottom ----
    auto *bottom = new QHBoxLayout();

    lblFeedback = new QLabel("");
    lblFeedback->setStyleSheet("color:#cccccc; font-size:10pt;");
    bottom->addWidget(lblFeedback);

    bottom->addStretch();

    btnNext = new QPushButton("Next");
    btnNext->setEnabled(false);
    btnNext->setStyleSheet(
        "QPushButton { padding:4px 16px; background:#555;"
        "color:white; border-radius:8px; }"
        "QPushButton:disabled { background:#333; color:#666; }");
    connect(btnNext, &QPushButton::clicked, this, &PracticePage::nextQuestion);
    bottom->addWidget(btnNext);

    root->addLayout(bottom);
}

// ---------------------------------
//         Kana pool
// ---------------------------------
void PracticePage::buildKanaPool()
{
    m_all.clear();

    auto addKana = [&](const QString &kana, bool hira)
    {
        if (kana.isEmpty())
            return;

        QuizKanaItem it;
        it.kana = kana;
        it.romaji = romajiOf(kana);
        it.isHiragana = hira;

        if (!it.romaji.isEmpty())
            m_all.append(it);
    };

    // ---- Hiragana ----
    const QVector<QVector<QString>> hira_gojuon = {
        {"あ","い","う","え","お"},
        {"か","き","く","け","こ"},
        {"さ","し","す","せ","そ"},
        {"た","ち","つ","て","と"},
        {"な","に","ぬ","ね","の"},
        {"は","ひ","ふ","へ","ほ"},
        {"ま","み","む","め","も"},
        {"や","","ゆ","","よ"},
        {"ら","り","る","れ","ろ"},
        {"わ","","","","を"},
        {"","","ん","",""}
    };
    const QVector<QVector<QString>> hira_dakuon = {
        {"が","ぎ","ぐ","げ","ご"},
        {"ざ","じ","ず","ぜ","ぞ"},
        {"だ","ぢ","づ","で","ど"}
    };
    const QVector<QVector<QString>> hira_handakuon = {
        {"ば","び","ぶ","べ","ぼ"},
        {"ぱ","ぴ","ぷ","ぺ","ぽ"}
    };
    const QVector<QVector<QString>> hira_yoon = {
        {"きゃ","きゅ","きょ"},
        {"ぎゃ","ぎゅ","ぎょ"},
        {"しゃ","しゅ","しょ"},
        {"じゃ","じゅ","じょ"},
        {"ちゃ","ちゅ","ちょ"},
        {"にゃ","にゅ","にょ"},
        {"ひゃ","ひゅ","ひょ"},
        {"みゃ","みゅ","みょ"},
        {"りゃ","りゅ","りょ"}
    };

    auto addMatrix = [&](const QVector<QVector<QString>> &m, bool isHira) {
        for (const auto &row : m)
            for (const auto &k : row)
                addKana(k, isHira);
    };

    addMatrix(hira_gojuon, true);
    addMatrix(hira_dakuon, true);
    addMatrix(hira_handakuon, true);
    addMatrix(hira_yoon, true);

    // ---- Katakana ----
    const QVector<QVector<QString>> kata_gojuon = {
        {"ア","イ","ウ","エ","オ"},
        {"カ","キ","ク","ケ","コ"},
        {"サ","シ","ス","セ","ソ"},
        {"タ","チ","ツ","テ","ト"},
        {"ナ","ニ","ヌ","ネ","ノ"},
        {"ハ","ヒ","フ","ヘ","ホ"},
        {"マ","ミ","ム","メ","モ"},
        {"ヤ","","ユ","","ヨ"},
        {"ラ","リ","ル","レ","ロ"},
        {"ワ","","","","ヲ"},
        {"","","ン","",""}
    };
    const QVector<QVector<QString>> kata_dakuon = {
        {"ガ","ギ","グ","ゲ","ゴ"},
        {"ザ","ジ","ズ","ゼ","ゾ"},
        {"ダ","ヂ","ヅ","デ","ド"}
    };
    const QVector<QVector<QString>> kata_handakuon = {
        {"バ","ビ","ブ","ベ","ボ"},
        {"パ","ピ","プ","ペ","ポ"}
    };
    const QVector<QVector<QString>> kata_yoon = {
        {"キャ","キュ","キョ"},
        {"ギャ","ギュ","ギョ"},
        {"シャ","シュ","ショ"},
        {"ジャ","ジュ","ジョ"},
        {"チャ","チュ","チョ"},
        {"ニャ","ニュ","ニョ"},
        {"ヒャ","ヒュ","ヒョ"},
        {"ミャ","ミュ","ミョ"},
        {"リャ","リュ","リョ"}
    };

    addMatrix(kata_gojuon, false);
    addMatrix(kata_dakuon, false);
    addMatrix(kata_handakuon, false);
    addMatrix(kata_yoon, false);
}

// ---------------------------------
//          romajiOf (копия
//          логики из таблицы)
// ---------------------------------
QString PracticePage::romajiOf(const QString &k)
{
    static QMap<QString, QString> R = {
        {"あ","a"},{"い","i"},{"う","u"},{"え","e"},{"お","o"},
        {"か","ka"},{"き","ki"},{"く","ku"},{"け","ke"},{"こ","ko"},
        {"さ","sa"},{"し","shi"},{"す","su"},{"せ","se"},{"そ","so"},
        {"た","ta"},{"ち","chi"},{"つ","tsu"},{"て","te"},{"と","to"},
        {"な","na"},{"に","ni"},{"ぬ","nu"},{"ね","ne"},{"の","no"},
        {"は","ha"},{"ひ","hi"},{"ふ","fu"},{"へ","he"},{"ほ","ho"},
        {"ま","ma"},{"み","mi"},{"む","mu"},{"め","me"},{"も","mo"},
        {"や","ya"},{"ゆ","yu"},{"よ","yo"},
        {"ら","ra"},{"り","ri"},{"る","ru"},{"れ","re"},{"ろ","ro"},
        {"わ","wa"},{"を","wo"},{"ん","n"},

        {"が","ga"},{"ぎ","gi"},{"ぐ","gu"},{"げ","ge"},{"ご","go"},
        {"ざ","za"},{"じ","ji"},{"ず","zu"},{"ぜ","ze"},{"ぞ","zo"},
        {"だ","da"},{"ぢ","ji"},{"づ","zu"},{"で","de"},{"ど","do"},
        {"ば","ba"},{"び","bi"},{"ぶ","bu"},{"べ","be"},{"ぼ","bo"},
        {"ぱ","pa"},{"ぴ","pi"},{"ぷ","pu"},{"ぺ","pe"},{"ぽ","po"},

        {"きゃ","kya"},{"きゅ","kyu"},{"きょ","kyo"},
        {"ぎゃ","gya"},{"ぎゅ","gyu"},{"ぎょ","gyo"},
        {"しゃ","sha"},{"しゅ","shu"},{"しょ","sho"},
        {"じゃ","ja"},{"じゅ","ju"},{"じょ","jo"},
        {"ちゃ","cha"},{"ちゅ","chu"},{"ちょ","cho"},
        {"にゃ","nya"},{"にゅ","nyu"},{"にょ","nyo"},
        {"ひゃ","hya"},{"ひゅ","hyu"},{"ひょ","hyo"},
        {"みゃ","mya"},{"みゅ","myu"},{"みょ","myo"},
        {"りゃ","rya"},{"りゅ","ryu"},{"りょ","ryo"},

        {"ア","a"},{"イ","i"},{"ウ","u"},{"エ","e"},{"オ","o"},
        {"カ","ka"},{"キ","ki"},{"ク","ku"},{"ケ","ke"},{"コ","ko"},
        {"サ","sa"},{"シ","shi"},{"ス","su"},{"セ","se"},{"ソ","so"},
        {"タ","ta"},{"チ","chi"},{"ツ","tsu"},{"テ","te"},{"ト","to"},
        {"ナ","na"},{"ニ","ni"},{"ヌ","nu"},{"ネ","ne"},{"ノ","no"},
        {"ハ","ha"},{"ヒ","hi"},{"フ","fu"},{"ヘ","he"},{"ホ","ho"},
        {"マ","ma"},{"ミ","mi"},{"ム","mu"},{"メ","me"},{"モ","mo"},
        {"ヤ","ya"},{"ユ","yu"},{"ヨ","yo"},
        {"ラ","ra"},{"リ","ri"},{"ル","ru"},{"レ","re"},{"ロ","ro"},
        {"ワ","wa"},{"ヲ","wo"},{"ン","n"},

        {"ガ","ga"},{"ギ","gi"},{"グ","gu"},{"ゲ","ge"},{"ゴ","go"},
        {"ザ","za"},{"ジ","ji"},{"ズ","zu"},{"ゼ","ze"},{"ゾ","zo"},
        {"ダ","da"},{"ヂ","ji"},{"ヅ","zu"},{"デ","de"},{"ド","do"},
        {"バ","ba"},{"ビ","bi"},{"ブ","bu"},{"ベ","be"},{"ボ","bo"},
        {"パ","pa"},{"ピ","pi"},{"プ","pu"},{"ペ","pe"},{"ポ","po"},

        {"キャ","kya"},{"キュ","kyu"},{"キョ","kyo"},
        {"シャ","sha"},{"シュ","shu"},{"ショ","sho"},
        {"チャ","cha"},{"チュ","chu"},{"チョ","cho"},
        {"ニャ","nya"},{"ニュ","nyu"},{"ニョ","nyo"},
        {"ヒャ","hya"},{"ヒュ","hyu"},{"ヒョ","hyo"},
        {"ミャ","mya"},{"ミュ","myu"},{"ミョ","myo"},
        {"リャ","rya"},{"リュ","ryu"},{"リョ","ryo"}
    };

    return R.value(k, "");
}


// ---------------------------------
//            Quiz
// ---------------------------------
void PracticePage::startQuiz()
{
    m_mode = static_cast<Mode>(cmbMode->currentIndex());
    m_script = static_cast<Script>(cmbScript->currentIndex());

    m_pool.clear();
    for (const auto &it : m_all) {
        if (m_script == Script::Hiragana && !it.isHiragana) continue;
        if (m_script == Script::Katakana && it.isHiragana) continue;
        m_pool.append(it);
    }

    if (m_pool.size() < 4) {
        lblQuestion->setText("Not enough kana");
        return;
    }

    askQuestion();
}

void PracticePage::askQuestion()
{
    for (auto &b : opt) {
        b->setEnabled(true);
        b->setStyleSheet(optionStyleNormal());
    }

    btnNext->setEnabled(false);
    lblFeedback->clear();

    m_current = m_pool[QRandomGenerator::global()->bounded(m_pool.size())];
    m_showKana = (m_mode == Mode::Mixed)
                     ? QRandomGenerator::global()->bounded(2)
                     : (m_mode == Mode::KanaToRomaji);

    lblQuestion->setText(m_showKana ? m_current.kana : m_current.romaji);
    lblSubtitle->setText(m_showKana ? "Kana → Romaji" : "Romaji → Kana");

    QString correct = m_showKana ? m_current.romaji : m_current.kana;
    m_correctIndex = QRandomGenerator::global()->bounded(4);

    QStringList used{correct};

    for (int i = 0; i < 4; ++i) {
        if (i == m_correctIndex) {
            opt[i]->setText(correct);
            continue;
        }

        QString wrong;
        do {
            auto &it = m_pool[QRandomGenerator::global()->bounded(m_pool.size())];
            wrong = m_showKana ? it.romaji : it.kana;
        } while (used.contains(wrong));

        used << wrong;
        opt[i]->setText(wrong);
    }

    m_questionActive = true;
}

// ---------------------------------
//         Answer
// ---------------------------------
void PracticePage::handleAnswer(int index)
{
    if (!m_questionActive)
        return;

    m_questionActive = false;
    bool correct = (index == m_correctIndex);

    for (auto &b : opt)
        b->setEnabled(false);

    if (correct) {
        opt[index]->setStyleSheet(optionStyleCorrect());

        // scale animation
        auto *anim = new QPropertyAnimation(opt[index], "geometry", this);
        QRect r = opt[index]->geometry();
        anim->setDuration(120);
        anim->setStartValue(r);
        anim->setKeyValueAt(0.5, QRect(r.adjusted(-4, -4, 4, 4)));
        anim->setEndValue(r);
        anim->setEasingCurve(QEasingCurve::OutBack);
        anim->start(QAbstractAnimation::DeleteWhenStopped);

        lblFeedback->setText("Correct!");

        progress->addCorrect(m_current.isHiragana);
        if (progress->getStreak(m_current.isHiragana) >= 3)
            progress->markMastered(m_current.isHiragana, m_current.romaji);
    }
    else {
        opt[index]->setStyleSheet(optionStyleWrong());
        opt[m_correctIndex]->setStyleSheet(optionStyleCorrect());

        lblFeedback->setText("Wrong. Correct: " + opt[m_correctIndex]->text());

        progress->addWrong(m_current.isHiragana);
    }

    for (int i = 0; i < 4; ++i) {
        if (i != m_correctIndex && (!correct && i != index)) {
            opt[i]->setStyleSheet(
                "QPushButton { background:#222; color:#666; border-radius:10px; }"
                );
        }
    }

    progress->addAnswered(correct);
    btnNext->setEnabled(true);
}

void PracticePage::answer1(){ handleAnswer(0); }
void PracticePage::answer2(){ handleAnswer(1); }
void PracticePage::answer3(){ handleAnswer(2); }
void PracticePage::answer4(){ handleAnswer(3); }

void PracticePage::nextQuestion()
{
    fadeOutAnim->start();
}
