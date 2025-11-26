#ifndef DETAILDIALOG_H
#define DETAILDIALOG_H

#include <QDialog>

class QLabel;
class QPushButton;
class QMediaPlayer;

class DetailDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DetailDialog(const QString &kana,
                          const QString &romaji,
                          QWidget *parent = nullptr);

private:
    QLabel *lblKana;
    QLabel *lblRomaji;
    QPushButton *btnPlay;
    QPushButton *btnClose;
    QMediaPlayer *player;

    QString soundName;

    void buildUi();
    void loadSound();
};

#endif // DETAILDIALOG_H
