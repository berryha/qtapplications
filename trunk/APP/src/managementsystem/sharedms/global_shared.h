/*
 * global_shared.h
 *
 *  Created on: 2009-4-27
 *      Author: 贺辉
 */

#ifndef GLOBAL_SHARED_H_
#define GLOBAL_SHARED_H_




#ifndef APP_NAME
#define APP_NAME	"Service"
#endif

#ifndef APP_VERSION
#define APP_VERSION	"2011.2.25.1"
#endif

#ifndef APP_ICON_PATH
#define APP_ICON_PATH	"/images/app.png"
#endif

#ifndef APP_AUTHOR
#define APP_AUTHOR	"HeHui"
#endif

#ifndef APP_AUTHOR_EMAIL
#define APP_AUTHOR_EMAIL	"hehui@hehui.org"
#endif

#ifndef APP_ORG
#define APP_ORG	"HeHui"
#endif

#ifndef APP_ORG_DOMAIN
#define APP_ORG_DOMAIN	"www.hehui.org"
#endif

#ifndef APP_LICENSE
#define APP_LICENSE	"LGPL"
#endif




#ifndef IP_MULTICAST_GROUP_ADDRESS
#define IP_MULTICAST_GROUP_ADDRESS "224.224.224.1" //"239.255.255.255"
#endif

//#ifndef RUDP_LISTENING_PORT
//#define RUDP_LISTENING_PORT	12346
//#endif

#ifndef UDT_LISTENING_PORT
#define UDT_LISTENING_PORT	12346
#endif

#ifndef IP_MULTICAST_GROUP_PORT
#define IP_MULTICAST_GROUP_PORT 12345
#endif

#ifndef TCP_LISTENING_PORT
#define TCP_LISTENING_PORT	12344
#endif

#ifndef WIN_ADMIN_PASSWORD
#define WIN_ADMIN_PASSWORD "trousetrouse" //"computermisdg"
#endif

#ifndef CRYPTOGRAPHY_KEY
#define CRYPTOGRAPHY_KEY "HEHUI"
#endif

#ifndef HEARTBEAT_TIMER_INTERVAL
#define HEARTBEAT_TIMER_INTERVAL 600000
#endif

#ifndef MIN_THREAD_COUNT
#define MIN_THREAD_COUNT 10
#endif

#ifndef MS_RUDP_KEEPALIVE_TIMER_INTERVAL
#define MS_RUDP_KEEPALIVE_TIMER_INTERVAL 30000 //1 minute
#endif

#ifndef FILE_PIECES_IN_ONE_REQUEST
#define FILE_PIECES_IN_ONE_REQUEST 100 //100 Pieces
#endif



#include "HHSharedNetwork/hglobal_network.h"


namespace HEHUI {

    //quint8 MSPacketType = quint8(UserDefinedPacket + 1);

    namespace MS {

        enum Command{

            ClientLookForServer = UserDefinedPacket + 1, //6
            ServerDeclare,
            Update,

            ClientOnline,
            ClientOffline,

            ServerOnline,
            ServerOffline, //12

            ClientDetailedInfoRequested,
            ClientResponseClientDetailedInfo,

            AdminOnline,
            AdminOffline,

            AdminRequestRemoteConsole,
            ClientResponseRemoteConsoleStatus,
            RemoteConsoleCMDFromAdmin,
            RemoteConsoleCMDResultFromClient,

            ServerRequestClientSummaryInfo,
            ClientResponseClientSummaryInfo,

            ClientRequestSoftwareVersion,
            ServerResponseSoftwareVersion,

            ServerAnnouncement,

            ClientLog,

            AdminRequestSetupUSBSD,
            ClientResponseUSBInfo,
            AdminRequestSetupProgrames,
            ClientResponseProgramesInfo,
            ShowAdmin,
            ModifyAdminGroupUser,
            AdminRequestConnectionToClient,
            ClientResponseAdminConnectionResult,
            
            ClientMessage,
           
            AdminSearchClient,
            AdminRequestRemoteAssistance,
            UserResponseRemoteAssistance,
            
            UpdateMSWUserPassword,
            InformUserNewPassword,
            NewPasswordRetrevedByUser,
            
            LocalServiceServerDeclare,
            LocalUserOnline,
            LocalUserOffline,

            RequestDownloadFile,
            RequestUploadFile,
            ResponseFileDownloadRequest,
            ResponseFileUploadRequest,
            RequestFileData,
            FileData,
            FileTXStatusChanged,
            FileTXError,



            

        };

        enum FileTXStatus{
            File_TX_Preparing,
            File_TX_Receiving,
            File_TX_Sending,
            //File_TX_Transferring,
            File_TX_Paused,
            File_TX_Aborted,
            File_TX_Done
        };



        enum Log{
            LOG_Unknown = 0,
            
            LOG_UpdateMSUserPassword,
            LOG_CheckMSUsersAccount,

            LOG_ClientUpdate,

            LOG_AdminSetupUSBSD,
            LOG_AdminSetupProgrames,
            LOG_AdminSetupOSAdministrators,
            LOG_AdminRunCMD,

            LOG_AdminLoggedin,
            LOG_AdminAddNewUserToOS,
            LOG_AdminQueryUserInfo,

            LOG_AdminRequestRemoteAssistance,
            LOG_AdminInformUserNewPassword,

            LOG_ServerAnnouncement,
            LOG_ServerTask,


            




            
        };
        
        

    } // namespace MS



} // namespace HEHUI






#endif /* GLOBAL_SHARED_H_ */
