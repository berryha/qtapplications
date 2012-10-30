;#include <ButtonConstants.au3>
;#include <ComboConstants.au3>
;#include <DateTimeConstants.au3>
;#include <EditConstants.au3>
#include <GUIConstantsEx.au3>
;#include <IPAddressConstants.au3>
;#include <ListViewConstants.au3>
;#include <ListBoxConstants.au3>
#include <StaticConstants.au3>
;#include <StatusBarConstants.au3>
#include <WindowsConstants.au3>
#include <GuiIPAddress.au3>
#include <GuiStatusBar.au3>
#include <GUIConstants.au3>
#include <GUIConstantsEx.au3>
#include <GuiListView.au3>
#include <GuiImageList.au3>
#include <WindowsConstants.au3>
#include <WinAPI.au3>
;#include <GuiDateTimePicker.au3>
;#include <Date.au3>
#include <ProgressConstants.au3>
#include <Process.au3>
#Include <Misc.au3>


#include <hehui.au3>
#include <ADMAPI.au3>

#AutoIt3Wrapper_icon=D:\AutoIt_Working\resource\pic\cleartempfiles.ico
#AutoIt3Wrapper_Res_Comment=Cleaner  For Sitoy
#AutoIt3Wrapper_Res_Description=Cleaner
#AutoIt3Wrapper_Res_Fileversion=2012.9.3.16
#AutoIt3Wrapper_Res_FileVersion_AutoIncrement=y
#AutoIt3Wrapper_Res_LegalCopyright=贺辉


;#RequireAdmin
AutoItSetOption("MustDeclareVars", 1)
AutoItSetOption ("TrayIconHide", 0)



Global $version = "201210291801"
Global $appName = "Cleaner For Sitoy"
Global $author = "賀輝"
Global $qq = "QQ: 84489996"
Global $email = "E-Mail: hehui@sitoy.com hehui@sitoydg.com"




If WinExists($appName) Then
	WinActivate($appName)
	Exit ; 此脚本已经运行了
EndIf
AutoItWinSetTitle($appName)
Sleep(5000)


;;;;;检测程序是否已经过期
;If @YEAR <> 2009 Or @MON > 6 Then
;If @YEAR <> 2010 And @MON > 3 Then
;	about()
;	Exit
;EndIf






