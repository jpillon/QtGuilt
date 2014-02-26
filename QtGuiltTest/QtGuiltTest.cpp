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

#include "QtGuiltTest.h"

QtGuiltTest::QtGuiltTest():
  p_commandIssuer(NULL),
  p_model(NULL),
  p_patch(NULL)
{
}

bool QtGuiltTest::removeDir(const QString &dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }

    return result;
}

void QtGuiltTest::initTestCase()
{
  p_commandIssuer = new QtGuiltCommandIssuer();
//  p_commandIssuer->setSilent(false);
//  p_commandIssuer->setEchoCommand(true);
  p_model = new QtGuiltModel();
//  p_model->setSilent(false);
//  p_model->setEchoCommand(true);
  p_patch = new QtGuiltPatch();

//  qDebug() << m_temporarypath;
}

QString QtGuiltTest::unique()
{
  QString res;
  {
    QTemporaryFile f(QDir::tempPath() + QDir::separator() + "temp_repo_XXXXXX");
    f.open();
    res = f.fileName();
    f.close();
  }
  m_uniques.append(res);
  return res;
}

void QtGuiltTest::cleanupTestCase()
{
  foreach (QString s, m_uniques) {
    removeDir(s);
  }
  delete p_patch;
  delete p_model;
  delete p_commandIssuer;
}

void QtGuiltTest::testCommandIssuer()
{
  QFETCH(QString, command);
  QFETCH(QStringList, args);
  p_commandIssuer->runCommand(command, args);
  QCOMPARE(p_commandIssuer->lastStdoutList()[0], args[0]);
}

void QtGuiltTest::testCommandIssuer_data()
{
  QTest::addColumn<QString>("command");
  QTest::addColumn<QStringList>("args");

  QTest::newRow("coucou") << "echo" << (QStringList() << "coucou");
  QTest::newRow("toto") << "echo" << (QStringList() << "toto");
  QTest::newRow("titi") << "echo" << (QStringList() << "titi");
  QTest::newRow("tata") << "echo" << (QStringList() << "tata");
}

void QtGuiltTest::testModel()
{
  p_model->createRepo(unique());
  QStringList tmp;
  QFETCH(QStringList, toadd);
  QFETCH(QStringList, rename);
  QFETCH(QStringList, move);
  QFETCH(QString, ext);
  qDebug() << p_model->repository();
  foreach (QString p, toadd) {
    QVERIFY(p_model->newPatch(p));
    QCoreApplication::processEvents();
  }
  QVERIFY(p_model->series(tmp));
//  qDebug() << toadd;
//  qDebug() << tmp;
//  qDebug() << "######";


//  p_model->tmp();
  QCOMPARE(tmp, toadd);

  QVERIFY(p_model->rename(rename[0], rename[1]));
  QCoreApplication::processEvents();
  toadd.replace(toadd.indexOf(rename[0]), rename[1]);
  QVERIFY(p_model->series(tmp));


//  qDebug() << toadd;
//  qDebug() << tmp;
//  qDebug() << "######";
  QCOMPARE(tmp, toadd);

  QVERIFY(p_model->move(move[0], move[1]));
  toadd.removeOne(move[0]);
  toadd.insert(toadd.indexOf(move[1]), move[0]);
  QCoreApplication::processEvents();
  QVERIFY(p_model->series(tmp));


//  qDebug() << toadd;
//  qDebug() << tmp;
//  qDebug() << "######";
  QCOMPARE(tmp, toadd);

  QVERIFY(p_model->runGuiltCommand(QStringList() << "push" << "-a"));
  QCoreApplication::processEvents();
  QVERIFY(p_model->runGuiltCommand(QStringList() << "new" << ext));

  QCoreApplication::processEvents();

  toadd.append(ext);
  QVERIFY(p_model->series(tmp));

//  qDebug() << toadd;
//  qDebug() << tmp;
//  qDebug() << "######";
  QCOMPARE(tmp, toadd);
  QVERIFY(p_model->runGuiltCommand(QStringList() << "pop" << "-a"));

//  for(int i=0;i<100;i++)
    QCoreApplication::processEvents();
  QVERIFY(p_model->applied(tmp));
  QVERIFY(tmp.isEmpty());
  QVERIFY(p_model->series(tmp));

  foreach (QString p, tmp) {
    QStringList tmp2;
    QVERIFY(p_model->del(p));
    QCoreApplication::processEvents();

    toadd.removeOne(p);
    QVERIFY(p_model->series(tmp2));
    QCOMPARE(tmp2, toadd);
  }
}

