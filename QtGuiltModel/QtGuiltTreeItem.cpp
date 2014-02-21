#include "QtGuiltTreeItem.h"
#include <QCoreApplication>
#include <QDebug>

QtGuiltTreeItem::QtGuiltTreeItem(QObject *parent) :
  QObject(parent),
  p_parent(NULL)
{
  p_parent = dynamic_cast<QtGuiltTreeItem*>(parent);
}

void QtGuiltTreeItem::setTreeParent(QtGuiltTreeItem *parent)
{
  p_parent = parent;
}

void QtGuiltTreeItem::addTreeChild(QtGuiltTreeItem *child)
{
  notifyBeginInsertRows(leftIndex(), treeChildrenCount(), treeChildrenCount());
  child->setTreeParent(this);
  m_children.append(child);
  connect(child, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(notify(QModelIndex,QModelIndex)));
  notifyEndInsertRows();
}

void QtGuiltTreeItem::deleteTreeChild(QtGuiltTreeItem *child)
{
  notifyBeginRemoveRows(leftIndex(), indexOfTreeChild(child), indexOfTreeChild(child));
  m_children.removeAll(child);
  disconnect(child, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(notify(QModelIndex,QModelIndex)));
  delete child;
  notifyEndRemoveRows();
}

void QtGuiltTreeItem::addTreeChildren(QList<QtGuiltTreeItem *> children)
{
  foreach(QtGuiltTreeItem* item, children)
    item->setTreeParent(this);
  m_children.append(children);
}

void QtGuiltTreeItem::clearTreeChildren()
{
  m_children.clear();
}

void QtGuiltTreeItem::setIndexes(const QModelIndex &left, const QModelIndex &right)
{
  m_left = left;
  m_right = right;
}

void QtGuiltTreeItem::updateIndex(const QModelIndex &index)
{
  if(!m_left.isValid())
    m_left = index;
  else if(!m_right.isValid())
    m_right = index;

  if(index.column() > m_right.column())
    m_right = index;
  if(index.column() < m_left.column())
    m_left = index;
}

void QtGuiltTreeItem::clearIndexes()
{
  m_left = QModelIndex();
  m_right = QModelIndex();
}

QtGuiltTreeItem *QtGuiltTreeItem::treeParent() const
{
  return p_parent;
}

QList<QtGuiltTreeItem *> QtGuiltTreeItem::treeChildren() const
{
  return m_children;
}

void QtGuiltTreeItem::clearTree()
{
  p_parent = NULL;
  clearTreeChildren();
  clearIndexes();
}

int QtGuiltTreeItem::indexOfTreeChild(QtGuiltTreeItem *child) const
{
  return m_children.indexOf(child);
}

int QtGuiltTreeItem::treeChildrenCount() const
{
  return m_children.size();
}

QModelIndex QtGuiltTreeItem::leftIndex() const
{
  return m_left;
}

QModelIndex QtGuiltTreeItem::rightIndex() const
{
  return m_right;
}

QtGuiltTreeItem *QtGuiltTreeItem::treeChild(const QVariant &comp, int column, int role) const
{
  foreach(QtGuiltTreeItem *child, treeChildren())
  {
    if(child->data(column, role) == comp)
      return child;
  }
  return NULL;
}

QList<QVariant> QtGuiltTreeItem::treeData(int column, int role) const
{
  QList<QVariant> res;
  foreach(QtGuiltTreeItem *child, treeChildren())
  {
    res.append(child->data(column, role));
  }
  return res;
}

void QtGuiltTreeItem::treeSortChildren(QList<QVariant> model, int column, int role)
{
  if(treeData(column, role) == model)
    return;
  QList<QtGuiltTreeItem*> notif;
  QList<QtGuiltTreeItem*> newChildren;
  int i=0;
  foreach(QVariant va, model)
  {
    QtGuiltTreeItem* current = treeChild(va, column, role);
    if(current)
    {
      if(current != m_children[i])
        notif.append(current);
      newChildren.append(current);
    }
  }
  m_children = newChildren;
  foreach(QtGuiltTreeItem* item, notif)
  {
    item->notifyModification();
  }
}

QVariant	QtGuiltTreeItem::data ( int column, int role ) const
{
  Q_UNUSED(column);
  Q_UNUSED(role);
  return QVariant();
}

void QtGuiltTreeItem::notifyModification()
{
  if(m_left.isValid() && m_right.isValid())
    notify(m_left, m_right);
  else if(m_left.isValid())
    notify(m_left, m_left);
  else if(m_right.isValid())
    notify(m_right, m_right);
}

void QtGuiltTreeItem::notify(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
  if(treeParent())
    treeParent()->notify(topLeft, bottomRight);
}

void QtGuiltTreeItem::notifyBeginInsertRows(const QModelIndex &parent, int first, int last)
{
  if(treeParent())
    treeParent()->notifyBeginInsertRows(parent, first, last);
}

void QtGuiltTreeItem::notifyEndInsertRows()
{
  if(treeParent())
    treeParent()->notifyEndInsertRows();
}

void QtGuiltTreeItem::notifyBeginRemoveRows(const QModelIndex &parent, int first, int last)
{
  if(treeParent())
    treeParent()->notifyBeginRemoveRows(parent, first, last);
}

void QtGuiltTreeItem::notifyEndRemoveRows()
{
  if(treeParent())
    treeParent()->notifyEndRemoveRows();
}
