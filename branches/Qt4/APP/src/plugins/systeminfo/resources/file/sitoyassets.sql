# SQL-Front 5.1  (Build 4.16)

/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE */;
/*!40101 SET SQL_MODE='' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES */;
/*!40103 SET SQL_NOTES='ON' */;


# Host: 200.200.200.40    Database: sitoyassets
# ------------------------------------------------------
# Server version 5.1.53-log

DROP DATABASE IF EXISTS `sitoyassets`;
CREATE DATABASE `sitoyassets` /*!40100 DEFAULT CHARACTER SET utf8 */;
USE `sitoyassets`;

#
# Source for table assetsinfo
#

DROP TABLE IF EXISTS `assetsinfo`;
CREATE TABLE `assetsinfo` (
  `ComputerName` varchar(32) NOT NULL,
  `Workgroup` varchar(32) NOT NULL,
  `Users` varchar(255) DEFAULT NULL,
  `OS` varchar(45) NOT NULL,
  `SN` int(11) NOT NULL,
  `Vender` varchar(45) DEFAULT NULL,
  `DateOfPurchase` date DEFAULT NULL,
  `Warranty` varchar(45) DEFAULT NULL,
  `ServiceNumber` varchar(45) DEFAULT NULL,
  `Registrant` varchar(45) DEFAULT NULL,
  `InstallationDate` date DEFAULT NULL,
  `OSKey` varchar(45) DEFAULT NULL,
  `CPU` varchar(128) DEFAULT NULL,
  `MotherboardName` varchar(255) DEFAULT NULL,
  `Chipset` varchar(64) DEFAULT NULL,
  `Memory` varchar(64) DEFAULT NULL,
  `Storage` varchar(256) DEFAULT NULL,
  `Video` varchar(128) DEFAULT NULL,
  `Audio` varchar(128) DEFAULT NULL,
  `NIC1` varchar(512) DEFAULT NULL,
  `NIC2` varchar(512) DEFAULT NULL,
  `Monitor` varchar(96) DEFAULT NULL,
  `UpdateTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `Remark` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`ComputerName`) USING BTREE,
  UNIQUE KEY `Index_ComputerName` (`ComputerName`),
  UNIQUE KEY `SN_UNIQUE` (`SN`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

#
# Source for table installedsoftware
#

DROP TABLE IF EXISTS `installedsoftware`;
CREATE TABLE `installedsoftware` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `ComputerName` varchar(32) NOT NULL,
  `SoftwareName` varchar(255) NOT NULL,
  `SoftwareVersion` varchar(45) DEFAULT NULL,
  `Size` varchar(45) DEFAULT NULL,
  `InstallationDate` varchar(45) DEFAULT NULL,
  `Publisher` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`ID`),
  KEY `FK_installedsoftware_ComputerName` (`ComputerName`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;

#
#  Foreign keys for table installedsoftware
#

ALTER TABLE `installedsoftware`
ADD CONSTRAINT `FK_installedsoftware_ComputerName` FOREIGN KEY (`ComputerName`) REFERENCES `assetsinfo` (`ComputerName`) ON DELETE CASCADE;


/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
