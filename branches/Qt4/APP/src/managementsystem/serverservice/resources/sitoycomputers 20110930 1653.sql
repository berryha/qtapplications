-- MySQL Administrator dump 1.4
--
-- ------------------------------------------------------
-- Server version	5.1.36-community


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;


--
-- Create schema sitoycomputers
--

CREATE DATABASE IF NOT EXISTS sitoycomputers;
USE sitoycomputers;

--
-- Definition of table `detailedinfo`
--

DROP TABLE IF EXISTS `detailedinfo`;
CREATE TABLE `detailedinfo` (
  `ComputerName` varchar(32) NOT NULL DEFAULT '',
  `InstallationDate` date DEFAULT NULL,
  `WindowsDir` varchar(12) DEFAULT NULL,
  `OSKey` varchar(45) DEFAULT NULL,
  `CPU` varchar(128) DEFAULT NULL,
  `DMIUUID` varchar(128) DEFAULT NULL,
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
  CONSTRAINT `FK_detailedinfo_ComputerName_summaryinfo` FOREIGN KEY (`ComputerName`) REFERENCES `summaryinfo` (`ComputerName`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `detailedinfo`
--

/*!40000 ALTER TABLE `detailedinfo` DISABLE KEYS */;
/*!40000 ALTER TABLE `detailedinfo` ENABLE KEYS */;


--
-- Definition of table `installedsoftware`
--

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
  KEY `FK_installedsoftware_ComputerName` (`ComputerName`),
  CONSTRAINT `FK_installedsoftware_ComputerName` FOREIGN KEY (`ComputerName`) REFERENCES `summaryinfo` (`ComputerName`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `installedsoftware`
--

/*!40000 ALTER TABLE `installedsoftware` DISABLE KEYS */;
/*!40000 ALTER TABLE `installedsoftware` ENABLE KEYS */;


--
-- Definition of table `logs`
--

DROP TABLE IF EXISTS `logs`;
CREATE TABLE `logs` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `ComputerName` varchar(16) NOT NULL,
  `Users` varchar(255) DEFAULT NULL,
  `IPAddress` varchar(16) NOT NULL,
  `Type` tinyint(3) unsigned DEFAULT NULL,
  `Content` varchar(512) NOT NULL,
  `ClientTime` datetime DEFAULT NULL,
  `ServerTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `logs`
--

/*!40000 ALTER TABLE `logs` DISABLE KEYS */;
/*!40000 ALTER TABLE `logs` ENABLE KEYS */;


--
-- Definition of table `software`
--

DROP TABLE IF EXISTS `software`;
CREATE TABLE `software` (
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

--
-- Dumping data for table `software`
--

/*!40000 ALTER TABLE `software` DISABLE KEYS */;
/*!40000 ALTER TABLE `software` ENABLE KEYS */;


--
-- Definition of table `summaryinfo`
--

DROP TABLE IF EXISTS `summaryinfo`;
CREATE TABLE `summaryinfo` (
  `ComputerName` varchar(32) NOT NULL,
  `Workgroup` varchar(32) NOT NULL,
  `Network` varchar(255) NOT NULL DEFAULT '',
  `Users` varchar(255) DEFAULT NULL,
  `OS` varchar(45) NOT NULL,
  `USBSD` tinyint(3) unsigned DEFAULT '0',
  `Programes` tinyint(3) unsigned DEFAULT '0',
  `Administrators` varchar(255) DEFAULT NULL,
  `LastOnlineTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `ClientVersion` varchar(32) DEFAULT NULL,
  PRIMARY KEY (`ComputerName`) USING BTREE,
  UNIQUE KEY `Index_ComputerName` (`ComputerName`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `summaryinfo`
--

/*!40000 ALTER TABLE `summaryinfo` DISABLE KEYS */;
/*!40000 ALTER TABLE `summaryinfo` ENABLE KEYS */;


--
-- Definition of table `windowsadminpassword`
--

DROP TABLE IF EXISTS `windowsadminpassword`;
CREATE TABLE `windowsadminpassword` (
  `Department` varchar(16) NOT NULL,
  `Password` varchar(45) NOT NULL DEFAULT 'trousetrouse',
  `OldPassword` varchar(45) NOT NULL DEFAULT 'computermisdg',
  `UpdateTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `Admin` varchar(45) NOT NULL DEFAULT 'HeHui',
  PRIMARY KEY (`Department`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `windowsadminpassword`
--

/*!40000 ALTER TABLE `windowsadminpassword` DISABLE KEYS */;
INSERT INTO `windowsadminpassword` (`Department`,`Password`,`OldPassword`,`UpdateTime`,`Admin`) VALUES 
 ('Account','trousetrouse','computermisdg','2011-05-28 17:01:16','HeHui'),
 ('AdminDept','trousetrouse','computermisdg','2011-05-28 17:01:16','HeHui'),
 ('Cost','trousetrouse','computermisdg','2011-05-28 17:01:16','HeHui'),
 ('Custom','trousetrouse','computermisdg','2011-05-28 17:01:16','HeHui'),
 ('GMO','trousetrouse','computermisdg','2011-05-28 17:01:16','HeHui'),
 ('HR','trousetrouse','computermisdg','2011-05-28 17:01:16','HeHui'),
 ('Marker','trousetrouse','computermisdg','2011-05-28 17:01:16','HeHui'),
 ('PDS','trousetrouse','computermisdg','2011-05-28 17:01:16','HeHui'),
 ('PG','trousetrouse','computermisdg','2011-05-28 17:01:16','HeHui'),
 ('Plan','trouseplantrouse','computermisdg','2011-05-28 17:01:16','HeHui'),
 ('PMC','trousetrouse','computermisdg','2011-05-28 17:01:16','HeHui'),
 ('Purchase','trousetrouse','computermisdg','2011-05-28 17:01:16','HeHui'),
 ('QC','trousetrouse','computermisdg','2011-05-28 17:01:16','HeHui'),
 ('Retail','trousetrouse','computermisdg','2011-05-28 17:01:16','HeHui'),
 ('Sales','trousetrouse','computermisdg','2011-05-28 17:01:16','HeHui'),
 ('Sample','trousetrouse','computermisdg','2011-05-28 17:01:16','HeHui'),
 ('Secretary','trousetrouse','computermisdg','2011-05-28 17:01:17','HeHui'),
 ('Ship','trousetrouse','computermisdg','2011-05-28 17:01:17','HeHui'),
 ('Shop','trousetrouse','computermisdg','2011-05-28 17:01:17','HeHui'),
 ('WHouse','trousetrouse','computermisdg','2011-05-28 17:01:17','HeHui');
/*!40000 ALTER TABLE `windowsadminpassword` ENABLE KEYS */;




/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
