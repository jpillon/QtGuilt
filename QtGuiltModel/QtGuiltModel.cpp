/**************************************************************************
**  Copyright (C) 2013 jpillon
**
**  This file is part of QtGuiltModel library.
**
**  QtGuiltModel library is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  QtGuiltModel library is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with QtGuiltModel library.  If not, see <http://www.gnu.org/licenses/>.
**
**************************************************************************/

#include <QtGuiltModel/QtGuiltModel.h>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <QtGuiltPatch.h>
#include <QTimer>

QString QtGuiltModel::m_fakeBasePatch = "QtGuiltFakeBase";

QtGuiltModel::QtGuiltModel(QObject *parent):
  QtGuiltCommandIssuer(parent),
  m_naction(0),
  m_nsave(0),
  m_savedTop(""),
  m_modified(true),
  p_modificationTimer(new QTimer(this))
{
  updateEverything();
}

QtGuiltModel::~QtGuiltModel()
{
  deAssociate();
}

bool QtGuiltModel::init()
{
  beginAction(__FUNCTION__);
  bool res = false;
  if(isGitRepo())
  {
    if(runGuiltCommand(QStringList() << "init"))
    {
      updateEverything();
      res = true;
    }
  } else if (isSVNRepo()) {
    //initialize new git
    QStringList files;
    if(runSVNCommand(QStringList() << "ls" << "-R"))
    {
      files = lastStdoutList();
      if(runGitCommand(QStringList() << "init"))
      {
        foreach (QString f, files) {
          if(!runGitCommand((QStringList() << "add") + files ))
          {
            break;
          }
        }
        if(res && runGitCommand(QStringList() << "commit" << "-m" << "fake head for guilt over svn"))
        {
          if(runGuiltCommand(QStringList() << "init"))
          {
            updateEverything();
            res = true;
          }
        }
      }
    }
  }
  endAction(__FUNCTION__);
  return res;
}

bool QtGuiltModel::createRepo(const QString &path)
{
  beginAction(__FUNCTION__);
  bool res = false;
  QDir d;
  if(d.mkpath(path))
  {
    setRepo(path);
    if(runGitCommand(QStringList() << "init"))
    {
      if(runGitCommand(QStringList() << "commit" << "--allow-empty" << "-m" << "Initial"))
      {
        res = init();
      }
    }
  }
  endAction(__FUNCTION__);
  return res;
}

bool QtGuiltModel::associate()
{
  connect(&m_seriesWatcher, SIGNAL(fileChanged(QString)), this, SLOT(updateSeries(QString)), Qt::DirectConnection);
  connect(&m_statusWatcher, SIGNAL(fileChanged(QString)), this, SLOT(updateStatus(QString)), Qt::DirectConnection);
//  connect(&m_guiltWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(updateEverything()), Qt::DirectConnection);
  connect(p_modificationTimer, SIGNAL(timeout()), this, SLOT(checkModified()));
  p_modificationTimer->start(1000);
  checkModified();
  return true;
}

bool QtGuiltModel::deAssociate()
{
  p_modificationTimer->stop();
  disconnect(&m_seriesWatcher, SIGNAL(fileChanged(QString)), this, SLOT(updateSeries(QString)));
  disconnect(&m_statusWatcher, SIGNAL(fileChanged(QString)), this, SLOT(updateStatus(QString)));
//  disconnect(&m_guiltWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(updateEverything()));
  disconnect(p_modificationTimer, SIGNAL(timeout()), this, SLOT(checkModified()));
  return true;
}

bool QtGuiltModel::commit()
{
  beginAction(__FUNCTION__);
  bool res = false;
  if(isSVNRepo())
  {
    //todo
    res = false;
  } else if(isGitRepo()) {
    res = runGuiltCommand(QStringList() << "commit" << "-a");
  }
  endAction(__FUNCTION__);
  return res;
}

