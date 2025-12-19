#include "statisticspage.h"
#include "wordapiservice.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QPushButton>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>


StatisticsPage::StatisticsPage(QWidget *parent)
    : QWidget(parent)
{
    buildUi();
}


void StatisticsPage::buildUi()
{
    auto *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto *content = new QWidget();
    scroll->setWidget(content);

    rootLayout = new QVBoxLayout(content);
    rootLayout->setContentsMargins(48, 48, 48, 48);
    rootLayout->setSpacing(24);

    auto *pageLayout = new QVBoxLayout(this);
    pageLayout->addWidget(scroll);
    setLayout(pageLayout);

    // Top bar
    auto *top = new QHBoxLayout();
    btnHome = new QPushButton("← Home");
    btnHome->setStyleSheet(
        "QPushButton { background:#333; color:white; padding:6px 14px;"
        "border-radius:8px; font-size:11pt; }"
        "QPushButton:hover { background:#444; }"
        );
    connect(btnHome, &QPushButton::clicked, this, &StatisticsPage::goHome);

    top->addWidget(btnHome);
    top->addStretch();
    rootLayout->addLayout(top);

    auto *title = new QLabel("Statistics");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color:white; font-size:30pt; font-weight:bold;");
    rootLayout->addWidget(title);

    // Summary
    auto *summaryTitle = new QLabel("Summary");
    summaryTitle->setStyleSheet("color:#f7a027; font-size:18pt; font-weight:bold;");
    rootLayout->addWidget(summaryTitle);

    lblTotal = new QLabel();
    lblAccuracy = new QLabel();
    rootLayout->addWidget(lblTotal);
    rootLayout->addWidget(lblAccuracy);

    // Hiragana
    auto *hiraTitle = new QLabel("Hiragana");
    hiraTitle->setStyleSheet("color:#ffaa00; font-size:18pt; font-weight:bold;");
    rootLayout->addWidget(hiraTitle);

    lblHiraCorrect = new QLabel();
    lblHiraWrong   = new QLabel();
    lblHiraStreak  = new QLabel();
    hiraMasteredWidget = new QWidget();

    rootLayout->addWidget(lblHiraCorrect);
    rootLayout->addWidget(lblHiraWrong);
    rootLayout->addWidget(lblHiraStreak);
    rootLayout->addWidget(hiraMasteredWidget);

    // Katakana
    auto *kataTitle = new QLabel("Katakana");
    kataTitle->setStyleSheet("color:#4fc3f7; font-size:18pt; font-weight:bold;");
    rootLayout->addWidget(kataTitle);

    lblKataCorrect = new QLabel();
    lblKataWrong   = new QLabel();
    lblKataStreak  = new QLabel();
    kataMasteredWidget = new QWidget();

    rootLayout->addWidget(lblKataCorrect);
    rootLayout->addWidget(lblKataWrong);
    rootLayout->addWidget(lblKataStreak);
    rootLayout->addWidget(kataMasteredWidget);

    rootLayout->addStretch();

    // Word of the Day
    auto *wordTitle = new QLabel("Word of the Day");
    wordTitle->setStyleSheet("color:#8bc34a; font-size:18pt; font-weight:bold;");
    rootLayout->addWidget(wordTitle);

    // Card
    wordCard = new QWidget();
    auto *cardLayout = new QVBoxLayout(wordCard);
    cardLayout->setSpacing(8);

    lblWordKana = new QLabel("—");
    lblWordKana->setAlignment(Qt::AlignCenter);
    lblWordKana->setStyleSheet("font-size:36pt; color:white;");

    lblWordRomaji = new QLabel();
    lblWordRomaji->setAlignment(Qt::AlignCenter);
    lblWordRomaji->setStyleSheet("font-size:14pt; color:#bbbbbb;");

    lblWordMeaning = new QLabel();
    lblWordMeaning->setAlignment(Qt::AlignCenter);
    lblWordMeaning->setWordWrap(true);
    lblWordMeaning->setStyleSheet("font-size:13pt;");

    btnNewWord = new QPushButton("New word");
    btnNewWord->setStyleSheet(
        "QPushButton { background:#444; color:white; padding:6px 14px;"
        "border-radius:8px; }"
        "QPushButton:hover { background:#555; }"
        );

    cardLayout->addWidget(lblWordKana);
    cardLayout->addWidget(lblWordRomaji);
    cardLayout->addWidget(lblWordMeaning);
    cardLayout->addSpacing(6);
    cardLayout->addWidget(btnNewWord, 0, Qt::AlignCenter);

    wordCard->setStyleSheet(
        "background:#2b2b2b;"
        "border-radius:16px;"
        "padding:16px;"
        );

    rootLayout->addWidget(wordCard);

    wordService = new WordApiService(this);

    connect(wordService, &WordApiService::wordReady,
            this, [this](const QString& kana,
                   const QString& romaji,
                   const QString& meaning)
            {
                lblWordKana->setText(kana);
                lblWordRomaji->setText(romaji);
                lblWordMeaning->setText(meaning);
            });

    connect(btnNewWord, &QPushButton::clicked, this, [this]() {
        lblWordKana->setText("…");
        lblWordRomaji->setText("");
        lblWordMeaning->setText("Loading new word...");
        wordService->fetchWord();
    });

    for (auto *lbl : content->findChildren<QLabel*>())
    {
        if (lbl == title || lbl == summaryTitle || lbl == hiraTitle || lbl == kataTitle)
            continue;

        lbl->setStyleSheet("color:white; font-size:14pt;");
        lbl->setWordWrap(true);
    }
}

