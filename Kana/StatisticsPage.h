#ifndef STATISTICSPAGE_H
#define STATISTICSPAGE_H

#include <QWidget>

class QLabel;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class WordApiService;

class StatisticsPage : public QWidget
{
    Q_OBJECT

public:
    explicit StatisticsPage(QWidget *parent = nullptr);
    void loadStats();

signals:
    void goHome();

private:
    void buildUi();

    // Helpers
    QWidget* createMasteredRow(const QStringList& romajiList, bool hiragana);
    QWidget* createMasteredCard(const QString& kana, const QString& romaji);
    QString  kanaFromRomaji(const QString& romaji, bool hiragana);

private:
    QVBoxLayout *rootLayout = nullptr;

    // Summary
    QLabel *lblTotal;
    QLabel *lblAccuracy;

    // Hiragana
    QLabel *lblHiraCorrect;
    QLabel *lblHiraWrong;
    QLabel *lblHiraStreak;
    QWidget *hiraMasteredWidget;

    // Katakana
    QLabel *lblKataCorrect;
    QLabel *lblKataWrong;
    QLabel *lblKataStreak;
    QWidget *kataMasteredWidget;

    // API
    QLabel *lblWordKana;
    QLabel *lblWordRomaji;
    QLabel *lblWordMeaning;
    WordApiService *wordService;
    QPushButton *btnNewWord;
    QWidget *wordCard;

    QPushButton *btnHome;
};

#endif
