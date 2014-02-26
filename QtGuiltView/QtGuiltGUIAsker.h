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

#ifndef QTGUILTGUIASKER_H
#define QTGUILTGUIASKER_H
#include "qtguiltview_global.h"
#include <QtGuiltModel/QtGuiltAbstractUserAsk.h>
#include <QDialog>
#include <QMap>

class QPushButton;
class QtGuiltDiffSyntaxHighlighter;
namespace Ui {
class QtGuiltGUIAsker;
}

class QTGUILTVIEWSHARED_EXPORT QtGuiltGUIAsker : public QDialog, public QtGuiltAbstractUserAsk
{
    Q_OBJECT

public:
    explicit QtGuiltGUIAsker(QWidget *parent = 0);
    ~QtGuiltGUIAsker();
    /**
      * @brief Ask the user if must do something when error occured
      * @param title The title of the question
      * @param text The message of the question
      * @param actions A map for possible actions.
      * @param textIsDiff true if text is diff content
      * @return the index of chosen action
      */
    virtual int askUserError(const QString& title, const QString& text, const QMap< int, QString>& actions, bool textIsDiff = false);
    /**
      * @brief Ask the user if must do something (question)
      * @param title The title of the question
      * @param text The message of the question
      * @param actions A map for possible actions.
      * @param textIsDiff true if text is diff content
      * @return the index of chosen action
      */
    virtual int askUser(const QString& title, const QString& text, const QMap< int, QString>& actions, bool textIsDiff = false);
    /**
      * @brief Ask the user to choose inside data
      * @param title The title of the question
      * @param text The message of the question
      * @param actions A map for possible actions
      * @param data The data where to choose
      * @param[inout] value The chosen value
      * @param textIsDiff true if text is diff content
      * @return the index of chosen action
      */
    virtual int askUserChoice(const QString& title, const QString& text, const QMap< int, QString>& actions, const QList<QVariant>& data, QVariant& value, bool textIsDiff = false);
    /**
      * @brief Ask the user for single data
      * @param title The title of the question
      * @param text The message of the question
      * @param actions A map for possible actions
      * @param[inout] value The value. Could be filled for default value
      * @param textIsDiff true if text is diff content
      * @return the index of chosen action
      */
    virtual int askUserData(const QString& title, const QString& text, const QMap< int, QString>& actions, QVariant& value, bool textIsDiff = false);
  public slots:
      /**
        * @brief Sets the result according to the sender
        */
      void buttonClicked();
  private:
    /**
      * @brief Adds buttons and do what needed for setResult...
      */
    void addButtons(const QMap< int, QString>& actions);

    /**
     * @brief Sets the highlighter if needed
     * @param set true if highlighter must be set
     */
    void updateHighlighter(bool set);
    QMap<int, QPushButton*> m_buttons;
    Ui::QtGuiltGUIAsker *ui;
    QtGuiltDiffSyntaxHighlighter* p_diffHighlighter;
    QFont m_savedFont;
};

#endif // QTGUILTGUIASKER_H