// Load stats
void StatisticsPage::loadStats()
{
    QFile f("data/user_stats.json");
    if (!f.open(QIODevice::ReadOnly))
        return;

    auto doc = QJsonDocument::fromJson(f.readAll());
    f.close();

    auto obj  = doc.object();
    auto hira = obj["hiragana"].toObject();
    auto kata = obj["katakana"].toObject();
    auto pr   = obj["practice"].toObject();

    int totalAnswered = pr["totalAnswered"].toInt();
    int totalCorrect  = pr["totalCorrect"].toInt();
    double acc = totalAnswered ? (100.0 * totalCorrect / totalAnswered) : 0;

    lblTotal->setText("Total answered: " + QString::number(totalAnswered));
    lblAccuracy->setText("Accuracy: " + QString::number(acc, 'f', 1) + "%");

    lblHiraCorrect->setText("Correct: " + QString::number(hira["correct"].toInt()));
    lblHiraWrong->setText("Wrong: " + QString::number(hira["wrong"].toInt()));
    lblHiraStreak->setText("Streak: " + QString::number(hira["streak"].toInt()));

    QStringList hm;
    for (auto v : hira["mastered"].toArray()) hm << v.toString();

    delete hiraMasteredWidget->layout();
    auto *hiraLayout = new QHBoxLayout(hiraMasteredWidget);
    hiraLayout->addWidget(createMasteredRow(hm, true));

    lblKataCorrect->setText("Correct: " + QString::number(kata["correct"].toInt()));
    lblKataWrong->setText("Wrong: " + QString::number(kata["wrong"].toInt()));
    lblKataStreak->setText("Streak: " + QString::number(kata["streak"].toInt()));

    QStringList km;
    for (auto v : kata["mastered"].toArray()) km << v.toString();

    delete kataMasteredWidget->layout();
    auto *kataLayout = new QHBoxLayout(kataMasteredWidget);
    kataLayout->addWidget(createMasteredRow(km, false));
    wordService->fetchWord();
}

// Mastered situation
QWidget* StatisticsPage::createMasteredRow(const QStringList& list, bool hira)
{
    auto *row = new QWidget();
    auto *l = new QHBoxLayout(row);
    l->setSpacing(12);

    for (const auto& r : list)
        l->addWidget(createMasteredCard(kanaFromRomaji(r, hira), r));

    l->addStretch();
    return row;
}

QWidget* StatisticsPage::createMasteredCard(const QString& kana, const QString& romaji)
{
    auto *w = new QWidget();
    auto *v = new QVBoxLayout(w);

    auto *k = new QLabel(kana);
    k->setAlignment(Qt::AlignCenter);
    k->setStyleSheet("font-size:26pt; color:white;");

    auto *r = new QLabel(romaji);
    r->setAlignment(Qt::AlignCenter);
    r->setStyleSheet("font-size:10pt; color:#bbbbbb;");

    v->addWidget(k);
    v->addWidget(r);

    w->setStyleSheet(
        "background:#2b2b2b;"
        "border-radius:12px;"
        "padding:8px;"
        );
    return w;
}

