#ifndef KANATABLEPAGE_H
#define KANATABLEPAGE_H

#include <QWidget>
#include <QJsonObject>

class QPushButton;
class QScrollArea;
class QVBoxLayout;
class QFrame;

class KanaTablePage : public QWidget
{
    Q_OBJECT
public:
    explicit KanaTablePage(QWidget *parent = nullptr);

    QPushButton *btnHome;  // чтобы MainWindow мог подключаться

private:
    // переключатели
    QPushButton *btnHiragana;
    QPushButton *btnKatakana;

    // прокручиваемая область
    QScrollArea *scrollArea;
    QWidget     *scrollContent;
    QVBoxLayout *mainLayout;

    // JSON база данных
    QJsonObject kanaData;

    // Методы
    void buildUi();
    void loadJson();
    void refreshTable();

    QVector<QPair<QString,QString>>
    getList(const QString &script, const QString &section);

    QWidget* createSection(const QString &title,
                           const QVector<QPair<QString,QString>> &kanaList);

    QWidget* createKanaCard(const QString &symbol,
                            const QString &romaji);
};

#endif // KANATABLEPAGE_H
