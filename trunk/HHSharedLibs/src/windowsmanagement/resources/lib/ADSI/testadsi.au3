

Local $dllPath = "E:\workspace\PowerBasic\pb\ADSI.dll"

Local $dll = DllOpen($dllPath)

Local $result = DllCall($dll, "long", "AD_Open", "wstr", "hehui", "wstr", "000...", "wstr", "200.200.200.106", "dword", 0)
If @error Then MsgBox(16, "Error", @error)
;MsgBox(4096, "AD_Open", $result[0] ) 


;$result = DllCall($dllPath, "wstr", "AD_GetObjectProperties", "wstr", "hehui", "wstr", "whenCreated")
;MsgBox(0, "AD_GetObjectAttribute", $result[0])

;$result = DllCall($dllPath, "long", "test")

;$result = DllCall($dll, "wstr", "AD_GetObjectAttribute", "wstr", "hehui", "wstr", "displayName" )
;MsgBox(4096, "AD_GetObjectAttribute", $result[0])  


;$result = DllCall($dll, "long", "AD_CreateOU", "wstr", "OU=DG",  "wstr", "TestOU")
;MsgBox(0, "AD_CreateOU", $result[0])

;$result = DllCall($dll, "wstr", "AD_GetLastErrorString")
;MsgBox(0, "AD_GetLastErrorString", $result[0])

;$result = DllCall($dll, "wstr", "AD_GetObjectsInOU", "wstr", "", "wstr", "(&(objectcategory=person)(objectclass=user)(cn=" & "test" & "*))", 2, "sAMAccountName,distinguishedName,displayname", "wstr", "displayname")
;$result = DllCall($dll, "wstr", "AD_GetObjectsInOU", "wstr", "OU=OU1,OU=TestOU1,DC=test,DC=local", "wstr", "(&(objectcategory=person)(objectclass=user)(cn=" & "test" & "*))" , "wstr", "displayName", "wstr", ";", "wstr", "|")
;MsgBox(4096, "AD_GetObjectsInOU", $result[0])  


;$result = DllCall($dll, "wstr", "AD_GetAllOUs", "wstr", "",  "wstr", ";", "wstr", "\")
;MsgBox(0, "AD_GetAllOUs", $result[0])

$result = DllCall($dll, "long", "AD_IsObjectDisabled", "wstr", "hehui")
MsgBox(0, "AD_GetAllOUs", $result[0])


$result = DllCall($dll, "long", "AD_Close")
DllClose($dll)

Exit




DllClose($dll)