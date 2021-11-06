#include "QtGuiltWidget.h"
#include "ui_QtGuiltWidget.h"
#include <QtGuiltModel/QtGuiltTreeModel.h>
#include <QtGuiltModel/QtGuiltTreeItem.h>
#include <QtGuiltModel/QtGuiltPatch.h>
#include <QtGuiltModel/QtGuiltPatchFile.h>
#include <QtGuiltModel/QtGuiltPatchChunk.h>
#include <QtGuiltModel/QtGuiltAskerModel.h>
#include <QtGuiltView/QtGuiltGUIAsker.h>
#include <QtGuiltView/QtGuiltGraphDialog.h>
#include <QToolBar>
#include <QGridLayout>
#include <QDebug>
#include <QFileDialog>
#include <QMenu>
#include <QClipboard>

QtGuiltWidget::QtGuiltWidget(QWidget *parent) :
  QWidget(parent),
  m_fromContextual(false),
  ui(new Ui::QtGuiltWidget)
{
  ui->setupUi(this);
  ui->toolBar->setLayout(new QGridLayout(ui->toolBar));
  p_toolbar = new QToolBar(ui->toolBar);
  ui->toolBar->layout()->addWidget(p_toolbar);

  p_asker = new QtGuiltGUIAsker(this);
  p_model = new QtGuiltTreeModel(p_asker, this);
  p_contextualMenu = new QMenu(ui->treeView);
  ui->treeView->setModel(p_model);

  ui->treeView->setDragEnabled(true);
  ui->treeView->viewport()->setAcceptDrops(true);
  ui->treeView->setDropIndicatorShown(true);
  ui->treeView->setDragDropOverwriteMode(false);
  ui->treeView->setDragDropMode(QAbstractItemView::InternalMove);

  p_graphDialog = new QtGuiltGraphDialog(this);
  p_graphDialog->hide();

  setActions();
  setConnections();
}

QtGuiltWidget::~QtGuiltWidget()
{
  delete ui;
}

QtGuiltTreeModel *QtGuiltWidget::treeModel() const
{
  return p_model;
}

void QtGuiltWidget::contextualMenuRequested(const QPoint &p)
{
  Q_UNUSED(p);
  m_fromContextual = true;
  QtGuiltPatch* patch = currentPatch(false);

  if(patch)
  {
    ui->actionActivate->setDisabled(patch->isActive());
    ui->actionDesactivate->setEnabled(patch->isActive());
    ui->actionPop->setEnabled(patch->isApplied());
    ui->actionPush->setDisabled(patch->isApplied());
    ui->actionMerge_on_Top->setDisabled(patch->isApplied());
    p_contextualMenu->exec(QCursor::pos());
  }
  ui->actionPop->setEnabled(p_model->guiltModel()->canPop());
  ui->actionPush->setEnabled(p_model->guiltModel()->canPush());
}

void QtGuiltWidget::begin(const QString &act)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  ui->statusBar->setText(tr("Performing ") + act + " ...");
}

void QtGuiltWidget::end(const QString &act)
{
  Q_UNUSED(act);
  while( QApplication::overrideCursor()) //Just in case it is set multiple times...
      QApplication::restoreOverrideCursor();
  ui->statusBar->setText("");

  p_model->guiltModel()->blockSignals(true);
  ui->actionPop->setEnabled(p_model->guiltModel()->canPop());
  ui->actionPush->setEnabled(p_model->guiltModel()->canPush());
  p_model->guiltModel()->blockSignals(false);
}

void QtGuiltWidget::activate()
{
  qDebug() << __FUNCTION__;

  QVariant whichone = currentPatch()->data();
  if(whichone.isValid())
    p_model->guiltAskerModel()->executeCommand(QtGuiltAskerModel::enablePatchCommand, whichone, true);
  m_fromContextual = false;
}

void QtGuiltWidget::desactivate()
{
  qDebug() << __FUNCTION__;

  QVariant whichone = currentPatch()->data();
  if(whichone.isValid())
    p_model->guiltAskerModel()->executeCommand(QtGuiltAskerModel::enablePatchCommand, whichone, false);
  m_fromContextual = false;
}

