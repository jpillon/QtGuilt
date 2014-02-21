/**************************************************************************
**  Copyright (C) 2013 jpillon
**
**  This file is part of %PROJECT%.
**
**  %PROJECT% is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  %PROJECT% is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with %PROJECT%.  If not, see <http://www.gnu.org/licenses/>.
**
**************************************************************************/

#include "QtGuiltDiffSyntaxHighlighter.h"

QtGuiltDiffSyntaxHighlighter::QtGuiltDiffSyntaxHighlighter(QObject *parent) :
    QSyntaxHighlighter(parent)
{
}

void QtGuiltDiffSyntaxHighlighter::highlightBlock(const QString &document)
{

    QString header = "^diff.*$";
    QTextCharFormat headerFormat;
    headerFormat.setFontWeight(QFont::Bold);

    QString newFile = "^new file .*$";
    QTextCharFormat newFileFormat;
    newFileFormat.setForeground(Qt::blue);

    QString deleted = "^deleted file .*$";
    QTextCharFormat deletedFormat;
    deletedFormat.setForeground(Qt::red);

    QString index = "^index .*$";
    QTextCharFormat indexFormat;
    indexFormat.setForeground(Qt::green);

    QString descBefore = "^--- .*$";
    QTextCharFormat descBeforeFormat;
    descBeforeFormat.setForeground(Qt::red);

    QString descAfter = "^\\+\\+\\+ .*$";
    QTextCharFormat descAfterFormat;
    descAfterFormat.setForeground(Qt::blue);

    QString descLines = "^@@.*@@.*$";
    QTextCharFormat descLinesFormat;
    descLinesFormat.setForeground(Qt::darkMagenta);

    QString text = "^ .*$";
    QTextCharFormat textFormat;
    textFormat.setForeground(Qt::black);

    QString changeBefore = "^-.*$";
    QTextCharFormat changeBeforeFormat;
    changeBeforeFormat.setForeground(Qt::red);

    QString changeAfter = "^\\+.*$";
    QTextCharFormat changeAfterFormat;
    changeAfterFormat.setForeground(Qt::blue);

    apply(header, headerFormat, document);
    apply(newFile, newFileFormat, document);
    apply(deleted, deletedFormat, document);
    apply(changeBefore, changeBeforeFormat, document);
    apply(index, indexFormat, document);
    apply(changeAfter, changeAfterFormat, document);
    apply(descBefore, descBeforeFormat, document);
    apply(descAfter, descAfterFormat, document);
    apply(descLines, descLinesFormat, document);
    apply(text, textFormat, document);
}

void QtGuiltDiffSyntaxHighlighter::apply(const QString &pattern, const QTextCharFormat &format, const QString &text)
{
    QRegExp expression(pattern);
    int index = text.indexOf(expression);
    while (index >= 0) {
        int length = expression.matchedLength();
        setFormat(index, length, format);
        index = text.indexOf(expression, index + length);
    }
}
