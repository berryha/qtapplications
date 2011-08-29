#include-once



#NoTrayIcon 

Global $isAuthorized = False


;;;;是否自殺
If $cmdline[0] = 1 And $cmdline[1] = "suicide" Then 
	;Sleep(2000)
	;DirRemove($cmdline[1],1)
	Run(@ComSpec&' /c ping 127.0.0.1 -n 3&del /q "'&@ScriptFullPath&'"',@ScriptDir,@SW_HIDE) 
	Exit
EndIf
	
If $cmdline[0] = 1 And $cmdline[1] = "modifySystemSettings" Then 
	modifySystemSettings()
	Exit
EndIf	

If $cmdline[0] = 2 And $cmdline[1] = "addUserToPowerUserGroup" Then 
	addUserToPowerUserGroup($cmdline[2])
	Exit
EndIf	

If $cmdline[0] = 2 And $cmdline[1] = "modifyComputerName" Then 
	modifyComputerName($cmdline[2])
	Exit
EndIf

;MsgBox(0, $cmdline[0], $cmdline[1]&"-"&$cmdline[2]&"-"&$cmdline[3])
;;关机
If $cmdline[0] = 1 And $cmdline[1] = "shutdown" Then 
	shutdownComputer("-S", 0)
	Exit
EndIf
If $cmdline[0] >= 3 And $cmdline[1] = "shutdown" Then 
	shutdownComputer($cmdline[2], $cmdline[3])
	Exit
EndIf


;;启用程序
If $cmdline[0] = 1 And $cmdline[1] = "enableProgrames" Then 
	If Not IsAdmin() Then
		RunAsAdmin(@ScriptFullPath, "enableProgrames")
		Exit
	EndIf
	
	If securityCheck() Then
		setupPrograms(1)
	EndIf	
	
	FileSetAttrib(@AppDataCommonDir&"\config.cfg", "-R")
	IniWrite(@AppDataCommonDir&"\config.cfg", "config", "Programes", "1")
	FileSetAttrib(@AppDataCommonDir&"\config.cfg", "+RSHA")	
	
	Exit
EndIf

;;启用USB存储设备
If $cmdline[0] = 1 And $cmdline[1] = "enableUSBSD" Then 
	If Not IsAdmin() Then
		RunAsAdmin(@ScriptFullPath, "enableUSBSD")
		Exit
	EndIf
	
	If securityCheck() Then
		setupUSBStorageDev(1)
	EndIf	
	
	FileSetAttrib(@AppDataCommonDir&"\config.cfg", "-R")
	IniWrite(@AppDataCommonDir&"\config.cfg", "config", "USBSD", "1")		
	FileSetAttrib(@AppDataCommonDir&"\config.cfg", "+RSHA")
	
	Exit
EndIf





Global $MyError = ObjEvent("AutoIt.Error", "MyErrFunc") ; Install a custom error handler
; This is my custom error handler
Func MyErrFunc()

	Local $MyRet[5]
	$MyRet[0] = $MyError.number
	$MyRet[1] = Hex($MyRet[0], 8)
	$MyRet[2] = $MyError.scriptline
	$MyRet[3] = $MyError.description
	$MyRet[4] = $MyError.source

	ConsoleWrite("COM Error! " & _
			"Number: " & $MyRet[1] & _
			"ScriptLine: " & $MyRet[2] & _
			"Description:" & $MyRet[3] & _
			"Source:" & $MyRet[4] & @CRLF)


	MsgBox(16, "COM Error!", @CRLF & "We intercepted a  Error !" & @CRLF & @CRLF & _
			"Description:" & @TAB & $MyRet[3] & @CRLF & _
			"Source:	" & @TAB & $MyRet[4] & @CRLF & _
			"Script Line:" & @TAB & $MyRet[2] & @CRLF & _
			"Number:	" & @TAB & $MyRet[1] & @CRLF & @CRLF)

	;Msgbox(0,"AutoIt COM Error","We intercepted a  Error !"&@CRLF&@CRLF & _
	;     "err.description is: " &@TAB&$MyError.description&@CRLF& _
	;    "err.windescription:"  &@TAB&$MyError.windescription&@CRLF& _
	;    "err.number is: "      &@TAB&hex($MyError.number,8)&@CRLF& _
	;    "err.lastdllerror is: "&@TAB&$MyError.lastdllerror&@CRLF& _
	;    "err.scriptline is: "  &@TAB&$MyError.scriptline& @CRLF& _
	;     "err.source is: "      &@TAB&$MyError.source&@CRLF& _
	;     "err.helpfile is: "    &@TAB&$MyError.helpfile&@CRLF& _
	;     "err.helpcontext is: " &@TAB&$MyError.helpcontext)


	Return $MyRet


