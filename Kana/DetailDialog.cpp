#include "DetailDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QFile>
#include <QDebug>

DetailDialog::DetailDialog(const QString &kana,
                           const QString &romaji,
                           QWidget *parent)
    : QDialog(parent),
    soundName(romaji)
{
    setWindowTitle("Kana details");
    setModal(true);
    setFixedSize(300, 350);
    setStyleSheet("background:#2a2a2a; color:white;");

    lblKana = new QLabel(kana);
    lblRomaji = new QLabel(romaji);

    buildUi();
    loadSound();
}

void DetailDialog::buildUi()
{
    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);

    // большой символ
    QFont f;
    f.setPointSize(72);
    f.setBold(true);
    lblKana->setFont(f);
    lblKana->setAlignment(Qt::AlignCenter);

    // ромадзи
    lblRomaji->setAlignment(Qt::AlignCenter);
    lblRomaji->setStyleSheet("font-size:20px; color:#bbbbbb;");

    // кнопка прослушать
    btnPlay = new QPushButton("🔊 Play");
    btnPlay->setStyleSheet(
        "QPushButton { background:#444; padding:8px; border-radius:6px; } "
        "QPushButton:hover { background:#555; }"
        );

    btnClose = new QPushButton("Close");
    btnClose->setStyleSheet(
        "QPushButton { background:#444; padding:8px; border-radius:6px; } "
        "QPushButton:hover { background:#555; }"
        );

    // audio player
    player = new QMediaPlayer(this);
    QAudioOutput *audioOut = new QAudioOutput(this);
    player->setAudioOutput(audioOut);

    connect(btnPlay, &QPushButton::clicked, this, [this](){
        if (player)
            player->play();
    });

    connect(btnClose, &QPushButton::clicked, this, [this](){
        close();
    });

    root->addWidget(lblKana);
    root->addSpacing(10);
    root->addWidget(lblRomaji);
    root->addSpacing(20);

    root->addWidget(btnPlay);
    root->addStretch();
    root->addWidget(btnClose);
}

void DetailDialog::loadSound()
{
    QString path = "data/sounds/" + soundName + ".mp3";

    if (!QFile::exists(path)) {
        qDebug() << "Sound missing:" << path;
        return;
    }

    player->setSource(QUrl::fromLocalFile(path));
}
