#pragma once
#include "qtguiltview_global.h"

#include <QDialog>

class QSvgWidget;

namespace Ui {
    class QtGuiltGraphDialog;
}

class QTGUILTVIEWSHARED_EXPORT QtGuiltGraphDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit QtGuiltGraphDialog(QWidget *parent = nullptr);
        ~QtGuiltGraphDialog();

        void showSVG(QByteArray svg);

    private:
        Ui::QtGuiltGraphDialog *ui;
        QSvgWidget* p_svg;
};

