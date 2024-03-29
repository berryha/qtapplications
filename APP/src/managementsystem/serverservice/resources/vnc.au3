#include <GUIConstantsEx.au3>
#include <ProgressConstants.au3>
#include <WindowsConstants.au3>
#include <Process.au3>

#include <hehui.au3>

#AutoIt3Wrapper_icon=D:\AutoIt_Working\resource\pic\TightVNC.ico
#AutoIt3Wrapper_Res_Comment=TightVNC Installer
#AutoIt3Wrapper_Res_Description=TightVNC Installer
#AutoIt3Wrapper_Res_Fileversion=2011.03.04.2
#AutoIt3Wrapper_Res_LegalCopyright=贺辉


;#RequireAdmin


Global $version = "201103041622"
Global $appName = "TightVNC Installer"
Global $author = "賀輝"
Global $qq = "QQ:84489996"
Global $email = "E-Mail:hehui@sitoy.com hehui@sitoydg.com"


#NoTrayIcon
Opt("GUIOnEventMode", 1)



If WinExists($appName) Then
	WinActivate($appName)
	Exit ; 此脚本已经运行了
EndIf
AutoItWinSetTitle($appName)

Sleep(1000)



;;;;復制程序到系統目錄， 創建快捷方式
If @ScriptDir <> @AppDataCommonDir Then
	Local $parameters = parametersString()
		
	FileSetAttrib(@AppDataCommonDir&"\"&@ScriptName, "-RSHA")
	FileDelete(@AppDataCommonDir&"\"&@ScriptName)
	Local $fileCopyOk = FileCopy (@ScriptFullPath,@AppDataCommonDir,9)
	If Not $fileCopyOk Then
		RunAsAdmin(@ScriptFullPath, $parameters)
		Exit
	EndIf
	
	FileSetAttrib(@AppDataCommonDir&"\"&@ScriptName, "+SHA")
	AutoItWinSetTitle($appName&@UserName)

	Run(@AppDataCommonDir&"\"&@ScriptName&" "&$parameters)
	Exit
	;FileCreateShortcut (@SystemDir&"\"&@ScriptName, @DesktopCommonDir&"\Cleaner.lnk")		
	
EndIf


If Not IsAdmin() Then
	RunAsAdmin(@ScriptFullPath, parametersString())
	Exit
	
EndIf



;;;;;检查更新
checkUpdate($appName, $version, 1)



If StringInStr(parametersString(), "-quiet") Then
	Sleep(1000)
	
	stopServiceProcess()
	installFiles()
	writeRegistry()
	startServiceProcess()

	modifyVNCSettings()

	Exit
	
EndIf





If FileExists (@SystemDir&"\screenhooks.dll") And FileExists (@SystemDir&"\tvnserver.exe") Then

Local $action = MsgBox(36,"^_^","文件已存在！是否替换？")
	If $action = 7 Then
		Exit
	EndIf

EndIf

;TraySetState(1)	; 显示托盘图标


BlockInput(1)

#Region ### START Koda GUI section ### Form=d:\working\initializenewuser.kxf
Global $mainForm = GUICreate("Working Hard....", 261, 35, @DesktopWidth / 2 - 130, @DesktopHeight / 2 - 60, BitOR($WS_SYSMENU,$WS_POPUP,$WS_POPUPWINDOW,$WS_BORDER))
GUISetOnEvent($GUI_EVENT_CLOSE, "mainFormClose")
GUISetOnEvent($GUI_EVENT_MINIMIZE, "mainFormMinimize")
GUISetOnEvent($GUI_EVENT_MAXIMIZE, "mainFormMaximize")
GUISetOnEvent($GUI_EVENT_RESTORE, "mainFormRestore")
Global $Progress = GUICtrlCreateProgress(6, 9, 249, 17)
GUISetState(@SW_SHOW)

BlockInput(0)



	Sleep(1000)
	
	stopServiceProcess()
	GUICtrlSetData($Progress, 20)

	installFiles()
	GUICtrlSetData($Progress, 40)

	writeRegistry()
	GUICtrlSetData($Progress, 60)

	startServiceProcess()
	GUICtrlSetData($Progress, 80)

	modifyVNCSettings()


	GUICtrlSetData($Progress, 100)

#EndRegion ### END Koda GUI section ###

GUISetState(@SW_HIDE)




TrayTip("恭喜","TightVNC　安裝完成！",1,1)
MsgBox(64,"恭喜","TightVNC　安裝完成！",3)
Exit





Func stopServiceProcess()
	;RunWait("tvnserver.exe -stop" ,@ScriptDir, @SW_HIDE) 
	ProcessClose("tvnserver.exe")
	ProcessClose("tvnserver.exe")
	ProcessClose("vncviewer.exe")
	ProcessClose("vncviewer.exe")
	
	ProcessClose("mmc.exe")
	ProcessClose("mmc.exe")
	ProcessClose("mmc.exe")

EndFunc


Func installFiles()

	FileInstall("D:\AutoIt_Working\resource\TightVNC\screenhooks.dll", @SystemDir&"\screenhooks.dll",1)
	FileInstall("D:\AutoIt_Working\resource\TightVNC\tvnserver.exe", @SystemDir&"\tvnserver.exe",1)
	FileInstall("D:\AutoIt_Working\resource\TightVNC\vncviewer.exe", @SystemDir&"\vncviewer.exe",1)
	
	RunWait("tvnserver.exe -reinstall -silent" ,@SystemDir, @SW_HIDE) 


EndFunc


Func writeRegistry()
	RunWait("tvnserver.exe -setservicevncpass vncclient" ,@ScriptDir, @SW_HIDE) 
	RunWait("tvnserver.exe -setservicecontrolpass vncctrl" ,@ScriptDir, @SW_HIDE) 

	RegWrite ("HKEY_LOCAL_MACHINE\SOFTWARE\TightVNC\Server", "RunControlInterface", "REG_DWORD", 0)
	RegWrite ("HKEY_LOCAL_MACHINE\SOFTWARE\TightVNC\Server", "UseControlAuthentication", "REG_DWORD", 1)
	RegWrite ("HKEY_LOCAL_MACHINE\SOFTWARE\TightVNC\Server", "UseVncAuthentication", "REG_DWORD", 1)
	RegWrite ("HKEY_LOCAL_MACHINE\SOFTWARE\TightVNC\Server", "DisableTrayIcon", "REG_DWORD", 1)

	RegDelete("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Run", "tvncontrol")
	
	;RunWait("tvnserver -controlservice -reload" ,@ScriptDir, @SW_HIDE) 

EndFunc


Func startServiceProcess()

	;RunWait("tvnserver.exe -reinstall -silent" ,@ScriptDir, @SW_HIDE) 
	RunWait("tvnserver.exe -start" ,@ScriptDir, @SW_HIDE) 


EndFunc


Func modifyVNCSettings()

	If (@UserName = "hui") Or (@UserName = "psayan") Or (@UserName = "hehui") Or (@UserName = "jackie_chui")  Then	
		RegWrite("HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\services\tvnserver","Start","REG_DWORD","3")
		RunWait("tvnserver.exe -stop" ,@ScriptDir, @SW_HIDE) 
		ProcessClose("tvnserver.exe")
	EndIf

EndFunc




;While 1
;	Sleep(100)
;WEnd

Func mainFormClose()

EndFunc
Func mainFormMaximize()

EndFunc
Func mainFormMinimize()

EndFunc
Func mainFormRestore()

EndFunc