void QtGuiltWidget::openRepo()
{
  qDebug() << __FUNCTION__;
  QString repo = QFileDialog::getExistingDirectory(this, tr("Open Repository"));
  if(!repo.isEmpty())
  {
    p_model->guiltModel()->setRepo(repo);
  }
  m_fromContextual = false;

  ui->actionPop->setEnabled(p_model->guiltModel()->canPop());
  ui->actionPush->setEnabled(p_model->guiltModel()->canPush());
}

void QtGuiltWidget::initRepo()
{
  qDebug() << __FUNCTION__;
  p_model->guiltModel()->init();
  m_fromContextual = false;
}

void QtGuiltWidget::pop()
{
  qDebug() << __FUNCTION__;
  QVariant whichone;
  if(m_fromContextual)
  {
    whichone = currentPatch()->data();
  }
  p_model->guiltAskerModel()->executeCommand(QtGuiltAskerModel::popPatchCommand, whichone);
  m_fromContextual = false;
}

void QtGuiltWidget::push()
{
  qDebug() << __FUNCTION__;
  QVariant whichone;
  if(m_fromContextual)
  {
    whichone = currentPatch()->data();
  }
  p_model->guiltAskerModel()->executeCommand(QtGuiltAskerModel::pushPatchCommand, whichone);
  m_fromContextual = false;
}

void QtGuiltWidget::copyToClipBoard()
{
  QtGuiltPatch* patch = currentPatch();
  if(patch)
  {
    QString path = patch->filePath();
    QApplication::clipboard()->setText(path);
  }
}

void QtGuiltWidget::Goto()
{
  qDebug() << __FUNCTION__;
  QVariant whichone = currentPatch()->data();
  if(whichone.isValid())
    p_model->guiltAskerModel()->executeCommand(QtGuiltAskerModel::pushPatchCommand, whichone);
}

void QtGuiltWidget::addFiles()
{
  qDebug() << __FUNCTION__;
  QStringList files = QFileDialog::getOpenFileNames(this, tr("Files to add"), p_model->guiltModel()->repository());
  if(!files.isEmpty())
      p_model->guiltAskerModel()->executeCommand( QtGuiltAskerModel::addFilesToPatchCommand,
                                                  currentPatch()->data(),
                                                  files);
  m_fromContextual = false;
}

void QtGuiltWidget::removeFiles()
{
  qDebug() << __FUNCTION__;
  QStringList files = QFileDialog::getOpenFileNames(this, tr("Files to remove"), p_model->guiltModel()->repository());
  if(!files.isEmpty())
      p_model->guiltAskerModel()->executeCommand( QtGuiltAskerModel::delFilesFromPatchCommand,
                                                  currentPatch()->data(),
                                                  files);
  m_fromContextual = false;
}

void QtGuiltWidget::mergeOnTop()
{
  qDebug() << __FUNCTION__;
  p_model->guiltAskerModel()->executeCommand(QtGuiltAskerModel::mergePatchesCommand,
                                             currentPatch()->data());
  m_fromContextual = false;
}

void QtGuiltWidget::explode()
{
  qDebug() << __FUNCTION__;

  QVariant whichone = currentPatch()->data();
  if(whichone.isValid())
    p_model->guiltAskerModel()->executeCommand(QtGuiltAskerModel::explodePatchCommand, whichone);
  m_fromContextual = false;
}

void QtGuiltWidget::newPatch()
{
  qDebug() << __FUNCTION__;
  p_model->guiltAskerModel()->executeCommand(QtGuiltAskerModel::newPatchCommand);
  m_fromContextual = false;
}

void QtGuiltWidget::rename()
{
  qDebug() << __FUNCTION__;
  ui->treeView->edit(ui->treeView->currentIndex());
  m_fromContextual = false;
}

void QtGuiltWidget::refresh()
{
  qDebug() << __FUNCTION__;

  QVariant whichone;
  if(m_fromContextual)
  {
    whichone = currentPatch()->data();
  }
  p_model->guiltAskerModel()->executeCommand(QtGuiltAskerModel::refreshPatchCommand, whichone);

  m_fromContextual = false;
}

void QtGuiltWidget::diffz()
{
  qDebug() << __FUNCTION__;
  p_model->guiltAskerModel()->executeCommand(QtGuiltAskerModel::unrefreshedCommand);
  m_fromContextual = false;
}

