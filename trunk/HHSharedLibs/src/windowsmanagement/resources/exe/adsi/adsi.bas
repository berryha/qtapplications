
  ' #COMPILE EXE

    #COMPILE DLL

REM #COMPILE DLL
#DIM ALL
%USEMACROS = 1
#INCLUDE ONCE "modules\adsi.inc"

'''----------------------------------------------------------

FUNCTION testADSI () AS LONG



     'IF ad_open("hehui", "000...", "200.200.200.106", 0 ) = 0 THEN
     IF ad_open("dgadmin", "dmsto&*(", "200.200.200.106", 0 ) = 0 THEN
        MSGBOX AD_GetLastErrorString(), %MB_ICONERROR, "Error"
     END IF


   ' MSGBOX AD_GetObjectAttribute("test", "objectGUID")

    'IF AD_CreateUser("OU=TestOU,OU=DG,DC=test,DC=local", "test1", "TestCN1") = 0 THEN
    '   MSGBOX AD_GetLastErrorString(), %MB_ICONERROR, "Error"
    'END IF

    'if AD_SetPassword("test4", "123456") = 0 then
    '    MSGBOX AD_GetLastErrorString(), %MB_ICONERROR, "Error"
   ' end if


   ' IF AD_DeleteObject("Test1", "user")  = 0 THEN
   '     MSGBOX AD_GetLastErrorString(), %MB_ICONERROR, "Error"
   ' END IF


'  IF AD_MoveObject("OU=DG,DC=test,DC=local", "Test1")  = 0 THEN
 '       MSGBOX AD_GetLastErrorString(), %MB_ICONERROR, "Error"
 '   END IF


   ' IF AD_RenameObject("test3", "Test")  = 0 THEN
  '      MSGBOX AD_GetLastErrorString(), %MB_ICONERROR, "Error"
   ' END IF

   'AD_ModifyAttribute("test4", "telephoneNumber", "12345", 0)

   ' IF AD_SetAccountExpire("test3", "2013-04-08")  = 0 THEN
   '     MSGBOX AD_GetLastErrorString(), %MB_ICONERROR, "Error"
   ' END IF

  '  IF AD_EnableObject("test1", 0)  = 0 THEN
  '      MSGBOX AD_GetLastErrorString(), %MB_ICONERROR, "Error"
  '  END IF

   ' IF AD_IsObjectDisabled("test3")  = 0 THEN
    '    MSGBOX AD_GetLastErrorString(), %MB_ICONERROR, "Error"
   ' END IF


   ' AD_SetUserPasswordChange("test1", 0)

   MSGBOX AD_GetAllOUs("")

   'MSGBOX AD_GetObjectsInOU("OU=TestOU1,DC=test,DC=local", "(&(objectcategory=person)(objectclass=user)(cn=" & "test" & "*))" , "sAMAccountName,distinguishedName,objectSid", ";", "|")
   ' MSGBOX AD_GetObjectsInOU("OU=TestOU1,DC=test,DC=local", "(&(objectcategory=person)(objectclass=user)(sAMAccountName=" & "test" & "*)(displayName=Tes*))" , "memberOf", ";", "|")
  '   MSGBOX AD_GetObjectsInOU("OU=OU1,OU=TestOU1,DC=test,DC=local", "(&(objectcategory=person)(objectclass=user)(sAMAccountName=" & "test" & "*))" , "displayName", ";", "|")




    'test()

END FUNCTION

''''''''''----------------------------------------------------------

#IF %PB_EXE

FUNCTION PBMAIN () AS LONG
    CALL testADSI()
END FUNCTION

#ELSE
''''''''''----------------------------------------------------------
GLOBAL ghInstance AS DWORD
FUNCTION LIBMAIN (BYVAL hInstance   AS LONG, _
                  BYVAL fwdReason   AS LONG, _
                  BYVAL lpvReserved AS LONG) AS LONG
    SELECT CASE fwdReason
    CASE %DLL_PROCESS_ATTACH : ghInstance = hInstance
                               FUNCTION = 1
    CASE %DLL_PROCESS_DETACH : FUNCTION = 1
    CASE %DLL_THREAD_ATTACH  : FUNCTION = 1
    CASE %DLL_THREAD_DETACH  : FUNCTION = 1
    END SELECT
END FUNCTION

#ENDIF
