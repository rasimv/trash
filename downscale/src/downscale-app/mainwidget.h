#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QImage>

namespace Ui
{
    class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

private slots:
    void on_pushButton_loadImage_clicked();
    void on_pushButton_downscale_clicked();

private:
    static QImage downscale(const QImage &a_source, int a_newWidth, int a_newHeight);

    Ui::MainWidget *ui;
    QImage m_image;
};

#endif // MAINWIDGET_H
