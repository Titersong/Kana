#ifndef STATISTICSPAGE_H
#define STATISTICSPAGE_H

#include <QWidget>

class QLabel;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;

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

    // helpers
    QWidget* createMasteredRow(const QStringList& romajiList, bool hiragana);
    QWidget* createMasteredCard(const QString& kana, const QString& romaji);
    QString  kanaFromRomaji(const QString& romaji, bool hiragana);

private:
    QVBoxLayout *rootLayout = nullptr;

    // SUMMARY
    QLabel *lblTotal;
    QLabel *lblAccuracy;

    // HIRAGANA
    QLabel *lblHiraCorrect;
    QLabel *lblHiraWrong;
    QLabel *lblHiraStreak;
    QWidget *hiraMasteredWidget;

    // KATAKANA
    QLabel *lblKataCorrect;
    QLabel *lblKataWrong;
    QLabel *lblKataStreak;
    QWidget *kataMasteredWidget;

    QPushButton *btnHome;
};

#endif
