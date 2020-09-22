#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <JsonAnalizer.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_startAnalyze_clicked();

private slots:
    void on_selectFile_clicked();

private slots:
    void on_enable_oks_stateChanged(int arg1);

private slots:
    void on_enable_errors_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
    JsonAnalizer * jsonAnalizer = nullptr;
    QString file_path;
};

#endif // MAINWINDOW_H
