#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QPainter>
#include <QMouseEvent>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget),
    m_movable(0, 0, 200, 150), m_stationary(200, 150, 500, 400)
{
    ui->setupUi(this);
    resize(1000, 700);
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::mouseMoveEvent(QMouseEvent *a)
{
    m_movable.moveTo(QPointF(a->pos()));
    const Rect l_intersection(intersection(qrectf2rect(m_stationary), qrectf2rect(m_movable)));
    m_intersection = rect2qrectf(l_intersection);
    repaint();
}

void MainWidget::paintEvent(QPaintEvent *a)
{
    QPainter l_painter(this);
    l_painter.drawRect(m_stationary);
    l_painter.drawRect(m_movable);
    l_painter.setBrush(QBrush(QColor(255, 0, 0)));
    l_painter.drawRect(m_intersection);
}

QRectF MainWidget::rect2qrectf(const Rect &a)
{
    return QRectF(QPointF(a.p1.x, a.p1.y), QPointF(a.p2.x, a.p2.y));
}

Rect MainWidget::qrectf2rect(const QRectF &a)
{
    return Rect{ { float(a.topLeft().x()), float(a.topLeft().y()) },
                 { float(a.bottomRight().x()), float(a.bottomRight().y()) } };
}
