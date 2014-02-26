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

#ifndef QTGUILTMODEL_H
#define QTGUILTMODEL_H

#include <QObject>
#include <QStringList>
#include <QFileSystemWatcher>
#include <QtGuiltModel/QtGuiltCommandIssuer.h>

#include <qtguiltmodel_global.h>

class QTimer;

/**
 * @brief This is the base class for guilt manipulations. There is no control, so user must know what he does.
 *
 * Every commands keeps the same top if possible
 */
class QTGUILTMODELSHARED_EXPORT QtGuiltModel : public QtGuiltCommandIssuer
{
    Q_OBJECT
  public:
    QtGuiltModel(QObject* parent = NULL);
    virtual ~QtGuiltModel();
  public:
    /**
     * @brief Initializes the guilt repo
     * @return true if command succeeded, false otherwise
     *
     * @todo check svn support
     */
    bool init();

    /**
     * @brief Creates an empty git/guilt repository
     * @param path where to create
     * @return true if command succeeded, false otherwise (of directory already exists for example)
     */
    bool createRepo(const QString& path);

    /**
     * @brief Craetes inner connections
     * @return true
     */
    bool associate();
    /**
     * @brief Clears all connections
     * @return true if ok
     */
    bool deAssociate();
    /**
     * @brief commit
     * @return true if command succeeded, false otherwise
     *
     * @todo check svn support
     */
    bool commit();

    /**
     * @brief Makes a patch top
     * @param patchname the patch to be pushed (next in series if QString())
     * @param force if true, discard unrefreshed before
     * @return true if command succeeded, false otherwise
     *
     * If the wanted patch is not applied, just push it. If not, pops the next (acts as a goto patch)
     */
    bool push(const QString& patchname = QString(), const bool force = false);
    /**
     * @brief Pops a patch
     * @param patchname the patch to be popped (top if QString())
     * @param force if true, discard unrefreshed before
     * @return true if command succeeded, false otherwise
     */
    bool pop(const QString& patchname = QString(), const bool force = false);

    /**
     * @brief Moves a patch
     * @param patchname the patch to move
     * @param where where to move the patch (on top of series if empty)
     * @return true if command succeeded, false otherwise
     */
    bool move(const QString& patchname, const QString& where);

    /**
     * @brief Merges patchname2 into patchname1.
     * @param patchname1 first patch
     * @param patchname2 second patch
     * @return true if command succeeded, false otherwise
     *
     *  - If patchname2 is upper in series, make patchname1 top and fold.
     *  - If patchname1 is upper, make patchname2 top, fold and rename
     */
    bool merge(const QString& patchname1, const QString& patchname2);

    /**
     * @brief Renames a patch
     * @param patchname the patch to rename
     * @param newname the new name of the patch
     * @return true if command succeeded, false otherwise
     */
    bool rename(const QString& patchname, const QString& newname);

    /**
     * @brief Explodes a patch according to its modified files
     * @param patchname the patch to explode
     * @return true if command succeeded, false otherwise
     *
     * The resulting patches names are in form PATCHNAME_FILENAME.patch
     */
    bool explode(const QString& patchname);

    /**
     * @brief Extracts modifications related to given file modification into a new patch.
     * @param patchname the patch from chich to extract
     * @param filename the file to extract from patch
     * @return true if command succeeded, false otherwise
     *
     * The resulting patche name is PATCHNAME_FILENAME.patch
     *
     */
    bool extract(const QString& patchname, const QString& filename);

    /**
     * @brief Deletes a patch
     * @param patchname the patch to delete
     * @return true if command succeeded, false otherwise
     */
    bool del(const QString& patchname);

    /**
     * @brief Adds a new patch
     * @param patchname the name of the patch to add (free patch name if QString() )
     * @return true if command succeeded, false otherwise
     */
    bool newPatch(const QString& patchname = QString());

    /**
     * @brief Imports a patch
     * @param patchpath path to the patch to import
     * @return true if command succeeded, false otherwise
     */
    bool import(const QString& patchpath);

    /**
     * @brief Applies unrefreshed modifications into a patch
     * @param patchname the patch where to add modifications (top if QString())
     * @return true if command succeeded, false otherwise
     *
     * If patchname is not QString() :
     *  - if patchname exists :
     *    - apply to new temporary (new -f TMP)
     *    - merge temporary into patchname
     *  - else :
     *    - apply to new (new -f patchname)
     */
    bool refresh(const QString& patchname = QString());

    /**
     * @brief Enables or disables a patch
     * @param patchname the patch to enable or disable
     * @param enabled true to enable, false to disable
     * @return true if command succeeded, false otherwise
     */
    bool setEnabled(const QString& patchname, bool enabled = true);

    /**
     * @brief Adds files to a patch
     * @param patchname where to add files
     * @param files the list of files to add
     * @return true if command succeeded, false otherwise
     *
     * If patchname is not top, goto patchname then guilt add
     */
    bool addFiles(const QString& patchname, const QStringList& files);

