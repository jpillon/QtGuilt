#ifndef QTGUILTPATCH_H
#define QTGUILTPATCH_H

#include <QObject>
#include <QList>
#include <QFileInfo>
#include <QtGuiltModel/QtGuiltTreeItem.h>

class QFileSystemWatcher;
class QtGuiltPatchFile;

class QtGuiltPatch : public QtGuiltTreeItem
{
    Q_OBJECT
  public:
    explicit QtGuiltPatch(QObject *parent = 0);
    QtGuiltPatch(const QString& filename, QObject *parent = 0);
    ~QtGuiltPatch();
    QString filename() const;
    QString filePath() const;
    QString header() const;
    QString content() const;
    QString content(const QString& filename) const;
    void removeFile(const QString& filename);
    void addFile(const QString& content);
    void setHeader(const QString& header);
    QStringList files() const;
    bool isActive() const;
    void setActive(bool active);
    bool isApplied() const;
    void setApplied(bool applied);
    bool isTop() const;
    void setTop(bool top);
    bool isModified() const;
    virtual QVariant data(int column = 0, int role = Qt::UserRole) const;

  signals:
    void topChanged();
  public slots:
    void updateFromFile(const QString& filename);
    void clear();
    void writeToFile(const QString& filename = QString());
  private:
    QFileInfo m_fileinfo;
    QString m_header;
    QFileSystemWatcher* p_watcher;
    bool m_active;
    bool m_applied;
    bool m_isTop;
};

#endif // QTGUILTPATCH_H
