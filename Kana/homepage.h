#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>

class QPushButton;
class QLabel;

class HomePage : public QWidget
{
    Q_OBJECT
public:
    explicit HomePage(QWidget *parent = nullptr);

signals:
    void openKanaTable();
    void openPractice();
    void openStatistics();

private:
    void buildUi();

    QLabel *lblTitle;
    QLabel *lblSubtitle;

    QPushButton *btnKana;
    QPushButton *btnPractice;
    QPushButton *btnStats;
};

#endif // HOMEPAGE_H
