#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "homepage.h"
#include "kanatablepage.h"
#include "practicesetuppage.h"
#include "practicesessionpage.h"
#include "statisticspage.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // ---------- Pages ----------
    auto *homePage = new HomePage(this);
    auto *kanaTablePage = new KanaTablePage(this);
    auto *practiceSetupPage = new PracticeSetupPage(this);
    auto *practiceSessionPage = new PracticeSessionPage(this);
    auto *statisticsPage = new StatisticsPage(this);

    auto *stack = ui->stackedWidget;
    stack->addWidget(homePage);
    stack->addWidget(kanaTablePage);
    stack->addWidget(practiceSetupPage);
    stack->addWidget(practiceSessionPage);
    stack->addWidget(statisticsPage);

    stack->setCurrentWidget(homePage);

    // ---------- HOME ----------
    connect(homePage, &HomePage::openKanaTable, this, [=]() {
        stack->setCurrentWidget(kanaTablePage);
    });

    connect(homePage, &HomePage::openPractice, this, [=]() {
        stack->setCurrentWidget(practiceSetupPage);
    });

    connect(homePage, &HomePage::openStatistics, this, [=]() {
        statisticsPage->loadStats();
        stack->setCurrentWidget(statisticsPage);
    });

    // ---------- PRACTICE ----------
    connect(practiceSetupPage, &PracticeSetupPage::startPractice,
            this, [=](const PracticeConfig &config) {
                practiceSessionPage->startSession(config);
                stack->setCurrentWidget(practiceSessionPage);
            });

    connect(practiceSessionPage, &PracticeSessionPage::backToSetup,
            this, [=]() {
                stack->setCurrentWidget(practiceSetupPage);
            });

    // ---------- BACK TO HOME ----------
    connect(kanaTablePage, &KanaTablePage::goHome, this, [=]() {
        stack->setCurrentWidget(homePage);
    });

    connect(practiceSetupPage, &PracticeSetupPage::goHome, this, [=]() {
        stack->setCurrentWidget(homePage);
    });

    connect(statisticsPage, &StatisticsPage::goHome, this, [=]() {
        stack->setCurrentWidget(homePage);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