void QtGuiltWidget::showGraph()
{
    QtGuiltCommandIssuer issuer;
    issuer.runGuiltCommand(QStringList() << "graph");
    QString dotGraph = issuer.lastStdout();
    issuer.runCommand("dot", QStringList() << "-Tsvg", dotGraph.toUtf8());
    p_graphDialog->showSVG(issuer.lastStdout().toUtf8());
}

void QtGuiltWidget::deletePatch()
{
  qDebug() << __FUNCTION__;

  QVariant whichone = currentPatch()->data();
  if(whichone.isValid())
    p_model->guiltAskerModel()->executeCommand(QtGuiltAskerModel::delPatchCommand, whichone);
  m_fromContextual = false;
}

void QtGuiltWidget::saveHeader()
{
  qDebug() << __FUNCTION__;
  currentPatch()->setHeader(ui->plainTextEditHeader->toPlainText());
  ui->actionSave_header->setEnabled(false);
  m_fromContextual = false;
}

void QtGuiltWidget::setConnections()
{
  connect(ui->treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextualMenuRequested(QPoint)));

  connect(p_model->guiltModel(), SIGNAL(actionBegin(QString)), this, SLOT(begin(QString)));
  connect(p_model->guiltModel(), SIGNAL(actionEnd(QString)), this, SLOT(end(QString)));

  connect(p_model->guiltModel(), SIGNAL(modifiedChanged(bool)), ui->actionRefresh, SLOT(setEnabled(bool)));
  connect(p_model->guiltModel(), SIGNAL(modifiedChanged(bool)), ui->actionUnrefreshed, SLOT(setEnabled(bool)));

  connect(ui->plainTextEditHeader, SIGNAL(modificationChanged(bool)), ui->actionSave_header, SLOT(setEnabled(bool)));
  connect(ui->treeView, SIGNAL(activated(QModelIndex)), ui->actionGoto, SLOT(trigger()));
  connect(ui->treeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionChanged(QItemSelection,QItemSelection)));

  connect(p_model, SIGNAL(topChanged(QModelIndex)), ui->treeView, SLOT(setCurrentIndex(QModelIndex)));

  //Connect contextual
  connectAction(ui->actionActivate,         SLOT(activate()));
  connectAction(ui->actionDesactivate,      SLOT(desactivate()));
  connectAction(ui->actionDelete_Patch,     SLOT(deletePatch()));
  connectAction(ui->actionGoto,             SLOT(Goto()));
  connectAction(ui->actionAdd_Files,        SLOT(addFiles()));
  connectAction(ui->actionRemove_Files,     SLOT(removeFiles()));
  connectAction(ui->actionMerge_on_Top,     SLOT(mergeOnTop()));
  connectAction(ui->actionExplode,          SLOT(explode()));
  connectAction(ui->actionRename,           SLOT(rename()));
  connectAction(ui->actionRefresh,          SLOT(refresh()));
  connectAction(ui->actionPop,              SLOT(pop()));
  connectAction(ui->actionPush,             SLOT(push()));
  connectAction(ui->actionCopy_Path_to_ClipBoard, SLOT(copyToClipBoard()));
  //Connect the rest
  connectAction(ui->actionOpen_Repository,  SLOT(openRepo()));
  connectAction(ui->actionInit_Repository,  SLOT(initRepo()));
  connectAction(ui->actionNew_Patch,        SLOT(newPatch()));
  connectAction(ui->actionSave_header,      SLOT(saveHeader()));
  connectAction(ui->actionUnrefreshed,      SLOT(diffz()));
  connectAction(ui->actionGraph,            SLOT(showGraph()));

  //Ugly, to avoid weird segfault...
//  connect(ui->treeView->itemDelegate(), SIGNAL(closeEditor(QWidget*)), p_model, SLOT(editEnd()), Qt::UniqueConnection);
}

