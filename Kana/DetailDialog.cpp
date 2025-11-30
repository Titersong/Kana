#include "DetailDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>
#include <QFile>
#include <QDebug>
#include <QResizeEvent>

// Convert hira → kata
static QString toKatakana(const QString &h)
{
    if (h.isEmpty()) return h;
    ushort u = h[0].unicode();
    if (u >= 0x3041 && u <= 0x3096)
        return QString(QChar(u + 0x60));
    return h;
}

// Convert kata → hira
static QString toHiragana(const QString &k)
{
    if (k.isEmpty()) return k;
    ushort u = k[0].unicode();
    if (u >= 0x30A1 && u <= 0x30F6)
        return QString(QChar(u - 0x60));
    return k;
}

DetailDialog::DetailDialog(const QString &kana,
                           const QString &romaji,
                           bool isHiragana,
                           QWidget *parent)
    : QDialog(parent),
    m_romaji(romaji),
    m_isHiragana(isHiragana)
{
    // формируем обе версии
    m_kana_hira = isHiragana ? kana : toHiragana(kana);
    m_kana_kata = toKatakana(m_kana_hira);

    setModal(true);
    setWindowTitle("Kana");    // ← вернули крестик окна
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);

    buildUi();
    loadContent();

    resize(420, 600);
}

void DetailDialog::buildUi()
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(24, 24, 24, 24);
    root->setSpacing(16);

    // BIG SYMBOL
    lblKana = new QLabel();
    QFont fKana; fKana.setPointSize(64); fKana.setBold(true);
    lblKana->setFont(fKana);
    lblKana->setAlignment(Qt::AlignHCenter);
    lblKana->setStyleSheet("color: white;");
    root->addWidget(lblKana);

    // SCRIPT NAME
    lblScript = new QLabel();
    lblScript->setAlignment(Qt::AlignHCenter);
    lblScript->setStyleSheet("color: #bbbbbb; font-size: 12pt;");
    root->addWidget(lblScript);

    // ROMAJI
    lblRomaji = new QLabel();
    QFont fRom; fRom.setPointSize(22); fRom.setBold(true);
    lblRomaji->setFont(fRom);
    lblRomaji->setAlignment(Qt::AlignHCenter);
    lblRomaji->setStyleSheet("color: white;");
    root->addWidget(lblRomaji);

    // BUTTONS
    btnSound = new QPushButton("🔊");
    btnSound->setFixedSize(40,40);
    btnSound->setStyleSheet(
        "QPushButton{border:none;background:#555;border-radius:20px;color:white;font-size:18px;}"
        "QPushButton:hover{background:#777;}"
        );

    btnSwitch = new QPushButton("⇆");
    btnSwitch->setFixedSize(40,40);
    btnSwitch->setStyleSheet(
        "QPushButton{border:none;background:#555;border-radius:20px;color:white;font-size:18px;}"
        "QPushButton:hover{background:#777;}"
        );

    auto *hBtns = new QHBoxLayout();
    hBtns->addStretch();
    hBtns->addWidget(btnSound);
    hBtns->addWidget(btnSwitch);
    hBtns->addStretch();
    root->addLayout(hBtns);

    connect(btnSound,  &QPushButton::clicked, this, &DetailDialog::playSound);
    connect(btnSwitch, &QPushButton::clicked, this, &DetailDialog::switchScript);

    // STROKE IMAGE
    strokeLabel = new QLabel();
    strokeLabel->setAlignment(Qt::AlignCenter);
    strokeLabel->setMinimumHeight(300);
    strokeLabel->setStyleSheet("background:#111; border-radius:16px;");
    root->addWidget(strokeLabel, 1);
}

void DetailDialog::loadContent()
{
    // SET LABELS
    lblKana->setText(m_isHiragana ? m_kana_hira : m_kana_kata);
    lblScript->setText(m_isHiragana ? "Hiragana" : "Katakana");
    lblRomaji->setText(m_romaji);

    loadSound();
    loadStrokeImage();
}

void DetailDialog::loadSound()
{
    QString path = QString("data/sounds/%1.mp3").arg(m_romaji);

    if (!QFile::exists(path)) {
        qDebug() << "NO SOUND:" << path;
        btnSound->setEnabled(false);
        return;
    }

    btnSound->setEnabled(true);

    if (!m_player) {
        m_player = new QMediaPlayer(this);
        m_audio  = new QAudioOutput(this);
        m_player->setAudioOutput(m_audio);
        m_audio->setVolume(1.0);
    }

    m_player->setSource(QUrl::fromLocalFile(path));
}

void DetailDialog::playSound()
{
    if (!m_player) return;
    m_player->stop();
    m_player->play();
}

void DetailDialog::switchScript()
{
    m_isHiragana = !m_isHiragana;
    loadContent();
}

void DetailDialog::loadStrokeImage()
{
    QString prefix = m_isHiragana ? "hira_" : "kata_";
    QString path   = QString("data/strokes/%1%2.jpg")
                       .arg(prefix, m_romaji);

    if (!QFile::exists(path)) {
        strokeLabel->setVisible(false);
        return;
    }

    m_strokePixmap.load(path);
    strokeLabel->setVisible(true);
    updateStrokePixmap();
}

void DetailDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    updateStrokePixmap();
}

void DetailDialog::updateStrokePixmap()
{
    if (m_strokePixmap.isNull()) return;

    strokeLabel->setPixmap(
        m_strokePixmap.scaled(
            strokeLabel->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            )
        );
}