    /**
     * @brief Deletes files from a patch
     * @param patchname where to del files
     * @param files the list of files to del
     * @return true if command succeeded, false otherwise
     *
     * If patchname is not top, goto patchname then guilt rm
     */
    bool delFiles(const QString& patchname, const QStringList& files);

    /**
     * @brief Discards currently unrefreshed changes
     * @return true if command succeeded, false otherwise
     */
    bool discard();

    /**************************** Readers ****************************/

    /**
     * @brief Retrieves the current unrefreshed modifications (diff -z)
     * @param[out] content the current unrefreshed modifications (in patch form)
     * @return true if command succeeded, false otherwise
     */
    bool unrefreshed(QString& content, bool silent = false);

    /**
     * @brief Retrieves the available patch list
     * @param[out] list the current patch list
     * @param onlyactive if true, only retrieve active patches
     * @return true if command succeeded, false otherwise
     */
    bool series(QStringList& list, bool onlyactive = false);

    /**
     * @brief Retrieves the current applied patch list
     * @param[out] list the current applied patch list
     * @return true if command succeeded, false otherwise
     */
    bool applied(QStringList& list);

    /**
     * @brief Retrieves top patchname
     * @param[out] patchname the top patch name
     * @return true if command succeeded, false otherwise
     */
    bool top(QString& patchname);

    /**
     * @brief Retrieves the next active patchname
     * @param patchname which next patch we want (top if empty)
     * @param[out] nextpatch the next patch of patchname (empty if patchname is last patch)
     * @param active set to true if nextpatch must be active
     * @return true if command succeeded, false otherwise
     */
    bool next(const QString& patchname, QString& nextpatch, bool active = true);

    /**
     * @brief Retrieves the previous active patchname
     * @param patchname which previous patch we want (top if empty)
     * @param[out] previouspatch the previous patch of patchname (empty if patchname is root)
     * @return true if command succeeded, false otherwise
     */
    bool previous(const QString& patchname, QString& previouspatch, bool active = true);

    /**
     * @brief Checks if patchname is enabled or not
     * @param patchname the patch to check
     * @param[out] enabled patchname is enabled or not
     * @return true if command succeeded, false otherwise
     */
    bool isEnabled(const QString& patchname, bool& enabled);

    /**
     * @brief Retrieves the list of files modified by given patch
     * @param patchname the patch to read
     * @param[out] list the list of files modified by the patch
     * @return true if command succeeded, false otherwise
     *
     */
    bool files(const QString& patchname, QStringList& list);
/*******************Utils***********************/

    /**
     * @brief Retrieves a temporary patch name that does not already exist
     * @param[out] name the new temporary patch name
     * @return true if command succeeded, false otherwise
     */
    bool temporaryPatchName(QString& name);

    /**
     * @brief checks if patch is a git repository
     * @return true if it is
     */
    bool isGitRepo();

    /**
     * @brief checks if patch is a svn local copy
     * @return true if it is
     */
    bool isSVNRepo();

    /**
     * @brief checks if patch is a guilt repository
     * @return true if it is
     */
    bool isGuiltRepo();

    /**
     * @brief Fills the series file accodring to inner data
     * @return
     */
    bool saveSeries();

    void tmp();
    QString guiltDir();
    QString gitDir();
    QString svnDir();
    QString branch();
    QString patchFile(const QString& patchname);
    QString seriesFile();
    QString statusFile();
    QString repository();
  public slots:
    void setRepo(const QString& newrepo);
    void updateEverything();
    void updateSeries(QString file = QString());
    void updateStatus(QString file = QString());
  protected slots:
    void checkModified();
  signals:
    void seriesChanged();
    void statusChanged();
    void repoAboutToChange();
    void repoChanged();
    void actionBegin(QString action);
    void actionProgress(int minval, int maxval, int currentval);
    void actionEnd(QString action);
    void modifiedChanged(bool modified);
  private:
    inline bool saveTop();
    inline bool restoreTop();
    inline void beginAction(const QString& action);
    inline void progressAction(int minval, int maxval, int currentval);
    inline void endAction(const QString& action);
    bool canAddPatchName(const QString& patchname);
  private:
    unsigned int        m_naction;
    unsigned int        m_nsave;
    QString             m_savedTop;
    bool                m_modified;
    QTimer*             p_modificationTimer;
    QString             m_repo;
    QStringList         m_series;
    QStringList         m_onlyactive_series;
    QStringList         m_applied;
    QFileSystemWatcher  m_seriesWatcher;
    QFileSystemWatcher  m_statusWatcher;
    QFileSystemWatcher  m_guiltWatcher;
    static QString      m_fakeBasePatch;
};

#endif // QTGUILTMODEL_H
