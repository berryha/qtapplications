/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE */;
/*!40101 SET SQL_MODE='STRICT_TRANS_TABLES,NO_AUTO_CREATE_USER,NO_ENGINE_SUBSTITUTION' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES */;
/*!40103 SET SQL_NOTES='ON' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS */;
/*!40014 SET FOREIGN_KEY_CHECKS=0 */;


DROP DATABASE IF EXISTS `im`;
CREATE DATABASE `im` /*!40100 DEFAULT CHARACTER SET utf8 */;
USE `im`;
CREATE TABLE `cachedchatmessages` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `SenderID` varchar(16) NOT NULL COMMENT '发送者ID',
  `RecieverID` varchar(16) NOT NULL COMMENT '接收者ID',
  `TransmittingTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '发送时间',
  `Message` varchar(255) NOT NULL COMMENT '消息',
  PRIMARY KEY (`ID`),
  KEY `FK_cachemessages_users_ID_ReceiverID` (`RecieverID`),
  KEY `FK_cachemessages_users_ID_SenderID` (`SenderID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='缓存的消息';
LOCK TABLES `cachedchatmessages` WRITE;
/*!40000 ALTER TABLE `cachedchatmessages` DISABLE KEYS */;

/*!40000 ALTER TABLE `cachedchatmessages` ENABLE KEYS */;
UNLOCK TABLES;
CREATE TABLE `cachedinterestgroupchatmessages` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `SenderID` varchar(16) NOT NULL COMMENT '发送者ID',
  `InterestGroupID` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '接收组ID',
  `TransmittingTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '发送时间',
  `Message` varchar(255) NOT NULL COMMENT '消息',
  PRIMARY KEY (`ID`),
  KEY `FK_cachemessages_users_ID_ReceiverID` (`InterestGroupID`),
  KEY `FK_cachemessages_users_ID_SenderID` (`SenderID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT COMMENT='缓存的消息';
LOCK TABLES `cachedinterestgroupchatmessages` WRITE;
/*!40000 ALTER TABLE `cachedinterestgroupchatmessages` DISABLE KEYS */;

