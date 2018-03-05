#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cuda-lib/cuda-lib.h>

MainWindow::MainWindow(QWidget *a) :
    QWidget(a),
    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
    func(123);
}

MainWindow::~MainWindow()
{
    delete m_ui;
}
