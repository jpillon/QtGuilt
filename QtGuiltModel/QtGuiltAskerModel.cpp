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

#include "QtGuiltAskerModel.h"
#include <QtGuiltCommandIssuer.h>
#include <QtGuiltAbstractUserAsk.h>

QtGuiltAskerModel::QtGuiltAskerModel(QtGuiltAbstractUserAsk *asker, QObject *parent) :
  QtGuiltModel(parent),
  ptr_asker(asker)
{
  Q_ASSERT(ptr_asker != NULL);
}

bool QtGuiltAskerModel::executeCommand(QtGuiltAskerModel::QtGuiltCommand command, QVariant arg1, QVariant arg2)
{
  bool result = true;
  result = realExecuteCommand(command, arg1, arg2);
  if(!result)
    result = whatToDoWithUnrefreshed(command, arg1, arg2);
  if(!result && !lastError().isEmpty())
  {
    QMap<int, QString> actions;
    actions[0] = tr("Ok");
    ptr_asker->askUserError(tr("Error"), tr("Error occured : \n") + lastError(), actions );
  }
  return result;
}

QtGuiltModel *QtGuiltAskerModel::guiltModel() const
{
  return const_cast<QtGuiltAskerModel *>(this);
}

bool QtGuiltAskerModel::realExecuteCommand(QtGuiltAskerModel::QtGuiltCommand command, QVariant arg1, QVariant arg2)
{
  bool result = true;
  QMap<int, QString> actions;
  switch(command)
  {
    case pushPatchCommand:
      result = push(arg1.toString(), arg2.toBool());
      break;
    case popPatchCommand:
      result = pop(arg1.toString(), arg2.toBool());
      break;
    case movePatchCommand:
      result = move(arg1.toString(), arg2.toString());
      break;
    case mergePatchesCommand:
      if(mustConfirm(command, arg1, arg2))
      {
        if(arg2.toString().isEmpty())
        {
          QString tmp;
          top(tmp);
          arg2 = tmp;
        }
        result = merge(arg1.toString(), arg2.toString());
      }
      break;
    case renamePatchCommand:
      result = rename(arg1.toString(), arg2.toString());
      break;
    case explodePatchCommand:
      if(mustConfirm(command, arg1, arg2))
        result = explode(arg1.toString());
      break;
    case extractPatchCommand:
      if(mustConfirm(command, arg1, arg2))
        result = extract(arg1.toString(), arg2.toString());
      break;
    case delPatchCommand:
      if(mustConfirm(command, arg1, arg2))
        result = del(arg1.toString());
      break;
    case newPatchCommand:
      result = newPatch(arg1.toString());
      break;
    case importPatchCommand:
      result = import(arg1.toString());
      break;
    case refreshPatchCommand:
      result = refresh(arg1.toString());
      break;
    case enablePatchCommand:
      result = setEnabled(arg1.toString(), arg2.toBool());
      break;
    case addFilesToPatchCommand:
      result = addFiles(arg1.toString(), arg2.toStringList());
      break;
    case delFilesFromPatchCommand:
      if(mustConfirm(command, arg1, arg2))
        result = delFiles(arg1.toString(), arg2.toStringList());
      break;
    case discardChangesCommand:
      if(mustConfirm(command, arg1, arg2))
        result = discard();
      break;
    case unrefreshedCommand:
    {
      QString content;
      if(unrefreshed(content))
      {
        int answer = -1;
        actions.clear();
        actions[0] = tr("Refresh");
        actions[1] = tr("Apply changes to other");
        actions[2] = tr("Apply changes to new");
        actions[3] = tr("Cancel");
        answer = ptr_asker->askUser(tr("Unrefreshed modifications"), content, actions, true);
        if(answer == 0)
        {
          result = realExecuteCommand(refreshPatchCommand);
        } else if(answer == 1)
        {
          actions.clear();
          actions[0] = tr("Choose");
          actions[1] = tr("Cancel");
          QStringList s;
          QList<QVariant> choices;
          QVariant patch;
          if(series(s))
          {
            foreach(QString p, s)
            {
              choices.append(p);
            }
            if(ptr_asker->askUserChoice(  tr("Where to merge ?"),
                                          tr("In which patch do you whant to apply changes ?"),
                                          actions,
                                          choices,
                                          patch) == 0)
            {
              result = realExecuteCommand(refreshPatchCommand, patch);
            }
          }
        } else if(answer == 2)
        {
          QString patch;
          if(temporaryPatchName(patch))
          {
            result = realExecuteCommand(refreshPatchCommand, patch);
          }
        }
      }
    }
      break;
    default:
      result = false;
      m_lastError = tr("Unknown command : ") + QString::number(command);
  }
  return result;
}

