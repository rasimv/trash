#ifndef __MAINWIDGET_H
#define __MAINWIDGET_H

#include <QWidget>
#include <QRectF>
#include <downscale/downscale.h>

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

protected:
    virtual void mouseMoveEvent(QMouseEvent *a) override;
    virtual void paintEvent(QPaintEvent *a) override;

private:
    static QRectF rect2qrectf(const Rect &a);
    static Rect qrectf2rect(const QRectF &a);

    Ui::MainWidget *ui;
    QRectF m_movable, m_stationary, m_intersection;
};

#endif // __MAINWIDGET_H