void QtGuiltTest::testModel_data()
{
  QTest::addColumn<QStringList>("toadd");
  QTest::addColumn<QStringList>("rename");
  QTest::addColumn<QStringList>("move");
  QTest::addColumn<QString>("ext");
  QStringList patches;
  for(int i=0;i<7;i++)
  {
    patches << QString::number(i) + ".patch";
  }
  for(int i=0;i<2;i++)
  {
    QTest::newRow(QString::number(i).toStdString().c_str())
        << patches
        << (QStringList() << "6.patch" << "toto.patch")
        << (QStringList() << "toto.patch" << "5.patch")
        << "aze.patch";
  }
}

void QtGuiltTest::testModel2()
{
  QFETCH(QString, patchPath);
  QFileInfo f(patchPath);

  QString repo = unique();
  QStringList tmp;
  p_model->createRepo(repo);
  QVERIFY(p_model->import(patchPath));
  QCoreApplication::processEvents();
  qDebug() << p_model->lastStderr();
  qDebug() << p_model->lastStdout();
  QVERIFY(p_model->series(tmp));
  qDebug() << f.fileName();
  QVERIFY(tmp.contains(f.fileName()));
  QVERIFY(p_model->explode(f.fileName()));
  QVERIFY(p_model->series(tmp));
  qDebug() << tmp;

  QVERIFY(p_model->runGuiltCommand(QStringList() << "push" << "-a"));
  QCoreApplication::processEvents();
  QVERIFY(p_model->applied(tmp));
  qDebug() << tmp;
  p_model->deAssociate();
  removeDir(repo);
}

void QtGuiltTest::testModel2_data()
{
  QTest::addColumn<QString>("patchPath");

  QTest::newRow("0") << SRCDIR"/data/test.patch";
}

void QtGuiltTest::testPatchFile()
{
//  QDir d(unique());
  QDir d("/tmp/toto");
  QFETCH(QString, dataPath);
  qDebug() << d.absolutePath();
  d.mkpath(".");

  QString patchfile = d.absolutePath() + "/test.patch";
  QString newpatchfile = d.absolutePath() + "/test2.patch";
  QFile::remove(patchfile);
  QVERIFY(QFile::copy(dataPath + "test.patch", patchfile));
  QFile tmp(newpatchfile);
  if(tmp.exists())
    tmp.remove();
  p_patch->updateFromFile(patchfile);
//  p_patch->writeToFile(newpatchfile);
#if 1
  QtGuiltPatch newpatch(newpatchfile);
  QStringList files = p_patch->files();
  foreach(QString filename, files)
    newpatch.addFile(p_patch->content(filename));
  QStringList newfiles = newpatch.files();
  QCOMPARE(files, newfiles);
  foreach(QString filename, files)
    QCOMPARE(p_patch->content(filename), newpatch.content(filename));
  foreach(QString filename, files)
    p_patch->removeFile(filename);
  files = p_patch->files();
  QCOMPARE(files.size(),0);
  QCOMPARE(p_patch->content(), QString(""));
#endif
  return;
}

void QtGuiltTest::testPatchFile_data()
{
  QTest::addColumn<QString>("dataPath");

  QTest::newRow("0") << SRCDIR"/data/";
}
