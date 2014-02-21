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
#ifndef QTGUILTCOMMANDISSUER_H
#define QTGUILTCOMMANDISSUER_H

#include <QObject>

/**
 * @brief This class centralizes the git/guilt/svn etc. commands runner.
 */
class QtGuiltCommandIssuer : public QObject
{
    Q_OBJECT
  public:
    explicit QtGuiltCommandIssuer(QObject *parent = 0);

    /**
     * @brief getter on the last standard error
     * @return the last standard error
     */
    QString lastStderr() const;

    /**
     * @brief getter on the last standard output
     * @return the last standard output
     */
    QString lastStdout() const;

    /**
     * @brief getter on the last standard output in form of QStringList
     * @return the last standard output
     */
    QStringList lastStdoutList() const;
  signals:
    void workingDirChanged(QString newWorkingDir);

  public slots:
    /**
     * @brief Runs a command with working directory set to m_projectDir
     * @param program the programm to run
     * @param args the arguments
     * @return true if command went OK, false otherwise
     */
    bool runCommand(const QString& program, const QStringList& args);

    /**
     * @brief Runs a guilt command
     * @param args the arguments to give to guilt
     * @return true if command went OK, false otherwise
     */
    bool runGuiltCommand(const QStringList& args);

    /**
     * @brief Runs a git command
     * @param args the arguments to give to git
     * @return true if command went OK, false otherwise
     */
    bool runGitCommand(const QStringList& args);

    /**
     * @brief Runs a svn command
     * @param args the arguments to give to svn
     * @return true if command went OK, false otherwise
     */
    bool runSVNCommand(const QStringList& args);

    /**
     * @brief Sets the project directory
     * @param project_dir the new path
     */
    void setWorkingDir(const QString& project_dir);

    /**
     * @brief Sets the path to the git executable
     * @param git the path to git
     */
    void setGitPath(const QString& git);

    /**
     * @brief Sets the path to the guilt executable
     * @param guilt the path to guilt
     */
    void setGuiltPath(const QString& guilt);

    /**
     * @brief Sets the path to the svn executable
     * @param svn the path to svn
     */
    void setSVNPath(const QString& svn);

    /**
     * @brief Getter on last error string
     * @return the last error string
     */
    QString lastError() const;

    /**
      * @brief Sets std silent
      * @param silent silent if true
      */
    void setStdSilent(const bool& silent = true);

    /**
      * @brief Sets err silent
      * @param silent silent if true
      */
    void setErrSilent(const bool& silent = true);

    /**
      * @brief Sets std and err silent
      * @param silent silent if true
      */
    void setSilent(const bool& silent = true);

    /**
      * @brief Sets echo command
      * @param echo echoes command if true
      */
    void setEchoCommand(const bool& echo = true);
  protected:
    QString m_lastError;    /**< @brief the last error string */
    QString m_gitPath;      /**< @brief path to git */
    QString m_guiltPath;    /**< @brief path to guilt */
    QString m_svnPath;      /**< @brief path to svn */
    QString m_workingDir;   /**< @brief path to current project dir */
    QString m_lastStdout;   /**< @brief content of last command standard output */
    QString m_lastStderr;   /**< @brief content of last command standard error */
    bool    m_errSilent;    /**< @brief if false, prints error message when fail */
    bool    m_stdSilent;    /**< @brief if false, prints standard output */
    bool    m_echoCommand;  /**< @brief if true, prints the issued command */
};

#endif // QTGUILTCOMMANDISSUER_H
