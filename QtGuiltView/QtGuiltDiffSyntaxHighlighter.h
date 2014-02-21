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

#ifndef QTGUILTDIFFSYNTAXHIGHLIGHTER_H
#define QTGUILTDIFFSYNTAXHIGHLIGHTER_H

#include "qtguiltview_global.h"
#include <QSyntaxHighlighter>

class QTGUILTVIEWSHARED_EXPORT QtGuiltDiffSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit QtGuiltDiffSyntaxHighlighter(QObject *parent = 0);
    
signals:
    
public slots:
protected:
    virtual void highlightBlock (const QString & document );
    void apply(const QString& pattern, const QTextCharFormat& format, const QString& text);
};

#endif // QTGUILTDIFFSYNTAXHIGHLIGHTER_H
