/**************************************************************************
**  Copyright (C) 2013 jpillon
**
**  This file is part of %PROJECT%.
**
**  %PROJECT% is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  %PROJECT% is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with %PROJECT%.  If not, see <http://www.gnu.org/licenses/>.
**
**************************************************************************/

#include "QtGuiltGUIAsker.h"


#include "QtGuiltGUIAsker.h"
#include "ui_QtGuiltGUIAsker.h"
#include <QtGuiltDiffSyntaxHighlighter.h>
#include <QPushButton>

QtGuiltGUIAsker::QtGuiltGUIAsker(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::QtGuiltGUIAsker)
{
  ui->setupUi(this);
  p_diffHighlighter = new QtGuiltDiffSyntaxHighlighter(this);
  for(int i=0;i<10;i++)
  {
    QPushButton* b = new QPushButton(ui->groupBoxButtons);
    ui->groupBoxButtons->layout()->addWidget(b);
    b->hide();
    m_buttons[i] = b;
    connect(b, SIGNAL(clicked()), this, SLOT(buttonClicked()));
  }
  m_savedFont = ui->textEdit->font();
}

QtGuiltGUIAsker::~QtGuiltGUIAsker()
{
  delete ui;
}

int QtGuiltGUIAsker::askUserError(const QString &title, const QString &text, const QMap<int, QString> &actions, bool textIsDiff)
{
  updateHighlighter(textIsDiff);
  setWindowTitle(title);
  ui->textEdit->setText(text);
  ui->lineEdit->hide();
  ui->comboBox->hide();
  addButtons(actions);
  exec();
  return result();
}

int QtGuiltGUIAsker::askUser(const QString &title, const QString &text, const QMap<int, QString> &actions, bool textIsDiff)
{
  updateHighlighter(textIsDiff);
  setWindowTitle(title);
  ui->textEdit->setText(text);
  ui->lineEdit->hide();
  ui->comboBox->hide();
  addButtons(actions);
  exec();
  return result();
}

int QtGuiltGUIAsker::askUserChoice(const QString &title, const QString &text, const QMap<int, QString> &actions, const QList<QVariant> &data, QVariant &value, bool textIsDiff)
{
  updateHighlighter(textIsDiff);
  setWindowTitle(title);
  ui->textEdit->setText(text);
  foreach(QVariant v, data)
    ui->comboBox->addItem(v.toString());
  ui->lineEdit->hide();
  ui->comboBox->show();
  addButtons(actions);
  exec();
  value = ui->comboBox->currentText();
  return result();
}

int QtGuiltGUIAsker::askUserData(const QString &title, const QString &text, const QMap<int, QString> &actions, QVariant &value, bool textIsDiff)
{
  updateHighlighter(textIsDiff);
  setWindowTitle(title);
  ui->textEdit->setText(text);
  ui->lineEdit->setText(value.toString());
  ui->lineEdit->show();
  ui->comboBox->hide();
  addButtons(actions);
  exec();
  value = ui->lineEdit->text();
  return result();
}

void QtGuiltGUIAsker::addButtons(const QMap<int, QString> &actions)
{
  foreach(QPushButton* b, m_buttons.values())
    b->hide();
  foreach(int val, actions.keys())
  {
    m_buttons[val]->setText(actions[val]);
    m_buttons[val]->show();
  }
}

void QtGuiltGUIAsker::updateHighlighter(bool set)
{
  p_diffHighlighter->setDocument(NULL);
  ui->textEdit->setFont(m_savedFont);
  if(set)
  {
    QFont f("monospace");
    ui->textEdit->setFont(f);
    p_diffHighlighter->setDocument(ui->textEdit->document());
  }
}

void QtGuiltGUIAsker::buttonClicked()
{
  QPushButton* b = (QPushButton*)sender();
  done(m_buttons.key(b));
}