bool QtGuiltModel::push(const QString &patchname, const bool force)
{
  beginAction(__FUNCTION__);
  bool res = true;
  if(force)
    res = discard();
  if(res)
  {
    res = false;
    if(patchname.isEmpty())
    {
      res = runGuiltCommand(QStringList() << "push");
    } else {
      QStringList s;
      QString t;
      if(patchname == m_fakeBasePatch)
      {
        res = runGuiltCommand(QStringList() << "pop" << "-a");
      } else
      {
        if(top(t))
        {
          if(t != patchname)
          {
            if(series(s))
            {
              if(s.indexOf(patchname) >= s.indexOf(t))
              {
                res = runGuiltCommand(QStringList() << "push" << patchname);
              }
              else
              {
                QString n;
                if(next(patchname,n))
                {
                  res = pop(n, force);
                }
              }
            }
          } else
            res = true;
        }
      }
    }
  }
  updateEverything();
  endAction(__FUNCTION__);
  return res;
}

bool QtGuiltModel::pop(const QString &patchname, const bool force)
{
  beginAction(__FUNCTION__);
  bool res = false;
  QStringList a;
  QStringList args;
  if(applied(a))
  {
    if(patchname.isEmpty() || a.contains(patchname))
    {
      args << "pop";
      if(force)
        args << "-f";
      if(!patchname.isEmpty())
        args << patchname;
      res = runGuiltCommand(args);
    } else {
      //Nothing to do...
      res = true;
    }
  }
  updateEverything();
  endAction(__FUNCTION__);
  return res;
}

bool QtGuiltModel::move(const QString &patchname, const QString &where)
{
  beginAction(__FUNCTION__);
  bool res = false;
  if(!saveTop())
  {
    endAction(__FUNCTION__);
    return false;
  }
  if(runGuiltCommand(QStringList() << "pop" << "-a"))
  {
    QStringList tmp;
    if(series(tmp))
    {
      if(tmp.removeOne(patchname))
      {
        if(where.isEmpty())
          tmp.append(patchname);
        else
          tmp.insert(tmp.indexOf(where), patchname);
        m_series = tmp;
        res = saveSeries();
      }
    }
  }
  res = restoreTop() && res;
  endAction(__FUNCTION__);
  return res;
}

bool QtGuiltModel::merge(const QString &patchname1, const QString &patchname2)
{
  beginAction(__FUNCTION__);
  QStringList s;

  bool res = false;
  if(!series(s))
  {
    endAction(__FUNCTION__);
    return false;
  }
  if(!saveTop())
  {
    endAction(__FUNCTION__);
    return false;
  }

  if(s.indexOf(patchname2) > s.indexOf(patchname1))
  {
    if(push(patchname1))
    {
      if(runGuiltCommand(QStringList() << "fold" << patchname2))
        res = true;
    }
  } else
  {
    if(push(patchname2))
    {
      if(runGuiltCommand(QStringList() << "fold" << patchname1))
        if(rename(patchname2, patchname1))
          res = true;
    }
  }
  res = restoreTop() && res;
  endAction(__FUNCTION__);
  return res;
}

bool QtGuiltModel::rename(const QString &patchname, const QString &newname)
{
    qDebug() << patchname << newname;
  beginAction(__FUNCTION__);
  bool res = false;
  if(!saveTop())
  {
    endAction(__FUNCTION__);
    return false;
  }
  if(m_onlyactive_series.contains(patchname))
  {
    if(push(patchname))
    {
      QStringList args;
      args << "fork";
      args << newname;
      if(runGuiltCommand(args))
      {
        if(del(patchname))
        {
          if(m_savedTop == patchname)
            m_savedTop = newname;
          res = true;
        }
      }
    }
  } else
  {
    //Just rename in m_series and rename the patch file...
    if(m_series.contains(patchname))
    {
      m_series.replace(m_series.indexOf(patchname), newname);
      QFile f(patchFile(patchname));
      if(f.copy(newname))
      {
        f.remove();
        res = saveSeries();
      }
    }
  }

  res = restoreTop() && res;
  endAction(__FUNCTION__);
  return res;
}

