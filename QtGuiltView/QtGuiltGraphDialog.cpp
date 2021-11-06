#include "QtGuiltGraphDialog.h"
#include <QtSvg/QSvgWidget>
#include "ui_QtGuiltGraphDialog.h"

QtGuiltGraphDialog::QtGuiltGraphDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QtGuiltGraphDialog),
    p_svg(nullptr)
{
    ui->setupUi(this);
    p_svg = new QSvgWidget(ui->scrollArea);
}

QtGuiltGraphDialog::~QtGuiltGraphDialog()
{
    delete ui;
}

void QtGuiltGraphDialog::showSVG(QByteArray svg)
{
    p_svg->load(svg);
    p_svg->adjustSize();
    ui->scrollArea->setWidget(p_svg);
    show();
}
