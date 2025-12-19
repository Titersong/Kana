#include "WordApiService.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QRandomGenerator>

WordApiService::WordApiService(QObject *parent)
    : QObject(parent)
{
    net = new QNetworkAccessManager(this);
}

void WordApiService::fetchWord()
{
    // Pool for searching
    static QStringList seeds = {
        "あ","い","う","か","さ","た","な",
        "日","人","水","山","火"
    };

    QString seed = seeds[
        QRandomGenerator::global()->bounded(seeds.size())
    ];

    QUrl url("https://jisho.org/api/v1/search/words?keyword=" + seed);

    QNetworkRequest req(url);

    auto *reply = net->get(req);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError)
            return;

        auto doc = QJsonDocument::fromJson(reply->readAll());
        auto root = doc.object();
        auto data = root["data"].toArray();
        if (data.isEmpty())
            return;

        auto entry = data[
                         QRandomGenerator::global()->bounded(data.size())
        ].toObject();
        auto jp = entry["japanese"].toArray().first().toObject();
        auto sense = entry["senses"].toArray().first().toObject();

        QString kana = jp["reading"].toString();
        QString romaji = jp["word"].toString();
        QStringList meanings;
        for (const auto& v : sense["english_definitions"].toArray())
            meanings << v.toString();

        QString meaning = meanings.join(", ");
        QString id = entry["slug"].toString();
        if (id == lastWordId)
            return;

        lastWordId = id;
        emit wordReady(kana, romaji, meaning);
    });
}