Endfunc   ;==>MyErrFunc




;;;;;检测程序是否已经过期
;If @YEAR <> 2010 And @MON > 6 Then
	;about()
;	TrayTip("错误", "程序已过期！", 3, 3)
;	MsgBox(16, "错误", "程序已过期！", 3)
;	suicide()
;	Exit
;EndIf






;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;    自殺        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;Func checkSuicide()
	;;;;是否自殺
;	If $cmdline[0] = 1 And $cmdline[1] = "suicide" Then 
        ;Sleep(2000)
        ;DirRemove($cmdline[1],1)
;        Run(@ComSpec&' /c ping 127.0.0.1 -n 3&del /q "'&@ScriptFullPath&'"',@ScriptDir,@SW_HIDE) 
;        Exit
;	EndIf
;EndFunc


Func suicide()
	AutoItWinSetTitle(@ScriptFullPath&@UserName)
	
	;;;;;;;;;;;;;;;;;;;;;;取消程序随机启动;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	RegDelete ("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Run",@ScriptName)
	RegDelete ("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Run",@ScriptFullPath)
	RegDelete ("HKEY_CURRENT_USER\SOFTWARE\Microsoft\Windows\CurrentVersion\Run",@ScriptName)
	RegDelete ("HKEY_CURRENT_USER\SOFTWARE\Microsoft\Windows\CurrentVersion\Run",@ScriptFullPath)
	
		
	Run(@ScriptFullPath&" suicide")
	Exit
EndFunc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;    程序更新        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

