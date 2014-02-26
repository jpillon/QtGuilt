#ifndef QTGUILTPATCHEDITOR_H
#define QTGUILTPATCHEDITOR_H

#include <QWidget>

class QtGuiltDiffSyntaxHighlighter;
class QFileSystemWatcher;

namespace Ui {
  class QtGuiltPatchEditor;
}

class QToolBar;

class QtGuiltPatchEditor : public QWidget
{
    Q_OBJECT
    
  public:
    explicit QtGuiltPatchEditor(QWidget *parent = 0);
    ~QtGuiltPatchEditor();
  public slots:
    void setFile(const QString& filename, bool readonly, int index);
    void setFile(const QString& filename, bool force = true);
    void save();
    void highlightCurrentLine();
  private:
    QtGuiltDiffSyntaxHighlighter* p_highlighter;
    QString m_filename;
    QString m_content;
    QString m_header;
    int m_firstDiff;
    Ui::QtGuiltPatchEditor *ui;
    QToolBar* p_toolbar;
    QFileSystemWatcher* p_watcher;
};

#endif // QTGUILTPATCHEDITOR_H
