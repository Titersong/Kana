#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class KanaTablePage;
class PracticePage;
class PracticeSetupPage;
class PracticeSessionPage;
class StatisticsPage;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    Ui::MainWindow *ui;

    KanaTablePage       *kanaTablePage = nullptr;
    PracticeSessionPage *practiceSessionPage;
    PracticeSetupPage   *practiceSetupPage;
    StatisticsPage *statisticsPage;
};

#endif // MAINWINDOW_H