bool QtGuiltModel::explode(const QString &patchname)
{
  beginAction(__FUNCTION__);
  bool res = false;
  QStringList l;
  if(!saveTop())
  {
    endAction(__FUNCTION__);
    return false;
  }
  if(files(patchname, l))
  {
    res = true;
    foreach (QString f, l) {
      if(!extract(patchname, f))
        res = false;
    }
  }
  res = restoreTop() && res;
  endAction(__FUNCTION__);
  return res;
}

bool QtGuiltModel::extract(const QString &patchname, const QString &filename)
{
  beginAction(__FUNCTION__);
  bool res = false;
  if(!saveTop())
  {
    endAction(__FUNCTION__);
    return false;
  }
  if(pop(patchname))
  {
    QString newpatchname = patchname;
    newpatchname = newpatchname.remove(QRegExp(".patch$"));
    newpatchname = newpatchname + "_" + filename;
    newpatchname = newpatchname.replace("/", "_");
    newpatchname = newpatchname.replace("\\", "_") + ".patch";
    if(newPatch(newpatchname))
    {
      QtGuiltPatch current(patchFile(patchname));
      QtGuiltPatch newpatch(patchFile(newpatchname));
      QString content = current.content(filename);
      if(pop(newpatchname))
      {
        current.removeFile(filename);
        newpatch.addFile(content);
        newpatch.setHeader(filename + " part of " + patchname + " : \n" + current.header());
        res = true;
      }
    }
  }
  res = restoreTop() && res;
  endAction(__FUNCTION__);

  return res;
}

bool QtGuiltModel::del(const QString &patchname)
{
  beginAction(__FUNCTION__);
  bool res = runGuiltCommand(QStringList() << "delete" << "-f" << patchname);
  endAction(__FUNCTION__);

  return res;
}

bool QtGuiltModel::newPatch(const QString &patchname)
{
  beginAction(__FUNCTION__);
  bool res = false;
  QString pn = patchname;
  if(patchname.isEmpty())
  {
    if(temporaryPatchName(pn))
      res = runGuiltCommand(QStringList() << "new" << pn);
  }
  else
    res = runGuiltCommand(QStringList() << "new" << pn);
  endAction(__FUNCTION__);

  return res;
}

bool QtGuiltModel::import(const QString &patchpath)
{
  beginAction(__FUNCTION__);
  bool res = runGuiltCommand(QStringList() << "import" << patchpath);
  endAction(__FUNCTION__);

  return res;
}

bool QtGuiltModel::refresh(const QString &patchname)
{
  beginAction(__FUNCTION__);
  bool res = false;
  if(patchname.isEmpty())
  {
    res = runGuiltCommand(QStringList() << "refresh");
  } else {
    QStringList s;
    if(series(s))
    {
      if(s.contains(patchname))
      {
        QString tmp;
        if(temporaryPatchName(tmp))
        {
          if(runGuiltCommand(QStringList() << "new" << "-f" << tmp))
          {
            res = merge(patchname, tmp);
          }
        }
      } else {
        if(runGuiltCommand(QStringList() << "new" << "-f" << patchname))
        {
          res = pop();
        }
      }
    }
  }
  endAction(__FUNCTION__);
  return res;
}

bool QtGuiltModel::setEnabled(const QString &patchname, bool enabled)
{
  beginAction(__FUNCTION__);
  bool res = false;
  QString prev;

  if(!previous(patchname, prev))
  {
    endAction(__FUNCTION__);
    return false;
  }
  if(!saveTop())
  {
    endAction(__FUNCTION__);
    return false;
  }
//  if(push(prev))
  if(push(m_fakeBasePatch))
  {
    QCoreApplication::processEvents();
    if(enabled)
      m_onlyactive_series.append(patchname);
    else
      m_onlyactive_series.removeOne(patchname);
    res = saveSeries();
    QCoreApplication::processEvents();
  }
  res = restoreTop() && res;
  endAction(__FUNCTION__);
  return res;
}

