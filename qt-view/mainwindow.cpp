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
    ui(new Ui::MainWindow),
    file_path()
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete jsonAnalizer;
}

void MainWindow::on_selectFile_clicked()
{
    file_path = QFileDialog::getOpenFileName(this,"Open a file", "/home/data/data_yak/config");
    ui->filePath->setText(file_path);
}

void MainWindow::on_startAnalyze_clicked()
{
    jsonAnalizer = new JsonAnalizer(file_path.toStdString());
    on_enable_errors_stateChanged(ui->enable_errors->checkState());
    on_enable_oks_stateChanged(ui->enable_oks->checkState());

    jsonAnalizer->startAnalizing();
    delete jsonAnalizer;

    QFile file("analyzer_logs.txt");

    file.open(QFile::ReadOnly);
    QString content = QString::fromUtf8(file.readAll());
    ui->plainTextEdit->setPlainText(content);
    file.close();
}

void MainWindow::on_enable_oks_stateChanged(int arg1)
{
    if(jsonAnalizer != nullptr)
        jsonAnalizer->set_print_ok(arg1);
}

void MainWindow::on_enable_errors_stateChanged(int arg1)
{
    if(jsonAnalizer != nullptr)
        jsonAnalizer->set_print_error(arg1);
}
