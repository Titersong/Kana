#include "progressmanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

ProgressManager::ProgressManager(QObject *parent)
    : QObject(parent)
{
    filePath = "data/user_stats.json";
    load();
}

// ---------------------------------------------------------
//  Создаём дефолтные записи, если их нет в JSON
// ---------------------------------------------------------
void ProgressManager::ensureDefaults()
{
    if (!data.contains("practice"))
    {
        QJsonObject o;
        o["totalAnswered"] = 0;
        o["totalCorrect"]  = 0;
        data["practice"] = o;
    }

    if (!data.contains("hiragana"))
    {
        QJsonObject o;
        o["correct"]  = 0;
        o["wrong"]    = 0;
        o["streak"]   = 0;
        o["mastered"] = QJsonArray();
        data["hiragana"] = o;
    }

    if (!data.contains("katakana"))
    {
        QJsonObject o;
        o["correct"]  = 0;
        o["wrong"]    = 0;
        o["streak"]   = 0;
        o["mastered"] = QJsonArray();
        data["katakana"] = o;
    }
}

// ---------------------------------------------------------
//  Load JSON
// ---------------------------------------------------------
void ProgressManager::load()
{
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly))
    {
        qDebug() << "Stats file not found. Creating new.";
        data = QJsonObject();
        ensureDefaults();
        save();
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    data = doc.object();
    f.close();

    ensureDefaults();
}

// ---------------------------------------------------------
//  Save JSON
// ---------------------------------------------------------
void ProgressManager::save()
{
    QFile f(filePath);
    if (!f.open(QIODevice::WriteOnly))
    {
        qDebug() << "Cannot write stats file!";
        return;
    }

    QJsonDocument doc(data);
    f.write(doc.toJson(QJsonDocument::Indented));
    f.close();
}

// ---------------------------------------------------------
// PRACTICE SECTION
// ---------------------------------------------------------
int ProgressManager::getTotalAnswered() const
{
    return data["practice"].toObject()["totalAnswered"].toInt();
}

int ProgressManager::getTotalCorrect() const
{
    return data["practice"].toObject()["totalCorrect"].toInt();
}

void ProgressManager::addAnswered(bool correct)
{
    QJsonObject o = data["practice"].toObject();
    o["totalAnswered"] = o["totalAnswered"].toInt() + 1;
    if (correct)
        o["totalCorrect"] = o["totalCorrect"].toInt() + 1;
    data["practice"] = o;
    save();
}

// ---------------------------------------------------------
//  Kana counters
// ---------------------------------------------------------
void ProgressManager::addCorrect(bool isHiragana)
{
    QString key = isHiragana ? "hiragana" : "katakana";

    QJsonObject o = data[key].toObject();
    o["correct"] = o["correct"].toInt() + 1;
    o["streak"]  = o["streak"].toInt() + 1;
    data[key] = o;
    save();
}

void ProgressManager::addPracticeAnswer(bool correct)
{
    QFile f("data/user_stats.json");
    if (!f.open(QIODevice::ReadOnly))
        return;

    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();

    QJsonObject root = doc.object();
    QJsonObject practice = root["practice"].toObject();

    int totalAnswered = practice["totalAnswered"].toInt();
    int totalCorrect  = practice["totalCorrect"].toInt();

    totalAnswered++;
    if (correct)
        totalCorrect++;

    practice["totalAnswered"] = totalAnswered;
    practice["totalCorrect"]  = totalCorrect;
    root["practice"] = practice;

    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;

    f.write(QJsonDocument(root).toJson());
    f.close();
}

void ProgressManager::addWrong(bool isHiragana)
{
    QString key = isHiragana ? "hiragana" : "katakana";

    QJsonObject o = data[key].toObject();
    o["wrong"]  = o["wrong"].toInt() + 1;
    o["streak"] = 0;
    data[key] = o;
    save();
}

int ProgressManager::getCorrect(bool isHiragana) const
{
    QString key = isHiragana ? "hiragana" : "katakana";
    return data[key].toObject()["correct"].toInt();
}

int ProgressManager::getWrong(bool isHiragana) const
{
    QString key = isHiragana ? "hiragana" : "katakana";
    return data[key].toObject()["wrong"].toInt();
}

int ProgressManager::getStreak(bool isHiragana) const
{
    QString key = isHiragana ? "hiragana" : "katakana";
    return data[key].toObject()["streak"].toInt();
}

// ---------------------------------------------------------
//  Mastery
// ---------------------------------------------------------
void ProgressManager::markMastered(bool isHiragana, const QString &romaji)
{
    QString key = isHiragana ? "hiragana" : "katakana";

    QJsonObject o = data[key].toObject();
    QJsonArray arr = o["mastered"].toArray();

    if (!arr.contains(romaji))
    {
        arr.append(romaji);
        o["mastered"] = arr;
        data[key] = o;
        save();
    }
}

QStringList ProgressManager::getMastered(bool isHiragana) const
{
    QString key = isHiragana ? "hiragana" : "katakana";
    QStringList list;

    QJsonArray arr = data[key].toObject()["mastered"].toArray();
    for (const auto &v : arr)
        list.append(v.toString());
    return list;
}