bool QtGuiltModel::addFiles(const QString &patchname, const QStringList &files)
{
  beginAction(__FUNCTION__);
  bool res = false;
  if(!saveTop())
  {
    endAction(__FUNCTION__);
    return false;
  }

  if(push(patchname))
  {
    QStringList args;
    args << "add";
    foreach (QString f, files) {
      args << f;
    }
    if(runGuiltCommand(args))
    {
      res = true;
    }
  }
  res = restoreTop() && res;
  endAction(__FUNCTION__);
  return res;
}

bool QtGuiltModel::delFiles(const QString &patchname, const QStringList &files)
{
  beginAction(__FUNCTION__);
  bool res = false;
  if(!saveTop())
  {
    endAction(__FUNCTION__);
    return false;
  }

  if(push(patchname))
  {
    QStringList args;
    args << "rm";
    foreach (QString f, files) {
      args << f;
    }
    if(runGuiltCommand(args))
    {
      res = true;
    }
  }
  res = restoreTop() && res;
  endAction(__FUNCTION__);
  return res;
}

bool QtGuiltModel::discard()
{
  beginAction(__FUNCTION__);
  bool res = false;
  if(pop(QString(), true))
    res = push();
  endAction(__FUNCTION__);
  return res;
}

bool QtGuiltModel::unrefreshed(QString &content, bool silent)
{
  if(!silent)
    beginAction(__FUNCTION__);
  bool res = false;
  if(runGuiltCommand(QStringList() << "diff" << "-z"))
  {
    content = lastStdout();
    res = true;
  }
  if(!silent)
    endAction(__FUNCTION__);
  return res;
}

bool QtGuiltModel::series(QStringList &list, bool onlyactive)
{
  beginAction(__FUNCTION__);
  list.clear();
  if(onlyactive)
    list = m_onlyactive_series;
  else
    list = m_series;
  endAction(__FUNCTION__);
  return true;
}

bool QtGuiltModel::applied(QStringList &list)
{
  beginAction(__FUNCTION__);
  list = m_applied;
  endAction(__FUNCTION__);
  return true;
}

bool QtGuiltModel::top(QString &patchname)
{
  beginAction(__FUNCTION__);
  QStringList a;
  bool res = false;
  if(applied(a))
  {
    if(a.size() > 0)
      patchname = a.last();
    else
      patchname = m_fakeBasePatch;
    res = true;
  }
  endAction(__FUNCTION__);
  return res;
}

bool QtGuiltModel::next(const QString &patchname, QString &nextpatch, bool active)
{
  beginAction(__FUNCTION__);
  bool res = false;
  QStringList s;
  if(series(s))
  {
    if(active)
    {
      bool isactive;
      for(int i = s.indexOf(patchname)+1 ; i<s.size() ; i++)
      {
        if(!isEnabled(s[i], isactive))
        {
          endAction(__FUNCTION__);
          return false;
        }
        if(isactive)
        {
          nextpatch = s[i];
          break;
        }
      }
      res = true;
    } else {
      if(s.indexOf(patchname)+1 < s.size())
        nextpatch = s[s.indexOf(patchname)+1];
      res = true;
    }
  }
  endAction(__FUNCTION__);
  return res;
}

bool QtGuiltModel::previous(const QString &patchname, QString &previouspatch, bool active)
{
  beginAction(__FUNCTION__);
  QStringList s;
  bool res = false;
  if(series(s))
  {
    if(active)
    {
      bool isactive;
      for(int i = s.indexOf(patchname)-1 ; i>=s.size() ; i--)
      {
        if(!isEnabled(s[i], isactive))
        {
          endAction(__FUNCTION__);
          return false;
        }
        if(isactive)
        {
          previouspatch = s[i];
          break;
        }
      }
      res = true;
    } else {
      if(s.indexOf(patchname)-1 >= 0)
        previouspatch = s[s.indexOf(patchname)-1];
      res = true;
    }
  }

  endAction(__FUNCTION__);
  return res;
}

