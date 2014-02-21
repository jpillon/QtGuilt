#include "QtGuiltPatchChunk.h"
#include <QStringList>

QtGuiltPatchChunk::QtGuiltPatchChunk(const QString &content, int startIndex, QtGuiltPatchFile *patchFile):
  QtGuiltTreeItem((QObject*)patchFile),
  m_content(content),
  m_startIndex(startIndex)
{
  m_type = PatchFileChunk;
  m_presentation = presentation(m_content);
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
  return m_startIndex;
}
