/**************************************************************************
**  Copyright (C) 2013 jpillon
**
**  This file is part of QtGuilt.
**
**  QtGuilt is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  QtGuilt is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with QtGuilt.  If not, see <http://www.gnu.org/licenses/>.
**
**************************************************************************/

#ifndef QTGUILTMAINWINDOW_H
#define QTGUILTMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
  class QtGuiltMainWindow;
}

class QtGuiltMainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    explicit QtGuiltMainWindow(QWidget *parent = 0);
    ~QtGuiltMainWindow();
    virtual void closeEvent(QCloseEvent *event);
    void setRepo(const QString& repo);
  private:
    static QString m_geometryTagName;
    static QString m_stateTagName;
    Ui::QtGuiltMainWindow *ui;
};

#endif // QTGUILTMAINWINDOW_H
