#include "kanatablepage.h"
#include "DetailDialog.h"

#include <QScrollArea>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QDebug>
#include <QMouseEvent>

// ----------------------------
// Constructor
// ----------------------------
KanaTablePage::KanaTablePage(QWidget *parent)
    : QWidget(parent)
{
    loadStaticKana();
    buildUi();
    refreshTable();
}

// ----------------------------
// Build UI
// ----------------------------
void KanaTablePage::buildUi()
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);

    // ==== Верхняя панель ====
    auto *topBar = new QHBoxLayout();

    btnHome = new QPushButton("← Home");
    btnHome->setStyleSheet("padding:6px 14px; background:#444; color:white; border-radius:6px;");

    btnHiragana = new QPushButton("Hiragana");
    btnKatakana = new QPushButton("Katakana");

    btnHiragana->setCheckable(true);
    btnKatakana->setCheckable(true);
    btnHiragana->setChecked(true);

    QString seg =
        "QPushButton { background:#444; color:white; padding:8px 16px; border-radius:8px; }"
        "QPushButton:checked { background:#e68b0e; color:black; }";

    btnHiragana->setStyleSheet(seg);
    btnKatakana->setStyleSheet(seg);

    topBar->addWidget(btnHome);
    topBar->addStretch();
    topBar->addWidget(btnHiragana);
    topBar->addWidget(btnKatakana);

    root->addLayout(topBar);

    // Signals
    connect(btnHome, &QPushButton::clicked, this, [this](){ emit goHome(); });

    connect(btnHiragana, &QPushButton::clicked, this, [this](){
        btnHiragana->setChecked(true);
        btnKatakana->setChecked(false);
        refreshTable();
    });

    connect(btnKatakana, &QPushButton::clicked, this, [this](){
        btnKatakana->setChecked(true);
        btnHiragana->setChecked(false);
        refreshTable();
    });

    // ==== Scroll ====
    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scrollContent = new QWidget(scrollArea);
    mainLayout = new QVBoxLayout(scrollContent);
    mainLayout->setSpacing(26);

    scrollArea->setWidget(scrollContent);
    root->addWidget(scrollArea);
}

// ----------------------------
// Static Kana Tables
// ----------------------------
void KanaTablePage::loadStaticKana()
{
    // ---- Hiragana ----
    hira_gojuon = {
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

    hira_dakuon = {
        {"が","ぎ","ぐ","げ","ご"},
        {"ざ","じ","ず","ぜ","ぞ"},
        {"だ","ぢ","づ","で","ど"}
    };

    hira_handakuon = {
        {"ば","び","ぶ","べ","ぼ"},
        {"ぱ","ぴ","ぷ","ぺ","ぽ"}
    };

    hira_yoon = {
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

    // ---- Katakana ----
    kata_gojuon = {
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

    kata_dakuon = {
        {"ガ","ギ","グ","ゲ","ゴ"},
        {"ザ","ジ","ズ","ゼ","ゾ"},
        {"ダ","ヂ","ヅ","デ","ド"}
    };

    kata_handakuon = {
        {"バ","ビ","ブ","ベ","ボ"},
        {"パ","ピ","プ","ペ","ポ"}
    };

    kata_yoon = {
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
}

// ----------------------------
// Refresh table
// ----------------------------
void KanaTablePage::refreshTable()
{
    // Clear layout
    QLayoutItem *it;
    while ((it = mainLayout->takeAt(0))) {
        delete it->widget();
        delete it;
    }

    bool isHira = btnHiragana->isChecked();

    mainLayout->addWidget(createSection("Gojuon (basic)",
                                        isHira ? hira_gojuon : kata_gojuon,
                                        5));

    mainLayout->addWidget(createSection("Dakuon",
                                        isHira ? hira_dakuon : kata_dakuon,
                                        5));

    mainLayout->addWidget(createSection("Handakuon",
                                        isHira ? hira_handakuon : kata_handakuon,
                                        5));

    mainLayout->addWidget(createSection("Yoon",
                                        isHira ? hira_yoon : kata_yoon,
                                        3));

    mainLayout->addStretch();
}

// ----------------------------
// Create section
// ----------------------------
QWidget* KanaTablePage::createSection(const QString &title,
                                      const QVector<QVector<QString>> &matrix,
                                      int columns)
{
    QWidget *sec = new QWidget();
    auto *V = new QVBoxLayout(sec);
    V->setSpacing(12);

    QLabel *lbl = new QLabel(title);
    QFont f; f.setPointSize(16); f.setBold(true);
    lbl->setFont(f);
    lbl->setStyleSheet("color:white;");
    V->addWidget(lbl);

    QWidget *container = new QWidget();
    auto *grid = new QGridLayout(container);
    grid->setHorizontalSpacing(14);
    grid->setVerticalSpacing(14);

    int r = 0;
    for (auto &row : matrix) {
        for (int c = 0; c < columns; c++) {
            if (c < row.size() && !row[c].isEmpty()) {
                QString kana = row[c];
                QString rom = romajiOf(kana);
                grid->addWidget(createCard(kana, rom), r, c);
            }
        }
        r++;
    }

    V->addWidget(container);
    return sec;
}

// ----------------------------
// Create Card
// ----------------------------
QWidget* KanaTablePage::createCard(const QString &kana, const QString &romaji)
{
    QWidget *card = new QWidget();
    card->setStyleSheet("background:#2a2a2a; border-radius:14px;");
    card->setMinimumHeight(90);

    auto *lay = new QVBoxLayout(card);
    lay->setContentsMargins(0, 6, 0, 6);

    QLabel *Lkana = new QLabel(kana);
    QFont f; f.setPointSize(28); f.setBold(true);
    Lkana->setFont(f);
    Lkana->setAlignment(Qt::AlignCenter);

    QLabel *Lrom = new QLabel(romaji);
    Lrom->setStyleSheet("color:#bbbbbb; font-size:11pt;");
    Lrom->setAlignment(Qt::AlignCenter);

    lay->addWidget(Lkana);
    lay->addWidget(Lrom);

    card->setCursor(Qt::PointingHandCursor);
    card->installEventFilter(this);

    card->setProperty("kana", kana);
    card->setProperty("romaji", romaji);

    return card;
}

// ----------------------------
// Romaji dictionary
// ----------------------------
QString KanaTablePage::romajiOf(const QString &k)
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
                                       {"しゃ","sha"},{"しゅ","shu"},{"しょ","sho"},
                                       {"ちゃ","cha"},{"ちゅ","chu"},{"ちょ","cho"},
                                       {"にゃ","nya"},{"にゅ","nyu"},{"にょ","nyo"},
                                       {"ひゃ","hya"},{"ひゅ","hyu"},{"ひょ","hyo"},
                                       {"みゃ","mya"},{"みゅ","myu"},{"みょ","myo"},
                                       {"りゃ","rya"},{"りゅ","ryu"},{"りょ","ryo"},

                                       // Katakana duplicates
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
                                       {"リャ","rya"},{"リュ","ryu"},{"リョ","ryo"},
                                       };

    return R.value(k, "");
}

// ----------------------------
// Event filter
// ----------------------------
bool KanaTablePage::eventFilter(QObject *obj, QEvent *ev)
{
    if (ev->type() == QEvent::MouseButtonRelease) {
        QWidget *w = qobject_cast<QWidget*>(obj);
        if (!w) return false;

        QString kana = w->property("kana").toString();
        QString rom  = w->property("romaji").toString();

        if (!kana.isEmpty()) {
            DetailDialog dlg(kana, rom, this);
            dlg.exec();
        }
    }
    return false;
}
