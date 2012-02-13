/*
 * clientinfo.h
 *
 *  Created on: 2010-9-27
 *      Author: HeHui
 */

#ifndef CLIENTINFO_H_
#define CLIENTINFO_H_


#include <QObject>
#include <QString>
#include <QDateTime>
#include <QStringList>

#include "sharedmslib.h"


namespace HEHUI {

class SHAREDMSLIB_API ClientInfo : public QObject{

public:
    ClientInfo(const QString &computerName = "", QObject *parent = 0);
    virtual ~ClientInfo();


public:
    bool isValid();



    QString getNic1Info() const
    {
        return nic1Info;
    }

    QString getNic2Info() const
    {
        return nic2Info;
    }

    void setNic1Info(QString nic1Info)
    {
        this->nic1Info = nic1Info;
    }

    void setNic2Info(QString nic2Info)
    {
        this->nic2Info = nic2Info;
    }

    void setIsJoinedToDomain(bool joined){
        this->m_isJoinedToDomain = joined;
    }

    bool isJoinedToDomain(){
        return m_isJoinedToDomain;
    }

    QString getStorage() const
    {
        return storage;
    }

    void setStorage(QString storage)
    {
        this->storage = storage;
    }

    QString getAdministrators() const
    {
        return administrators;
    }

    QString getAudio() const
    {
        return audio;
    }

    QString getChipset() const
    {
        return chipset;
    }

    QString getClientUDTListeningAddress() const
    {
        return clientUDTListeningAddress;
    }

    quint16 getClientUDTListeningPort() const
    {
        return clientUDTListeningPort;
    }

    QString getClientVersion() const
    {
        return clientVersion;
    }

    QString getComputerName() const
    {
        return computerName;
    }

    QString getCpu() const
    {
        return cpu;
    }

    bool getDetailedInfoSavedTODatabase() const
    {
        return detailedInfoSavedTODatabase;
    }

    QString getDmiUUID() const
    {
        return dmiUUID;
    }

    QString getInstallationDate() const
    {
        return installationDate;
    }

    QDateTime getLastHeartbeatTime() const
    {
        return lastHeartbeatTime;
    }

    QDateTime getLastOnlineTime() const
    {
        return lastOnlineTime;
    }

    QString getMemory() const
    {
        return memory;
    }

    QString getMonitor() const
    {
        return monitor;
    }

    QString getMotherboardName() const
    {
        return motherboardName;
    }

    QString getNetwork() const
    {
        return network;
    }

    bool getOnline() const
    {
        return online;
    }

    QString getOs() const
    {
        return os;
    }

    QString getOsKey() const
    {
        return osKey;
    }

    bool getProgramsEnabled() const
    {
        return programsEnabled;
    }

    bool getSummaryInfoSavedTODatabase() const
    {
        return summaryInfoSavedTODatabase;
    }

    QString getUpdateDetailedInfoStatement() const
    {
        return updateDetailedInfoStatement;
    }

    QString getUpdateSummaryInfoStatement() const
    {
        return updateSummaryInfoStatement;
    }

    bool getUsbSDEnabled() const
    {
        return usbSDEnabled;
    }

    QString getUsers() const
    {
        return users;
    }

    QString getVideo() const
    {
        return video;
    }

    QString getWindowsDir() const
    {
        return windowsDir;
    }

    QString getWorkgroup() const
    {
        return workgroup;
    }

    void setAdministrators(QString administrators)
    {
        this->administrators = administrators;
    }

    void setAudio(QString audio)
    {
        this->audio = audio;
    }

    void setChipset(QString chipset)
    {
        this->chipset = chipset;
    }

    void setClientUDTListeningAddress(QString clientUDTListeningAddress)
    {
        this->clientUDTListeningAddress = clientUDTListeningAddress;
    }

    void setClientUDTListeningPort(quint16 clientUDTListeningPort)
    {
        this->clientUDTListeningPort = clientUDTListeningPort;
    }

    void setClientVersion(QString clientVersion)
    {
        this->clientVersion = clientVersion;
    }

    void setComputerName(QString computerName)
    {
        this->computerName = computerName;
    }

    void setCpu(QString cpu)
    {
        this->cpu = cpu;
    }

    void setDetailedInfoSavedTODatabase(bool detailedInfoSavedTODatabase)
    {
        this->detailedInfoSavedTODatabase = detailedInfoSavedTODatabase;
    }

