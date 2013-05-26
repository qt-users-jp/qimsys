#include <qpa/qplatforminputcontextplugin_p.h>
#include <QtCore/QStringList>
#include <qimsysdebug.h>
#include "inputcontext.h"

class InputContextPlugin : public QPlatformInputContextPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QPlatformInputContextFactoryInterface" FILE "qimsys.json")

public:
    QPlatformInputContext *create(const QString&, const QStringList&);
};

QPlatformInputContext *InputContextPlugin::create(const QString& system, const QStringList& paramList)
{
    if (!qgetenv("QIMSYS_DEBUG").isEmpty()) {
        qimsysDebugOn();
    }
    qimsysDebugIn() << system << paramList;
    Q_UNUSED(paramList);
    QPlatformInputContext *ret  = 0;

    if (system.compare(system, QStringLiteral("qimsys"), Qt::CaseInsensitive) == 0)
        ret = new InputContext;
    qimsysDebugOut() << ret;
    return ret;
}

#include "main.moc"
