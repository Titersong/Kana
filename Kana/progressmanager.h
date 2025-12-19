#ifndef PROGRESSMANAGER_H
#define PROGRESSMANAGER_H

#include <QObject>
#include <QJsonObject>

class ProgressManager : public QObject
{
    Q_OBJECT
public:
    explicit ProgressManager(QObject *parent = nullptr);

    void load();
    void save();

    // practice global stats
    int  getTotalAnswered() const;
    int  getTotalCorrect()  const;
    void addAnswered(bool correct);
    void addPracticeAnswer(bool correct);
    // kana-specific stats
    void addCorrect(bool isHiragana);
    void addWrong(bool isHiragana);
    int  getCorrect(bool isHiragana) const;
    int  getWrong(bool isHiragana)  const;
    int  getStreak(bool isHiragana) const;

    void markMastered(bool isHiragana, const QString &romaji);
    void addSymbolAnswer(bool isHiragana, const QString &romaji, bool correct);
    QStringList getMastered(bool isHiragana) const;

private:
    QString filePath;
    QJsonObject data;

    void ensureDefaults();
};

#endif