    void setDmiUUID(QString dmiUUID)
    {
        this->dmiUUID = dmiUUID;
    }

    void setInstallationDate(QString installationDate)
    {
        this->installationDate = installationDate;
    }

    void setLastHeartbeatTime(QDateTime lastHeartbeatTime)
    {
        this->lastHeartbeatTime = lastHeartbeatTime;
    }

    void setLastOnlineTime(QDateTime lastOnlineTime)
    {
        this->lastOnlineTime = lastOnlineTime;
    }

    void setMemory(QString memory)
    {
        this->memory = memory;
    }

    void setMonitor(QString monitor)
    {
        this->monitor = monitor;
    }

    void setMotherboardName(QString motherboardName)
    {
        this->motherboardName = motherboardName;
    }

    void setNetwork(QString network)
    {
        this->network = network;
    }

    void setOnline(bool online)
    {
        this->online = online;
    }

    void setOs(QString os)
    {
        this->os = os;
    }

    void setOsKey(QString osKey)
    {
        this->osKey = osKey;
    }

    void setProgramsEnabled(bool programsEnabled)
    {
        this->programsEnabled = programsEnabled;
    }

    void setSummaryInfoSavedTODatabase(bool summaryInfoSavedTODatabase)
    {
        this->summaryInfoSavedTODatabase = summaryInfoSavedTODatabase;
    }

    void setUpdateDetailedInfoStatement(QString updateDetailedInfoStatement)
    {
        this->updateDetailedInfoStatement = updateDetailedInfoStatement;
    }

    void setUpdateSummaryInfoStatement(QString updateSummaryInfoStatement)
    {
        this->updateSummaryInfoStatement = updateSummaryInfoStatement;
    }

    void setUsbSDEnabled(bool usbSDEnabled)
    {
        this->usbSDEnabled = usbSDEnabled;
    }

    void setUsers(QString users)
    {
        this->users = users;
    }

    void setVideo(QString video)
    {
        this->video = video;
    }

    void setWindowsDir(QString windowsDir)
    {
        this->windowsDir = windowsDir;
    }

    void setWorkgroup(QString workgroup)
    {
        this->workgroup = workgroup;
    }

    //    QStringList getInstalledSoftwaresInfo(){
    //        return installedSoftwaresInfo;
    //    }
    //    void setInstalledSoftwaresInfo(const QStringList &list){
    //        this->installedSoftwaresInfo = list;
    //    }

    int getinstalledSoftwaresCount(){
        return installedSoftwaresCount;
    }
    void setInstalledSoftwaresCount(int count){
        this->installedSoftwaresCount = count;
    }

    QString getUpdateInstalledSoftwaresInfoStatement(){
        return updateInstalledSoftwaresInfoStatement;
    }
    void setUpdateInstalledSoftwaresInfoStatement(const QString &statement){
        this->updateInstalledSoftwaresInfoStatement = statement;
    }
    bool isInstalledSoftwaresInfoSavedTODatabase(){
        return installedSoftwaresInfoSavedTODatabase;
    }
    void setInstalledSoftwaresInfoSavedTODatabase(bool saved){
        this->installedSoftwaresInfoSavedTODatabase = saved;
    }

private:

    QString computerName;
    QString workgroup;
    QString network;
    QString users;
    QString os;
    bool usbSDEnabled;
    bool programsEnabled;
    QString administrators;
    QDateTime lastOnlineTime;
    QString clientVersion;

    QString updateSummaryInfoStatement;
    bool summaryInfoSavedTODatabase;


    ///////////////////////////////////////////
    QString installationDate;
    QString windowsDir;
    QString osKey;

    QString cpu;
    QString memory;
    QString motherboardName;
    QString dmiUUID;
    QString chipset;
    QString video;
    QString monitor;
    QString audio;
    QString storage;

    QString nic1Info;
    QString nic2Info;

    bool m_isJoinedToDomain;


    QString updateDetailedInfoStatement;
    bool detailedInfoSavedTODatabase;


    /////////////////////////////////////////////////

    //    QStringList installedSoftwaresInfo;
    int installedSoftwaresCount;
    QString updateInstalledSoftwaresInfoStatement;
    bool installedSoftwaresInfoSavedTODatabase;

    /////////////////////////////////////////////////




    QDateTime lastHeartbeatTime;

    bool online;

    QString clientUDTListeningAddress;
    quint16 clientUDTListeningPort;






};

}

#endif /* CLIENTINFO_H_ */
