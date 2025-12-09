#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "practicepage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class KanaTablePage;
class PracticePage;

class MainWindow : public QMainWindow
{
    Q_OBJECT

private slots:
    void showHome();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    KanaTablePage *kanaTablePage;
    PracticePage  *practicePage;
};

#endif // MAINWINDOW_H
