#include "QtGuiltTreeRootItem.h"
#include <QtGuiltTreeModel.h>
#include <QtGuiltModel.h>
#include <QtGuiltPatch.h>
#include <QtGuiltPatchFile.h>
#include <QCoreApplication>
#include <QDebug>

QtGuiltTreeRootItem::QtGuiltTreeRootItem(QtGuiltTreeModel *treeModel) :
  QtGuiltTreeItem(treeModel),
  p_model(NULL),
  p_treeModel(NULL),
  m_modified(true)
{
  setTreeModel(treeModel);
}

void QtGuiltTreeRootItem::updateSeries()
{
  if(p_model->series(m_series))
  {
    if(p_model->series(m_onlyActiveSeries, true))
    {
      treeItemUpdate();
    }
  }
}

void QtGuiltTreeRootItem::updateStatus()
{
  p_model->applied(m_status);
  treeItemUpdate();
}

void QtGuiltTreeRootItem::notify(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
  p_treeModel->notify(topLeft, bottomRight);
}

void QtGuiltTreeRootItem::setModel(QtGuiltModel *model)
{
  if(p_model)
  {
    disconnect(p_model, SIGNAL(seriesChanged()), this, SLOT(updateSeries()));
    disconnect(p_model, SIGNAL(statusChanged()), this, SLOT(updateStatus()));
    disconnect(p_model, SIGNAL(modifiedChanged(bool)), this, SLOT(setModified(bool)));
  }
  p_model = model;
  if(p_model)
  {
    connect(p_model, SIGNAL(seriesChanged()), this, SLOT(updateSeries()));
    connect(p_model, SIGNAL(statusChanged()), this, SLOT(updateStatus()));
    connect(p_model, SIGNAL(modifiedChanged(bool)), this, SLOT(setModified(bool)));
  }
}

void QtGuiltTreeRootItem::setTreeModel(QtGuiltTreeModel *model)
{
  p_treeModel = model;
  if(p_treeModel)
    setModel(p_treeModel->guiltModel());
}

void QtGuiltTreeRootItem::treeItemUpdate()
{
  //Add news...
  foreach(QString patch, m_series)
  {
    if(!treeChild(patch))
    {
      QtGuiltTreeItem* child = new QtGuiltPatch(p_model->patchFile(patch), this);
      connect(child, SIGNAL(topChanged()), this, SLOT(notifyTopChanged()));
      addTreeChild(child);
    }
  }
//  QCoreApplication::processEvents();

  //Remove deleted
  foreach(QVariant patch, treeData())
  {
    if(!m_series.contains(patch.toString()))
    {
      QtGuiltTreeItem* child = treeChild(patch.toString());
      deleteTreeChild(child);
    }
  }
//  QCoreApplication::processEvents();
  //Sort
  QList<QVariant> l;
  foreach(QString s, m_series)
    l.append(s);
  treeSortChildren(l);
//  QCoreApplication::processEvents();
  QString topPatch;
  p_model->top(topPatch);
  //Update
  foreach(QtGuiltTreeItem* item, treeChildren())
  {
    QtGuiltPatch* patch = dynamic_cast<QtGuiltPatch*>(item);
    QString name = patch->filename();
    patch->setActive(m_onlyActiveSeries.contains(name));
    patch->setApplied(m_status.contains(name));
    patch->setTop(name == topPatch);
  }
//  QCoreApplication::processEvents();
//  if(redo)
  //    treeItemUpdate(false);
}

void QtGuiltTreeRootItem::notifyTopChanged()
{
  QtGuiltTreeItem* item = dynamic_cast<QtGuiltTreeItem*>(sender());
  if(item)
  {
    emit topChanged(indexOfTreeChild(item));
  }
}

void QtGuiltTreeRootItem::notifyBeginInsertRows(const QModelIndex &parent, int first, int last)
{
  p_treeModel->notifyBeginInsertRows(parent, first, last);
}

void QtGuiltTreeRootItem::notifyEndInsertRows()
{
  p_treeModel->notifyEndInsertRows();
}

void QtGuiltTreeRootItem::notifyBeginRemoveRows(const QModelIndex &parent, int first, int last)
{
  p_treeModel->notifyBeginRemoveRows(parent, first, last);
}

void QtGuiltTreeRootItem::notifyEndRemoveRows()
{
  p_treeModel->notifyEndRemoveRows();
}

bool QtGuiltTreeRootItem::isModified() const
{
  return m_modified;
}

void QtGuiltTreeRootItem::setModified(bool modified)
{
  bool old = m_modified;
  m_modified = modified;
  if(old != m_modified)
  {
    QString topPatch;
    p_model->top(topPatch);
    //Update
    foreach(QtGuiltTreeItem* item, treeChildren())
    {
      QtGuiltPatch* patch = dynamic_cast<QtGuiltPatch*>(item);
      QString name = patch->filename();
      if(name == topPatch)
      {
        item->notifyModification();
        break;
      }
    }
  }
}
