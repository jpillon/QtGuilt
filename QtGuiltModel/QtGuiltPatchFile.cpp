#include "QtGuiltPatchFile.h"
#include <QStringList>
#include <QtGui/QIcon>
#include <QtGuiltPatchChunk.h>

QtGuiltPatchFile::QtGuiltPatchFile(const QString &content, const int &start_index, QtGuiltPatch *patch):
  QtGuiltTreeItem((QObject*)patch),
  p_patch(patch),
  m_content(content),
  m_startIndex(start_index),
  m_plus(true),
  m_minus(true)
{
  if(!m_content.endsWith("\n"))
    m_content += "\n";
  m_type = PatchFile;
  m_filename = filenameFromDiffContent(m_content, true);
  updateTreeChildren();
}

QString QtGuiltPatchFile::filename() const
{
  return m_filename;
}

QString QtGuiltPatchFile::content() const
{
  return m_content;
}

QString QtGuiltPatchFile::filenameFromDiffContent(const QString &content, bool updateMinusPlus)
{
  QString res;
  QString before;
  QString after;
  bool breakafter = false, breakbefore = false;
  bool minus = true;
  bool plus = true;
  foreach (QString l, content.split("\n")) {
    l = l.trimmed();
    if(l.startsWith("---"))
    {
      before = l.remove("---").trimmed();
      if(before == "/dev/null")
      {
        if(updateMinusPlus)
        {
          minus = false;
        }
        before = "";
      }
      breakbefore = true;
    }
    if(l.startsWith("+++"))
    {
      after = l.remove("+++").trimmed();
      if(after == "/dev/null")
      {
        if(updateMinusPlus)
        {
          plus = false;
        }
        after = "";
      }
      breakafter = true;
    }
    if(breakbefore && breakafter)
      break;
  }
  if(!before.isEmpty())
    res = before;
  else if(!after.isEmpty())
    res = after;
  else
  {
    QStringList tmp = content.split("\n")[0].split(" ");
    res = tmp[tmp.size()-2];
  }
  //let assume path is in form something/path/to/file and get rid of "something/"
  int indexofsep = res.indexOf(QRegExp("(/)|(\\\\)"));
  if(indexofsep < res.size()-1 && indexofsep > 0)
    res = res.mid(indexofsep+1);
  if(content.contains("new file mode"))
  {
    plus = true;
    minus = false;
  }
  if(content.contains("deleted file mode"))
  {
    plus = false;
    minus = true;
  }
  if(updateMinusPlus)
  {

    if(m_minus != minus || m_plus != plus)
    {
      m_minus = minus;
      m_plus = plus;
      notifyModification();
    }
  }
  return res;
}

QtGuiltPatch *QtGuiltPatchFile::patch() const
{
  return p_patch;
}

int QtGuiltPatchFile::charIndex() const
{
  return m_startIndex;
}

QVariant QtGuiltPatchFile::data(int column, int role) const
{
  QVariant res;
  //file
  if(column == 0)
  {
    switch(role)
    {
      case Qt::DisplayRole:
      case Qt::UserRole:
        res = filename();
        break;
      case Qt::DecorationRole:
        if(m_plus && m_minus)
          res = QIcon(":/patches/minusplus");
        else if(m_plus)
          res = QIcon(":/patches/plus");
        else if(m_minus)
          res = QIcon(":/patches/minus");
        else
          res = QIcon(":/patches/patchfile");
        break;
    }
  }
  return res;
}

void QtGuiltPatchFile::updateTreeChildren()
{
  QList<int> ats;

  int found = 0;
  do {
    ats.append(found);
    found = m_content.indexOf(QRegExp("[\r\n]@@"), ats.last())+1;
  } while(found > ats.last());
  ats.append(m_content.size()-1);
  for(int i=0;i<ats.size()-1;i++)
  {
    QString part = m_content.mid(ats[i], ats[i+1] - ats[i]);
    if(part.startsWith("@@"))
    {
      addTreeChild(new QtGuiltPatchChunk(ats[i] + charIndex(), ats[i+1] - ats[i], this));
    }
  }
}
