#include "kanatablepage.h"

#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGridLayout>
#include <QFrame>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>

KanaTablePage::KanaTablePage(QWidget *parent)
    : QWidget(parent)
{
    buildUi();
    loadJson();
    refreshTable();
}

void KanaTablePage::buildUi()
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);

    // ==== Верхняя панель (Домой + переключатель) ====
    auto *topBar = new QHBoxLayout();

    btnHome = new QPushButton("← Home");
    btnHiragana = new QPushButton("Hiragana");
    btnKatakana = new QPushButton("Katakana");

    btnHome->setStyleSheet("QPushButton { padding:6px 12px; }");
    btnHiragana->setCheckable(true);
    btnKatakana->setCheckable(true);
    btnHiragana->setChecked(true);

    QString segStyle =
        "QPushButton { background:#444; color:white; padding:8px 16px; border-radius:8px; }"
        "QPushButton:checked { background:#e68b0e; }";

    btnHiragana->setStyleSheet(segStyle);
    btnKatakana->setStyleSheet(segStyle);

    topBar->addWidget(btnHome);
    topBar->addStretch();
    topBar->addWidget(btnHiragana);
    topBar->addWidget(btnKatakana);

    root->addLayout(topBar);

    connect(btnHiragana, &QPushButton::clicked, this, [this]{
        btnHiragana->setChecked(true);
        btnKatakana->setChecked(false);
        refreshTable();
    });

    connect(btnKatakana, &QPushButton::clicked, this, [this]{
        btnKatakana->setChecked(true);
        btnHiragana->setChecked(false);
        refreshTable();
    });

    // ==== Scroll Area (контент таблицы) ====
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scrollContent = new QWidget(scrollArea);
    mainLayout = new QVBoxLayout(scrollContent);
    mainLayout->setSpacing(24);

    scrollArea->setWidget(scrollContent);
    root->addWidget(scrollArea);
}

void KanaTablePage::loadJson()
{
    QFile f("data/kana.json");
    if (!f.open(QIODevice::ReadOnly)) {
        qDebug() << "ERROR: cannot open data/kana.json";
        return;
    }

    kanaData = QJsonDocument::fromJson(f.readAll()).object();
    f.close();
}

QVector<QPair<QString,QString>>
KanaTablePage::getList(const QString &script, const QString &section)
{
    QVector<QPair<QString,QString>> out;

    QJsonArray arr =
        kanaData[script].toObject()[section].toArray();

    for (auto v : arr) {
        QJsonObject o = v.toObject();
        out.append({ o["kana"].toString(), o["romaji"].toString() });
    }
    return out;
}

void KanaTablePage::refreshTable()
{
    // очистка старых элементов
    QLayoutItem *item;
    while ((item = mainLayout->takeAt(0))) {
        delete item->widget();
        delete item;
    }

    QString script = btnHiragana->isChecked()
                         ? "hiragana"
                         : "katakana";

    auto g = getList(script, "gojuon");
    auto d = getList(script, "dakuon");
    auto h = getList(script, "handakuon");
    auto y = getList(script, "yoon");

    mainLayout->addWidget(createSection("Gojuon (basic kana)", g));
    mainLayout->addWidget(createSection("Dakuon", d));
    mainLayout->addWidget(createSection("Handakuon", h));
    mainLayout->addWidget(createSection("Yoon", y));

    mainLayout->addStretch();
}

QWidget* KanaTablePage::createSection(const QString &title,
                                      const QVector<QPair<QString,QString>> &kanaList)
{
    QWidget *section = new QWidget();
    auto *layout = new QVBoxLayout(section);
    layout->setSpacing(8);

    QLabel *lbl = new QLabel(title);
    QFont f = lbl->font();
    f.setPointSize(14);
    f.setBold(true);
    lbl->setFont(f);
    lbl->setStyleSheet("color:white;");
    layout->addWidget(lbl);

    auto *grid = new QGridLayout();
    grid->setSpacing(14);

    int row = 0, col = 0;
    for (auto &p : kanaList) {
        grid->addWidget(createKanaCard(p.first, p.second), row, col);

        col++;
        if (col >= 5) { col = 0; row++; }
    }

    layout->addLayout(grid);
    return section;
}

QWidget* KanaTablePage::createKanaCard(const QString &symbol,
                                       const QString &romaji)
{
    QFrame *card = new QFrame();
    card->setStyleSheet("background:#2f2f2f; border-radius:10px;");
    card->setFrameShape(QFrame::StyledPanel);

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(8, 8, 8, 8);

    QLabel *lblSym = new QLabel(symbol);
    lblSym->setAlignment(Qt::AlignCenter);
    lblSym->setStyleSheet("color:white;");
    QFont f; f.setPointSize(26); f.setBold(true);
    lblSym->setFont(f);

    QLabel *lblRom = new QLabel(romaji);
    lblRom->setAlignment(Qt::AlignCenter);
    lblRom->setStyleSheet("color:#bbb; font-size:12px;");

    layout->addWidget(lblSym);
    layout->addWidget(lblRom);

    return card;
}
