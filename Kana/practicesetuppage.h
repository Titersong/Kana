#ifndef PRACTICESETUPPAGE_H
#define PRACTICESETUPPAGE_H

#include <QWidget>
#include "practiceconfig.h"

class QPushButton;
class QLabel;

class PracticeSetupPage : public QWidget
{
    Q_OBJECT
public:
    explicit PracticeSetupPage(QWidget *parent = nullptr);

signals:
    void startPractice(const PracticeConfig &config);
    void goHome();

private:
    void buildUi();
    void updateButtonStates();

    // State
    PracticeConfig m_config;

    // Buttons
    QPushButton *btnMode[3];
    QPushButton *btnScript[3];
    QPushButton *btnSourceAll;
    QPushButton *btnSourceMastered;
    QPushButton *btnCount[4];
    QPushButton *btnStart;
};

#endif // PRACTICESETUPPAGE_H
