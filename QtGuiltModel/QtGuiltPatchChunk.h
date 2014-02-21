#ifndef QTGUILTPATCHCHUNK_H
#define QTGUILTPATCHCHUNK_H
#include <QtGuiltModel/QtGuiltTreeItem.h>

class QtGuiltPatchFile;

class QtGuiltPatchChunk: public QtGuiltTreeItem
{
  public:
    QtGuiltPatchChunk(const QString& content = QString(), int startIndex = 0, QtGuiltPatchFile *patchFile = 0);
    QString content() const;
    virtual QVariant data(int column = 0, int role = Qt::UserRole) const;
    QString presentation(const QString& content) const;
    int charIndex() const;
  private:
    QString m_content;
    int m_startIndex;
    QString m_presentation;
};

#endif // QTGUILTPATCHCHUNK_H