bool QtGuiltModel::isEnabled(const QString &patchname, bool &enabled)
{
  beginAction(__FUNCTION__);
  bool res = false;
  QStringList s;
  if(series(s, true))
  {
    enabled = s.contains(patchname);
    res = true;
  }
  endAction(__FUNCTION__);
  return res;
}

bool QtGuiltModel::files(const QString &patchname, QStringList &list)
{
  beginAction(__FUNCTION__);
  QtGuiltPatch p(patchFile(patchname));
  list = p.files();
  endAction(__FUNCTION__);
  return true;
}

bool QtGuiltModel::temporaryPatchName(QString &name)
{
  beginAction(__FUNCTION__);
  QStringList s;
  QString pattern = "TMP-NewPatch_";
  if(!series(s))
  {
    endAction(__FUNCTION__);
    return false;
  }
  int i=0;
  while(!canAddPatchName(pattern + QString("%1").arg(QString::number(i), 3, '0') + ".patch"))
    i++;
  name = pattern + QString("%1").arg(QString::number(i), 3, '0') + ".patch";
  endAction(__FUNCTION__);
  return true;
}

bool QtGuiltModel::isGitRepo()
{
  QDir d(gitDir());
  return d.exists();
}

bool QtGuiltModel::isSVNRepo()
{
  QDir d(svnDir());
  return d.exists();
}

bool QtGuiltModel::isGuiltRepo()
{
  QDir d(guiltDir());
  return d.exists();
}

bool QtGuiltModel::saveSeries()
{
  beginAction(__FUNCTION__);
  QFile f(seriesFile());
  QString content;
  QStringList s;
  QStringList sa;
  if(!series(s))
  {
    endAction(__FUNCTION__);
    return false;
  }
  if(!series(sa, true))
  {
    endAction(__FUNCTION__);
    return false;
  }
  content += "# Generated by QtGuiltModel\n";
  content += "# Any change will be discarded\n\n";

  foreach(QString patch, s)
  {
    QString h = "";
    if(!sa.contains(patch))
      h="#";
    content += h + patch + "\n";
  }
  if(f.open(QIODevice::WriteOnly))
  {
    f.write( content.toStdString().c_str() );
    f.close();
    endAction(__FUNCTION__);
    return true;
  }
  endAction(__FUNCTION__);
  return false;
}

QString QtGuiltModel::guiltDir()
{
  return gitDir() + QDir::separator() + "patches" + QDir::separator() + branch();
}

QString QtGuiltModel::gitDir()
{
  return m_repo + QDir::separator() + ".git";
}

QString QtGuiltModel::svnDir()
{
  return m_repo + QDir::separator() + ".svn";
}

QString QtGuiltModel::branch()
{;
  QStringList r;
  if(!runGitCommand(QStringList() << "branch"))
    return "";
  r = lastStdoutList();
  foreach (QString b, r) {
    if(b.startsWith("*")){
      return b.replace("*", "").trimmed();
    }
  }
  return "";
}

QString QtGuiltModel::patchFile(const QString &patchname)
{
  return guiltDir() + QDir::separator() + patchname;
}

QString QtGuiltModel::seriesFile()
{
  return guiltDir() + QDir::separator() + "series";
}

QString QtGuiltModel::statusFile()
{
  return guiltDir() + QDir::separator() + "status";
}

QString QtGuiltModel::repository()
{
  return m_repo;
}

bool QtGuiltModel::saveTop()
{
  if(m_nsave++ == 0)
  {
    if(!top(m_savedTop))
    {
      m_nsave--;
      return false;
    }
  }
  return true;
}

bool QtGuiltModel::restoreTop()
{
  if(--m_nsave == 0)
  {

    if(m_series.contains(m_savedTop) && !push(m_savedTop))
    {
      m_nsave++;
      return false;
    }
  }
  return true;
}

void QtGuiltModel::beginAction(const QString &action)
{
  if(m_naction++ == 0)
  {
    p_modificationTimer->stop();
    emit actionBegin(action);
  }
}

