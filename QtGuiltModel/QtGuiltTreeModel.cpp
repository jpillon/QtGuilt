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

#include "QtGuiltTreeModel.h"
#include "QtGuiltTreeRootItem.h"
#include <QtGuiltModel/QtGuiltAskerModel.h>
#include <QCoreApplication>
#include <QtGuiltPatch.h>
#include <QDebug>
#include <QMimeData>

QtGuiltTreeModel::QtGuiltTreeModel(QtGuiltAbstractUserAsk *asker, QObject *parent) :
  QAbstractItemModel(parent),
  p_askerModel(new QtGuiltAskerModel(asker, parent))
{
  p_root_item = new QtGuiltTreeRootItem(this);
  editEnd();
}

QtGuiltTreeModel::~QtGuiltTreeModel()
{
}

int QtGuiltTreeModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return 1;
}

QVariant QtGuiltTreeModel::data(const QModelIndex &index, int role) const
{
  QVariant res;
  QtGuiltTreeItem* item = (QtGuiltTreeItem*)index.internalPointer();
  if(item)
    res = item->data(index.column(), role);
  return res;
}

bool QtGuiltTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  bool result = false;
  QtGuiltPatch* patch = dynamic_cast<QtGuiltPatch*>((QtGuiltTreeItem*)index.internalPointer());
  if(role == Qt::DisplayRole)
  {
    //Moved...
    if(patch)
      result = p_askerModel->executeCommand(QtGuiltAskerModel::movePatchCommand, value, patch->data());
    else if(!value.toString().isEmpty())
      result = p_askerModel->executeCommand(QtGuiltAskerModel::movePatchCommand, value, QVariant());
  } else
  {
    if(!index.isValid())
        return result;
    if(patch)
    {
      if(index.column()==0)
      {
        switch(role)
        {
          case Qt::EditRole:
          //Renamed
            if(patch->data() != value)
            {
              editBegin();
              result = p_askerModel->executeCommand(QtGuiltAskerModel::renamePatchCommand, patch->data(), value);
              editEnd();
            }
            break;
        }
      }
    }
  }
  return result;
}

QModelIndex QtGuiltTreeModel::index(int row, int column, const QModelIndex &parent) const
{
  QModelIndex res;
  QtGuiltTreeItem* item = (QtGuiltTreeItem*)parent.internalPointer();
  if(!item)
  {
    item = p_root_item;
  }
  if(row < item->treeChildrenCount())
  {
    res = createIndex(row, column, item->treeChildren()[row]);
    item->treeChildren()[row]->updateIndex(res);
  }
  return res;
}

QModelIndex QtGuiltTreeModel::parent(const QModelIndex &index) const
{
  QModelIndex res;
  if(index.isValid())
  {
    QtGuiltTreeItem* item = (QtGuiltTreeItem*)index.internalPointer();
    if(item->treeParent())
    {
      res = item->treeParent()->leftIndex();
    }
  }
  return res;
}

int QtGuiltTreeModel::rowCount(const QModelIndex &parent) const
{
  QtGuiltTreeItem* item = NULL;
  if(parent.isValid())
  {
    //count modified files in patch
    item = (QtGuiltTreeItem*)parent.internalPointer();
  } else
  {
    //count patches
    item = p_root_item;
  }

  if(item)
    return item->treeChildrenCount();
  return 0;
}

Qt::ItemFlags QtGuiltTreeModel::flags(const QModelIndex &index) const
{
  QtGuiltPatch* patch = dynamic_cast<QtGuiltPatch*>((QtGuiltTreeItem*)index.internalPointer());
  Qt::ItemFlags result = QAbstractItemModel::flags(index);

  if(patch || (!index.isValid() && !index.parent().isValid()))
  {
      result = Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEditable | result;
  }
  return result;
}

Qt::DropActions QtGuiltTreeModel::supportedDropActions() const
{
    return QAbstractItemModel::supportedDropActions() | Qt::MoveAction;
}

bool QtGuiltTreeModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
    if(row >= rowCount(parent)) {
        // Special case when moving a patch at the top
        return setData(QModelIndex(), QString(data->data("application/x-QtGuiltTreeModelPatchName")), Qt::DisplayRole);
    } else {
        return QAbstractItemModel::dropMimeData(data, action, row, column, parent);
    }
}

QMimeData* QtGuiltTreeModel::mimeData(const QModelIndexList& indexes) const
{
    QMimeData* res = QAbstractItemModel::mimeData(indexes);
    if(indexes.length() == 1) {
        QtGuiltPatch* patch = dynamic_cast<QtGuiltPatch*>((QtGuiltTreeItem*)indexes[0].internalPointer());
        if(patch) {
            res->setData("application/x-QtGuiltTreeModelPatchName", patch->filename().toLatin1());
        }
    }
    return res;
}

QtGuiltAskerModel *QtGuiltTreeModel::guiltAskerModel() const
{
  return p_askerModel;
}

QtGuiltModel *QtGuiltTreeModel::guiltModel() const
{
  return p_askerModel->guiltModel();
}

void QtGuiltTreeModel::notify(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
  //Why cannot just emit dataChanged() ?
  emit dataChanged(topLeft, bottomRight);
  emit layoutChanged();
}

void QtGuiltTreeModel::notifyBeginInsertRows(const QModelIndex &parent, int first, int last)
{
  beginInsertRows(parent, first, last);
}

void QtGuiltTreeModel::notifyEndInsertRows()
{
  endInsertRows();
}

void QtGuiltTreeModel::notifyBeginRemoveRows(const QModelIndex &parent, int first, int last)
{
  beginRemoveRows(parent, first, last);
}

void QtGuiltTreeModel::notifyEndRemoveRows()
{
  endRemoveRows();
}

void QtGuiltTreeModel::notifyTopChanged(int i)
{
  QModelIndex ind = createIndex(i, 0, p_root_item->treeChildren()[i]);
  emit topChanged(ind);
}

void QtGuiltTreeModel::repoChanged()
{
  beginResetModel();
  endResetModel();
}

void QtGuiltTreeModel::editBegin() const
{
  disconnect(p_root_item, SIGNAL(topChanged(int)), this, SLOT(notifyTopChanged(int)));
}

void QtGuiltTreeModel::editEnd() const
{
  connect(p_root_item, SIGNAL(topChanged(int)), this, SLOT(notifyTopChanged(int)), Qt::UniqueConnection);
}
