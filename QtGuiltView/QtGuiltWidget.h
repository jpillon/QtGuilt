#ifndef QTGUILTWIDGET_H
#define QTGUILTWIDGET_H

#include <QWidget>
#include <QItemSelection>
class QtGuiltTreeModel;
class QtGuiltGUIAsker;

namespace Ui {
  class QtGuiltWidget;
}

class QToolBar;
class QMenu;
class QtGuiltTreeModel;
class QtGuiltPatch;
class QtGuiltPatchFile;
class QtGuiltPatchChunk;
class QtGuiltGraphDialog;

class QtGuiltWidget : public QWidget
{
    Q_OBJECT
    
  public:
    explicit QtGuiltWidget(QWidget *parent = 0);
    ~QtGuiltWidget();
    QtGuiltTreeModel* treeModel() const;
  public slots:
    void contextualMenuRequested(const QPoint& p);
    void begin(const QString& act);
    void end(const QString& act);
    //Available in contextual
    void activate();
    void desactivate();
    void deletePatch();
    void Goto();
    void addFiles();
    void removeFiles();
    void mergeOnTop();
    void explode();
    void rename();
    void refresh();
    void pop();
    void push();
    void copyToClipBoard();
    //Others
    void openRepo();
    void initRepo();
    void newPatch();
    void saveHeader();
    void diffz();
    void showGraph();

    void selectionChanged(QItemSelection selected,QItemSelection deselected);
  signals:
    void editPatchChanged(const QString& filepath, bool readonly, int line);
  private:
    void setConnections();
    void setActions();
    void connectAction(QAction *action, const char *slot);
    QtGuiltPatch* patchOf(const QModelIndex& index, bool walk = true) const;
    QModelIndex patchIndexOf(const QModelIndex& index) const;
    QtGuiltPatchFile* patchFileOf(const QModelIndex& index) const;
    QtGuiltPatchChunk* patchChunkOf(const QModelIndex& index) const;
    QtGuiltPatch* currentPatch(bool walk = true) const;
    QtGuiltPatchFile* currentPatchFile() const;
    QtGuiltPatchChunk* currentPatchChunk() const;
    QtGuiltTreeModel* p_model;
    QtGuiltGUIAsker* p_asker;
    QList<QAction*> m_toolBarActions;
    QList<QAction*> m_contextualActions;
    bool m_fromContextual;
    QMenu* p_contextualMenu;
    Ui::QtGuiltWidget *ui;
    QToolBar* p_toolbar;
    QtGuiltGraphDialog* p_graphDialog;
};

#endif // QTGUILTWIDGET_H
