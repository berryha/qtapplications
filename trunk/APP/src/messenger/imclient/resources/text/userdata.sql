Pragma foreign_keys=false;
Begin Transaction;
Drop Table If Exists [contactchatmessages];
CREATE TABLE "contactchatmessages"(
[ID] integer PRIMARY KEY AUTOINCREMENT NOT NULL
,[SenderID] varchar(16) NOT NULL
,[ReceiverID] varchar(16) NOT NULL
,[Message] varchar(512) NOT NULL
,[Time] timestamp NOT NULL DEFAULT current_timestamp
   
);
Drop Table If Exists [contactgroups];
CREATE TABLE "contactgroups" (
"GroupID"  INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL ON CONFLICT FAIL,
"GroupName"  varchar(32) NOT NULL,
"GroupInfoVersion"  INTEGER NOT NULL DEFAULT 0,
"MemberListVersion"  INTEGER NOT NULL DEFAULT 0,
UNIQUE ("GroupName" )
);
Insert  Into [contactgroups] ([GroupID],[GroupName],[GroupInfoVersion],[MemberListVersion]) Values("-1","Strangers","0","0");
Insert  Into [contactgroups] ([GroupID],[GroupName],[GroupInfoVersion],[MemberListVersion]) Values("0","Blacklist","0","0");
Insert  Into [contactgroups] ([GroupID],[GroupName],[GroupInfoVersion],[MemberListVersion]) Values("1","Friends","0","0");
Drop Table If Exists [interestgroupchatmessages];
CREATE TABLE "interestgroupchatmessages"(
[ID] integer PRIMARY KEY AUTOINCREMENT NOT NULL
,[SenderID] varchar(16) NOT NULL
,[GroupID] integer NOT NULL
,[Message] varchar(512)
,[Time] timestamp NOT NULL DEFAULT current_timestamp
   
);
Drop Table If Exists [interestgroupmembers];
CREATE TABLE "interestgroupmembers"(
[ID] integer PRIMARY KEY AUTOINCREMENT
,[MemberID] varchar(16) NOT NULL
,[GroupID] int NOT NULL
,[MemberRole] int NOT NULL
   
);
Drop Table If Exists [interestgroups];
CREATE TABLE "interestgroups" (
"GroupID"  integer NOT NULL,
"TypeID"  int(10),
"ParentGroup"  int(10) DEFAULT NULL,
"Creator"  int(10),
"GroupName"  varchar(32),
"CreationTime"  datetime,
"GroupInfoVersion"  int(10) NOT NULL ON CONFLICT REPLACE DEFAULT 1,
"MemberListVersion"  int(10) NOT NULL ON CONFLICT REPLACE DEFAULT 1,
"Description"  varchar(64),
"Announcement"  varchar(64) DEFAULT NULL,
"Remark"  varchar(64) DEFAULT NULL,
PRIMARY KEY ("GroupID" )
);
Drop Table If Exists [my_detailed_info];
CREATE TABLE "my_detailed_info" (
"SysID"  INTEGER,
"UserID"  varchar(16) NOT NULL,
"TrueName"  varchar(20) DEFAULT NULL,
"NickName"  varchar(20) DEFAULT NULL,
"Gender"  char(1) NOT NULL DEFAULT 0,
"Age"  tinyint NOT NULL DEFAULT 0,
"Face"  varchar(255) DEFAULT 0,
"PersonalContactGroupsInfoVersion"  int(10) NOT NULL DEFAULT 0,
"InterestGroupsInfoVersion"  int(10) NOT NULL DEFAULT 0,
"BlacklistInfoVersion"  int(10) NOT NULL DEFAULT 0,
"PersonalSummaryInfoVersion"  int(10) NOT NULL DEFAULT 0,
"PersonalDetailInfoVersion"  INT(10) NOT NULL DEFAULT 0,
"PersonalMessageInfoVersion"  INT(10) NOT NULL DEFAULT 0,
"FriendshipApply"  Tinyint(1),
"ShortTalk"  Tinyint(1),
"Role"  INTEGER,
"Description"  VARCHAR(255),
"AccountState"  Tinyint(3) NOT NULL DEFAULT 1,
"PersonalMessage"  varchar(64),
"HomeAddress"  varchar(255) DEFAULT NULL,
"HomePhoneNumber"  varchar(32) DEFAULT NULL,
"HomeZipCode"  varchar(12) DEFAULT NULL,
"PersonalHomepage"  varchar(255) DEFAULT NULL,
"PersonalEmailAddress"  varchar(255) DEFAULT NULL,
"CompanyName"  varchar(255) DEFAULT NULL,
"JobTitle"  varchar(255) DEFAULT NULL,
"BusinessAddress"  varchar(255) DEFAULT NULL,
"BusinessPhoneNumber"  varchar(255) DEFAULT NULL,
"BusinessZipCode"  varchar(255) DEFAULT NULL,
"BusinessFaxNumber"  varchar(255) DEFAULT NULL,
"BusinessHomepage"  varchar(255) DEFAULT NULL,
"BusinessEmailAddress"  varchar(255) DEFAULT NULL,
"LastLoginHostAddress"  char(15) DEFAULT NULL,
"LastLoginHostPort"  smallint(5) DEFAULT NULL
);
Drop Table If Exists [contacts_detailed_info];
CREATE TABLE "contacts_detailed_info" (
"UserID"  varchar(16) NOT NULL,
"TrueName"  varchar(32) DEFAULT NULL,
"NickName"  varchar(32) DEFAULT NULL,
"Gender"  tinyint(1) DEFAULT 0,
"Age"  tinyint(3) DEFAULT 0,
"Face"  varchar(255) DEFAULT NULL,
"ContactGroupID"  INTEGER NOT NULL ON CONFLICT REPLACE DEFAULT -1,
"PersonalSummaryInfoVersion"  INT(10) NOT NULL ON CONFLICT REPLACE DEFAULT 1,
"PersonalDetailInfoVersion"  INT(10) NOT NULL ON CONFLICT REPLACE DEFAULT 0,
"PersonalMessageInfoVersion"  INT(10) NOT NULL ON CONFLICT REPLACE DEFAULT 0,
"Role"  int(10) DEFAULT '3',
"Description"  varchar(255),
"AccountState"  Tinyint(3) NOT NULL ON CONFLICT REPLACE DEFAULT 1,
"PersonalMessage"  varchar(64),
"HomeAddress"  varchar(255) DEFAULT NULL,
"HomePhoneNumber"  varchar(32) DEFAULT NULL,
"HomeZipCode"  varchar(12) DEFAULT NULL,
"PersonalHomepage"  varchar(255) DEFAULT NULL,
"PersonalEmailAddress"  varchar(255) DEFAULT NULL,
"CompanyName"  varchar(255) DEFAULT NULL,
"JobTitle"  varchar(255) DEFAULT NULL,
"BusinessAddress"  varchar(255) DEFAULT NULL,
"BusinessPhoneNumber"  varchar(255) DEFAULT NULL,
"BusinessZipCode"  varchar(255) DEFAULT NULL,
"BusinessFaxNumber"  varchar(255) DEFAULT NULL,
"BusinessHomepage"  varchar(255) DEFAULT NULL,
"BusinessEmailAddress"  varchar(255) DEFAULT NULL,
"RemarkName"  varchar(32) DEFAULT NULL,
PRIMARY KEY ("UserID" ),
CONSTRAINT "FK-ContactGroupID" FOREIGN KEY ("ContactGroupID") REFERENCES "contactgroups" ("GroupID") ON DELETE SET DEFAULT ON UPDATE CASCADE,
UNIQUE ("UserID" )
);
Commit Transaction;
Pragma foreign_keys=true;
