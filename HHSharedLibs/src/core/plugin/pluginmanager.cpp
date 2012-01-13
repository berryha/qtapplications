/*
 ****************************************************************************
 * pluginmanager.cpp
 *
 * Created On: 2010-5-11
 *     Author: 贺辉
 *    License: LGPL
 *    Comment:
 *
 *
 *    =============================  Usage  =============================
 *|
 *|
 *    ===================================================================
 *
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 ****************************************************************************
 */

/*
 ***************************************************************************
 * Last Modified On: 2010-5-20
 * Last Modified By: 贺辉
 ***************************************************************************
 */

#include <QtCore>

#include "pluginmanager.h"

namespace HEHUI {


PluginManager* PluginManager::pluginManager = 0;

PluginManager* PluginManager::instance() {
    qDebug("----PluginManager::instance()");

    if (!pluginManager) {
        pluginManager = new PluginManager();
    }

    return pluginManager;

}

PluginManager::PluginManager(QObject *parent) :
    QObject(parent) {

    //loadPlugins();

}

PluginManager::~PluginManager() {
    // TODO Auto-generated destructor stub
}

const QHash<QString, AbstractPluginInterface *> & PluginManager::getPluginsHash() const {
    return pluginsHash;

}

QList<AbstractPluginInterface *> PluginManager::pluginsList() const {
    return pluginsHash.values();

}

QString PluginManager::pluginFilePath(AbstractPluginInterface *plugin) const {

    return pluginsHash.key(plugin);
}

void PluginManager::loadPlugins(const QString &pluginsDirPath) {
    qDebug() << "----PluginManager::loadPlugins(...)";
    qDebug() << "~~ Plug-ins Dir:" << pluginsDirPath;

    QDir pluginsDir(pluginsDirPath);
    foreach(QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QString absoluteFilePath = pluginsDir.absoluteFilePath(fileName);
        loadPlugin(absoluteFilePath);

        //			if (pluginsHash.contains(absoluteFilePath)) {
        //				qDebug()<<QString("Plugin '%1' has already been loaded!").arg(absoluteFilePath);
        //				break;
        //			}
        //
        //			qDebug() << QString("~~ Testing library %1").arg(absoluteFilePath);
        //
        //			QPluginLoader pluginLoader(absoluteFilePath);
        //                        QObject *pluginInstance = pluginLoader.instance();
        //                        if (pluginInstance) {
        //                                AbstractPluginInterface *plugin = qobject_cast<AbstractPluginInterface *> (
        //                                                pluginInstance);
        //                                if (plugin) {
        //                                        pluginsHash.insert(absoluteFilePath, plugin);
        //					//corePlugins.append(coreInterface);
        //                                        qDebug() << QString("~~ Loading Plugin %1").arg(
        //							fileName);
        //
        //				} else {
        //                                    qCritical() << QString("XX Unknown Plugin: %1").arg(absoluteFilePath);
        //					break;
        //				}
        //
        //			} else {
        //				qDebug() << QString(
        //						"XX An error occurred while loading plugin : %1").arg(
        //						pluginLoader.errorString());
        //			}

    }

}

AbstractPluginInterface * PluginManager::loadPlugin(const QString &pluginFilePath, QString *errorString) {
    qDebug("----PluginManager::loadPlugin(const QString &pluginFilePath)");
    //Q_ASSERT(QFileInfo(pluginFilePath).exists());

    QString error = "";

    if (!QFileInfo(pluginFilePath).exists()) {
        error = tr("File '%1' does not exist!").arg(pluginFilePath);
        //qCritical("File '%s' does not exist!", qPrintable(pluginFilePath));
        qCritical()<<"ERROR! An error occurred while loading plugin! "<<error;
        if(errorString){
            *errorString = error;
        }
        return 0;
    }

    if (pluginsHash.contains(pluginFilePath)) {
        error = tr("Plugin '%1' has already been loaded!").arg(pluginFilePath);
        qCritical()<<"ERROR! An error occurred while loading plugin! "<<error;
        if(errorString){
            *errorString = error;
        }
        return 0;
    }

    qDebug() << QString("~~ Testing library %1").arg(pluginFilePath);

    QPluginLoader *pluginLoader;
    if (pluginLoadersHash.contains(pluginFilePath)) {
        pluginLoader = pluginLoadersHash.value(pluginFilePath);
    } else {
        pluginLoader = new QPluginLoader(pluginFilePath);
    }

    Q_ASSERT(pluginLoader);

    QObject *pluginInstance = pluginLoader->instance();
    if (pluginInstance) {
        AbstractPluginInterface *plugin = qobject_cast<AbstractPluginInterface *> (pluginInstance);
        if (plugin) {
            pluginLoadersHash[pluginFilePath] = pluginLoader;
            pluginsHash[pluginFilePath] = plugin;

            emit signalPluginLoaded(plugin);
            emit signalPluginLoaded(pluginFilePath);

            error = tr("Plug-in '%1' loaded!").arg(pluginFilePath);
            qWarning() << error;
            if(errorString){
                *errorString = error;
            }
            return plugin;

        } else {
            error = tr("Unknown Plug-in: %1").arg(pluginFilePath);
            qCritical()<<"ERROR! An error occurred while loading plugin! "<<error;
            if(errorString){
                *errorString = error;
            }
            return 0;
        }

    } else {

        pluginLoader->unload();
        delete pluginLoader;
        pluginLoader = 0;

        error = tr("Error: %1").arg(pluginLoader->errorString());
        qCritical()<<"ERROR! An error occurred while loading plugin! "<<error;
        if(errorString){
            *errorString = error;
        }
        return 0;

    }

}

bool PluginManager::reloadPlugin(const QString &pluginFilePath) {
    qDebug("----PluginManager::reloadPlugin(const QString &pluginFilePath)");
    Q_ASSERT(QFileInfo(pluginFilePath).exists());

    if (!unloadPlugin(pluginFilePath)) {
        return false;
    }

    if (!loadPlugin(pluginFilePath)) {
        return false;
    }

    return true;

}

bool PluginManager::unloadPlugins(){
    qDebug()<<"--PluginManager::unloadPlugins()";
    foreach (AbstractPluginInterface *plugin, pluginsHash.values()) {
        if(!plugin){continue;}
        unloadPlugin(plugin);
    }

    return pluginsHash.isEmpty();

}

bool PluginManager::unloadPlugin(AbstractPluginInterface *plugin) {
    qDebug("----PluginManager::unloadPlugin(AbstractPluginInterface *plugin)");
    Q_ASSERT(plugin);

    if (plugin && plugin->unload() ) {
        QString pluginFilePath = pluginsHash.key(plugin);
        pluginsHash.remove(pluginFilePath);
        //delete plugin;
        //plugin = 0;

        QPluginLoader *pluginLoader = pluginLoadersHash.value(pluginFilePath);
        if (pluginLoader && pluginLoader->unload()) {
            pluginLoadersHash.remove(pluginFilePath);
            delete pluginLoader;
            pluginLoader = 0;
        }

        //emit signalPluginUnloaded(pluginsHash.key(plugin));
        return true;
    } else {
        return false;
    }

}

bool PluginManager::unloadPlugin(const QString &pluginFilePath) {
    qDebug("----PluginManager::unloadPlugin(const QString &pluginFilePath)");

    if (pluginsHash.contains(pluginFilePath)) {
        //TODO:Enhance the security!
        AbstractPluginInterface *plugin = static_cast<AbstractPluginInterface *> (pluginsHash.value(pluginFilePath));
        if (plugin && plugin->unload()) {
            pluginsHash.remove(pluginFilePath);
            delete plugin;
            plugin = 0;

            QPluginLoader *pluginLoader = pluginLoadersHash.value(pluginFilePath);
            if (pluginLoader && pluginLoader->unload()) {
                pluginLoadersHash.remove(pluginFilePath);
                delete pluginLoader;
                pluginLoader = 0;
            }

            //emit signalPluginUnloaded(pluginFilePath);
            return true;
        } else {
            return false;
        }

    }

    return true;

}









} //namespace HEHUI
