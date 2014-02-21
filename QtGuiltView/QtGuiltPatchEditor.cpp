#include "QtGuiltPatchEditor.h"
#include "ui_QtGuiltPatchEditor.h"
#include <QtGuiltDiffSyntaxHighlighter.h>
#include <QToolBar>
#include <QFileSystemWatcher>
#include <QFile>

QtGuiltPatchEditor::QtGuiltPatchEditor(QWidget *parent) :
  QWidget(parent),
  p_highlighter(new QtGuiltDiffSyntaxHighlighter(this)),
  m_filename(""),
  ui(new Ui::QtGuiltPatchEditor),
  p_watcher(new QFileSystemWatcher(this))
{
  ui->setupUi(this);
  ui->toolBar->setLayout(new QGridLayout(ui->toolBar));
  p_toolbar = new QToolBar(ui->toolBar);
  ui->toolBar->layout()->addWidget(p_toolbar);
  p_highlighter->setDocument(ui->plainTextEdit->document());
  ui->toolButtonSave->setDefaultAction(ui->actionSave);
  ui->actionSave->setEnabled(false);
  ui->plainTextEdit->setReadOnly(true);

  connect(ui->plainTextEdit, SIGNAL(modificationChanged(bool)), ui->actionSave, SLOT(setEnabled(bool)));
  connect(p_watcher, SIGNAL(fileChanged(QString)), this, SLOT(setFile(QString)));
  connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));
  connect(ui->plainTextEdit, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
}

QtGuiltPatchEditor::~QtGuiltPatchEditor()
{
  delete p_highlighter;
  delete ui;
}

void QtGuiltPatchEditor::setFile(const QString &filename, bool readonly, int index)
{
  setFile(filename);
  ui->plainTextEdit->setReadOnly(false);
  ui->statusBar->setText(tr(""));
  if(readonly)
  {
    ui->plainTextEdit->setReadOnly(true);
    ui->statusBar->setText(tr("Read only"));
  } else if(m_content.contains("\r"))
  {
    ui->plainTextEdit->setReadOnly(true);
    ui->statusBar->setText(tr("Read only as the file contains '\\r'"));
  }
//Update line...
  QTextCursor cursor = ui->plainTextEdit->textCursor();
  if(ui->plainTextEdit->toPlainText().length())
    cursor.setPosition(ui->plainTextEdit->toPlainText().length()-1);
  ui->plainTextEdit->setTextCursor(cursor);
  if(index == 0)
    cursor.setPosition(index);
  else
    cursor.setPosition(index - m_firstDiff);
  ui->plainTextEdit->setTextCursor(cursor);
}

void QtGuiltPatchEditor::setFile(const QString &filename)
{
  QString old = m_filename;
  m_filename = filename;
  if(m_filename == "")
  {
    ui->plainTextEdit->setPlainText("");
  }else if(old != m_filename)
  {
    QFile f(m_filename);
    if(f.open(QIODevice::ReadOnly))
    {
      m_content = f.readAll();
      f.close();
      if(m_content.startsWith("diff"))
        m_firstDiff = 0;
      else
        m_firstDiff = m_content.indexOf(QRegExp("[\r\n]diff"));
      if(m_firstDiff <= 0)
        m_firstDiff = 0;
      else
        m_firstDiff += 1;
      m_header = m_content.mid(0, m_firstDiff);
      ui->plainTextEdit->setPlainText(m_content.mid(m_firstDiff));
    }
  }
}

void QtGuiltPatchEditor::save()
{
  QFile f(m_filename);
  if(f.open(QIODevice::WriteOnly))
  {
    f.write((m_header + ui->plainTextEdit->toPlainText()).toAscii());
    f.close();
  }
  ui->actionSave->setEnabled(false);
}

void QtGuiltPatchEditor::highlightCurrentLine()
{
  QTextEdit::ExtraSelection selection;

  QColor color = QColor(Qt::blue).lighter(190);

  selection.format.setBackground(color);
  selection.format.setProperty(QTextFormat::FullWidthSelection, true);
  selection.cursor = ui->plainTextEdit->textCursor();
  selection.cursor.clearSelection();

  ui->plainTextEdit->setExtraSelections(QList<QTextEdit::ExtraSelection>() << selection);
}
