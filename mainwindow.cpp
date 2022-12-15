#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rescalethread.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_resampleButton_clicked()
{
    RescaleThread *thread = new RescaleThread(this);
    thread->start();
}