bool QtGuiltAskerModel::whatToDoWithUnrefreshed(QtGuiltAskerModel::QtGuiltCommand command, QVariant arg1, QVariant arg2)
{
  bool result = false;
  QMap<int, QString> actions;
  if(lastError().contains("Uncommited changes detected. Refresh first."))
  {
    int answer = -1;
    actions[0] = tr("Force");
    actions[1] = tr("Refresh");
    actions[2] = tr("Apply changes to other");
    actions[3] = tr("Apply changes to new");
    actions[4] = tr("Cancel");
    answer = ptr_asker->askUserError(tr("Error"), lastError() + tr("\nWhat would you like to do ?"), actions);
    if(answer == 0)
    {
      if(realExecuteCommand(discardChangesCommand))
        result = realExecuteCommand(command, arg1, arg2);
    } else if(answer == 1)
    {
      if(realExecuteCommand(refreshPatchCommand))
      {
        result = realExecuteCommand(command, arg1, arg2);
      }
    } else if(answer == 2)
    {
      actions[0] = tr("Choose");
      actions[1] = tr("Cancel");
      QStringList s;
      QList<QVariant> choices;
      QVariant patch;
      if(series(s))
      {
        foreach(QString p, s)
        {
          choices.append(p);
        }
        if(ptr_asker->askUserChoice(  tr("Where to merge ?"),
                                      tr("In which patch do you whant to apply changes ?"),
                                      actions,
                                      choices,
                                      patch) == 0)
        {
          if(realExecuteCommand(refreshPatchCommand, patch))
          {
            result = realExecuteCommand(command, arg1, arg2);
          }
        }
      }
    } else if(answer == 3)
    {
      QString patch;
      if(temporaryPatchName(patch))
      {
        if(realExecuteCommand(refreshPatchCommand, patch))
        {
          result = realExecuteCommand(command, arg1, arg2);
        }
      }
    }
  }
  return result;
}

bool QtGuiltAskerModel::mustConfirm(QtGuiltAskerModel::QtGuiltCommand command, QVariant arg1, QVariant arg2)
{
  QMap<int, QString> actions;
  actions[0] = tr("yes");
  actions[1] = tr("no");
  m_lastError = "";
  if(ptr_asker->askUser(tr("confirmation needed"),
                        confirmationMessage(command, arg1, arg2),
                        actions) == 0)
  {
    return true;
  }
  return false;
}

QString QtGuiltAskerModel::confirmationMessage(QtGuiltAskerModel::QtGuiltCommand command, QVariant arg1, QVariant arg2)
{
  QString result = "";
  QString what = tr("Are you sure you want to ");
  switch(command)
  {
    case pushPatchCommand:
      break;
    case popPatchCommand:
      break;
    case movePatchCommand:
      break;
    case mergePatchesCommand:
    {
      QString second = arg2.toString();
      if(second.isEmpty())
        second = tr("top patch");
      result = (what + tr("merge %1 into %2")).arg(arg1.toString()).arg(second);
      break;
    }
    case renamePatchCommand:
      break;
    case explodePatchCommand:
      result = (what + tr("explode %1")).arg(arg1.toString());
      break;
    case extractPatchCommand:
      result = (what + tr("extract %1 modifications from %2")).arg(arg2.toString()).arg(arg1.toString());
      break;
    case delPatchCommand:
      result = (what + tr("delete %1")).arg(arg1.toString());
      break;
    case newPatchCommand:
      break;
    case importPatchCommand:
      break;
    case refreshPatchCommand:
      break;
    case enablePatchCommand:
      break;
    case addFilesToPatchCommand:
      break;
    case delFilesFromPatchCommand:
      result = (what + tr("delete these file(s) from %1")).arg(arg1.toString());
      break;
    case discardChangesCommand:
      result = (what + tr("discard unrefreshed modifications"));
      break;
    case unrefreshedCommand:
      break;
    default:
      break;
  }
  return result + " ?";
}
