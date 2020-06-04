#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <string>
#include <filesystem>

#include <QString>

#include <QFileDialog>

#include <fstream>
#include <iostream>

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

    delete jsonAnalizer;
    jsonAnalizer = new JsonAnalizer(file_path.toStdString());
}

void MainWindow::on_pushButton_2_clicked()
{
    jsonAnalizer->startAnalizing();
    QFile file("analyzer_logs.txt");

    file.open(QFile::ReadOnly);
    QString content = QString::fromUtf8(file.readAll());
    ui->plainTextEdit->setPlainText(content);
    file.close();
}
