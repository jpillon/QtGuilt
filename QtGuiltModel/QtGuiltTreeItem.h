#ifndef QTGUILTTREEITEM_H
#define QTGUILTTREEITEM_H

#include <QObject>
#include <QList>
#include <QModelIndex>
#include <QtGuiltModel/QtGuiltTreeModel.h>

class QtGuiltTreeItem : public QObject
{
    Q_OBJECT
  public:
    enum QtGuiltTreeItemType {
      Patch,
      PatchFile,
      PatchFileChunk
    };

    explicit QtGuiltTreeItem(QObject *parent = 0);
    void setTreeParent(QtGuiltTreeItem* parent);
    void addTreeChild(QtGuiltTreeItem* child);
    void deleteTreeChild(QtGuiltTreeItem* child);
    void addTreeChildren(QList<QtGuiltTreeItem*> children);
    void clearTreeChildren();
    void setIndexes(const QModelIndex& left, const QModelIndex& right);
    void updateIndex(const QModelIndex& index);
    void clearIndexes();
    QtGuiltTreeItem* treeParent() const;
    QList<QtGuiltTreeItem*> treeChildren() const;
    void clearTree();
    int indexOfTreeChild(QtGuiltTreeItem* child) const;
    int treeChildrenCount() const;
    QModelIndex leftIndex() const;
    QModelIndex rightIndex() const;
    QtGuiltTreeItem* treeChild(const QVariant& comp, int column = 0, int role = Qt::UserRole) const;
    QList<QVariant> treeData(int column = 0, int role = Qt::UserRole) const;
    void treeSortChildren(QList<QVariant> model, int column = 0, int role = Qt::UserRole);
    virtual QVariant	data ( int column = 0, int role = Qt::UserRole) const;
    void treeUpdate();
  protected:
    QtGuiltTreeItemType m_type;
  signals:
    void	dataChanged ( const QModelIndex & topLeft, const QModelIndex & bottomRight ) const;
  public slots:
    virtual	void notify ( const QModelIndex & topLeft, const QModelIndex & bottomRight );
    virtual void notifyBeginInsertRows ( const QModelIndex & parent, int first, int last );
    virtual void notifyEndInsertRows ();
    virtual void notifyBeginRemoveRows ( const QModelIndex & parent, int first, int last );
    virtual void notifyEndRemoveRows ();
  public:
    void notifyModification();
  protected:
    QModelIndex m_left;
    QModelIndex m_right;
  private:
    QtGuiltTreeItem* p_parent;
    QList<QtGuiltTreeItem*> m_children;
};

#endif // QTGUILTTREEITEM_H
