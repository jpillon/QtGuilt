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

#ifndef QTGUILTVIEW_GLOBAL_H
#define QTGUILTVIEW_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QTGUILTVIEW_LIBRARY)
#  define QTGUILTVIEWSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QTGUILTVIEWSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QTGUILTVIEW_GLOBAL_H
