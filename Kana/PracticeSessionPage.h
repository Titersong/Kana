#ifndef PRACTICESESSIONPAGE_H
#define PRACTICESESSIONPAGE_H

#include <QWidget>
#include <QVector>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

#include "practiceconfig.h"
#include "progressmanager.h"

struct QuizKanaItem
{
    QString kana;
    QString romaji;
    bool    isHiragana = true;
};


class PracticeSessionPage : public QWidget
{
    Q_OBJECT

public:
    explicit PracticeSessionPage(QWidget *parent = nullptr);
    void startSession(const PracticeConfig &config);

signals:
    void backToSetup();

private slots:
    void answer(int index);
    void nextQuestion();
    void exitSession();

private:
    // UI
    void buildUi();
    void finishSession();
    void stopSession();
    // Test logic
    void buildKanaPool();
    void askQuestion();
    QString romajiOf(const QString &kana);
    QSet<QString> m_masteredRomaji;
    void loadMasteredFromStats();

private:
    PracticeConfig m_config;

    QVector<QuizKanaItem> m_all;
    QVector<QuizKanaItem> m_pool;

    QuizKanaItem m_current;

    int  m_correctIndex = 0;
    int  m_questionIndex = 0;
    int  m_correctCount = 0;
    bool m_showKana = true;
    bool m_active = false;

    ProgressManager *progress = nullptr;

    // UI elements
    QPushButton *btnHome = nullptr;

    QLabel *lblCounter  = nullptr;
    QLabel *lblQuestion = nullptr;
    QLabel *lblSubtitle = nullptr;
    QLabel *lblFeedback = nullptr;

    QPushButton *opt[4] = {};

    QPushButton *btnNext = nullptr;

    // Finish screen
    QWidget     *resultWidget = nullptr;
    QLabel      *lblResult = nullptr;
    QPushButton *btnBack = nullptr;
    QPushButton *btnStop = nullptr;

    // Fade animation
    QGraphicsOpacityEffect *opacity = nullptr;
    QPropertyAnimation     *fadeOut = nullptr;
    QPropertyAnimation     *fadeIn  = nullptr;
};

#endif // PRACTICESESSIONPAGE_H
