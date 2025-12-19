#ifndef DETAILDIALOG_H
#define DETAILDIALOG_H

#include <QDialog>
#include <QPixmap>

class QLabel;
class QPushButton;
class QMediaPlayer;
class QAudioOutput;

class DetailDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DetailDialog(const QString &kana,
                          const QString &romaji,
                          bool isHiragana,
                          QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void playSound();
    void switchScript();

private:
    void buildUi();
    void loadContent();
    void loadSound();
    void loadStrokeImage();
    void updateStrokePixmap();

    QString m_kana_hira;
    QString m_kana_kata;
    QString m_romaji;
    bool    m_isHiragana;

    QLabel       *lblKana;
    QLabel       *lblScript;
    QLabel       *lblRomaji;
    QPushButton  *btnSound;
    QPushButton  *btnSwitch;
    QLabel       *strokeLabel;

    QMediaPlayer *m_player = nullptr;
    QAudioOutput *m_audio  = nullptr;

    QPixmap       m_strokePixmap;
};

#endif
