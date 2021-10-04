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

#ifndef QTGUILTTEST_H
#define QTGUILTTEST_H

#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <QTemporaryFile>

#include <QtGuiltModel/QtGuiltCommandIssuer.h>
#include <QtGuiltModel/QtGuiltModel.h>
#include <QtGuiltModel/QtGuiltPatch.h>

class QtGuiltTest : public QObject
{
    Q_OBJECT

  public:
    QtGuiltTest();

  private Q_SLOTS:
    void initTestCase();
    void testPatchFile();
    void testPatchFile_data();
    void testExtract();
    void testExtract_data();
    void testModel2();
    void testModel2_data();
    void testCommandIssuer();
    void testCommandIssuer_data();
    void testModel();
    void testModel_data();
    void cleanupTestCase();

  private:
  private:
    bool removeDir(const QString &dirName);
    QString unique();
    QStringList m_uniques;
    QtGuiltCommandIssuer *p_commandIssuer;
    QtGuiltModel *p_model;
    QtGuiltPatch* p_patch;
    QString m_temporarypath;
};


#endif // QTGUILTTEST_H
