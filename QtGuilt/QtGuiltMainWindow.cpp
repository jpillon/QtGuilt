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

#include "QtGuiltMainWindow.h"
#include "ui_QtGuiltMainWindow.h"
#include <QtGuiltModel/QtGuiltTreeModel.h>
#include <QtGuiltModel/QtGuiltAskerModel.h>
#include <QtGuiltView/QtGuiltGUIAsker.h>
#include <QSettings>
#include <QProgressBar>
#include <QDebug>

QString QtGuiltMainWindow::m_geometryTagName = "geometry";
QString QtGuiltMainWindow::m_stateTagName = "state";

QtGuiltMainWindow::QtGuiltMainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::QtGuiltMainWindow)
{
  ui->setupUi(this);

  QSettings settings(COMPANY_NAME, APPLICATION_NAME);
  if(settings.contains(m_geometryTagName))
      restoreGeometry(settings.value(m_geometryTagName).toByteArray());
  if(settings.contains(m_stateTagName))
      restoreState(settings.value(m_stateTagName).toByteArray(), SAVE_RESTORE_VERSION);
}

QtGuiltMainWindow::~QtGuiltMainWindow()
{
  delete ui;
}

void QtGuiltMainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings(COMPANY_NAME, APPLICATION_NAME);
    settings.setValue(m_geometryTagName, saveGeometry());
    settings.setValue(m_stateTagName, saveState(SAVE_RESTORE_VERSION));
    QMainWindow::closeEvent(event);
}

void QtGuiltMainWindow::setRepo(const QString &repo)
{
  ui->widgetGuilt->treeModel()->guiltModel()->setRepo(repo);
}