// Romaji to Kana
QString StatisticsPage::kanaFromRomaji(const QString& r, bool hira)
{
    static const QMap<QString, QString> H = {
                                             // Hiragana
                                             {"a","あ"},{"i","い"},{"u","う"},{"e","え"},{"o","お"},
                                             {"ka","か"},{"ki","き"},{"ku","く"},{"ke","け"},{"ko","こ"},
                                             {"sa","さ"},{"shi","し"},{"su","す"},{"se","せ"},{"so","そ"},
                                             {"ta","た"},{"chi","ち"},{"tsu","つ"},{"te","て"},{"to","と"},
                                             {"na","な"},{"ni","に"},{"nu","ぬ"},{"ne","ね"},{"no","の"},
                                             {"ha","は"},{"hi","ひ"},{"fu","ふ"},{"he","へ"},{"ho","ほ"},
                                             {"ma","ま"},{"mi","み"},{"mu","む"},{"me","め"},{"mo","も"},
                                             {"ya","や"},{"yu","ゆ"},{"yo","よ"},
                                             {"ra","ら"},{"ri","り"},{"ru","る"},{"re","れ"},{"ro","ろ"},
                                             {"wa","わ"},{"wo","を"},{"n","ん"},

                                             {"ga","が"},{"gi","ぎ"},{"gu","ぐ"},{"ge","げ"},{"go","ご"},
                                             {"za","ざ"},{"ji","じ"},{"zu","ず"},{"ze","ぜ"},{"zo","ぞ"},
                                             {"da","だ"},{"ji(di)","ぢ"},{"zu(du)","づ"},{"de","で"},{"do","ど"},

                                             {"ba","ば"},{"bi","び"},{"bu","ぶ"},{"be","べ"},{"bo","ぼ"},
                                             {"pa","ぱ"},{"pi","ぴ"},{"pu","ぷ"},{"pe","ぺ"},{"po","ぽ"},

                                             {"kya","きゃ"},{"kyu","きゅ"},{"kyo","きょ"},
                                             {"gya","ぎゃ"},{"gyu","ぎゅ"},{"gyo","ぎょ"},
                                             {"sha","しゃ"},{"shu","しゅ"},{"sho","しょ"},
                                             {"ja","じゃ"},{"ju","じゅ"},{"jo","じょ"},
                                             {"cha","ちゃ"},{"chu","ちゅ"},{"cho","ちょ"},
                                             {"ja(chi)","ぢゃ"},{"ju(chi)","ぢゅ"},{"jo(chi)","ぢょ"},
                                             {"nya","にゃ"},{"nyu","にゅ"},{"nyo","にょ"},
                                             {"hya","ひゃ"},{"hyu","ひゅ"},{"hyo","ひょ"},
                                             {"mya","みゃ"},{"myu","みゅ"},{"myo","みょ"},
                                             {"rya","りゃ"},{"ryu","りゅ"},{"ryo","りょ"},
                                             };

    static const QMap<QString, QString> K = {
                                             // Katakana
                                             {"a","ア"},{"i","イ"},{"u","ウ"},{"e","エ"},{"o","オ"},
                                             {"ka","カ"},{"ki","キ"},{"ku","ク"},{"ke","ケ"},{"ko","コ"},
                                             {"sa","サ"},{"shi","シ"},{"su","ス"},{"se","セ"},{"so","ソ"},
                                             {"ta","タ"},{"chi","チ"},{"tsu","ツ"},{"te","テ"},{"to","ト"},
                                             {"na","ナ"},{"ni","ニ"},{"nu","ヌ"},{"ne","ネ"},{"no","ノ"},
                                             {"ha","ハ"},{"hi","ヒ"},{"fu","フ"},{"he","ヘ"},{"ho","ホ"},
                                             {"ma","マ"},{"mi","ミ"},{"mu","ム"},{"me","メ"},{"mo","モ"},
                                             {"ya","ヤ"},{"yu","ユ"},{"yo","ヨ"},
                                             {"ra","ラ"},{"ri","リ"},{"ru","ル"},{"re","レ"},{"ro","ロ"},
                                             {"wa","ワ"},{"wo","ヲ"},{"n","ン"},

                                             {"ga","ガ"},{"gi","ギ"},{"gu","グ"},{"ge","ゲ"},{"go","ゴ"},
                                             {"za","ザ"},{"ji","ジ"},{"zu","ズ"},{"ze","ゼ"},{"zo","ゾ"},
                                             {"da","ダ"},{"ji(di)","ヂ"},{"zu(du)","ヅ"},{"de","デ"},{"do","ド"},

                                             {"ba","バ"},{"bi","ビ"},{"bu","ブ"},{"be","ベ"},{"bo","ボ"},
                                             {"pa","パ"},{"pi","ピ"},{"pu","プ"},{"pe","ペ"},{"po","ポ"},

                                             {"kya","キャ"},{"kyu","キュ"},{"kyo","キョ"},
                                             {"gya","ギャ"},{"gyu","ギュ"},{"gyo","ギョ"},
                                             {"sha","シャ"},{"shu","シュ"},{"sho","ショ"},
                                             {"ja","ジャ"},{"ju","ジュ"},{"jo","ジョ"},
                                             {"cha","チャ"},{"chu","チュ"},{"cho","チョ"},
                                             {"ja(chi)","ヂャ"},{"ju(chi)","ヂュ"},{"jo(chi)","ヂョ"},
                                             {"nya","ニャ"},{"nyu","ニュ"},{"nyo","ニョ"},
                                             {"hya","ヒャ"},{"hyu","ヒュ"},{"hyo","ヒョ"},
                                             {"mya","ミャ"},{"myu","ミュ"},{"myo","ミョ"},
                                             {"rya","リャ"},{"ryu","リュ"},{"ryo","リョ"},
                                             };

    return hira ? H.value(r, r) : K.value(r, r);
}