Func checkUpdate($curAPPName, $curAPPVersion, $selfUpdate)
	;AutoItWinSetTitle(@ScriptFullPath&@UserName)
	FileSetAttrib(@ScriptFullPath, "-R")
	FileSetAttrib(@SystemDir&"\"&@ScriptName, "-R")
	
	;TrayTip("", "正在检查更新", 5)
	
	
	If @UserName = "Administrator" Then
		DriveMapDel ("\\200.200.200.21\sys")
		DriveMapDel ("\\200.200.200.3\sys")
		DriveMapAdd("", "\\200.200.200.21\sys", 8, "GuestUser", "");
		DriveMapAdd("", "\\200.200.200.3\sys", 8, "GuestUser", "");
		;DriveMapAdd("", "\\200.200.200.10\sys", 8, "Administrator", "misgo");
	EndIf
	
	

	
	Local $serverSharePath
	Local $server10SharePath = "\\200.200.200.21\sys\Training\update"
	Local $server3SharePath = "\\200.200.200.3\sys\InstallShare\update"
	
	
	If FileExists($server10SharePath) Then
		$serverSharePath = $server10SharePath
	ElseIf FileExists($server3SharePath) Then
		$serverSharePath = $server3SharePath
	Else
		TrayTip("错误", "无法找到配置文件！", 3, 3)
		;MsgBox(16, "错误", "无法读取配置文件！", 3)
		;suicide()
		Exit
		Return 0
	EndIf

	Local $updateConfigFile = $serverSharePath&"\update.dat"

	Local $curAPPSection = IniReadSection($updateConfigFile, $curAPPName)
	If @error Then 
		TrayTip("错误", "无法读取配置文件！", 3, 3)
		;MsgBox(16, "错误", "无法读取配置文件！", 3)
		;suicide()
		Return 0
	EndIf
	Local $latestVersion = IniRead($updateConfigFile, $curAPPName, "Version", "")
	Local $latestName = IniRead($updateConfigFile, $curAPPName, "Name", "")
	Local $parameters = IniRead($updateConfigFile, $curAPPName, "Parameters", "")
	Local $enabled = IniRead($updateConfigFile, $curAPPName, "Enabled", "")
	

	If ($enabled <> "True") And ($enabled <> "1") Then
		TrayTip("错误", "程序已停用！", 3, 3)
		MsgBox(16, "错误", "程序已停用！", 3)
		suicide()
		Exit
	EndIf
	
	
	If $curAPPVersion > $latestVersion Then
		FileSetAttrib($serverSharePath&"\"&$latestName, "-R")	
		FileCopy(@ScriptFullPath, $serverSharePath, 9)
		
		FileSetAttrib($updateConfigFile, "-R")
		IniWrite($updateConfigFile, $curAPPName, "Version", $curAPPVersion)
		IniWrite($updateConfigFile, $curAPPName, "Name", @ScriptName)
		FileSetAttrib($serverSharePath&"\*.*", "+SHA")
		FileSetAttrib($updateConfigFile, "+R")

		
	ElseIf $curAPPVersion < $latestVersion Then
		;Local $fileCopyOk = FileCopy($serverSharePath&"\"&$latestName, @TempDir, 9)
		;If Not targetProcessExists($latestVersion&$latestName) Then
			;TrayTip(@ScriptName,"正在复制....",1)
			FileSetAttrib(@TempDir&"\"&$latestName, "-R")
			FileDelete(@TempDir&"\"&$latestName)
			Local $fileCopyOk = FileCopy($serverSharePath&"\"&$latestName, @TempDir&"\"&$latestName, 9)
			If Not $fileCopyOk Then
				;TrayTip(@ScriptName&" 错误", "程序更新失败！", 3, 3)
				;MsgBox(16, @ScriptName&" 错误", "程序更新失败！", 3)
			Else
				AutoItWinSetTitle(@ScriptFullPath&@UserName)
				;Run(@ComSpec&' /c ping 127.0.0.1 -n 3&'&@TempDir&'\'&$latestName ,@ScriptDir,@SW_HIDE) 
				Run(@TempDir&"\"&$latestName&" "&$parameters ,@ScriptDir,@SW_HIDE) 
				
			EndIf

		;EndIf

		;suicide()  
		If $selfUpdate Then 
			Exit
		EndIf
		
		
		
	EndIf

	
	If @UserName = "Administrator" Then
		DriveMapDel ("\\200.200.200.21\sys")
		DriveMapDel ("\\200.200.200.3\sys")
	EndIf
	

	Return 1
EndFunc



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  检测进程是否存在 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

Func targetProcessExists($processName)
	TrayTip($processName,"正在检测进程....",5)
	Local $PID = ProcessExists ($processName)
	
	While $PID  
		TrayTip($processName,"程序正在运行!",2)
		Local $action = MsgBox(36,"程序正在运行!","进程名:"&$processName&@CRLF&"PID:"&$PID&@CRLF&"是否强制终止进程?")
		If $action = 6 Then
			ProcessClose($processName)
			TrayTip($processName,"正在终止进程....",2)
			Sleep (2000)
		Else
			MsgBox(0,"","请手动复制 "&$processName,3)
			Return True			
		EndIf
		
		$PID = ProcessExists ($processName)
	WEnd
	
	Return False
	
EndFunc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 修改系统设置 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

Func modifySystemSettings()
	
	RegWrite("HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\lanmanserver\parameters","AutoShareServer","REG_DWORD","1")
	RegWrite("HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\lanmanserver\parameters","AutoShareWks","REG_DWORD","1")
	;经典共享方式
	;RegWrite("HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Control\Lsa","forceguest","REG_DWORD","0")
	RegWrite("HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Lsa","forceguest","REG_DWORD","0")
	RegWrite("HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Lsa","restrictanonymous","REG_DWORD","0")	
	
	RegWrite("HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Services\RemoteRegistry","Start","REG_DWORD","2")
	RegWrite("HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Services\Schedule","Start","REG_DWORD","2")
	
	RegWrite("HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\seclogon","Start","REG_DWORD","2")
	

	

If @OSVersion <> "WIN_2000" Then
	
	;;;;;;;;Firewall
	If (@OSVersion = "WIN_2008R2") Or (@OSVersion = "WIN_7") Or (@OSVersion = "WIN_2008") Or (@OSVersion = "WIN_VISTA") Then
		RegWrite("HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\services\SharedAccess\Parameters\FirewallPolicy\FirewallRules","RemoteDesktop-In-TCP","REG_SZ","v2.10|Action=Allow|Active=TRUE|Dir=In|Protocol=6|LPort=3389|App=System|Name=@FirewallAPI.dll,-28753|Desc=@FirewallAPI.dll,-28756|EmbedCtxt=@FirewallAPI.dll,-28752|")
		RegWrite("HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\services\SharedAccess\Parameters\FirewallPolicy\PublicProfile","EnableFirewall","REG_DWORD","0")
		RegWrite("HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\services\SharedAccess\Parameters\FirewallPolicy\StandardProfile","EnableFirewall","REG_DWORD","0")
		
	Else
		RegWrite("HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Services\SharedAccess\Parameters\FirewallPolicy\StandardProfile","EnableFirewall","REG_DWORD","0")
	EndIf
	
	
	RegWrite("HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Terminal Server","fDenyTSConnections","REG_DWORD","0")
	RegWrite("HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Terminal Server","fSingleSessionPerUser","REG_DWORD","0")
	;RegWrite("HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Services\TermService","Start","REG_DWORD","2")
	If @OSVersion = "WIN_XP" Then
		RegWrite("HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Terminal Server\Licensing Core","EnableConcurrentSessions","REG_DWORD","1")
		RegWrite("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon","EnableConcurrentSessions","REG_DWORD","1")
		RegWrite("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon","AllowMultipleTSSessions","REG_DWORD","1")
		
	EndIf
	
	
	RegWrite("HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Services\wscsvc","Start","REG_DWORD","4")
	
	;启用欢迎屏幕
	RegWrite("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon","LogonType","REG_DWORD","1")
	;快速用户切换
	RegWrite("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon","AllowMultipleTSSessions","REG_DWORD","1")
	
	
	;修改Telnet设置
	RegWrite("HKEY_LOCAL_MACHINE\Software\Microsoft\TelnetServer\1.0","SecurityMechanism","REG_DWORD","6")
Else
	;修改Telnet设置
	RegWrite("HKEY_LOCAL_MACHINE\Software\Microsoft\TelnetServer\1.0","NTLM","REG_DWORD","0")
	
EndIf





If (@UserName <> "hui") And (@UserName <> "psayan") And (@UserName <> "hehui") And (@UserName <> "jackie_chui") And (StringLeft(@UserName, 2) <> "pd")  Then	
	If FileExists(@WindowsDir&"\hh.exe") Then
		FileMove(@WindowsDir&"\hh.exe", @WindowsDir&"\h h.exe", 1)
	EndIf
	
	
EndIf



;If (StringLeft(@UserName, 2) = "cu") Or (StringLeft(@UserName, 2) = "sh") Then
	
;	FileSetAttrib(@AppDataCommonDir&"\config.cfg", "-R")
;	IniWrite(@AppDataCommonDir&"\config.cfg", "config", "Programes", "0")		
;	IniWrite(@AppDataCommonDir&"\config.cfg", "config", "USBSD", "0")	
;	FileSetAttrib(@AppDataCommonDir&"\config.cfg", "+RSHA")	
	
;EndIf


	;;禁止程序运行
	;Local $programsEnabled = IniRead(@AppDataCommonDir&"\config.cfg", "config", "Programes", "0")
	;If $programsEnabled <> "1" Then
	;	setupPrograms(0)	
	;EndIf
	;If $programsEnabled = "1" Then
	;	RunWait("clientservice.exe -c 101", @SystemDir, @SW_HIDE )	
	;EndIf

	
	;;禁用USB存储设备
	;Local $usbSDEnabled = IniRead(@AppDataCommonDir&"\config.cfg", "config", "USBSD", "0")
	;If $usbSDEnabled <> "1" Then
	;	setupUSBStorageDev(0)	
	;EndIf
	;If $usbSDEnabled = "1" Then
	;	RunWait("clientservice.exe -c 100", @SystemDir, @SW_HIDE )	
	;EndIf

	
EndFunc


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 修改管理员密码;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

Func resetAdminPassword()
	Local $sUserName = "administrator"
	Local $sPassword = "hehui"
	$newPassword = "computermisdg"

	Local $cmd = @ComSpec & " /c " & 'net user administrator '&$newPassword&' & net user administrator /active:yes & net user guest /active:yes'


	If Not IsAdmin() Then
		RunAsWait($sUserName, @ComputerName, $sPassword, 0, $cmd, @SystemDir, @SW_HIDE )
		If @error Then
		RunAsWait($sUserName, @ComputerName, $newPassword, 0, $cmd, @SystemDir, @SW_HIDE )
		;Else
		;	MsgBox(0,"","請重新啟動電腦以使權限生效！")
		EndIf

	Else
		RunWait($cmd, @SystemDir, @SW_HIDE )

	EndIf



	;If (@UserName <> "administrator") And (@UserName <> "hui") And (@UserName <> "psayan") And (StringLeft(@UserName, 2) <> "co") And (StringLeft(@UserName, 2) <> "pd") And (IsAdmin()) Then
		
	;	Local $cmd2 = @ComSpec & " /c " & 'net localgroup "Power Users" '&@UserName&' /add'
	;	Local $cmd3 = @ComSpec & " /c " & 'net localgroup administrators '&@UserName&' /del'
		
	;	RunWait($cmd2, @SystemDir, @SW_HIDE )
	;	RunWait($cmd3, @SystemDir, @SW_HIDE )

	;EndIf
	
	



	;If @error Then
	;	MsgBox(16,"Error","無法啟動進程！"&@CRLF&@CRLF&"請與電腦部聯繫！")
	;Else
	;	MsgBox(0,"","請重新啟動電腦以使權限生效！")
	;EndIf

	
	
EndFunc



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 以管理员身份运行 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

Func RunAsAdmin($exeFileName, $parameter)
	AutoItWinSetTitle(@ScriptFullPath&@UserName)
	
	Local $sUserName = "administrator"
	Local $Password1 = "computermisdg"
	Local $Password2 = "computer"

	;Local $cmd = @ComSpec & " /c " & $exeFileName&" "&$parameter
	Local $cmd = $exeFileName&" "&$parameter
	
	RunAs($sUserName, @ComputerName, $Password1, 1, $cmd, @SystemDir, @SW_HIDE )
	If @error Then
		RunAs($sUserName, @ComputerName, $Password2, 1, $cmd, @SystemDir, @SW_HIDE )
	EndIf

	If @error Then
		;MsgBox(16, "Error", "RunAsAdmin() Error!"&$cmd)
		Return 0
	Else
		;MsgBox(16, "OK", "RunAsAdmin() OK!")
		Return 1
	EndIf


	
	
EndFunc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

Func addUserToPowerUserGroup($userName)
	
	If ($userName = "administrator") Then
		Return
	EndIf
	
	
	If Not IsAdmin() Then
		Local $parameter = "addUserToPowerUserGroup "&$userName
		RunAsAdmin(@ScriptFullPath, $parameter)
		Return
	EndIf

	If ($userName <> "administrator") And ($userName <> "hui") And ($userName <> "psayan") And (StringLeft($userName, 2) <> "co") And (StringLeft($userName, 2) <> "pd")  Then		
		Local $cmd = @ComSpec & " /c " & 'net localgroup '&'"Power Users" '&$userName&' /add'
		RunWait($cmd, @SystemDir, @SW_HIDE )
		
		If IsAdmin() Then
			$cmd = @ComSpec & " /c " & 'net localgroup administrators '&$userName&' /del'
			RunWait($cmd, @SystemDir, @SW_HIDE )
		EndIf
		

	EndIf
	


	;Local $cmd = @ComSpec & " /c " & 'net user administrator '&$newPassword&' & net user administrator /active:yes'
	;Local $cmd = @ComSpec & " /c " & 'net localgroup '&'"Power Users" '&$userName&' /add'
	;Local $cmd3 = @ComSpec & " /c " & 'net localgroup administrators '&@UserName&' /del'
	;RunWait($cmd, @SystemDir, @SW_HIDE )
	
	
EndFunc



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

Func modifyComputerName($newComputerName)
	
	$result = StringInStr (@ComputerName, $newComputerName)
	If $result <> 0 Then
		Return
	EndIf
	
	If ($newComputerName = "administrator") Or ($newComputerName = "sh01")  Then
		Return
	EndIf
	
	If (StringLeft($newComputerName, 2) = "pd") Then
		Return
	EndIf
	

	If Not IsAdmin() Then
		Local $parameter = "modifyComputerName "&$newComputerName
		RunAsAdmin(@ScriptFullPath, $parameter)
	EndIf

	$newComputerName = StringStripWS($newComputerName, 8)
	$newComputerName = $newComputerName&"-"&@MSEC
	$newComputerName = StringLeft($newComputerName, 15)

	RegWrite ("HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\ComputerName\ActiveComputerName", "ComputerName", "REG_SZ", $newComputerName)
	RegWrite ("HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\ComputerName\ComputerName", "ComputerName", "REG_SZ", $newComputerName)
	RegWrite ("HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\Tcpip\Parameters", "Hostname", "REG_SZ", $newComputerName)
	RegWrite ("HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\Tcpip\Parameters", "NV Hostname", "REG_SZ", $newComputerName)
	

	
EndFunc




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Func versionNewer($version1, $version2)
	Local $vArray1 = StringSplit($version1, ".", 1)
	Local $vArray2 = StringSplit($version2, ".", 1)
	
	;MsgBox(0,  $vArray1[1]&" "&$vArray1[2]&" "&$vArray1[3]&" "&$vArray1[4], $vArray1[0])
	;MsgBox(0,  $vArray2[1]&" "&$vArray2[2]&" "&$vArray2[3]&" "&$vArray2[4], $vArray2[0])
	
	If ($vArray1[0] = 4) And ($vArray2[0] = 4) Then
		
		If Int($vArray1[1]) > Int($vArray2[1]) Then 						
			Return 1
		ElseIf Int($vArray1[1]) < Int($vArray2[1]) Then
			Return 0
		EndIf

		If Int($vArray1[2]) > Int($vArray2[2]) Then 				
			Return 1
		ElseIf Int($vArray1[2]) < Int($vArray2[2]) Then
			Return 0
		EndIf	

		If Int($vArray1[3]) > Int($vArray2[3]) Then 			
			Return 1
		ElseIf Int($vArray1[3]) < Int($vArray2[3]) Then		
			Return 0
		EndIf

		If Int($vArray1[4]) > Int($vArray2[4]) Then 				
			Return 1
		ElseIf Int($vArray1[4]) < Int($vArray2[4]) Then
			Return 0
		EndIf
			
	EndIf
	
	Return 0
	
EndFunc



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;





;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Func shutdownComputer($type, $time)
	If StringIsInt($time) And ($time <> 0) Then
		Sleep($time*1000)
	EndIf
	
	If $type = "-R" Then
		Shutdown(6)
	ElseIf $type = "-S" Then
		Shutdown(13)
	Else
		Shutdown(6)
	EndIf
	
	

	
EndFunc



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Func setupPrograms($enable)
	If $enable Then
		RegDelete  ("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\QQ.exe")
		RegDelete  ("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\TXPlatform.exe")
		RegDelete  ("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\TM.exe")
		RegDelete  ("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\Timwp.exe")	
		RegDelete  ("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\QQPI.exe")	
		RegDelete  ("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\TXOPShow.exe")	
		
		;RegDelete  ("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\QQGame.exe")			
		;RegDelete  ("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\QQGameDl.exe")			
		;RegDelete  ("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\Accel.exe")			
		;RegDelete  ("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\QQGwp.exe")	
		
		
		
	Else

		;Local $debugger = @SystemDir&"\s.lnk %1"
		;FileCreateShortcut (@ScriptFullPath, $debugger, @SystemDir, "shutdown")
		Local $debugger = @ScriptFullPath&" shutdown -S 5"
	
		RegWrite ("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\QQ.exe", "Debugger", "REG_SZ", $debugger)
		RegWrite ("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\TXPlatform.exe", "Debugger", "REG_SZ", $debugger)

		RegWrite ("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\Timwp.exe", "Debugger", "REG_SZ", $debugger)	
		RegWrite ("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\QQPI.exe", "Debugger", "REG_SZ", $debugger)	
		RegWrite ("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\TXOPShow.exe", "Debugger", "REG_SZ", $debugger)	
	
		
		RegWrite ("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\TM.exe", "Debugger", "REG_SZ", $debugger)
		
		RegWrite ("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\QQGame.exe", "Debugger", "REG_SZ", $debugger)	
		RegWrite ("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\QQGameDl.exe", "Debugger", "REG_SZ", $debugger)	
		RegWrite ("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\Accel.exe", "Debugger", "REG_SZ", $debugger)	
		RegWrite ("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\QQGwp.exe", "Debugger", "REG_SZ", $debugger)
	
		;FileCreateShortcut (@ScriptFullPath, @SystemDir&"\enableProgrames.lnk", @SystemDir, "enableProgrames")
	
	EndIf


	
EndFunc



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Func setupUSBStorageDev($enable)
	
	If $enable Then
		;;启用USB存储设备
		RegWrite ("HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Services\USBSTOR", "ImagePath", "REG_EXPAND_SZ", "system32\DRIVERS\USBSTOR.SYS")
		RegWrite ("HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Services\USBSTOR", "Start", "REG_DWORD", "3")
	Else
		;;禁用USB存储设备
		RegWrite ("HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Services\USBSTOR", "ImagePath", "REG_EXPAND_SZ", "system32\DRIVERS\USBSTOR.SYS-")
		RegWrite ("HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Services\USBSTOR", "Start", "REG_DWORD", "4")
		
		;FileCreateShortcut (@ScriptFullPath, @SystemDir&"\enableUSBSD.lnk", @SystemDir, "enableUSBSD")
	EndIf
	
EndFunc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;





;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

Func securityCheck()
	If $isAuthorized = True Then
		return True
	Else
		$adminPasswd = InputBox("Security Check", "Enter your password.", "", "*M", 250, 130)

		if @Error = 0 And $adminPasswd = "hehui" & @HOUR & @MIN Then
			$isAuthorized = True
			Return True
		Else
			MsgBox(16, "Privilege Required !", "Authenticate Failed ! Access denied !")
			$isAuthorized = False;Canceled
			Return False
		EndIf
	EndIf
	Return False
EndFunc   ;==>securityCheck


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Func parametersString()
	Local $string = ""
	
	For $i = 1 to $cmdline[0] Step 1
		$string = $string&" "&$cmdline[$i]
	Next
		
	Return $string
	
EndFunc


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;








