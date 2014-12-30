/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   qimsys                                                                  *
 *   Copyright (C) 2010 by Tasuku Suzuki <stasuku@gmail.com>                 *
 *                                                                           *
 *   This program is free software; you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Lesser Public License as          *
 *   published by the Free Software Foundation; either version 2 of the      *
 *   License, or (at your option) any later version.                         *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *   GNU Lesser General Public License for more details.                     *
 *                                                                           *
 *   You should have received a copy of the GNU Lesser General Public        *
 *   License along with this program; if not, write to the                   *
 *   Free Software Foundation, Inc.,                                         *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "application.h"
#include <qimsysserver.h>
#include <qimsysapplicationmanager.h>
#include <qimsyspluginmanager.h>
#include <qimsysdebug.h>

#include <QDir>
#ifndef QIMSYS_NO_GUI
#include <QIcon>
#endif
#include <QLibraryInfo>
#include <QStringList>
#include <QSettings>
#include <QTranslator>

class Application::Private : private QObject
{
    Q_OBJECT
public:
    Private(Application *parent);
    ~Private();

private slots:
    void displayLanguageChanged(const QString &displayLanguage);

private:
    void setPluginPath();

private:
    Application *q;
    QimsysApplicationManager applicationManager;
};

Application::Private::Private(Application *parent)
    : QObject(parent)
    , q(parent)
{
    qimsysDebugIn() << parent;
    q->setOrganizationName("LinuxLife");
    q->setOrganizationDomain("linux-life.net");
    q->setApplicationName("qimsys");
    q->setApplicationVersion(QIMSYS_MACRO_TO_STRING(QIMSYS_VERSION));

#ifndef QIMSYS_NO_GUI
    QIcon icon;
    QDir icons(":/icons/");
    foreach(const QString &png, icons.entryList(QStringList() << "qimsys_*x*.png")) {
        icon.addPixmap(icons.absoluteFilePath(png));
    }
    q->setWindowIcon(icon);
#endif

    setPluginPath();

    QStringList confDir;
    confDir << QDir::homePath();
    confDir << QLatin1String(".config");
    confDir << QCoreApplication::organizationName();
    confDir << QCoreApplication::applicationName();
    QDir dir(confDir.join("/"));
    if (!dir.exists()) {
        dir.mkpath(dir.absolutePath());
    }

    QSettings settings;
    settings.beginGroup("Application");
    displayLanguageChanged(settings.value("Language", tr("en")).toString());

#ifndef QIMSYS_NO_GUI
    q->setQuitOnLastWindowClosed(false);
#endif

    new QimsysServer(this);

    QimsysPluginManager::objects<QimsysAbstractPluginObject>(this);

    applicationManager.init();
    connect(&applicationManager, SIGNAL(displayLanguageChanged(QString)), this, SLOT(displayLanguageChanged(QString)));

    qimsysDebugOut();
}

Application::Private::~Private()
{
    qimsysDebugIn();
    qimsysDebugOut();
}

void Application::Private::displayLanguageChanged(const QString &displayLanguage)
{
    qimsysDebugIn() << displayLanguage;
#ifdef QIMSYS_APPLICATION_DIR_PATH
    QDir rootDir(QIMSYS_MACRO_TO_STRING2(QIMSYS_APPLICATION_DIR_PATH));
#else
    QDir rootDir = QCoreApplication::applicationDirPath();
#endif
    // up to root dir
    for (int i = 0; i < QString(QIMSYS_MACRO_TO_STRING(QIMSYS_APP_PATH)).count(QLatin1Char('/')) + 1; i++) {
        rootDir.cdUp();
    }

    rootDir.cd(QLatin1String(QIMSYS_MACRO_TO_STRING(QIMSYS_TRANSLATIONS_PATH)));

    foreach (QTranslator *translator, findChildren<QTranslator*>()) {
        q->removeTranslator(translator);
        translator->deleteLater();
    }

    QTranslator *translator = new QTranslator(this);
    qimsysDebug() << QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    translator->load(QString("qt_%1").arg(displayLanguage), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    qimsysDebug() << rootDir.absolutePath();
    translator->load(QString("qimsys_%1").arg(displayLanguage), rootDir.absolutePath());
    q->installTranslator(translator);
    qimsysDebugOut();
}

void Application::Private::setPluginPath()
{
//    qimsysDebugOn();
    qimsysDebugIn();

#ifdef QIMSYS_APPLICATION_DIR_PATH
    QDir rootDir(QIMSYS_MACRO_TO_STRING2(QIMSYS_APPLICATION_DIR_PATH));
#else
    QDir rootDir = QCoreApplication::applicationDirPath();
#endif
    qimsysDebug() << rootDir;

    // up to root dir
    QString appPath(QIMSYS_MACRO_TO_STRING(QIMSYS_APP_PATH));
    if (!appPath.isEmpty()) {
        for (int i = 0; i < appPath.count(QLatin1Char('/')) + 1; i++) {
            rootDir.cdUp();
        }
    }
    qimsysDebug() << rootDir << QLatin1String(QIMSYS_MACRO_TO_STRING(QIMSYS_PLUGIN_PATH));

    rootDir.cd(QLatin1String(QIMSYS_MACRO_TO_STRING(QIMSYS_PLUGIN_PATH)));
    qimsysDebug() << rootDir;

    qimsysDebug() << rootDir.absolutePath();
    setQimsysPluginPath(rootDir.absolutePath());
#ifndef Q_OS_LINUX
    if (rootDir.exists(QLatin1String("inputmethods"))) {
        q->addLibraryPath(rootDir.absolutePath());
    }
#endif
    qimsysDebugOut();
//    qimsysDebugOff();
}

Application::Application(int &argc, char **argv)
#ifndef QIMSYS_NO_GUI
    : QApplication(argc, argv)
#else
    : QCoreApplication(argc, argv)
#endif
{
    if (!qgetenv("QIMSYS_DEBUG").isEmpty()) {
        qimsysDebugOn();
    }
    qimsysDebugIn();
    d = new Private(this);
    qimsysDebugOut();
}

Application::~Application()
{
    qimsysDebugIn();
    delete d;
    qimsysDebugOut();
}

#include "application.moc"
