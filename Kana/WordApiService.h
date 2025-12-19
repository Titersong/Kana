#ifndef WORDAPISERVICE_H
#define WORDAPISERVICE_H

#include <QObject>

class QNetworkAccessManager;

class WordApiService : public QObject
{
    Q_OBJECT
public:
    explicit WordApiService(QObject *parent = nullptr);
    void fetchWord();

signals:
    void wordReady(const QString& kana,
                   const QString& romaji,
                   const QString& meaning);

private:
    QNetworkAccessManager *net;
    QString lastWordId;
};

#endif
