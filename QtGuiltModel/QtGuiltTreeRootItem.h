#ifndef QTGUILTTREEROOTITEM_H
#define QTGUILTTREEROOTITEM_H
#include <QtGuiltModel/QtGuiltTreeItem.h>
#include <QStringList>

class QtGuiltModel;
class QtGuiltTreeModel;

class QtGuiltTreeRootItem : public QtGuiltTreeItem
{
    Q_OBJECT
  public:
    explicit QtGuiltTreeRootItem(QtGuiltTreeModel* treeModel);
    
  signals:
    
  public slots:
    void updateSeries();
    void updateStatus();
    virtual	void notify ( const QModelIndex & topLeft, const QModelIndex & bottomRight );
    virtual void notifyBeginInsertRows ( const QModelIndex & parent, int first, int last );
    virtual void notifyEndInsertRows ();
    virtual void notifyBeginRemoveRows ( const QModelIndex & parent, int first, int last );
    virtual void notifyEndRemoveRows ();
    bool isModified() const;
    void setModified(bool modified);
  protected:
    void setModel(QtGuiltModel* model);
    void setTreeModel(QtGuiltTreeModel* model);
    void treeItemUpdate();
  protected slots:
    void notifyTopChanged();
  signals:
    void topChanged(int index);
  private:
    QStringList m_series;
    QStringList m_status;
    QStringList m_onlyActiveSeries;
    QtGuiltModel* p_model;
    QtGuiltTreeModel* p_treeModel;
    bool m_modified;
};

#endif // QTGUILTTREEROOTITEM_H
