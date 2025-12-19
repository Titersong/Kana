#ifndef KANATABLEPAGE_H
#define KANATABLEPAGE_H

#include <QWidget>

class QPushButton;
class QScrollArea;
class QVBoxLayout;

class KanaTablePage : public QWidget
{
    Q_OBJECT

public:
    explicit KanaTablePage(QWidget *parent = nullptr);

signals:
    void goHome();

private:
    // UI
    QPushButton *btnHome;
    QPushButton *btnHiragana;
    QPushButton *btnKatakana;

    QScrollArea *scrollArea;
    QWidget     *scrollContent;
    QVBoxLayout *mainLayout;

    // Static tables
    QVector<QVector<QString>> hira_gojuon;
    QVector<QVector<QString>> hira_dakuon;
    QVector<QVector<QString>> hira_handakuon;
    QVector<QVector<QString>> hira_yoon;

    QVector<QVector<QString>> kata_gojuon;
    QVector<QVector<QString>> kata_dakuon;
    QVector<QVector<QString>> kata_handakuon;
    QVector<QVector<QString>> kata_yoon;

    // Methods
    void buildUi();
    void loadStaticKana();
    void refreshTable();

    QWidget* createSection(const QString &title,
                           const QVector<QVector<QString>> &matrix,
                           int columns);

    QWidget* createCard(const QString &kana, const QString &romaji);

    QString romajiOf(const QString &kana);

    bool eventFilter(QObject *obj, QEvent *ev) override;
};

#endif // KANATABLEPAGE_H
