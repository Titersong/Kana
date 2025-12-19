#include "practicesessionpage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QRandomGenerator>
#include <QMap>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

// ---------- Styles ----------

static QString optionStyleNormal()
{
    return QStringLiteral(
        "QPushButton {"
        "  background:#333333;"
        "  color:white;"
        "  border-radius:12px;"
        "  padding:14px 18px;"
        "  font-size:15pt;"
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
        "  border-radius:12px;"
        "  padding:14px 18px;"
        "  font-size:15pt;"
        "}"
        );
}

static QString optionStyleWrong()
{
    return QStringLiteral(
        "QPushButton {"
        "  background:#c62828;"
        "  color:white;"
        "  border-radius:12px;"
        "  padding:14px 18px;"
        "  font-size:15pt;"
        "}"
        );
}


PracticeSessionPage::PracticeSessionPage(QWidget *parent)
    : QWidget(parent)
{
    progress = new ProgressManager(this);
    buildUi();
    buildKanaPool();
}


// Start Session
void PracticeSessionPage::startSession(const PracticeConfig &config)
{
    m_config = config;
    btnStop->setVisible(m_config.questionLimit == -1);
    m_pool.clear();

    if (m_config.source == PracticeConfig::Source::Mastered)
        loadMasteredFromStats();

    for (const auto &it : m_all)
    {
        if (m_config.script == PracticeConfig::Script::Hiragana && !it.isHiragana)
            continue;
        if (m_config.script == PracticeConfig::Script::Katakana && it.isHiragana)
            continue;

        if (m_config.source == PracticeConfig::Source::Mastered)
        {
            if (!m_masteredRomaji.contains(it.romaji))
                continue;
        }

        m_pool.append(it);
    }

    if (m_pool.isEmpty())
    {
        lblQuestion->setText("No mastered symbols yet");
        lblSubtitle->setText("Practice some symbols first");

        for (auto b : opt)
            b->hide();

        btnNext->hide();
        m_active = false;
        return;
    }

    m_questionIndex = 0;
    m_correctCount = 0;
    m_active = false;

    resultWidget->hide();
    btnHome->show();
    lblFeedback->show();
    lblQuestion->show();
    lblSubtitle->show();
    btnNext->show();

    for (auto b : opt)
        b->show();

    opacity->setOpacity(1.0);
    fadeOut->stop();
    fadeIn->stop();

    askQuestion();
}

// UI
void PracticeSessionPage::buildUi()
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(16);

    // Top bar
    auto *top = new QHBoxLayout();
    btnHome = new QPushButton("← Back");
    btnHome->setStyleSheet(
        "QPushButton { padding:6px 14px; background:#444;"
        "color:white; border-radius:6px; }"
        "QPushButton:hover { background:#555; }"
        );
    connect(btnHome, &QPushButton::clicked, this, &PracticeSessionPage::exitSession);

    top->addWidget(btnHome);
    top->addStretch();

    lblCounter = new QLabel("");
    lblCounter->setStyleSheet("color:#bbbbbb; font-size:11pt;");
    top->addWidget(lblCounter);

    root->addLayout(top);

    // Question label
    lblQuestion = new QLabel("...");
    QFont fq; fq.setPointSize(42); fq.setBold(true);
    lblQuestion->setFont(fq);
    lblQuestion->setAlignment(Qt::AlignCenter);
    lblQuestion->setStyleSheet("color:white;");
    root->addWidget(lblQuestion);

    lblSubtitle = new QLabel("");
    lblSubtitle->setAlignment(Qt::AlignCenter);
    lblSubtitle->setStyleSheet("color:#bbbbbb; font-size:13pt;");
    root->addWidget(lblSubtitle);

    // Fade animation
    opacity = new QGraphicsOpacityEffect(this);
    lblQuestion->setGraphicsEffect(opacity);

    fadeOut = new QPropertyAnimation(opacity, "opacity", this);
    fadeOut->setDuration(120);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);

    fadeIn = new QPropertyAnimation(opacity, "opacity", this);
    fadeIn->setDuration(160);
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);

    connect(fadeOut, &QPropertyAnimation::finished, this, [this]() {
        askQuestion();
        fadeIn->start();
    });

    // Grid of options
    auto *grid = new QGridLayout();
    grid->setSpacing(16);

    for (int i = 0; i < 4; ++i)
    {
        opt[i] = new QPushButton("...");
        opt[i]->setStyleSheet(optionStyleNormal());
        opt[i]->setMinimumHeight(65);
        opt[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        connect(opt[i], &QPushButton::clicked, this, [this, i]() {
            answer(i);
        });

        grid->addWidget(opt[i], i / 2, i % 2);
    }

    root->addLayout(grid);

    lblFeedback = new QLabel("");
    lblFeedback->setStyleSheet("color:white; font-size:12pt;");
    root->addWidget(lblFeedback);

    btnNext = new QPushButton("Next");
    btnNext->setEnabled(false);
    btnNext->setMinimumHeight(50);
    btnNext->setStyleSheet(
        "QPushButton { background:#555; color:white; border-radius:10px; font-size:13pt; }"
        "QPushButton:disabled { background:#333; color:#666; }"
        );
    connect(btnNext, &QPushButton::clicked, this, &PracticeSessionPage::nextQuestion);
    root->addWidget(btnNext);

    btnStop = new QPushButton("Stop");
    btnStop->setMinimumHeight(50);
    btnStop->setStyleSheet(
        "QPushButton { background:#8c2f2f; color:white; border-radius:10px; font-size:13pt; }"
        "QPushButton:hover { background:#a93a3a; }"
        );
    btnStop->hide();

    connect(btnStop, &QPushButton::clicked, this, &PracticeSessionPage::stopSession);

    root->addWidget(btnStop);

    // Result screen
    resultWidget = new QWidget(this);
    auto *r = new QVBoxLayout(resultWidget);

    lblResult = new QLabel("Finished");
    lblResult->setAlignment(Qt::AlignCenter);
    lblResult->setStyleSheet("font-size:24pt; color:white;");

    r->addStretch();
    r->addWidget(lblResult);
    r->addStretch();

    btnBack = new QPushButton("Back to Setup");
    btnBack->setStyleSheet(
        "QPushButton { background:#f7a027; color:black; padding:12px 24px;"
        "border-radius:14px; font-size:15pt; }"
        );
    btnBack->setMinimumHeight(55);

    r->addWidget(btnBack, 0, Qt::AlignCenter);
    connect(btnBack, &QPushButton::clicked, this, &PracticeSessionPage::backToSetup);

    resultWidget->hide();
    root->addWidget(resultWidget);
}


