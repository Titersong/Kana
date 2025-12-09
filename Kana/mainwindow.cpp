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
    practicePage  = new PracticePage(this);

    ui->stackedWidget->addWidget(kanaTablePage);
    ui->stackedWidget->addWidget(practicePage);

    // кнопки на главной странице
    connect(ui->btnKanaTable, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(kanaTablePage);
    });

    connect(ui->btnPractice, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(practicePage);
    });

    // возврат домой из страниц
    connect(kanaTablePage,  &KanaTablePage::goHome, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->page);
    });
    connect(practicePage,  &PracticePage::goHome, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->page);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showHome()
{
    ui->stackedWidget->setCurrentIndex(0); // 0 — это HomePage
}
