#include "dialog.h"
#include "ui_dialog.h"

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QInputContextFactory>
#include <QInputContext>
#include <QMessageBox>
#include <QSettings>

struct GtkImModuleInfo {
    QString key;
    QString name;
    QStringList data;
};

class Dialog::Private : private QObject
{
    Q_OBJECT
public:
    Private(Dialog *parent);

private slots:
    void accepted();

private:
    QList<GtkImModuleInfo> readGtkImModules(const QString &gtk_immodules);

private:
    Dialog *q;
    Ui::Dialog ui;
    int current_gtk_im_module_index;
    int currentQtImModuleIndex;
};

Dialog::Private::Private(Dialog *parent)
    : QObject(parent)
    , q(parent)
    , current_gtk_im_module_index(-1)
    , currentQtImModuleIndex(-1)
{
    ui.setupUi(q);
    connect(q, SIGNAL(accepted()), this, SLOT(accepted()));

    // gtk
    {
        ui.immodule4gtk->addItem(tr("System"));

        QList<GtkImModuleInfo> gtk_immodules = readGtkImModules(QLatin1String("/etc/gtk-2.0/gtk.immodules"));

        QString gtk_im_module_file(qgetenv("GTK_IM_MODULE_FILE"));
        QString current(qgetenv("GTK_IM_MODULE"));
        if (!gtk_im_module_file.isEmpty()) {
            QList<GtkImModuleInfo> modules = readGtkImModules(gtk_im_module_file);
            if (!modules.isEmpty())
                current = modules.first().key;
        }

        foreach (const GtkImModuleInfo &info, gtk_immodules) {
            ui.immodule4gtk->addItem(QString("%1(%2)").arg(info.name).arg(info.key), info.data);
            if (current == info.key) {
                ui.immodule4gtk->setCurrentIndex(ui.immodule4gtk->count() - 1);
            }
        }
        current_gtk_im_module_index = ui.immodule4gtk->currentIndex();
    }

    // Qt
    {
        ui.immodule4qt->addItem(tr("System"));

        QString current(qgetenv("QT_IM_MODULE"));
        if (current.isEmpty()) {
            QSettings settings(QLatin1String("Trolltech"));
            settings.beginGroup(QLatin1String("Qt"));
            current = settings.value(QLatin1String("DefaultInputMethod")).toString();
        }

        foreach (const QString &key, QInputContextFactory::keys()) {
            if (key == QLatin1String("imsw-multi")) continue;
            ui.immodule4qt->addItem(QString("%1(%2)").arg(QInputContextFactory::description(key)).arg(key), key);
            if (current == key) {
                ui.immodule4qt->setCurrentIndex(ui.immodule4qt->count() - 1);
            }
        }
        currentQtImModuleIndex = ui.immodule4qt->currentIndex();
    }
}

QList<GtkImModuleInfo> Dialog::Private::readGtkImModules(const QString &gtk_immodules)
{
    QList<GtkImModuleInfo> ret;
    QFile file(gtk_immodules);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        while (!stream.atEnd()) {
            QString line = stream.readLine();
            if (line.startsWith(QLatin1String("#"))) continue;
            if (!line.isEmpty()) {
                QStringList im;
                im.append(line);
                line = stream.readLine();
                im.append(line);
                QStringList fields = line.mid(1, line.length() - 2).split("\" \"");
                GtkImModuleInfo info;
                info.key = fields.at(0);
                info.name = fields.at(1);
                info.data = im;
                ret.append(info);
            }
        }
        file.close();
    }
    return ret;
}

void Dialog::Private::Private::accepted()
{
    bool dirty = false;
    bool rebootNeeded = false;
    // gtk
    if (current_gtk_im_module_index != ui.immodule4gtk->currentIndex())
    {
        QString dot_gtk_immodules(QDir::home().absoluteFilePath(QLatin1String(".gtk.immodules")));
        QStringList immodule4gtk = ui.immodule4gtk->itemData(ui.immodule4gtk->currentIndex()).toStringList();
        if (immodule4gtk.isEmpty()) {
            // remove user gtk.immodules file and create symbolic link
            QFile::remove(dot_gtk_immodules);
            QFile::link(QLatin1String("/etc/gtk-2.0/gtk.immodules"), dot_gtk_immodules);
        } else {
            QString gtk_im_module_file(qgetenv("GTK_IM_MODULE_FILE"));

            // create .profile to set GTK_IM_MODULE_FILE
            if (gtk_im_module_file.isEmpty() || gtk_im_module_file != dot_gtk_immodules) {
                QFile file(QDir::home().absoluteFilePath(QLatin1String(".profile")));
                if (file.open(QFile::Append | QFile::Text)) {
                    QTextStream stream(&file);
                    stream << QString("export GTK_IM_MODULE_FILE=%1").arg(dot_gtk_immodules) << '\n';
                    rebootNeeded = true;
                    file.close();
                } else {
                    QMessageBox msgBox;
                    msgBox.setWindowTitle(file.fileName());
                    msgBox.setText(file.errorString());
                    msgBox.exec();
                }
            }

            // create .profile to set GTK_IM_MODULE=qimsys
            if (qgetenv("GTK_IM_MODULE") != QByteArray("qimsys")) {
                QFile file(QDir::home().absoluteFilePath(QLatin1String(".profile")));
                if (file.open(QFile::Append | QFile::Text)) {
                    QTextStream stream(&file);
                    stream << QString("export GTK_IM_MODULE=qimsys") << '\n';
                    rebootNeeded = true;
                    file.close();
                } else {
                    QMessageBox msgBox;
                    msgBox.setWindowTitle(file.fileName());
                    msgBox.setText(file.errorString());
                    msgBox.exec();
                }
            }

            QFile file(QDir::home().absoluteFilePath(QLatin1String(".gtk.immodules")));
            QFile::remove(dot_gtk_immodules);
            if (file.open(QFile::WriteOnly | QFile::Text)) {
                QTextStream stream(&file);
                foreach (const QString &line, immodule4gtk) {
                    stream << line << '\n';
                }
                file.close();
            } else {
                QMessageBox msgBox;
                msgBox.setWindowTitle(file.fileName());
                msgBox.setText(file.errorString());
                msgBox.exec();
            }
        }
        dirty = true;
    }

    // Qt
    if (currentQtImModuleIndex != ui.immodule4qt->currentIndex()) {
        QString immodule4qt = ui.immodule4qt->itemData(ui.immodule4qt->currentIndex()).toString();

        QSettings settings(QLatin1String("Trolltech"));
        settings.beginGroup(QLatin1String("Qt"));
        settings.setValue(QLatin1String("DefaultInputMethod"), immodule4qt);

        dirty = true;
    }

    if (dirty) {
        // check environment variables

        if (!qgetenv("QT_IM_MODULE").isEmpty()) {
            QMessageBox msgBox;
            msgBox.setText(tr("Unset QT_IM_MODULE environment variables, then restart the device."));
            msgBox.exec();
            rebootNeeded = false;
        }
    }

    if (rebootNeeded) {
        QMessageBox msgBox;
        msgBox.setText(tr("You need to restart the device to apply the change"));
        msgBox.exec();
    }
}

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
{
    d = new Private(this);
}

Dialog::~Dialog()
{
    delete d;
}

#include "dialog.moc"
