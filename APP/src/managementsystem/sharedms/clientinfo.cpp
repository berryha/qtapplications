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
        updateDetailedInfoStatement = "";
        detailedInfoSavedTODatabase = false;


        lastHeartbeatTime = QDateTime();

        online = false;

        clientRUDPListeningAddress = "";
        clientRUDPListeningPort = 0;

}

ClientInfo::~ClientInfo() {
	// TODO Auto-generated destructor stub
}

bool ClientInfo::isValid(){
	return computerName.trimmed().size();

}
















} //namespace HEHUI
