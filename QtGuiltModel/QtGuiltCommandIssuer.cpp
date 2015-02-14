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

#include "QtGuiltCommandIssuer.h"
#include <QProcess>
#include <QCoreApplication>
#include <QDebug>

QtGuiltCommandIssuer::QtGuiltCommandIssuer(QObject *parent) :
  QObject(parent),
  m_gitPath("git"),
  m_guiltPath("guilt"),
  m_svnPath("svn"),
  m_workingDir("."),
  m_lastStdout(""),
  m_lastStderr(""),
  m_errSilent(true),
  m_stdSilent(true),
  m_echoCommand(false),
  m_patchFileEmpty(false)
{
}

QString QtGuiltCommandIssuer::lastStderr() const
{
  QString tmp = m_lastStderr;
  return tmp.replace('\r', "");
}

QString QtGuiltCommandIssuer::lastStdout() const
{
  QString tmp = m_lastStdout;
  return tmp.replace('\r', "");
}

QStringList QtGuiltCommandIssuer::lastStdoutList() const
{
  return lastStdout().split("\n", QString::SkipEmptyParts);
}

bool QtGuiltCommandIssuer::runCommand(const QString &program, const QStringList &args) const
{
  const_cast<QtGuiltCommandIssuer*>(this)->blockSignals(true);
  QProcess process;
  bool result;

  m_lastStdout = "";
  m_lastStderr = "";
  m_lastError = "";

  process.setWorkingDirectory(m_workingDir);
  process.start(program, args, QIODevice::ReadOnly);
  process.waitForFinished(-1);
  result = (process.exitCode() == 0);
  m_lastStdout = process.readAllStandardOutput();
  m_lastStderr = process.readAllStandardError();
  if(!result)
    m_lastError = m_lastStderr;
  const_cast<QtGuiltCommandIssuer*>(this)->blockSignals(false);
  if(m_echoCommand)           qDebug() << "Cmd::" << program << args;
  if(!m_stdSilent)            qDebug() << "Std::" << m_lastStdout;
  if(!result && !m_errSilent) qDebug() << "Err::" << m_lastStderr;
  return result;
}

bool QtGuiltCommandIssuer::runGuiltCommand(const QStringList &args) const
{
  bool result;
  result = runCommand(m_guiltPath, args);
  

  if(m_lastStderr.contains("fatal:"))
  {
      if(args.contains("push") && m_patchFileEmpty)
      { //happens when patch is empty... so not an error
        qDebug() << "Patchfile is empty, don't cry...";
      } else
      {
        result = false;
        if(!result && !m_errSilent) qDebug() << m_lastStderr;
      }
  }
  if(!result)
    m_lastError = m_lastStderr;
  m_patchFileEmpty = false;
  return result;
}

bool QtGuiltCommandIssuer::runGitCommand(const QStringList &args) const
{
  bool result;
  result = runCommand(m_gitPath, args);
  return result;
}

bool QtGuiltCommandIssuer::runSVNCommand(const QStringList &args) const
{
  bool result;
  result = runCommand(m_svnPath, args);
  return result;
}

void QtGuiltCommandIssuer::setWorkingDir(const QString &project_dir)
{
  m_workingDir = project_dir;
//  emit workingDirChanged(m_workingDir);
}

void QtGuiltCommandIssuer::setGitPath(const QString &git)
{
  m_gitPath = git;
}

void QtGuiltCommandIssuer::setGuiltPath(const QString &guilt)
{
  m_guiltPath = guilt;
}

void QtGuiltCommandIssuer::setSVNPath(const QString &svn)
{
  m_svnPath = svn;
}

QString QtGuiltCommandIssuer::lastError() const
{
  return m_lastError;
}

void QtGuiltCommandIssuer::setStdSilent(const bool &silent)
{
  m_stdSilent = silent;
}

void QtGuiltCommandIssuer::setErrSilent(const bool &silent)
{
  m_errSilent = silent;
}

void QtGuiltCommandIssuer::setSilent(const bool &silent)
{
  setStdSilent(silent);
  setErrSilent(silent);
}

void QtGuiltCommandIssuer::setEchoCommand(const bool &echo)
{
  m_echoCommand = echo;
}
