#ifndef QTGUILTPATCHFILE_H
#define QTGUILTPATCHFILE_H
#include <QString>
#include <QObject>
#include <QtGuiltModel/QtGuiltTreeItem.h>

class QtGuiltPatch;

class QtGuiltPatchFile: public QtGuiltTreeItem
{
  public:
    explicit QtGuiltPatchFile(const QString& content = QString(), const int& start_index = 0, QtGuiltPatch* patch = 0);
    QString filename() const;
    QString content() const;
    QtGuiltPatch* patch() const;
    int charIndex() const;
    virtual QVariant data(int column = 0, int role = Qt::UserRole) const;
  protected:
    /**
     * @brief Extracts the modified filename from a diff content
     * @param content the diff content where to extract
     * @return the filename
     */
    QString filenameFromDiffContent(const QString& content, bool updateMinusPlus = false);
  private:
    void updateTreeChildren();
    QtGuiltPatch* p_patch;
    QString m_content;
    int m_startIndex;
    QString m_filename;
    bool m_plus;
    bool m_minus;
};

#endif // QTGUILTPATCHFILE_H