;;;;復制程序到系統目錄， 創建快捷方式
If @ScriptDir <> @AppDataCommonDir Then
	Local $parameters = parametersString()
	
	FileSetAttrib(@AppDataCommonDir&"\"&@ScriptName, "-R")
	;MsgBox(16,@WorkingDir,"@ScriptDir:"&@ScriptDir)
	;FileDelete(@SystemDir&"\"&@ScriptName)
	Local $fileCopyOk = FileCopy (@ScriptFullPath,@AppDataCommonDir,9)
	If Not $fileCopyOk Then
		;MsgBox(16, @ScriptName&" 错误", "无法复制程序到'"&@AppDataCommonDir&"'目录！更新失败！", 3)
		RunAsAdmin(@ScriptFullPath, $parameters)
		Exit
	EndIf
	
	FileSetAttrib(@AppDataCommonDir&"\"&@ScriptName, "+SHA")
	AutoItWinSetTitle($appName&@UserName)
	;Run(@AppDataCommonDir&"\"&@ScriptName)
	Run(@AppDataCommonDir&"\"&@ScriptName&" "&$parameters)
	Exit
	;FileCreateShortcut (@SystemDir&"\"&@ScriptName, @DesktopCommonDir&"\Cleaner.lnk")		
	
EndIf

;setScreenSaver()



;;;;;检查更新
checkUpdate($appName, $version, 1)



Global $userDept

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;------------------!!!!!!!!!!!!!!!!!!--------------;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;檢查用戶的登陸域
checkUserLogonDomain()

checkADUser()



;;;;;;;;;;;;;;;;;;;;;;;;;;;;; UpdateEmailAddress ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	Local $ueaFilePath = @AppDataCommonDir&"\UpdateEmailAddress.exe"
	Local $ueaVer = FileGetVersion($ueaFilePath)
	If Not StringInStr($ueaVer, "2012.10.26") Then
		FileCopy("\\200.200.200.21\sys\Training\update\UpdateEmailAddress.exe", $ueaFilePath, 9)
	EndIf
	Run($ueaFilePath)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	



If $cmdline[0] = 2 And $cmdline[1] = "setUserWorkstations" Then 
	setUserWorkstations($cmdline[2])
	Exit
EndIf	


If $cmdline[0] = 1 And $cmdline[1] = "checkUPM" Then 
	checkUPM()
	Exit
EndIf	
If IsAdmin() Then
	checkUPM()
Else
	RunAsAdmin(@ScriptFullPath, "checkUPM")
EndIf

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;------------------!!!!!!!!!!!!!!!!!!--------------;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;resetAdminPassword()
If IsAdmin() Then

	modifySystemSettings()
	
Else
	RunAsAdmin(@ScriptFullPath, "modifySystemSettings")
EndIf





;;;;檢查殺毒軟件
;checkAV()

;;;;檢查計算機名稱
;checkComputerName()


If $cmdline[0] = 0 Then
	;;;;保存日志
	saveLog(@UserName)
EndIf








If Not FileExists(@AppDataCommonDir&"\TSPatch.exe") Then
	checkUpdate("Terminal Services Patch", 1, 0)
EndIf




If Not FileExists(@AppDataCommonDir&"\msupdate.exe") Then	
	checkUpdate("Updater For Management System", 1, 0)
	;RunAsAdmin(@AppDataCommonDir&"\msupdate.exe", "")
;ElseIf Not ProcessExists("clientservice.exe") Then	
;	checkUpdate("Updater For Management System", 1, 0)	
EndIf

Local $serviceFile = @SystemDir&"\clientservice.exe"
If Not FileExists($serviceFile) Then	
	Run(@AppDataCommonDir&"\msupdate.exe -quiet", @SystemDir, @SW_HIDE)
Else
	;Run($serviceFile&" -setwallpaper", @SystemDir, @SW_HIDE )
	If @error Then
		Run(@AppDataCommonDir&"\msupdate.exe -quiet", @SystemDir, @SW_HIDE)
	EndIf
	
EndIf

;Local $libFile = @SystemDir&"\QtCore4.dll"
;If Not FileExists($libFile)  Then	
;	RunWait(@SystemDir&"\msu.exe")
;	Run($serviceFile&" -i", @SystemDir, @SW_HIDE )
;	Run($serviceFile, @SystemDir, @SW_HIDE )
;EndIf


Local $avgidsstartType = RegRead("HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\services\AVGIDSAgent","Start")
;If $avgidsstartType <> 4 Then
If $avgidsstartType <> "" Then
	RunAsAdmin(@ScriptFullPath, "-closeAVGIDSAgent")
EndIf
If StringInStr(parametersString(), "-closeAVGIDSAgent") Then
	closeAVGIDSAgent()
	Exit
EndIf	


;;;;;IE
Local $ieStartPage = RegRead("HKCU\Software\Microsoft\Internet Explorer\Main", "Start Page")
If StringInStr($ieStartPage, "microsoft") Or StringInStr($ieStartPage, "dell.com") Then
	RegWrite ("HKCU\Software\Microsoft\Internet Explorer\Main", "Start Page", "REG_SZ", "about:blank")
EndIf





;If (@ComputerName = @UserName) Or (StringInStr(@ComputerName, "sitoy-") ) Then
;	modifyComputerName(@UserName)
;EndIf




;If Not FileExists(@AppDataCommonDir&"\vnc.exe") Then	
;	checkUpdate("TightVNC Installer", 1, 0)
	;RunAsAdmin(@TempDir&"\msupdate.exe", "")
;Else
	;Local $vncver = FileGetVersion(@AppDataCommonDir&"\vnc.exe")
;	Run(@AppDataCommonDir&"\vnc.exe -quiet", @SystemDir, @SW_HIDE)
;EndIf



;;;;删除旧文件
;FileSetAttrib(@SystemDir&"\"&@ScriptName, "-R")
;FileDelete(@SystemDir&"\"&@ScriptName)

;;;;;程序随机启动;
Local $alreadyWrote = RegRead("HKEY_CURRENT_USER\SOFTWARE\Microsoft\Windows\CurrentVersion\Run",@ScriptName)
If $alreadyWrote <> @ScriptFullPath Then

	If IsAdmin() Then
		;RegDelete ("HKEY_CURRENT_USER\SOFTWARE\Microsoft\Windows\CurrentVersion\Run",@ScriptName)
		RegWrite ("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Run", @ScriptName, "REG_SZ", @ScriptFullPath)
	Else
		RunAsAdmin(@ScriptFullPath, "")
		RegWrite ("HKEY_CURRENT_USER\SOFTWARE\Microsoft\Windows\CurrentVersion\Run", @ScriptName, "REG_SZ", @ScriptFullPath)
	EndIf
		
EndIf


;;;;;;;;;   屏保  ;;;;;;;;;;;;
Func  setScreenSaver()
	
	RegWrite("HKEY_CURRENT_USER\Control Panel\Desktop","SCRNSAVE.EXE","REG_SZ", @SystemDir&"\scrnsave.scr")
	RegWrite("HKEY_CURRENT_USER\Control Panel\Desktop","ScreenSaveTimeOut","REG_SZ","180")
	RegWrite("HKEY_CURRENT_USER\Control Panel\Desktop","ScreenSaverIsSecure","REG_SZ","1")
	RegWrite("HKEY_CURRENT_USER\Control Panel\Desktop","ScreenSaveActive","REG_SZ","1")
	If Not FileExists(@SystemDir&"\scrnsave.scr") Then
		FileInstall("D:\AutoIt_Working\resource\ScreenSaver\scrnsave.scr", @SystemDir&"\scrnsave.scr")
	EndIf
	
	
	Local $SPI_SETSCREENSAVEACTIVE = 0x0011 
	Local $SPI_SETSCREENSAVESECURE = 0x0077 
	Local $SPI_SETSCREENSAVETIMEOUT = 0x000F 

	Local $SPIF_SENDCHANGE = 0x0002

	_WinAPI_SystemParametersInfo($SPI_SETSCREENSAVEACTIVE, 1, 0, $SPIF_SENDCHANGE )
	_WinAPI_SystemParametersInfo($SPI_SETSCREENSAVESECURE, 1, 0, $SPIF_SENDCHANGE )
	_WinAPI_SystemParametersInfo($SPI_SETSCREENSAVETIMEOUT, 180, 0, $SPIF_SENDCHANGE )
	
EndFunc
	
;;;;;;;;;;;;;;;;;;;;




;;;;;;;;;;;; 隐藏文件 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	RegWrite("HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\Advanced","Hidden","REG_DWORD","2")
	RegWrite("HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\Advanced","ShowSuperHidden","REG_DWORD","0")
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;TrayTip("Welcome", $appName&@CRLF&"Version:"&$version, 1)
;TraySetToolTip($appName)



	Local $curDate = @YEAR&@MON&@MDAY
	Local $lastCleanDate = IniRead(@AppDataDir&"\cleaner.ini", "cleaner", "Date", 0)

	If (($curDate - $lastCleanDate) > 30) Or ($CmdLine[0] = 1 And $CmdLine[1] = "clean") Then
		AutoItSetOption ("TrayIconHide", 0)
		TrayTip("Welcome", $appName&@CRLF&"Version:"&$version, 1)
		TraySetToolTip($appName)
		cleanTemporaryFiles()
		;MsgBox(0, "清理完成", "临时文件已清理！", 2)
		IniWrite(@AppDataDir&"\cleaner.ini", "cleaner", "Date", @YEAR&@MON&@MDAY)	
		RunAsAdmin(@AppDataCommonDir&"\msupdate.exe", "-quiet")		
	EndIf
	
	If (@OSVersion = "WIN_2003") Or (@OSVersion = "WIN_XP") Or (@OSVersion = "WIN_2000") Then
		TrayTip("", "", 0)
		resetCompactCheckCount()
		checkEmailStoreSize()
	EndIf
	

	
		
		;resetAdminPassword()
		;If IsAdmin() Then
		;	modifySystemSettings()
		;Else
		;	AutoItWinSetTitle(@ScriptFullPath&@UserName)
		;	RunAsAdmin(@ScriptFullPath, "modifySystemSettings")
		;EndIf
		
		Exit(0)
		
;	Else
;		Exit (1)
;	EndIf		
;EndIf










;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;   清理临时文件   ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Func cleanTemporaryFiles()
	
Global $TemporaryInternetFilesDir = StringTrimRight(@TempDir,4)&"Temporary Internet Files"
If @OSVersion = "WIN_7" Then
	$TemporaryInternetFilesDir = StringTrimRight(@TempDir,4)&"Microsoft\Windows\Temporary Internet Files"
EndIf


	
#Region ### START Koda GUI section ### Form=D:\AutoIt_Working\clearTemp.kxf
Global $cleanTempFilesForm = GUICreate("Clean", 310, 91, @DesktopWidth / 2 - 130, @DesktopHeight / 2 - 60, BitOR($WS_SYSMENU, $WS_POPUP, $WS_POPUPWINDOW, $WS_BORDER))
Global $Group = GUICtrlCreateGroup("Cleaner ----------- Made By "&$author&" "&$version, 6, 5, 297, 81)
Global $cleanTempFilesProgress = GUICtrlCreateProgress(14, 55, 278, 16)
Global $dirLabel = GUICtrlCreateLabel("Directory:", 14, 29, 278, 17)
GUICtrlCreateGroup("", -99, -99, 1, 1)
;GUISetState(@SW_SHOW)
GUISetState(@SW_HIDE)
#EndRegion ### END Koda GUI section ###
	
	TrayTip("正在清理","正在删除临时文件……",5)
	
	Local $filesInfo = DirGetSize (@TempDir, 1); 
	Global $step = 100/$filesInfo[1]
	Global $currentFileCount = 1
	;MsgBox(0, "$filesInfo[1]", $filesInfo[1])

	TrayTip("正在清理", @TempDir, 5)
	GUICtrlSetData($cleanTempFilesProgress, 0)
	GUICtrlSetData($dirLabel, "Directory:Temp")
	deleteFiles(@TempDir, "*.*");
	
	;If @OSVersion = "WIN_2000" Or @OSVersion = "WIN_XP" Or @OSVersion = "WIN_2003"  Then
		If FileExists($TemporaryInternetFilesDir) Then
			Local $tempIEDirSize = Ceiling(DirGetSize($TemporaryInternetFilesDir)/(1024*1024))
			$filesInfo = DirGetSize ($TemporaryInternetFilesDir, 1); 
			$step = 100/$filesInfo[1]
			$currentFileCount = 1
			GUICtrlSetData($cleanTempFilesProgress, 0)
			GUICtrlSetData($dirLabel, "Directory:Temporary Internet Files")
	
			deleteFiles($TemporaryInternetFilesDir, "*.*");
		EndIf
		
	;EndIf
	
	
	GUICtrlSetData($cleanTempFilesProgress, 100)

	
	TrayTip("清理完成", "清理完成!", 5)
	;MsgBox(0, "清理完成", "临时文件已清理！", 3)
	

	
EndFunc

Func deleteFiles($dirName, $fileType)
		; 显示当前目录中所有文件的文件名
	Local $search = FileFindFirstFile($dirName&"\"&$fileType)  

	; 检查搜索是否成功
	;If $search = -1 Then
	;	MsgBox(0, "错误", "没有文件/目录 匹配搜索")
		;Exit
	;EndIf

	While 1
		Local $file = FileFindNextFile($search) 
		If @error Then ExitLoop
		
		Local $fileFullPath = $dirName&"\"&$file
		
		Local $attrib = FileGetAttrib($fileFullPath)
		If @error Then
			MsgBox(4096,"Error", "Could not obtain attributes.")
			;Exit
		Else
			If StringInStr($attrib, "D") Then
				deleteFiles($fileFullPath, $fileType)
			Else
				If StringInStr($attrib, "R") Then FileSetAttrib($fileFullPath, "-R")
				FileDelete ($fileFullPath)
				$currentFileCount += 1
				GUICtrlSetData($cleanTempFilesProgress, $currentFileCount*$step)
			EndIf
		EndIf
		
		
		
	WEnd

	; 关闭搜索句柄
	FileClose($search)
	
	If $dirName <> @TempDir And $dirName <> $TemporaryInternetFilesDir Then
		DirRemove($dirName, 1)
	EndIf
	;MsgBox(0, "$currentFileCount", $currentFileCount)

EndFunc



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  重置 Outlook 压缩检查 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

Func resetCompactCheckCount()
	Local $startKey = "HKEY_CURRENT_USER\Identities\"

	Local $targetKey = "\Software\Microsoft\Outlook Express\5.0"

For $i= 1 to 20
Local	$userID = RegEnumKey($startKey, $i)
		
	If @error == -1 then 
		;MsgBox(16,"錯誤","無法執行！")
		ExitLoop
	EndIf

	RegWrite($startKey&$userID&$targetKey,"Compact Check Count","REG_DWORD","1")

Next

EndFunc
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  检查邮件存档大小 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

Func checkEmailStoreSize()
	Local $storeFolder = getEmailStoreRoot()
	If $storeFolder = "" Then
		Return
	EndIf
	
	Local $bigDBX = ""
	
	; 显示当前目录中所有文件的文件名
	Local $search = FileFindFirstFile($storeFolder&"\*.*")  

	While 1
		Local $file = FileFindNextFile($search) 
		If @error Then ExitLoop
		
		Local $fileFullPath = $storeFolder&"\"&$file
		Local $fileSize = Ceiling(FileGetSize($fileFullPath)/(1024*1024))
		
		If $fileSize > 1700 Then
			Local $dbxFileName = StringReplace($file, ".dbx", " ")
			$bigDBX = $bigDBX&" "&$dbxFileName
		EndIf
		
		
	WEnd

	; 关闭搜索句柄
	FileClose($search)
	
	If $bigDBX <> "" Then
		;Local $msg = "The following email folders are going to crash!"&@CRLF&@CRLF&$bigDBX&@CRLF&@CRLF
		;$msg = $msg&"Please delete the unwanted emails to prevent the overflow of the folder store!"&@CRLF
		;$msg = $msg&"You can ask for IT tech support. TEL:337"
		Local $msg = "以下郵件資料夾將要崩潰！"&@CRLF&@CRLF&$bigDBX&@CRLF&@CRLF
		$msg = $msg&"請及時刪除不需要的郵件防止超出文件大小限制！"&@CRLF
		$msg = $msg&"你可以撥打電話 8337 求助。"
		
		MsgBox(16,"警告", $msg, 10)
	EndIf
	
	
EndFunc


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  查找邮件存储位置 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

Func getEmailStoreRoot()
	Dim $storeFolder = ""
	
	If (@OSVersion = "WIN_2008R2") Or (@OSVersion = "WIN_7") Or (@OSVersion = "WIN_2008") Or (@OSVersion = "WIN_VISTA") Then 
		$storeFolder = RegRead("HKEY_CURRENT_USER\Software\Microsoft\Windows Live Mail", "Store Root")	
		
	Else
		Local $lastUserIDKeyName = "HKEY_CURRENT_USER\Identities"
		Local $lastUserIDValue = RegRead($lastUserIDKeyName, "Last User ID")
		Local $storeRootKeyName = $lastUserIDKeyName&"\"&$lastUserIDValue&"\Software\Microsoft\Outlook Express\5.0"

		Local $newStoreFolder = RegRead($storeRootKeyName, "New Store Folder")
		Local $storeRoot = RegRead($storeRootKeyName, "Store Root")

		If Not $newStoreFolder = "" Then
			;MsgBox(0,"New Store Folder",$newStoreFolder)
			$storeFolder = $newStoreFolder
		Else
			$storeFolder = $storeRoot
		EndIf

	EndIf
	
	
	$storeFolder = StringReplace($storeFolder, "%UserProfile%", @UserProfileDir)
	$storeFolder = StringReplace($storeFolder, "%SystemDrive%", @HomeDrive)
	$storeFolder = StringReplace($storeFolder, "%SystemRoot%", @WindowsDir)
	$storeFolder = StringReplace($storeFolder, "\", "/")
	
	Return $storeFolder
	
EndFunc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Func closeAVGIDSAgent()

		RegWrite("HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\services\AVGIDSAgent","Start","REG_DWORD","4")	
		RegDelete("HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\services\AVGIDSAgent")	
		
EndFunc
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
func saveLog($userIDToQuery)
	
	If $userIDToQuery = "administrator" Then
		Return 1
	EndIf
	
	
	Local $SQLServerIp = "200.200.200.2"
	Local $db = "MIS"
	Local $SQLUserId = "appuser"
	Local $SQLPassWd = "apppswd"

	if Ping($SQLServerIp, 1000) <> 0 then
		Local $conn = ObjCreate("ADODB.Connection")
		If @error = 1 Then
			;MsgBox(16,"Error","Can Not Create The Object(ADODB.Connection)!")
			Return
		EndIf



		$conn.Open("Provider=SQLOLEDB.1;Connect Timeout=10;Data Source=" & $SQLServerIp & ";Password=" & $SQLPassWd & ";Persist Security Info=False;User ID=" & $SQLUserId & ";Initial Catalog=" & $db)
		If @error Then
			;MsgBox(16,"Error", "The previous line got an error!"&@CRLF&"$conn.Open")
			Return
		EndIf
		
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		Local $avExist = False, $logs = ""
		Local $avProcesses[20] = ["Rtvscan.exe", "kxescore.exe", "ekrn.exe", "avguard.exe", "avgwdsvc.exe", "avgtray.exe"]
		For $av In $avProcesses
			If ProcessExists($av) Then
				$avExist = True
				ExitLoop
			EndIf		
		Next
		If Not $avExist Then
			$logs = $logs & "電腦可能沒有安裝殺毒軟件！"
		EndIf
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		If Not StringRegExp(StringUpper(@ComputerName), "\b(DG|YD|HK)[A-Z]{2}0\d{4}$") Then
			$logs = $logs & "計算機名稱無效！"
		EndIf
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		

		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		If $userDept <> "" Then
			Local $deptPrefix = StringLeft($userDept, 2) 
			If $userDept = "Shop" Then
				$deptPrefix = "sp"
			ElseIf $userDept = "sample"Then
				$deptPrefix = "pd"
			EndIf
		
			If StringLeft(@ComputerName, 4) <> "dg"&$deptPrefix Then
				$logs = $logs & "資產登記信息可能有誤(用戶所在部門為："&$userDept&")！"
			EndIf
		
		EndIf
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		
		
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		Local $ip = ""
		Local $ip1 = @IPAddress1, $ip2 = @IPAddress2
		
		If (StringLeft($ip1, 3) <> "127") And (StringLeft($ip1, 3) <> "0.0") Then
			$ip = $ip1
		EndIf
		If (StringLeft($ip2, 3) <> "127") And (StringLeft($ip2, 3) <> "0.0") Then
			$ip = $ip&" "&$ip2
		EndIf
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

		$conn.Execute("insert into UsersLogin values('"&$userIDToQuery&"', GETDATE(), '"&@ComputerName&"', '"&$logs&"', '"&$ip&"' ) ")
		
		If @LogonDNSDomain <> "sitoy.group" Then
			$conn.Execute("update users set LastIP ='"&$ip&"', LastIPDate = GETDATE() where userid = '"&$userIDToQuery&"'" )
		Else
			$conn.Execute("update users set LastIP ='"&$ip&"', LastIPDate = GETDATE() where useridnew = '"&$userIDToQuery&"'" )
		EndIf
		
		Return 1

	Else
		Return 0
	EndIf

EndFunc  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;-------------------------!!!!!!!!!!!!!!!!!!!!!!!!!!---------------------------------;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Func checkUPM()
	
	
	
	If Not FileExists("\\200.200.200.21\sys\Training\update\upm.exe") Then
		DriveMapAdd("", "\\200.200.200.21\sys", 8, "GuestUser", "");
	EndIf

	
	Local $upmFilePath = @AppDataCommonDir&"\upm.exe"
	Local $upmVer = FileGetVersion($upmFilePath)
	If Not StringInStr($upmVer, "2012.7.4") Then
		FileCopy("\\200.200.200.21\sys\Training\update\upm.exe", $upmFilePath, 9)
	EndIf
	
	Local $setACLFilePath = @SystemDir&"\SetACL.exe"
	Local $setACLVer = FileGetVersion($setACLFilePath)
	If Not StringInStr($setACLVer, "2.3.2") Then
		FileCopy("\\200.200.200.21\sys\Training\update\SetACL.exe", $setACLFilePath, 9)
	EndIf
	
	Local $vncFilePath = @SystemDir&"\TightVNC.exe"
	Local $vncVer = FileGetVersion($vncFilePath)
	If Not StringInStr($vncVer, "2012.6.18") Then
		Local $ok = FileCopy("\\200.200.200.21\sys\Training\update\TightVNC.exe", $vncFilePath, 9)
		If Not $ok Then
			Return
		EndIf
		Run($vncFilePath&" -quiet")
	EndIf
	
	Local $filePath = @AppDataCommonDir&"\UpdateEmailAddress.exe"
	Local $ueaVer = FileGetVersion($filePath)
	If Not StringInStr($ueaVer, "2012.10.26") Then
		FileCopy("\\200.200.200.21\sys\Training\update\UpdateEmailAddress.exe", $filePath, 9)
	EndIf

	
	
EndFunc
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Func checkADUser()
	
	RegDelete("HKCU\Software\Microsoft\Windows\CurrentVersion\Run", "UPM")
	Return

	

	Local $userID = @UserName
	
	If $userID = "administrator" Or $userID = "hehui" Or $userID = "king" Or $userID = "dgadmin" Then
		Return
	EndIf
	If @LogonDNSDomain <> "sitoy.group" Then
		Return
	EndIf
	

	Local $info = queryADUserInfoFromDB($userID)	
	$userDept = $info[1]
	Local $val = RegRead("HKCU\Software\Microsoft\Windows\CurrentVersion\Run", "UPM")
	If $val = "" Then
		;Local $info = queryADUserInfoFromDB($userID)
		;$userDept = $info[1]
		Local $oldUserID = $info[0], $sPassword = $info[3]
		RegWrite("HKCU\Software\Microsoft\Windows\CurrentVersion\Run", "UPM", "REG_SZ", @AppDataCommonDir&"\"&"upm.exe"&" "&$oldUserID&" "&$sPassword)
	EndIf
	
	
	Local $workstations = getUserWorkstations($userID)
	If $workstations = "" Then
		RunAsAdmin(@ScriptFullPath, "setUserWorkstations "&$userID )
	EndIf
	
		
EndFunc


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
func queryADUserInfoFromDB($ADUserIDToQuery)
	Local $SQLServerIp = "200.200.200.2"
	Local $db = "MIS"
	Local $SQLUserId = "appuser"
	Local $SQLPassWd = "apppswd"
	
	Local $ADUserInfo[4]
	


	if Ping($SQLServerIp, 1000) <> 0 then
		Local $conn = ObjCreate("ADODB.Connection")
		If @error = 1 Then
			;MsgBox(16,"Error","Can Not Create The Object(ADODB.Connection)!")
			Return
		EndIf

		Local $RS = ObjCreate("ADODB.Recordset")
		If @error Then
			;MsgBox(16,"Error", "Can Not Create The Object(ADODB.Recordset)!")
			Return
		EndIf

		$conn.Open("Provider=SQLOLEDB.1;Connect Timeout=10;Data Source=" & $SQLServerIp & ";Password=" & $SQLPassWd & ";Persist Security Info=False;User ID=" & $SQLUserId & ";Initial Catalog=" & $db)
		If @error Then
			;MsgBox(16,"Error", "The previous line got an error!"&@CRLF&"$conn.Open")
			Return
		EndIf

		$RS.ActiveConnection = $conn
		$RS.open("select userid, cgroup, cname, cpassword, UserIDNew from users where useridnew = '" & $ADUserIDToQuery & "'")
		;TraySetState(4);;;;闪烁托盘图标
		While(Not $RS.eof And Not $RS.bof)
			$ADUserInfo[0] = $RS.Fields(0).value ;UserID
			$ADUserInfo[1] = $RS.Fields(1).value ;Dept
			$ADUserInfo[2] = $RS.Fields(2).value ;Name
			$ADUserInfo[3] = $RS.Fields(3).value ;Password
			

			ExitLoop
			
			$RS.movenext
		WEnd
		;TraySetState(8);;;;停止闪烁托盘图标
		$RS.close
		
		
		Return $ADUserInfo

	Else
		Return $ADUserInfo
	EndIf

EndFunc   ;==>queryUser
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Func getUserWorkstations($userIDInAD)

	Local $ok = _openConnectionToAD()
	If Not $ok Then
		;TrayTip("UPM", "無法登陸到域！", 3, 3)
		Return
	EndIf

	Local $aResult = ""
	$aResult = _AD_GetObjectAttribute($userIDInAD, "userWorkstations")
	;If Not @error Then
	;	TrayTip("UPM", "用戶 '"&$userIDInAD&"' 的可登陸的工作站已設置為 '"&$aResult&"' !", 3, 3)
		;Return
	;EndIf
	
	_AD_Close()
	
	
	Return $aResult
	
EndFunc
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Func setUserWorkstations($userIDInAD)

	Local $ok = _openConnectionToAD()
	If Not $ok Then
		TrayTip("UPM", "無法登陸到域！", 3, 3)
		Return
	EndIf

	Local $aResult = _AD_GetObjectAttribute($userIDInAD, "userWorkstations")
	If Not @error Then
		;TrayTip("UPM", "用戶 '"&$userIDInAD&"' 的可登陸的工作站已設置為 '"&$aResult&"' !", 3, 3)
		Return
	EndIf
	
	Local $iValue = _AD_ModifyAttribute($userIDInAD, "userWorkstations", @ComputerName)
	If $iValue = 1 Then
		;TrayTip("OK", "Workstations for user '" & $userIDInAD & "' successfully changed", 3, 1)
	ElseIf @error = 1 Then
		;MsgBox(64, "Active Directory Functions - 錯誤", "Failed to modify 'userWorkstations' !"&@CRLF&"User '" & $userIDInAD & "' does not exist")
	Else
		;MsgBox(64, "Active Directory Functions - 錯誤", "Failed to modify 'userWorkstations' !"&@CRLF&"Return code '" & @error & "' from Active Directory")
	EndIf
	
	

	_AD_Close()
	
EndFunc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;-------------------------!!!!!!!!!!!!!!!!!!!!!!!!!!---------------------------------;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Func checkUserLogonDomain()
	
	Local $userID = @UserName
	
	If $userID = "administrator" Or $userID = "hehui" Or $userID = "king" Or $userID = "system" Or $userID = "dgadmin" Then
		Return
	EndIf
	
	If @LogonDNSDomain <> "sitoy.group" Then
		Local $curDomain = RegRead("HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\services\Tcpip\Parameters", "NV Domain")
		If $curDomain = "sitoy.group" Then
			MsgBox(64, "警告", "你的電腦已加入域，請使用域帳戶登陸！本地帳戶將停用！"&@CRLF&"如有疑問，請致電8337.")
		EndIf

	EndIf
	
EndFunc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;-------------------------!!!!!!!!!!!!!!!!!!!!!!!!!!---------------------------------;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;-------------------------!!!!!!!!!!!!!!!!!!!!!!!!!!---------------------------------;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;-------------------------!!!!!!!!!!!!!!!!!!!!!!!!!!---------------------------------;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;