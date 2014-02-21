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
#ifndef QTGUILTASKERMODEL_H
#define QTGUILTASKERMODEL_H

#include <qtguiltmodel_global.h>
#include <QtGuiltModel/QtGuiltModel.h>
#include <QVariant>
#include <QStack>

class QtGuiltAbstractUserAsk;

/**
 * @brief This is a layer on top of the model that asks user for confirmation etc.
 *
 * The cases are :
 *  - Risky action => must confirm
 *  - push or pop goes wrong with uncommited changes => what to do ? (refresh, apply on new, apply on other, discard)
 *  - diffz => what to do ? (refresh, apply on new, apply on other, discard)
 *  - something goes wrong => show error
 */
class QTGUILTMODELSHARED_EXPORT QtGuiltAskerModel : protected QtGuiltModel
{
    Q_OBJECT
  public:
    /**
     * @brief Available commands
     */
    enum QtGuiltCommand
    {
      pushPatchCommand,
      popPatchCommand,
      movePatchCommand,
      mergePatchesCommand,
      renamePatchCommand,
      explodePatchCommand,
      extractPatchCommand,
      delPatchCommand,
      newPatchCommand,
      importPatchCommand,
      refreshPatchCommand,
      enablePatchCommand,
      addFilesToPatchCommand,
      delFilesFromPatchCommand,
      discardChangesCommand,
      unrefreshedCommand
    };

    /**
     * @brief Constructor
     * @param asker The associated asker.
     * @param parent the parent QObject
     */
    explicit QtGuiltAskerModel(QtGuiltAbstractUserAsk* asker, QObject *parent = 0);

    /**
      * @brief Executes a command
      *
      * It executes the given command, and ask user before execute or after a fail if needed...
      *
      * @param command The command to executes
      * @param arg1 The first arg
      * @param arg2 The first second
      * @return true if success
      */
    bool executeCommand(QtGuiltCommand command, QVariant arg1 = QVariant(), QVariant arg2 = QVariant());

    /**
      * @brief Returns this as a QtGuiltModel
      * @return this as a QtGuiltModel
      */
    QtGuiltModel *guiltModel() const;
  private:
    bool realExecuteCommand(QtGuiltCommand command, QVariant arg1 = QVariant(), QVariant arg2 = QVariant());
    /**
     * @brief Used when unrefreshed modifications are detected
     *
     * @param command the command
     * @param arg1 eventual first arg
     * @param arg2 eventual second arg
     * @return true if success
     */
    bool whatToDoWithUnrefreshed(QtGuiltAskerModel::QtGuiltCommand command, QVariant arg1 = QVariant(), QVariant arg2 = QVariant());

    /**
     * @brief Ask for confirmation before doing anything
     *
     * @param command The command to be confirmed
     * @param arg1 the first arg of the command
     * @param arg2 the second arg of the command
     * @return true if user confirmed
     */
    bool mustConfirm(QtGuiltAskerModel::QtGuiltCommand command, QVariant arg1 = QVariant(), QVariant arg2 = QVariant());

    /**
     * @brief constructs the confirmation message concidering the command about to be executed
     *
     * @param command the command
     * @param arg1 the first arg given to the command
     * @param arg2 the second arg given to the command
     * @return the confirmation message
     */
    QString confirmationMessage(QtGuiltAskerModel::QtGuiltCommand command, QVariant arg1 = QVariant(), QVariant arg2 = QVariant());
    QtGuiltAbstractUserAsk* ptr_asker;


    QStack<QtGuiltAskerModel::QtGuiltCommand> m_confirmStack;
  signals:

  public slots:

};

#endif // QTGUILTASKERMODEL_H
