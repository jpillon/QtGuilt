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

#ifndef QTGUILTTREEMODEL_H
#define QTGUILTTREEMODEL_H
#include <QAbstractItemModel>
#include <QStringList>

class QtGuiltPatch;
class QtGuiltModel;
class QtGuiltAskerModel;
class QtGuiltAbstractUserAsk;
class QtGuiltTreeRootItem;

class QtGuiltTreeModel : public QAbstractItemModel
{
    Q_OBJECT
  public:
    enum QtGuiltRole {
        PatchNameRole = Qt::UserRole,
        PatchFilenameRole,
        PatchIsActiveRole,
        PatchHeaderRole,
        PatchDiffRole,
        PatchModifiedFiles,
        PatchIsCurrentRole,
        PatchIsAppliedRole,
        PreviousPatchRole,
        NextPatchRole,
        PreviousAppliedPatchRole,
        NextAppliedPatchRole,
        PatchFilesRole,
        DiffZRole
    };
    QtGuiltTreeModel(QtGuiltAbstractUserAsk *asker, QObject *parent = 0);
    virtual ~QtGuiltTreeModel();

    virtual int	            columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    virtual QVariant	      data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    virtual bool            setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    virtual QModelIndex	    index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    virtual QModelIndex	    parent ( const QModelIndex & index ) const;
    virtual int	            rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    virtual Qt::ItemFlags   flags ( const QModelIndex & index ) const;
    virtual Qt::DropActions supportedDropActions() const;


    QtGuiltAskerModel* guiltAskerModel() const;
    QtGuiltModel *guiltModel() const;

  signals:
    void topChanged(QModelIndex index);
  public slots:

    virtual void notify ( const QModelIndex & topLeft, const QModelIndex & bottomRight );
    virtual void notifyBeginInsertRows ( const QModelIndex & parent, int first, int last );
    virtual void notifyEndInsertRows ();
    virtual void notifyBeginRemoveRows ( const QModelIndex & parent, int first, int last );
    virtual void notifyEndRemoveRows ();

    virtual void notifyTopChanged(int i);

    void editBegin() const;
    void editEnd() const;
  private:
    QtGuiltTreeRootItem* p_root_item;
    QtGuiltAskerModel* p_askerModel;

};

#endif // QTGUILTTREEMODEL_H