/*!40000 ALTER TABLE `cachedinterestgroupchatmessages` ENABLE KEYS */;
UNLOCK TABLES;
CREATE TABLE `friendshipapply` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `SenderID` varchar(16) NOT NULL,
  `ReceiverID` varchar(16) NOT NULL,
  `ExtraMessage` varchar(32) DEFAULT NULL,
  `Result` char(1) NOT NULL DEFAULT '0' COMMENT '0:未知,1:接受,2:拒绝',
  `SenderRead` char(1) NOT NULL DEFAULT '0' COMMENT '0:未读,1：已读',
  `ReceiverRead` char(1) NOT NULL DEFAULT '0' COMMENT '0:未读,1：已读',
  PRIMARY KEY (`ID`),
  KEY `FK_friendshipapply_users_ID_SenderID` (`SenderID`),
  KEY `FK_friendshipapply_users_ID_ReceiverID` (`ReceiverID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='好友请求';
LOCK TABLES `friendshipapply` WRITE;
/*!40000 ALTER TABLE `friendshipapply` DISABLE KEYS */;

/*!40000 ALTER TABLE `friendshipapply` ENABLE KEYS */;
UNLOCK TABLES;
CREATE TABLE `groupmemberroles` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `RoleName` varchar(16) NOT NULL,
  `Description` varchar(45) DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8 COMMENT='组员在组中的角色';
LOCK TABLES `groupmemberroles` WRITE;
/*!40000 ALTER TABLE `groupmemberroles` DISABLE KEYS */;

INSERT INTO `groupmemberroles` VALUES (1,'Creator',NULL);
INSERT INTO `groupmemberroles` VALUES (2,'Administrator',NULL);
INSERT INTO `groupmemberroles` VALUES (3,'Member',NULL);
/*!40000 ALTER TABLE `groupmemberroles` ENABLE KEYS */;
UNLOCK TABLES;
CREATE TABLE `groupmembers` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `MemberUserID` varchar(16) NOT NULL COMMENT '成员ID',
  `GroupID` int(10) unsigned NOT NULL COMMENT '兴趣组ID',
  `MemberRole` int(10) unsigned NOT NULL COMMENT '成员角色',
  PRIMARY KEY (`ID`),
  UNIQUE KEY `Index_MemberUserID_GroupID` (`MemberUserID`,`GroupID`) USING BTREE,
  KEY `FK_groupmembers_GroupID` (`GroupID`)
) ENGINE=InnoDB AUTO_INCREMENT=7 DEFAULT CHARSET=utf8 COMMENT='组的成员';
LOCK TABLES `groupmembers` WRITE;
/*!40000 ALTER TABLE `groupmembers` DISABLE KEYS */;

INSERT INTO `groupmembers` VALUES (1,'hehui',1,1);
INSERT INTO `groupmembers` VALUES (2,'admin',1,2);
INSERT INTO `groupmembers` VALUES (3,'rebort',1,2);
INSERT INTO `groupmembers` VALUES (4,'king',1,2);
INSERT INTO `groupmembers` VALUES (5,'yu',1,2);
INSERT INTO `groupmembers` VALUES (6,'kiwa',1,2);
/*!40000 ALTER TABLE `groupmembers` ENABLE KEYS */;
UNLOCK TABLES;
CREATE TABLE `groups` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `TypeID` int(10) unsigned NOT NULL COMMENT '类型',
  `ParentGroup` int(10) unsigned DEFAULT NULL COMMENT '父组的ID',
  `Creator` varchar(16) NOT NULL COMMENT '创建者',
  `GroupName` varchar(32) NOT NULL COMMENT '组名称',
  `CreationTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
  `GroupInfoVersion` int(10) unsigned NOT NULL DEFAULT '1' COMMENT '组信息版本',
  `MemberListVersion` int(10) unsigned NOT NULL DEFAULT '1' COMMENT '组成员列表版本',
  `Description` varchar(64) DEFAULT NULL,
  `Announcement` varchar(64) DEFAULT NULL COMMENT '公告',
  `Remark` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`ID`),
  KEY `FK_groups_Creator` (`Creator`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 COMMENT='组';
LOCK TABLES `groups` WRITE;
/*!40000 ALTER TABLE `groups` DISABLE KEYS */;

INSERT INTO `groups` VALUES (1,1,NULL,'hehui','IT','2011-03-09 13:21:47',1,1,NULL,NULL,NULL);
/*!40000 ALTER TABLE `groups` ENABLE KEYS */;
UNLOCK TABLES;
CREATE TABLE `grouptypes` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `ParentType` int(10) unsigned DEFAULT NULL,
  `TypeName` varchar(45) NOT NULL,
  `Description` varchar(45) DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8 COMMENT='组类型';
LOCK TABLES `grouptypes` WRITE;
/*!40000 ALTER TABLE `grouptypes` DISABLE KEYS */;

INSERT INTO `grouptypes` VALUES (1,NULL,'System',NULL);
INSERT INTO `grouptypes` VALUES (2,NULL,'User',NULL);
/*!40000 ALTER TABLE `grouptypes` ENABLE KEYS */;
UNLOCK TABLES;
CREATE TABLE `loginhistories` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `UserID` varchar(16) NOT NULL,
  `IPAddress` varchar(15) NOT NULL,
  `LoginTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `LogoutTime` datetime DEFAULT NULL,
  PRIMARY KEY (`ID`),
  KEY `FK_loginhistories_UserID` (`UserID`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8 COMMENT='登陆历史';
LOCK TABLES `loginhistories` WRITE;
/*!40000 ALTER TABLE `loginhistories` DISABLE KEYS */;

INSERT INTO `loginhistories` VALUES (1,'admin','200.200.200.17','2011-09-28 17:48:34','2011-09-28 17:48:53');
INSERT INTO `loginhistories` VALUES (2,'admin','200.200.200.17','2011-09-29 11:52:24','2011-09-29 11:53:33');
INSERT INTO `loginhistories` VALUES (3,'admin','200.200.200.17','2011-09-29 11:55:26','2011-09-29 11:56:57');
/*!40000 ALTER TABLE `loginhistories` ENABLE KEYS */;
UNLOCK TABLES;
CREATE TABLE `membershipapply` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `ApplicantSystemID` int(10) unsigned NOT NULL,
  `GroupID` int(10) unsigned NOT NULL,
  `ExtraMessage` varchar(32) DEFAULT NULL,
  PRIMARY KEY (`ID`),
  KEY `FK_friendshipapply_users_ID_ReceiverID` (`GroupID`) USING BTREE,
  KEY `FK_friendshipapply_users_ID_SenderID` (`ApplicantSystemID`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT COMMENT='加入组请求';
LOCK TABLES `membershipapply` WRITE;
/*!40000 ALTER TABLE `membershipapply` DISABLE KEYS */;

/*!40000 ALTER TABLE `membershipapply` ENABLE KEYS */;
UNLOCK TABLES;
CREATE TABLE `software__` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `InternalName` varchar(64) NOT NULL,
  `Author` varchar(45) DEFAULT NULL,
  `LegalCopyright` varchar(45) DEFAULT NULL,
  `Comments` varchar(64) DEFAULT NULL,
  `LatestInternalVersion` varchar(45) NOT NULL,
  `UpdatePath` varchar(255) NOT NULL,
  `Framework` varchar(45) NOT NULL,
  `ExpirationDate` date NOT NULL,
  `NewFileName` varchar(64) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `Index_2` (`InternalName`,`Framework`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
LOCK TABLES `software__` WRITE;
/*!40000 ALTER TABLE `software__` DISABLE KEYS */;

/*!40000 ALTER TABLE `software__` ENABLE KEYS */;
UNLOCK TABLES;
CREATE TABLE `systemgroupmembers__` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `MemberID` varchar(16) NOT NULL COMMENT '成员ID',
  `SystemGroupID` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '系统组ID',
  `Role` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '角色',
  PRIMARY KEY (`ID`),
  UNIQUE KEY `Index_MemberID_SystemGroupID` (`MemberID`,`SystemGroupID`),
  KEY `FK_systemgroupmembers_SystemGroupID` (`SystemGroupID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT COMMENT='个人兴趣组的成员';
LOCK TABLES `systemgroupmembers__` WRITE;
/*!40000 ALTER TABLE `systemgroupmembers__` DISABLE KEYS */;

/*!40000 ALTER TABLE `systemgroupmembers__` ENABLE KEYS */;
UNLOCK TABLES;
CREATE TABLE `systemgroups__` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `GroupName` varchar(32) NOT NULL COMMENT '组名',
  `Description` varchar(64) DEFAULT NULL COMMENT '描述',
  `Creator` int(10) unsigned NOT NULL COMMENT '创建者ID',
  `CreationTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
  `Announcement` varchar(255) DEFAULT NULL COMMENT '公告',
  `ParentGroup` int(10) unsigned DEFAULT NULL COMMENT '父组ID',
  `Admins` varchar(45) NOT NULL COMMENT '管理员，用","分隔系统用户ID',
  `GroupInfoVersion` int(10) unsigned NOT NULL DEFAULT '1' COMMENT '组信息版本',
  `Remark` varchar(64) DEFAULT NULL COMMENT '备注',
  `MemberListVersion` int(10) unsigned NOT NULL,
  PRIMARY KEY (`ID`),
  KEY `FK_systemgroups_systemusers_ID` (`Creator`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8 COMMENT='系统预定义的组';
LOCK TABLES `systemgroups__` WRITE;
/*!40000 ALTER TABLE `systemgroups__` DISABLE KEYS */;

INSERT INTO `systemgroups__` VALUES (1,'Undefined',NULL,1,'2010-05-31 12:04:00',NULL,0,'1',1,NULL,0);
INSERT INTO `systemgroups__` VALUES (2,'IT',NULL,1,'2010-05-31 12:04:00',NULL,0,'1',1,NULL,0);
/*!40000 ALTER TABLE `systemgroups__` ENABLE KEYS */;
UNLOCK TABLES;
CREATE TABLE `systemusers__` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `UserID` varchar(24) NOT NULL,
  `UserName` varchar(16) NOT NULL,
  `Dept` varchar(16) NOT NULL,
  `PassWD` varchar(56) NOT NULL COMMENT 'Base64编码后的SHA-1加密内容',
  `LastloginIP` char(15) NOT NULL,
  `LastLoginTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '当前数据库时间',
  `Remark` varchar(255) DEFAULT NULL,
  `LoginTimes` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;
LOCK TABLES `systemusers__` WRITE;
/*!40000 ALTER TABLE `systemusers__` DISABLE KEYS */;

INSERT INTO `systemusers__` VALUES (1,'hehui','贺辉','IT','veRES9cTvQSNx8xh/i4TdVHi+nQ=','','2009-08-18 11:31:42',NULL,0);
INSERT INTO `systemusers__` VALUES (2,'administrator','Admin','IT','Y4WjrhimXU9udEG96cTK+8Y1x7g=','','2009-07-20 15:04:02',NULL,0);
/*!40000 ALTER TABLE `systemusers__` ENABLE KEYS */;
UNLOCK TABLES;
CREATE TABLE `userroles` (
  `ID` int(10) unsigned NOT NULL,
  `RoleName` varchar(16) NOT NULL,
  `Description` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
LOCK TABLES `userroles` WRITE;
/*!40000 ALTER TABLE `userroles` DISABLE KEYS */;

INSERT INTO `userroles` VALUES (1,'Super Admin','');
INSERT INTO `userroles` VALUES (2,'Admin','');
INSERT INTO `userroles` VALUES (3,'user','');
/*!40000 ALTER TABLE `userroles` ENABLE KEYS */;
UNLOCK TABLES;
CREATE TABLE `users_detailed_info` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `UserID` varchar(16) NOT NULL,
  `UserPassword` varchar(32) NOT NULL DEFAULT 'W6ph5Mm5Pz8GgiULbPgzG37mj9g=' COMMENT '密码',
  `TrueName` varchar(32) DEFAULT NULL,
  `NickName` varchar(32) DEFAULT NULL,
  `Gender` char(1) NOT NULL DEFAULT '0',
  `Age` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `Face` varchar(255) DEFAULT '0',
  `PersonalContactGroupsInfo` varchar(1024) DEFAULT NULL COMMENT '联系人信息',
  `PersonalContactGroupsInfoVersion` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '联系人版本',
  `InterestGroupsInfo` varchar(255) DEFAULT NULL COMMENT '","分隔',
  `InterestGroupsInfoVersion` int(10) unsigned NOT NULL DEFAULT '0',
  `BlacklistInfoVersion` int(10) unsigned DEFAULT '0',
  `Blacklist` varchar(512) DEFAULT NULL COMMENT '黒名单,用“,”分隔联系人的ID',
  `PersonalInfoVersion` int(10) unsigned NOT NULL DEFAULT '1' COMMENT '个人信息版本',
  `HomeAddress` varchar(255) DEFAULT NULL COMMENT '家庭住址',
  `HomePhoneNumber` varchar(32) DEFAULT NULL COMMENT '家庭电话号码',
  `HomeZipCode` varchar(12) DEFAULT NULL COMMENT '家庭邮编',
  `PersonalHomepage` varchar(255) DEFAULT NULL COMMENT '个人主页',
  `PersonalEmailAddress` varchar(255) DEFAULT NULL COMMENT '个人电子邮箱',
  `QuestionForSecurity` varchar(255) DEFAULT NULL,
  `AnswerForSecurity` varchar(255) DEFAULT NULL,
  `EmailForSecurity` varchar(255) DEFAULT NULL,
  `CompanyName` varchar(255) DEFAULT NULL,
  `JobTitle` varchar(255) DEFAULT NULL,
  `BusinessAddress` varchar(255) DEFAULT NULL,
  `BusinessPhoneNumber` varchar(255) DEFAULT NULL,
  `BusinessZipCode` varchar(255) DEFAULT NULL,
  `BusinessFaxNumber` varchar(255) DEFAULT NULL,
  `BusinessHomepage` varchar(255) DEFAULT NULL,
  `BusinessEmailAddress` varchar(255) DEFAULT NULL,
  `RegistrationTime` datetime DEFAULT NULL,
  `LoginTimes` int(10) unsigned DEFAULT NULL,
  `FriendshipApply` char(1) NOT NULL DEFAULT '0' COMMENT '好友请求(V:要求认证;A:自动接受)',
  `ShortTalk` char(1) NOT NULL DEFAULT '0' COMMENT '临时佳话(P:提示;D:拒绝;A:自动接受)',
  `Role` int(10) unsigned NOT NULL DEFAULT '3',
  `Description` varchar(256) DEFAULT NULL COMMENT '自我介绍',
  PRIMARY KEY (`ID`),
  UNIQUE KEY `index_users_UserID` (`UserID`)
) ENGINE=InnoDB AUTO_INCREMENT=7 DEFAULT CHARSET=utf8;
LOCK TABLES `users_detailed_info` WRITE;
/*!40000 ALTER TABLE `users_detailed_info` DISABLE KEYS */;

INSERT INTO `users_detailed_info` VALUES (1,'hehui','P66eKnCMcBPetxi+LMKG2P7cBSA=','贺辉','辉','1',0,'0','Friends,kiwa,admin',255,'1',3,0,'',1,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'0','0',1,NULL);
INSERT INTO `users_detailed_info` VALUES (2,'admin','P66eKnCMcBPetxi+LMKG2P7cBSA=','Admin','Admin','1',0,'0','Friends,hehui,kiwa',3,'1',3,0,NULL,1,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'0','0',2,NULL);
INSERT INTO `users_detailed_info` VALUES (3,'rebort','P66eKnCMcBPetxi+LMKG2P7cBSA=',NULL,NULL,'1',0,'0',NULL,0,'1',3,0,NULL,1,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'0','0',3,NULL);
INSERT INTO `users_detailed_info` VALUES (4,'king','P66eKnCMcBPetxi+LMKG2P7cBSA=',NULL,NULL,'1',0,'0',NULL,0,'1',1,0,NULL,1,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'0','0',3,NULL);
INSERT INTO `users_detailed_info` VALUES (5,'yu','P66eKnCMcBPetxi+LMKG2P7cBSA=',NULL,NULL,'1',0,'0',NULL,0,'1',0,0,NULL,1,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'0','0',3,NULL);
INSERT INTO `users_detailed_info` VALUES (6,'kiwa','P66eKnCMcBPetxi+LMKG2P7cBSA=',NULL,NULL,'1',0,'0',NULL,0,'1',0,0,NULL,1,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'0','0',3,NULL);
/*!40000 ALTER TABLE `users_detailed_info` ENABLE KEYS */;
UNLOCK TABLES;
CREATE TABLE `users_summary_info_` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `UserID` varchar(16) NOT NULL,
  `UserPassword` varchar(32) NOT NULL DEFAULT 'W6ph5Mm5Pz8GgiULbPgzG37mj9g=' COMMENT '密码',
  `TrueName` varchar(32) DEFAULT NULL,
  `NickName` varchar(32) DEFAULT NULL,
  `Gender` char(1) NOT NULL DEFAULT '0',
  `Age` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `Face` varchar(255) DEFAULT '0',
  `PersonalContactGroupsInfo` varchar(1024) DEFAULT NULL COMMENT '联系人信息',
  `PersonalContactGroupsInfoVersion` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '联系人版本',
  `InterestGroupsInfo` varchar(255) DEFAULT NULL,
  `InterestGroupsInfoVersion` int(11) unsigned NOT NULL DEFAULT '0',
  `BlacklistInfoVersion` int(11) unsigned DEFAULT '0',
  `Blacklist` varchar(512) DEFAULT NULL COMMENT '黒名单,用“,”分隔联系人的ID',
  `PersonalInfoVersion` int(10) unsigned NOT NULL DEFAULT '1' COMMENT '个人信息版本',
  `LastLoginTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '最后登陆时间',
  `LastLoginHostAddress` char(15) DEFAULT NULL,
  `LastLoginHostPort` smallint(5) unsigned DEFAULT NULL,
  `LoginTimes` int(10) unsigned DEFAULT NULL,
  `FriendshipApply` char(1) NOT NULL DEFAULT '0' COMMENT '好友请求(V:要求认证;A:自动接受)',
  `ShortTalk` char(1) NOT NULL DEFAULT '0' COMMENT '临时佳话(P:提示;D:拒绝;A:自动接受)',
  `Role` int(10) unsigned NOT NULL DEFAULT '3',
  PRIMARY KEY (`ID`),
  UNIQUE KEY `index_users_UserID` (`UserID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
LOCK TABLES `users_summary_info_` WRITE;
/*!40000 ALTER TABLE `users_summary_info_` DISABLE KEYS */;

/*!40000 ALTER TABLE `users_summary_info_` ENABLE KEYS */;
UNLOCK TABLES;

CREATE PROCEDURE `sp_userlogin`(IN userID varchar(16), IN ipAddress varchar(15), IN loginTime datetime)
begin
insert into loginhistories(UserID, IPAddress, LoginTime) values(userID, ipAddress, loginTime);
end;


CREATE PROCEDURE `sp_userlogout`(IN userID varchar(16), IN logoutTime datetime)
begin
set @MAXID= (select max(ID) from loginhistories where UserID = userID);
update loginhistories set LogoutTime = logoutTime where ID = @MAXID;
end;


ALTER TABLE `cachedchatmessages`
ADD CONSTRAINT `FK_cachemessages_users_UserID_ReceiverID` FOREIGN KEY (`RecieverID`) REFERENCES `users_detailed_info` (`UserID`) ON DELETE CASCADE ON UPDATE NO ACTION,
ADD CONSTRAINT `FK_cachemessages_users_UserID_SenderID` FOREIGN KEY (`SenderID`) REFERENCES `users_detailed_info` (`UserID`) ON DELETE CASCADE ON UPDATE NO ACTION;

ALTER TABLE `friendshipapply`
ADD CONSTRAINT `FK_friendshipapply_users_UserID_ReceiverID` FOREIGN KEY (`ReceiverID`) REFERENCES `users_detailed_info` (`UserID`) ON DELETE CASCADE ON UPDATE NO ACTION,
ADD CONSTRAINT `FK_friendshipapply_users_UserID_SenderID` FOREIGN KEY (`SenderID`) REFERENCES `users_detailed_info` (`UserID`) ON DELETE CASCADE ON UPDATE NO ACTION;

ALTER TABLE `groupmembers`
ADD CONSTRAINT `FK_groupmembers_GroupID` FOREIGN KEY (`GroupID`) REFERENCES `groups` (`ID`) ON DELETE CASCADE,
ADD CONSTRAINT `FK_groupmembers_MemberUserID` FOREIGN KEY (`MemberUserID`) REFERENCES `users_detailed_info` (`UserID`) ON DELETE CASCADE ON UPDATE CASCADE;

ALTER TABLE `groups`
ADD CONSTRAINT `FK_groups_Creator` FOREIGN KEY (`Creator`) REFERENCES `users_detailed_info` (`UserID`) ON UPDATE CASCADE;

ALTER TABLE `loginhistories`
ADD CONSTRAINT `FK_loginhistories_UserID` FOREIGN KEY (`UserID`) REFERENCES `users_detailed_info` (`UserID`) ON DELETE CASCADE;

ALTER TABLE `systemgroupmembers__`
ADD CONSTRAINT `FK_systemgroupmembers_MemberID` FOREIGN KEY (`MemberID`) REFERENCES `users_detailed_info` (`UserID`) ON DELETE CASCADE ON UPDATE CASCADE,
ADD CONSTRAINT `FK_systemgroupmembers_SystemGroupID` FOREIGN KEY (`SystemGroupID`) REFERENCES `systemgroups__` (`ID`) ON DELETE CASCADE ON UPDATE CASCADE;

ALTER TABLE `systemgroups__`
ADD CONSTRAINT `FK_systemgroups_systemusers_ID` FOREIGN KEY (`Creator`) REFERENCES `systemusers__` (`ID`) ON UPDATE NO ACTION;


/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
