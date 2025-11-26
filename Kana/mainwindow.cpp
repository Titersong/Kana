#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "kanatablepage.h"
#include "practicepage.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Создаём страницы
    kanaTablePage = new KanaTablePage(this);
    practicePage = new PracticePage(this);

    // Добавляем их в QStackedWidget
    ui->stackedWidget->addWidget(kanaTablePage);
    ui->stackedWidget->addWidget(practicePage);

    // Реакции на кнопки
    connect(ui->btnKanaTable, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(kanaTablePage);
    });

    connect(ui->btnPractice, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(practicePage);
    });

    connect(kanaTablePage, &KanaTablePage::goHome,
            this, &MainWindow::showHome);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showHome()
{
    ui->stackedWidget->setCurrentIndex(0); // 0 — это HomePage
}
