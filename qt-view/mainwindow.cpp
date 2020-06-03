#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <string>
#include <filesystem>

#include <QString>

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete jsonAnalizer;
}

void MainWindow::on_pushButton_clicked()
{
    QString file_path = QFileDialog::getOpenFileName(this,"Open a file", "~/");
    ui->label->setText(file_path);
    jsonAnalizer = new JsonAnalizer(file_path.toStdString());
}

void MainWindow::on_pushButton_2_clicked()
{
    jsonAnalizer->startAnalizing();
}
