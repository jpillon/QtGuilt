#include "QtGuiltPatchChunk.h"
#include <QtGuiltPatchFile.h>
#include <QStringList>

QtGuiltPatchChunk::QtGuiltPatchChunk(int startIndex, int endIndex, QtGuiltPatchFile *patchFile):
  QtGuiltTreeItem((QObject*)patchFile),
  m_startIndex(startIndex),
  m_endIndex(endIndex),
  ptr_patchFile(patchFile)
{
  m_type = PatchFileChunk;
  m_presentation = presentation(content());
}

QString QtGuiltPatchChunk::content() const
{
    return ptr_patchFile->content().mid(m_startIndex, m_endIndex);
}

QVariant QtGuiltPatchChunk::data(int column, int role) const
{
  QVariant res;
  //file
  if(column == 0)
  {
    switch(role)
    {
      case Qt::DisplayRole:
      case Qt::UserRole:
        res = m_presentation;
        break;
    }
  }
  return res;
}

QString QtGuiltPatchChunk::presentation(const QString &content) const
{
  return content.split("\n", QString::SkipEmptyParts)[0].trimmed();
}

int QtGuiltPatchChunk::charIndex() const
{
  return m_startIndex + ptr_patchFile->charIndex();
}
