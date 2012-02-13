/*
 * clientinfo.cpp
 *
 *  Created on: 2010-9-27
 *      Author: HeHui
 */


#include <QHostAddress>

#include "clientinfo.h"



namespace HEHUI {

ClientInfo::ClientInfo(const QString &computerName, QObject *parent)
    :QObject(parent)
{
    this->computerName = computerName;
    workgroup = "";
    network = "";
    users = "";
    os = "";
    usbSDEnabled = false;
    programsEnabled = false;
    administrators = "";
    lastOnlineTime = QDateTime();
    clientVersion = "";

    summaryInfoSavedTODatabase = false;
    updateSummaryInfoStatement = "";



    installationDate = "";
    windowsDir = "";
    osKey = "";
    cpu = "";
    memory = "";
    motherboardName = "";
    dmiUUID = "";
    chipset = "";
    video = "";
    monitor = "";
    audio = "";
    storage = "";
    nic1Info = "";
    nic2Info = "";
    m_isJoinedToDomain = false;

    updateDetailedInfoStatement = "";
    detailedInfoSavedTODatabase = false;

    //installedSoftwaresInfo.clear();
    installedSoftwaresCount = 0;
    updateInstalledSoftwaresInfoStatement = "";
    installedSoftwaresInfoSavedTODatabase = false;


    lastHeartbeatTime = QDateTime();

    online = false;

    clientUDTListeningAddress = "";
    clientUDTListeningPort = 0;

}

ClientInfo::~ClientInfo() {
    // TODO Auto-generated destructor stub
}

bool ClientInfo::isValid(){
    return computerName.trimmed().size();

}
















} //namespace HEHUI
