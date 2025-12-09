#ifndef PRACTICEPAGE_H
#define PRACTICEPAGE_H

#include <QWidget>
#include <QVector>
#include "progressmanager.h"
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QEasingCurve>

class QLabel;
class QPushButton;
class QComboBox;

struct QuizKanaItem
{
    QString kana;
    QString romaji;
    bool    isHiragana;
};

class PracticePage : public QWidget
{
    Q_OBJECT
public:
    explicit PracticePage(QWidget *parent = nullptr);

signals:
    void goHome();

private slots:
    void startQuiz();
    void answer1();
    void answer2();
    void answer3();
    void answer4();
    void nextQuestion();

private:
    enum class Mode   { KanaToRomaji, RomajiToKana, Mixed };
    enum class Script { Hiragana, Katakana, Both };
    ProgressManager *progress;

    QGraphicsOpacityEffect *questionOpacity = nullptr;
    QPropertyAnimation     *fadeOutAnim     = nullptr;
    QPropertyAnimation     *fadeInAnim      = nullptr;
    void buildUi();
    void buildKanaPool();
    void loadStats();
    void saveStats();
    void askQuestion();
    void handleAnswer(int index);
    QString romajiOf(const QString &k);

    // настройки
    Mode   m_mode;
    Script m_script;

    // данные
    QVector<QuizKanaItem> m_all;   // все символы
    QVector<QuizKanaItem> m_pool;  // после фильтра по скрипту
    QuizKanaItem          m_current;
    bool                  m_showKana;     // true => вопрос кана, ответы romaji
    int                   m_correctIndex; // 0..3
    bool                  m_questionActive = false;

    // статистика (глобальная, сохраняем в JSON)
    int m_totalAnswered = 0;
    int m_totalCorrect  = 0;

    // UI
    QPushButton *btnHome = nullptr;

    QComboBox   *cmbMode   = nullptr;
    QComboBox   *cmbScript = nullptr;
    QPushButton *btnStart  = nullptr;

    QLabel      *lblQuestion = nullptr;  // большая кана / ромадзи
    QLabel      *lblSubtitle = nullptr;  // подпись "Kana → Romaji" и т.п.
    QPushButton *opt[4]      = {nullptr, nullptr, nullptr, nullptr};
    QLabel      *lblFeedback = nullptr;
    QPushButton *btnNext     = nullptr;
};

#endif // PRACTICEPAGE_H