// Kana pool
void PracticeSessionPage::buildKanaPool()
{
    m_all.clear();

    auto addKana = [&](const QString &kana, bool hira)
    {
        if (kana.isEmpty()) return;
        QuizKanaItem it;
        it.kana = kana;
        it.romaji = romajiOf(kana);
        it.isHiragana = hira;
        if (!it.romaji.isEmpty())
            m_all.append(it);
    };

    // Hiragana
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

    auto addMatrix = [&](const QVector<QVector<QString>> &m, bool hira) {
        for (auto &row : m)
            for (auto &k : row)
                addKana(k, hira);
    };

    addMatrix(hira_gojuon, true);
    addMatrix(hira_dakuon, true);
    addMatrix(hira_handakuon, true);
    addMatrix(hira_yoon, true);

    // Katakana
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

// romajiOf
QString PracticeSessionPage::romajiOf(const QString &k)
{
    static QMap<QString, QString> R = {
        // Hiragana
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
        {"だ","da"},{"ぢ","ji(di)"},{"づ","zu(du)"},{"で","de"},{"ど","do"},
        {"ば","ba"},{"び","bi"},{"ぶ","bu"},{"べ","be"},{"ぼ","bo"},
        {"ぱ","pa"},{"ぴ","pi"},{"ぷ","pu"},{"ぺ","pe"},{"ぽ","po"},

        {"きゃ","kya"},{"きゅ","kyu"},{"きょ","kyo"},
        {"ぎゃ","gya"},{"ぎゅ","gyu"},{"ぎょ","gyo"},
        {"しゃ","sha"},{"しゅ","shu"},{"しょ","sho"},
        {"じゃ","ja"},{"じゅ","ju"},{"じょ","jo"},
        {"ちゃ","cha"},{"ちゅ","chu"},{"ちょ","cho"},
        {"ぢゃ","ja(chi)"},{"ぢゅ","ju(chi)"},{"ぢょ","jo(chi)"},
        {"にゃ","nya"},{"にゅ","nyu"},{"にょ","nyo"},
        {"ひゃ","hya"},{"ひゅ","hyu"},{"ひょ","hyo"},
        {"みゃ","mya"},{"みゅ","myu"},{"みょ","myo"},
        {"りゃ","rya"},{"りゅ","ryu"},{"りょ","ryo"},

        // Katakana
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
        {"ダ","da"},{"ヂ","ji(di)"},{"ヅ","zu(du)"},{"デ","de"},{"ド","do"},
        {"バ","ba"},{"ビ","bi"},{"ブ","bu"},{"ベ","be"},{"ボ","bo"},
        {"パ","pa"},{"ピ","pi"},{"プ","pu"},{"ペ","pe"},{"ポ","po"},

        {"キャ","kya"},{"キュ","kyu"},{"キョ","kyo"},
        {"ギャ","gya"},{"ギュ","gyu"},{"ギョ","gyo"},
        {"シャ","sha"},{"シュ","shu"},{"ショ","sho"},
        {"ジャ","ja"},{"ジュ","ju"},{"ジョ","jo"},
        {"チャ","cha"},{"チュ","chu"},{"チョ","cho"},
        {"ヂャ","ja(chi)"},{"ヂュ","ju(chi)"},{"ヂョ","jo(chi)"},
        {"ニャ","nya"},{"ニュ","nyu"},{"ニョ","nyo"},
        {"ヒャ","hya"},{"ヒュ","hyu"},{"ヒョ","hyo"},
        {"ミャ","mya"},{"ミュ","myu"},{"ミョ","myo"},
        {"リャ","rya"},{"リュ","ryu"},{"リョ","ryo"},
    };

    return R.value(k, "");
}
// Load mastered only
void PracticeSessionPage::loadMasteredFromStats()
{
    m_masteredRomaji.clear();

    QFile f("data/user_stats.json");
    if (!f.open(QIODevice::ReadOnly))
        return;

    auto doc = QJsonDocument::fromJson(f.readAll());
    f.close();

    auto obj = doc.object();

    auto readBlock = [&](const QJsonObject &o) {
        for (auto v : o["mastered"].toArray())
            m_masteredRomaji.insert(v.toString());
    };

    readBlock(obj["hiragana"].toObject());
    readBlock(obj["katakana"].toObject());
}

// Ask question
void PracticeSessionPage::askQuestion()
{
    if (m_config.questionLimit != -1 &&
        m_questionIndex >= m_config.questionLimit)
    {
        finishSession();
        return;
    }

    // reset UI
    for (auto &b : opt)
    {
        b->setEnabled(true);
        b->setStyleSheet(optionStyleNormal());
    }

    btnNext->setEnabled(false);
    lblFeedback->clear();

    m_current = m_pool[
        QRandomGenerator::global()->bounded(m_pool.size())
    ];

    if (m_config.mode == PracticeConfig::Mode::KanaToRomaji)
        m_showKana = true;
    else if (m_config.mode == PracticeConfig::Mode::RomajiToKana)
        m_showKana = false;
    else
        m_showKana = QRandomGenerator::global()->bounded(2);

    lblQuestion->setText(m_showKana ? m_current.kana : m_current.romaji);
    lblSubtitle->setText(m_showKana ? "Kana → Romaji" : "Romaji → Kana");

    QString correctText = m_showKana ? m_current.romaji : m_current.kana;

    m_correctIndex = QRandomGenerator::global()->bounded(4);

    QSet<QString> usedKana;
    QSet<QString> usedRomaji;

    usedKana.insert(m_current.kana);
    usedRomaji.insert(m_current.romaji);

    opt[m_correctIndex]->setText(correctText);

    auto pickDistractor = [&](const QVector<QuizKanaItem> &source,
                              QuizKanaItem &out) -> bool
    {
        for (int i = 0; i < 100; ++i)
        {
            const auto &it = source[
                QRandomGenerator::global()->bounded(source.size())
            ];

            if (it.kana == m_current.kana)
                continue;

            if (!m_showKana && it.isHiragana != m_current.isHiragana)
                continue;

            if (usedKana.contains(it.kana))
                continue;

            if (usedRomaji.contains(it.romaji))
                continue;

            out = it;
            return true;
        }
        return false;
    };

    for (int i = 0; i < 4; ++i)
    {
        if (i == m_correctIndex)
            continue;

        QuizKanaItem candidate;
        bool found = false;

        found = pickDistractor(m_pool, candidate);

        if (!found)
            found = pickDistractor(m_all, candidate);

        if (!found)
        {
            opt[i]->setText("—");
            continue;
        }

        usedKana.insert(candidate.kana);
        usedRomaji.insert(candidate.romaji);

        opt[i]->setText(
            m_showKana ? candidate.romaji : candidate.kana
            );
    }
    m_active = true;
    m_questionIndex++;

    if (m_config.questionLimit == -1)
        lblCounter->setText(QString::number(m_questionIndex));
    else
        lblCounter->setText(
            QString("%1 / %2")
                .arg(m_questionIndex)
                .arg(m_config.questionLimit)
            );
}


// Answer
void PracticeSessionPage::answer(int index)
{
    if (!m_active)
        return;

    m_active = false;

    bool correctAns = (index == m_correctIndex);

    for (auto &b : opt)
        b->setEnabled(false);

    if (correctAns)
    {
        opt[index]->setStyleSheet(optionStyleCorrect());
        lblFeedback->setText("Correct!");
        m_correctCount++;

        progress->addCorrect(m_current.isHiragana);
    }
    else
    {
        opt[index]->setStyleSheet(optionStyleWrong());
        opt[m_correctIndex]->setStyleSheet(optionStyleCorrect());

        lblFeedback->setText("Wrong. Correct: " + opt[m_correctIndex]->text());
        progress->addWrong(m_current.isHiragana);
    }
    progress->addAnswered(correctAns);
    btnNext->setEnabled(true);
}


// Next
void PracticeSessionPage::nextQuestion()
{
    fadeOut->start();
}


// Finish
void PracticeSessionPage::finishSession()
{
    btnHome->hide();
    lblFeedback->hide();
    btnStop->hide();

    for (auto b : opt)
        b->hide();

    btnNext->hide();
    lblQuestion->hide();
    lblSubtitle->hide();

    lblResult->setText(
        QString("🎉 Session finished 🎉\n\nCorrect: %1\nTotal: %2")
            .arg(m_correctCount)
            .arg(m_questionIndex)
        );

    resultWidget->show();
}

void PracticeSessionPage::stopSession()
{
    finishSession();
}

void PracticeSessionPage::exitSession()
{
    emit backToSetup();
}
