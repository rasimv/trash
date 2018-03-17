#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QFileDialog>
#include <QMessageBox>
#include <downscale/downscale.h>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::on_pushButton_loadImage_clicked()
{
    const QString l_filepath(QFileDialog::getOpenFileName(this,
                                                          windowTitle() + " - Open image file...",
                                                          "",
                                                          "Images (*.png *.jpg *.bmp)"));
    if (l_filepath.isEmpty()) return;
    if (!m_image.load(l_filepath))
    {
        ui->label_loaded->setText("");
        return;
    }
    ui->label_loaded->setText(QString("%1x%2").arg(m_image.width()).arg(m_image.height()));
}

void MainWidget::on_pushButton_downscale_clicked()
{
    const int l_newWidth = ui->lineEdit_newWidth->text().toInt(),
              l_newHeight = ui->lineEdit_newHeight->text().toInt();
    if (l_newWidth < 1 || l_newWidth >= m_image.width() ||
        l_newHeight < 1 || l_newHeight >= m_image.height())
    {
        QMessageBox::critical(this,
                              windowTitle() + " - Error",
                              "Invalid source image or\nnew size must be less than source");
        return;
    }

    const QString l_filepath(QFileDialog::getSaveFileName(this,
                                                          windowTitle() + " - Save image to file...",
                                                          "",
                                                          "Images (*.png)"));
    QImage l_result(downscale(m_image, l_newWidth, l_newHeight));
    l_result.save(l_filepath);
}

QImage MainWidget::downscale(const QImage &a_source, int a_newWidth, int a_newHeight)
{
    QImage l_result(a_newWidth, a_newHeight, QImage::Format_ARGB32);
    int (QColor::*const l_get[])() const =
        { &QColor::alpha, &QColor::red, &QColor::green, &QColor::blue };
    void (QColor::*const l_set[])(int) =
        { &QColor::setAlpha, &QColor::setRed, &QColor::setGreen, &QColor::setBlue };

    std::vector<uint16_t> l_s(a_source.width() * a_source.height());
    std::vector<uint16_t> l_d(a_newWidth * a_newHeight);
    for (auto q(std::begin(l_get)); q != std::end(l_get); q++)
    {
        for (int i = 0; i < a_source.height(); i++)
            for (int j = 0; j < a_source.width(); j++)
            {
                const QColor l_col(a_source.pixelColor(j, i));
                l_s[j + i * a_source.width()] = (l_col.*l_get[std::distance(std::begin(l_get), q)])();
            }

        ::downscale(l_s, a_source.width(), a_source.height(), l_d, a_newWidth, a_newHeight);
        for (int i = 0; i < l_result.height(); i++)
            for (int j = 0; j < l_result.width(); j++)
            {
                QColor l_col(l_result.pixelColor(j, i));
                (l_col.*l_set[std::distance(std::begin(l_get), q)])(l_d[j + i * l_result.width()]);
                l_result.setPixelColor(j, i, l_col);
            }
    }

    return l_result;
}