void QtGuiltModel::progressAction(int minval, int maxval, int currentval)
{
  if(m_naction == 1)
  {
    emit actionProgress(minval, maxval, currentval);
  }
}

void QtGuiltModel::endAction(const QString &action)
{
  if(--m_naction == 0)
  {
    p_modificationTimer->start(1000);

    emit actionEnd(action);
  }
}

bool QtGuiltModel::canAddPatchName(const QString &patchname)
{
  if(m_series.contains(patchname))
    return false;
  else
  {
    QFile f(patchFile(patchname));
    return !f.exists();
  }
}

void QtGuiltModel::setRepo(const QString &newrepo)
{
  emit repoAboutToChange();
  //just in case...
  m_repo = newrepo;
  m_workingDir = m_repo;
  updateEverything();
  emit repoChanged();
}

void QtGuiltModel::updateEverything()
{
  QDir d(guiltDir());
  if(d.exists())
  {
    m_guiltWatcher.blockSignals(true);

    if(!m_guiltWatcher.directories().isEmpty())
    {
      m_guiltWatcher.removePaths(m_guiltWatcher.directories());
      QCoreApplication::processEvents();
    }
    updateSeries();
    updateStatus();
    d.setCurrent(guiltDir());
    if(d.exists() && m_guiltWatcher.directories().isEmpty())
    {
      m_guiltWatcher.addPath(d.absolutePath());

    }

    m_guiltWatcher.blockSignals(false);
    associate();
  }
//  emit modifiedChanged(m_modified);
}

void QtGuiltModel::updateSeries(QString file)
{
  QDir d(repository());
  bool waitForFileToExist = false;
  m_seriesWatcher.blockSignals(true);
  if(file.isEmpty())
    file = seriesFile();
  else
  {
    waitForFileToExist = true;
  }
  if(!m_seriesWatcher.files().isEmpty())
  {
    m_seriesWatcher.removePaths(m_seriesWatcher.files());

  }
  while(d.exists() && waitForFileToExist && !QFile::exists(file))
  {
    QCoreApplication::processEvents();
  }
  if(QFile::exists(file))
  {
    QStringList newSeries;
    QStringList newOnlyActiveSeries;
    QFile s(file);
    s.open(QIODevice::ReadOnly);
    QStringList content = QString(s.readAll()).split("\n");
    s.close();
    foreach (QString line, content) {
      line = line.trimmed();
      bool active = !line.startsWith("#");
      QStringList tmp = line.split("#", QString::SkipEmptyParts);
      if(tmp.size() > 0 && QFile::exists(patchFile(tmp[0])))
      {
        newSeries.append(tmp[0]);
        if(active)
          newOnlyActiveSeries.append(tmp[0]);
      }
    }
    m_onlyactive_series = newOnlyActiveSeries;
    m_series = newSeries;

    if(m_seriesWatcher.files().isEmpty())
    {
      m_seriesWatcher.addPath(file);

    }

  }

  m_seriesWatcher.blockSignals(false);
  emit seriesChanged();
}

void QtGuiltModel::updateStatus(QString file)
{
  QStringList newStatus;
  m_statusWatcher.blockSignals(true);
  if(file.isEmpty())
    file = statusFile();
  if(!m_statusWatcher.files().isEmpty())
  {
    m_statusWatcher.removePaths(m_statusWatcher.files());

  }
  if(QFile::exists(file))
  {
    if(runGuiltCommand(QStringList() << "applied"))
      newStatus = lastStdoutList();
    m_applied = newStatus;

    if(m_statusWatcher.files().isEmpty())
    {
      m_statusWatcher.addPath(file);

    }
  }

  m_statusWatcher.blockSignals(false);
  emit statusChanged();
}

void QtGuiltModel::checkModified()
{
  QString tmp;
  this->unrefreshed(tmp, true);
  bool old = m_modified;
  m_modified = tmp != "";
  if(old != m_modified)
  {
    emit modifiedChanged(m_modified);
  }
}
