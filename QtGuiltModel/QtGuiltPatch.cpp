#include "QtGuiltPatch.h"
#include <QFileSystemWatcher>
#include <QStringList>
#include <QFile>
#include <QtGuiltPatchFile.h>
#include <QtGuiltTreeRootItem.h>
#include <QtGui/QIcon>
#include <QFont>
#include <QDebug>

QtGuiltPatch::QtGuiltPatch(QObject *parent) :
  QtGuiltTreeItem(parent),
  p_watcher(new QFileSystemWatcher(this)),
  m_active(true),
  m_applied(false)
{
  m_type = Patch;
}

QtGuiltPatch::QtGuiltPatch(const QString &filename, QObject *parent):
  QtGuiltTreeItem(parent),
  p_watcher(new QFileSystemWatcher(this))
{
  updateFromFile(filename);
}

QtGuiltPatch::~QtGuiltPatch()
{
  clear();
}

QString QtGuiltPatch::filename() const
{
  return m_fileinfo.fileName();
}

QString QtGuiltPatch::filePath() const
{
  return m_fileinfo.canonicalFilePath();
}

QString QtGuiltPatch::header() const
{
  QString res = m_header;
  while(res.size() && res[res.size()-1].isSpace())
    res.remove(res.size()-1, 1);
  if(!res.isEmpty())
    res += '\n';
  return res;
}

QString QtGuiltPatch::content() const
{
  QString res = header();
  foreach(QtGuiltTreeItem* item, treeChildren())
  {
    QtGuiltPatchFile* f = dynamic_cast<QtGuiltPatchFile*>(item);
    if(f)
      res += f->content();
  }
  return res;
}

QString QtGuiltPatch::content(const QString &filename) const
{
  QString res;
  QtGuiltPatchFile* f = dynamic_cast<QtGuiltPatchFile*>(treeChild(filename));
  if(f)
    res = f->content();
  return res;
}

void QtGuiltPatch::removeFile(const QString &filename)
{
  deleteTreeChild(treeChild(filename));
  //writeToFile and update...
  writeToFile();
}

void QtGuiltPatch::addFile(const QString &content)
{
  QtGuiltPatchFile* newOne = new QtGuiltPatchFile(content, 0, this);
  addTreeChild(newOne);
  //writeToFile and update...
  writeToFile();
}

void QtGuiltPatch::setHeader(const QString &header)
{
  m_header = header;
  //writeToFile and update...
  writeToFile();
}

QStringList QtGuiltPatch::files() const
{
  QStringList res;
  foreach(QVariant v, treeData())
    res.append(v.toString());
  return res;
}

bool QtGuiltPatch::isActive() const
{
  return m_active;
}

void QtGuiltPatch::setActive(bool active)
{
  bool old = m_active;
  m_active = active;
  if(old != m_active)
  {
    notifyModification();
  }
}

bool QtGuiltPatch::isApplied() const
{
  return m_applied;
}

void QtGuiltPatch::setApplied(bool applied)
{
  bool old = m_applied;
  m_applied = applied;
  if(old != m_applied)
  {
    notifyModification();
  }
}

bool QtGuiltPatch::isTop() const
{
  return m_isTop;
}

void QtGuiltPatch::setTop(bool top)
{
  bool old = m_isTop;
  m_isTop = top;
  if(old != m_isTop)
  {
    notifyModification();
    if(m_isTop)
      emit topChanged();
  }
}

bool QtGuiltPatch::isModified() const
{
  QtGuiltTreeRootItem* parent = dynamic_cast<QtGuiltTreeRootItem*>(treeParent());
  if(parent)
    return parent->isModified();
  return false;
}

QVariant QtGuiltPatch::data(int column, int role) const
{
  QVariant res;
  if(column == 0)
  {
    switch(role)
    {
      case Qt::FontRole:
        if(isTop() && isModified())
        {
          QFont f;
          f.setWeight(QFont::Bold);
          res = f;
        }
        break;
      case Qt::DisplayRole:
        if(isTop() && isModified())
          res = filename() + " *";
        else
          res = filename();
        break;
      case Qt::EditRole:
      case Qt::UserRole:
        res = filename();
      break;
      case Qt::DecorationRole:
        if(isActive())
        {
          if(isApplied())
            res = QIcon(":/patches/applied");
          else
            res = QIcon(":/patches/notapplied");
        } else
          res = QIcon(":/patches/desactivated");
      break;
    }
  }
  return res;
}

void QtGuiltPatch::updateFromFile(const QString &filename)
{
  disconnect(p_watcher, SIGNAL(fileChanged(QString)), this, SLOT(updateFromFile(QString)));
  clear();
  QFile f(filename);
  if(!f.exists())
  {
    f.open(QIODevice::WriteOnly);
    f.close();
  }
  m_fileinfo.setFile(filename);
  if(!p_watcher->files().isEmpty())
    p_watcher->removePaths(p_watcher->files());
  if(!p_watcher->directories().isEmpty())
    p_watcher->removePaths(p_watcher->directories());
  f.setFileName(m_fileinfo.canonicalFilePath());
  if(f.exists())
  {
    if(f.open(QIODevice::ReadOnly))
    {
      QString total;

      total = QString(f.readAll());
      f.close();
      QList<int> diffs;

      int found = 0;
      do {
        diffs.append(found);
        found = total.indexOf(QRegExp("[\r\n]diff"), diffs.last())+1;
      } while(found > diffs.last());
      diffs.append(total.size()-1);
      for(int i=0;i<diffs.size()-1;i++)
      {
        QString part = total.mid(diffs[i], diffs[i+1] - diffs[i]);
        if(!part.startsWith("diff"))
        {
          m_header = part;
        } else
        {
          addTreeChild(new QtGuiltPatchFile(part, diffs[i], this));
        }
      }
    }
    p_watcher->addPath(filename);
  }
  connect(p_watcher, SIGNAL(fileChanged(QString)), this, SLOT(updateFromFile(QString)));
}

void QtGuiltPatch::clear()
{
  clearTreeChildren();
  m_header.clear();
  m_fileinfo = QFileInfo();
}

void QtGuiltPatch::writeToFile(const QString &filename)
{
  if(filename != "")
    m_fileinfo.setFile(filename);
  QFile f(m_fileinfo.canonicalFilePath());

  if(f.open(QIODevice::WriteOnly))
  {
    f.write(content().toStdString().c_str());
  }
  f.close();
  updateFromFile(m_fileinfo.canonicalFilePath());
}