void QtGuiltWidget::setActions()
{
  m_contextualActions << ui->actionActivate
                      << ui->actionDesactivate
                      << ui->actionDelete_Patch
                      << ui->actionGoto
                      << ui->actionAdd_Files
                      << ui->actionRemove_Files
                      << ui->actionMerge_on_Top
                      << ui->actionExplode
                      << ui->actionRename
                      << ui->actionRefresh
                      << ui->actionPop
                      << ui->actionPush
                      << ui->actionCopy_Path_to_ClipBoard;

  m_toolBarActions    << ui->actionOpen_Repository
                      << ui->actionInit_Repository
                      << ui->actionNew_Patch
                      << ui->actionRefresh
                      << ui->actionPop
                      << ui->actionPush
                      << ui->actionUnrefreshed
                      << ui->actionAuto_Expand_Collapse
                      << ui->actionGraph;
  ui->actionAuto_Expand_Collapse->setCheckable(true);
  p_contextualMenu->addActions(m_contextualActions);
  p_toolbar->addActions(m_toolBarActions);

  ui->actionSave_header->setEnabled(false);
  ui->toolButtonSaveHeader->setDefaultAction(ui->actionSave_header);
}

void QtGuiltWidget::connectAction(QAction *action, const char *slot)
{
    action->setIconVisibleInMenu(true);
  connect(action, SIGNAL(triggered()), this, slot);
}

QtGuiltPatch *QtGuiltWidget::patchOf(const QModelIndex &indexin, bool walk) const
{
  QtGuiltPatch* patch;
  QtGuiltTreeItem* item;
  QModelIndex index = indexin;
  do
  {
    item = (QtGuiltTreeItem*)index.internalPointer();
    patch = dynamic_cast<QtGuiltPatch*>(item);
    index = index.parent();
  } while(walk && !patch && index.internalPointer());
  return patch;
}

QModelIndex QtGuiltWidget::patchIndexOf(const QModelIndex &indexin) const
{
  QtGuiltPatch* patch;
  QtGuiltTreeItem* item;
  QModelIndex index = indexin;

  item = (QtGuiltTreeItem*)index.internalPointer();
  patch = dynamic_cast<QtGuiltPatch*>(item);
  while(!patch && index.parent().internalPointer())
  {
    index = index.parent();
    item = (QtGuiltTreeItem*)index.internalPointer();
    patch = dynamic_cast<QtGuiltPatch*>(item);
  }
  return index;
}

QtGuiltPatchFile *QtGuiltWidget::patchFileOf(const QModelIndex &index) const
{
  QtGuiltPatchFile* patchfile;
  QtGuiltTreeItem* item;

  item = (QtGuiltTreeItem*)index.internalPointer();
  patchfile = dynamic_cast<QtGuiltPatchFile*>(item);
  return patchfile;
}

QtGuiltPatchChunk *QtGuiltWidget::patchChunkOf(const QModelIndex &index) const
{
  QtGuiltPatchChunk* patchchunk;
  QtGuiltTreeItem* item;

  item = (QtGuiltTreeItem*)index.internalPointer();
  patchchunk = dynamic_cast<QtGuiltPatchChunk*>(item);
  return patchchunk;
}

QtGuiltPatch *QtGuiltWidget::currentPatch(bool walk) const
{
  return patchOf(ui->treeView->currentIndex(), walk);
}

QtGuiltPatchFile *QtGuiltWidget::currentPatchFile() const
{
  return patchFileOf(ui->treeView->currentIndex());
}

QtGuiltPatchChunk *QtGuiltWidget::currentPatchChunk() const
{
  return patchChunkOf(ui->treeView->currentIndex());
}

void QtGuiltWidget::selectionChanged(QItemSelection selected, QItemSelection deselected)
{
  Q_UNUSED(selected);
  Q_UNUSED(deselected);
  QtGuiltPatch* patch = currentPatch();
  QtGuiltPatchFile* patchfile = currentPatchFile();
  QtGuiltPatchChunk* patchchunk = currentPatchChunk();
  if(patch)
  {
    ui->plainTextEditHeader->setPlainText(patch->header());
    int line = 0;
    if(patchfile)
      line = patchfile->charIndex();
    if(patchchunk)
      line = patchchunk->charIndex();

    if(ui->actionAuto_Expand_Collapse->isChecked())
    {
      if(deselected.indexes().size())
      {
        QModelIndex index = deselected.indexes().at(0);
        if(patchOf(index) != patch)
        {
          ui->treeView->collapse(patchIndexOf(index));
        }
      }
      ui->treeView->expand(ui->treeView->currentIndex());
    }
    emit editPatchChanged(currentPatch()->filePath(), patch->isApplied(), line);
  }
}
