#ifndef QTGUILTPATCHCHUNK_H
#define QTGUILTPATCHCHUNK_H
#include <QtGuiltModel/QtGuiltTreeItem.h>

class QtGuiltPatchFile;

class QtGuiltPatchChunk: public QtGuiltTreeItem
{
  public:
    QtGuiltPatchChunk(int startIndex = 0, int endIndex = 0, QtGuiltPatchFile *patchFile = 0);
    QString content() const;
    virtual QVariant data(int column = 0, int role = Qt::UserRole) const;
    QString presentation(const QString& content) const;
    int charIndex() const;
  private:
    int m_startIndex;
    int m_endIndex;
    QString m_presentation;
    QtGuiltPatchFile* ptr_patchFile;
};

#endif // QTGUILTPATCHCHUNK_H
