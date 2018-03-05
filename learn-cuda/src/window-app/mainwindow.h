#ifndef __MAINWINDOW_H
#define __MAINWINDOW_H

#include <QWidget>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *a = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *m_ui;
};

#endif // __MAINWINDOW_H
