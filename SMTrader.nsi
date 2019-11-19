############################################################################################
#      NSIS Installation Script created by NSIS Quick Setup Script Generator v1.09.18
#               Entirely Edited with NullSoft Scriptable Installation System                
#              by Vlasis K. Barkas aka Red Wine red_wine@freemail.gr Sep 2006               
############################################################################################

!define APP_NAME "SMTrader"
!define COMP_NAME "Signal Master"
!define WEB_SITE "http://www.signalmaster.com"
!define VERSION "1.0.0.0"
!define COPYRIGHT "Author  ?Signal Master 2019"
!define DESCRIPTION "Signal Master SMTrader"
!define INSTALLER_NAME "C:\Project\SMTrader\SMTraderSetup.exe"
!define MAIN_APP_EXE "SMTrader.exe"
!define INSTALL_TYPE "SetShellVarContext current"
!define REG_ROOT "HKCU"
!define REG_APP_PATH "Software\Microsoft\Windows\CurrentVersion\App Paths\${MAIN_APP_EXE}"
!define UNINSTALL_PATH "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"

######################################################################

VIProductVersion  "${VERSION}"
VIAddVersionKey "ProductName"  "${APP_NAME}"
VIAddVersionKey "CompanyName"  "${COMP_NAME}"
VIAddVersionKey "LegalCopyright"  "${COPYRIGHT}"
VIAddVersionKey "FileDescription"  "${DESCRIPTION}"
VIAddVersionKey "FileVersion"  "${VERSION}"

######################################################################

SetCompressor ZLIB
Name "${APP_NAME}"
Caption "${APP_NAME}"
OutFile "${INSTALLER_NAME}"
BrandingText "${APP_NAME}"
XPStyle on
InstallDirRegKey "${REG_ROOT}" "${REG_APP_PATH}" ""
InstallDir "$PROGRAMFILES\Signal Master\SMTrader"

######################################################################

!include "MUI.nsh"
!include "x64.nsh"
!include "VCRedist15.nsh"

!define MUI_ABORTWARNING
!define MUI_UNABORTWARNING

!define MUI_LANGDLL_REGISTRY_ROOT "${REG_ROOT}"
!define MUI_LANGDLL_REGISTRY_KEY "${UNINSTALL_PATH}"
!define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

!insertmacro MUI_PAGE_WELCOME

!ifdef LICENSE_TXT
!insertmacro MUI_PAGE_LICENSE "${LICENSE_TXT}"
!endif

!ifdef REG_START_MENU
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "SMTrader"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${REG_ROOT}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${UNINSTALL_PATH}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${REG_START_MENU}"
!insertmacro MUI_PAGE_STARTMENU Application $SM_Folder
!endif

!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM

!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Korean"

!insertmacro MUI_RESERVEFILE_LANGDLL

RequestExecutionLevel admin
;SilentInstall silent

ShowInstDetails show
ShowUnInstDetails show

######################################################################

Function .onInit
!insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

######################################################################

Section -MainProgram
${INSTALL_TYPE}
SetOverwrite ifnewer
SetOutPath "$INSTDIR"

ReadRegStr $1 HKLM "Software\Microsoft\DevDiv\vc\Servicing\14.0\RuntimeMinimum" Install
    StrCmp $1 "1" InstallVCRedist11_32bitFinish

    ;MESSAGEBOX MB_OK "test"
    ; Install 32-bit Visual Studio 2015 Redistributable
    !insertmacro _VCRedist11_SetStatus "Extracting Visual C++ Redistributable (32-bit)"
    SetOutPath "$INSTDIR"
    File "C:\Project\SMTrader\Setup\vc_redist.x86.exe"

    !insertmacro _VCRedist11_SetStatus "Installing Visual C++ Redistributable (32-bit)"
    ClearErrors
    ExecWait '"$INSTDIR\vc_redist.x86.exe" /s /v" /qn"'
    ;ExecWait '"$INSTDIR\vc_redist.x86.exe" /v" /qn"'
    !insertmacro _VCRedist11_MessageBoxIfError "Failed to install 32-bit Visual C++ Redistributable."
InstallVCRedist11_32bitFinish:
;File "C:\Project\SMTrader\Setup\systemlist.dat"
File "C:\Project\SMTrader\Setup\AxInterop.HDFCommAgentLib.dll"
File "C:\Project\SMTrader\Setup\chartdir.lic"
File "C:\Project\SMTrader\Setup\chartdir60.dll"
File "C:\Project\SMTrader\Setup\CodeMaster.dll"
File "C:\Project\SMTrader\Setup\CommApi.dll"
File "C:\Project\SMTrader\Setup\CppUnit.dll"
File "C:\Project\SMTrader\Setup\CreVolDBC.dll"
File "C:\Project\SMTrader\Setup\CreVolXMLSpy.dll"
File "C:\Project\SMTrader\Setup\HDFCommAgent.ocx"
File "C:\Project\SMTrader\Setup\HDFCommClient.exe"
File "C:\Project\SMTrader\Setup\HDFCommClientCSharp.exe"
File "C:\Project\SMTrader\Setup\HDFComms.exe"
File "C:\Project\SMTrader\Setup\Interop.HDFCommAgentLib.dll"
File "C:\Project\SMTrader\Setup\LinkInfoMng.dll"
File "C:\Project\SMTrader\Setup\mfc100.dll"
File "C:\Project\SMTrader\Setup\MiraePKIX_v4.dll"
File "C:\Project\SMTrader\Setup\msvcp100.dll"
File "C:\Project\SMTrader\Setup\msvcr100.dll"
File "C:\Project\SMTrader\Setup\nsldap32v11.dll"
File "C:\Project\SMTrader\Setup\nsldap32v50.dll"
File "C:\Project\SMTrader\Setup\PocoData.dll"
File "C:\Project\SMTrader\Setup\PocoDataMySQL.dll"
File "C:\Project\SMTrader\Setup\PocoFoundation.dll"
File "C:\Project\SMTrader\Setup\PocoJSON.dll"
File "C:\Project\SMTrader\Setup\PocoNet.dll"
File "C:\Project\SMTrader\Setup\PocoUtil.dll"
File "C:\Project\SMTrader\Setup\PocoXML.dll"
File "C:\Project\SMTrader\Setup\product.cod"
File "C:\Project\SMTrader\Setup\SFSmartCard.dll"
File "C:\Project\SMTrader\Setup\skcommcm.dll"
File "C:\Project\SMTrader\Setup\skcommem.dll"
File "C:\Project\SMTrader\Setup\skcommic.dll"
File "C:\Project\SMTrader\Setup\skcommif.dll"
File "C:\Project\SMTrader\Setup\skcommsc.dll"
File "C:\Project\SMTrader\Setup\SMTrader.exe"
File "C:\Project\SMTrader\Setup\SMTrader.ini"
File "C:\Project\SMTrader\Setup\SrShare.dll"
File "C:\Project\SMTrader\Setup\xcon30.dll"
File "C:\Project\SMTrader\Setup\XecureASN_v20.dll"
File "C:\Project\SMTrader\Setup\XecureCMP_v20.dll"
File "C:\Project\SMTrader\Setup\XecureCodec_v20.dll"
File "C:\Project\SMTrader\Setup\XecureCRL_v20.dll"
File "C:\Project\SMTrader\Setup\XecureCrypto_v20.dll"
File "C:\Project\SMTrader\Setup\XecureCSP_v20.dll"
File "C:\Project\SMTrader\Setup\XecureCTL_v20.dll"
File "C:\Project\SMTrader\Setup\XecureIO_v20.dll"
File "C:\Project\SMTrader\Setup\XecureLDAP_v20.dll"
File "C:\Project\SMTrader\Setup\XecureOCSP_v20.dll"
File "C:\Project\SMTrader\Setup\XecurePKCS12_v20.dll"
File "C:\Project\SMTrader\Setup\XecurePKCS5.dll"
File "C:\Project\SMTrader\Setup\XecurePKCS5_v20.dll"
File "C:\Project\SMTrader\Setup\XecurePKCS7_v20.dll"
File "C:\Project\SMTrader\Setup\XecurePKCS8_v20.dll"
File "C:\Project\SMTrader\Setup\XecurePKC_v20.dll"
File "C:\Project\SMTrader\Setup\XecurePVD_v20.dll"
File "C:\Project\SMTrader\Setup\XecureTSP_v20.dll"
File "C:\Project\SMTrader\Setup\xwcs_client.dll"
File "C:\Project\SMTrader\Setup\XWebCertMng.dll"
File "C:\Project\SMTrader\Setup\XWebCMP.dll"
File "C:\Project\SMTrader\Setup\XWebCS.dll"
File "C:\Project\SMTrader\Setup\XWebSSL.dll"
File "C:\Project\SMTrader\Setup\XWebUI.dll"
File "C:\Project\SMTrader\Setup\XWebUtil.dll"
File "C:\Project\SMTrader\Setup\★★API_OCX등록.bat"
SetOutPath "$INSTDIR\Xecure3\License"
File "C:\Project\SMTrader\Setup\Xecure3\License\gensysid.exe"
File "C:\Project\SMTrader\Setup\Xecure3\License\license.exp"
SetOutPath "$INSTDIR\Xecure3\filter"
File "C:\Project\SMTrader\Setup\Xecure3\filter\xc_filter.txt"
SetOutPath "$INSTDIR\Xecure3\conf"
File "C:\Project\SMTrader\Setup\Xecure3\conf\xc_conf.txt"
SetOutPath "$INSTDIR\Xecure3\cert"
File "C:\Project\SMTrader\Setup\Xecure3\cert\kmCert.der"
File "C:\Project\SMTrader\Setup\Xecure3\cert\kmPri.key"
File "C:\Project\SMTrader\Setup\Xecure3\cert\signCert.der"
File "C:\Project\SMTrader\Setup\Xecure3\cert\signPri.key"
SetOutPath "$INSTDIR\TrData"
File "C:\Project\SMTrader\Setup\TrData\0051_국내선물호가.dat"
File "C:\Project\SMTrader\Setup\TrData\0052_국내옵션호가.dat"
File "C:\Project\SMTrader\Setup\TrData\0056_국내주식선물호가.dat"
File "C:\Project\SMTrader\Setup\TrData\0058_상품선물호가실시간.dat"
File "C:\Project\SMTrader\Setup\TrData\0059_상품옵션호가실시간.dat"
File "C:\Project\SMTrader\Setup\TrData\0065_국내선물체결.dat"
File "C:\Project\SMTrader\Setup\TrData\0066_국내옵션체결.dat"
File "C:\Project\SMTrader\Setup\TrData\0068_국내주식선물시세.dat"
File "C:\Project\SMTrader\Setup\TrData\0071_상품선물시세실시간.dat"
File "C:\Project\SMTrader\Setup\TrData\0073_상품옵션시세실시간.dat"
File "C:\Project\SMTrader\Setup\TrData\0075_CME선물호가.dat"
File "C:\Project\SMTrader\Setup\TrData\0076_해외선물호가.dat"
File "C:\Project\SMTrader\Setup\TrData\0077_CME선물체결.dat"
File "C:\Project\SMTrader\Setup\TrData\0078_CME옵션호가.dat"
File "C:\Project\SMTrader\Setup\TrData\0079_CME옵션체결.dat"
File "C:\Project\SMTrader\Setup\TrData\0082_해외선물체결.dat"
File "C:\Project\SMTrader\Setup\TrData\0101_지수실시간.dat"
File "C:\Project\SMTrader\Setup\TrData\0161_뉴스제목실시간.dat"
File "C:\Project\SMTrader\Setup\TrData\0171_FX 시세.dat"
File "C:\Project\SMTrader\Setup\TrData\0181_국내주문실시간접수.dat"
File "C:\Project\SMTrader\Setup\TrData\0182_국내주문실시간미체결.dat"
File "C:\Project\SMTrader\Setup\TrData\0183_국내주문실시간미결제.dat"
File "C:\Project\SMTrader\Setup\TrData\0184_국내주문실시간잔고.dat"
File "C:\Project\SMTrader\Setup\TrData\0185_국내주문실시간체결.dat"
File "C:\Project\SMTrader\Setup\TrData\0186_해외주문실시간미체결.dat"
File "C:\Project\SMTrader\Setup\TrData\0187_해외주문실시간미결제.dat"
File "C:\Project\SMTrader\Setup\TrData\0188_해외주문실시간잔고.dat"
File "C:\Project\SMTrader\Setup\TrData\0189_해외주문실시간체결.dat"
File "C:\Project\SMTrader\Setup\TrData\0190_해외순미결제실시간.dat"
File "C:\Project\SMTrader\Setup\TrData\0191_FX주문실시간미체결.dat"
File "C:\Project\SMTrader\Setup\TrData\0192_FX주문실시간미청산.dat"
File "C:\Project\SMTrader\Setup\TrData\0193_FX주문실시간청산.dat"
File "C:\Project\SMTrader\Setup\TrData\0194_FX실시간자산내역.dat"
File "C:\Project\SMTrader\Setup\TrData\0195_FX실시간요약.dat"
File "C:\Project\SMTrader\Setup\TrData\0196_해외주문실시간접수.dat"
File "C:\Project\SMTrader\Setup\TrData\0197_FX주문실시간접수.dat"
File "C:\Project\SMTrader\Setup\TrData\0202_현물_USD실시간체결.dat"
File "C:\Project\SMTrader\Setup\TrData\0208_영업일취득.dat"
File "C:\Project\SMTrader\Setup\TrData\0210_현물_금실시간체결.dat"
File "C:\Project\SMTrader\Setup\TrData\0211_CME주문실시간미체결.dat"
File "C:\Project\SMTrader\Setup\TrData\0212_CME주문실시간미결제.dat"
File "C:\Project\SMTrader\Setup\TrData\0213_CME주문실시간체결.dat"
File "C:\Project\SMTrader\Setup\TrData\0241_해외옵션호가.dat"
File "C:\Project\SMTrader\Setup\TrData\0242_해외옵션체결.dat"
File "C:\Project\SMTrader\Setup\TrData\0243_해외옵션체결차트.dat"
File "C:\Project\SMTrader\Setup\TrData\0244_해외옵션민감도.dat"
File "C:\Project\SMTrader\Setup\TrData\0261_국내주문실시간접수.dat"
File "C:\Project\SMTrader\Setup\TrData\0262_국내주문실시간미체결.dat"
File "C:\Project\SMTrader\Setup\TrData\0265_국내주문실시간체결.dat"
File "C:\Project\SMTrader\Setup\TrData\0271_CME주문실시간미체결.dat"
File "C:\Project\SMTrader\Setup\TrData\0273_CME주문실시간체결.dat"
File "C:\Project\SMTrader\Setup\TrData\0286_해외주문실시간미체결.dat"
File "C:\Project\SMTrader\Setup\TrData\0289_해외주문실시간체결.dat"
File "C:\Project\SMTrader\Setup\TrData\0296_해외주문실시간접수.dat"
File "C:\Project\SMTrader\Setup\TrData\0310_국내예상체결.dat"
File "C:\Project\SMTrader\Setup\TrData\g11002.DQ0104&.dat"
File "C:\Project\SMTrader\Setup\TrData\g11002.DQ0107&.dat"
File "C:\Project\SMTrader\Setup\TrData\g11002.DQ0110&.dat"
File "C:\Project\SMTrader\Setup\TrData\g11002.DQ0116&.dat"
File "C:\Project\SMTrader\Setup\TrData\g11002.DQ0119&.dat"
File "C:\Project\SMTrader\Setup\TrData\g11002.DQ0122&.dat"
File "C:\Project\SMTrader\Setup\TrData\g11002.DQ0125&.dat"
File "C:\Project\SMTrader\Setup\TrData\g11002.DQ0217&.dat"
File "C:\Project\SMTrader\Setup\TrData\g11002.DQ0242&.dat"
File "C:\Project\SMTrader\Setup\TrData\g11002.DQ0321&.dat"
File "C:\Project\SMTrader\Setup\TrData\g11002.DQ0502&.dat"
File "C:\Project\SMTrader\Setup\TrData\g11002.DQ0509&.dat"
File "C:\Project\SMTrader\Setup\TrData\g11002.DQ0521&.dat"
File "C:\Project\SMTrader\Setup\TrData\g11002.DQ0622&.dat"
File "C:\Project\SMTrader\Setup\TrData\g11002.DQ1211&.dat"
File "C:\Project\SMTrader\Setup\TrData\g11002.DQ1302&.dat"
File "C:\Project\SMTrader\Setup\TrData\g11002.DQ1303&.dat"
File "C:\Project\SMTrader\Setup\TrData\g11002.DQ1305&.dat"
File "C:\Project\SMTrader\Setup\TrData\g11002.DQ1306&.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0101%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0128%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0301%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0302%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0303%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0401%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0402%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0403%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0404%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0405%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0408%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0409%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0415%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0450%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0495%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0602%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0605%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0607%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0613%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0636%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0712%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0715%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0725%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0901%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0904%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0906%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0908%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0910%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0911%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0914%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0915%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0920%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AQ0923%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AS0129%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AS0132%.dat"
File "C:\Project\SMTrader\Setup\TrData\g11004.AS0134%.dat"
File "C:\Project\SMTrader\Setup\TrData\g12001.DO1601&.dat"
File "C:\Project\SMTrader\Setup\TrData\g12001.DO1701&.dat"
File "C:\Project\SMTrader\Setup\TrData\g12001.DO1901&.dat"
File "C:\Project\SMTrader\Setup\TrData\g12001.DO2001&.dat"
File "C:\Project\SMTrader\Setup\TrData\g12001.DO2101&.dat"
File "C:\Project\SMTrader\Setup\TrData\g12001.DO2201&.dat"
File "C:\Project\SMTrader\Setup\TrData\g12003.AO0401%.dat"
File "C:\Project\SMTrader\Setup\TrData\g12003.AO0402%.dat"
File "C:\Project\SMTrader\Setup\TrData\g12003.AO0403%.dat"
File "C:\Project\SMTrader\Setup\TrData\g12003.AO0501%.dat"
File "C:\Project\SMTrader\Setup\TrData\g12003.AO0502%.dat"
File "C:\Project\SMTrader\Setup\TrData\g12003.AO0503%.dat"
File "C:\Project\SMTrader\Setup\TrData\l41600.dat"
File "C:\Project\SMTrader\Setup\TrData\l41601.dat"
File "C:\Project\SMTrader\Setup\TrData\l41602.dat"
File "C:\Project\SMTrader\Setup\TrData\l41603.dat"
File "C:\Project\SMTrader\Setup\TrData\l41619.dat"
File "C:\Project\SMTrader\Setup\TrData\l41703.dat"
File "C:\Project\SMTrader\Setup\TrData\n51000.dat"
File "C:\Project\SMTrader\Setup\TrData\n51001.dat"
File "C:\Project\SMTrader\Setup\TrData\n51003.dat"
File "C:\Project\SMTrader\Setup\TrData\n51006.dat"
File "C:\Project\SMTrader\Setup\TrData\o44005.dat"
File "C:\Project\SMTrader\Setup\TrData\o44011.dat"
File "C:\Project\SMTrader\Setup\TrData\o51000.dat"
File "C:\Project\SMTrader\Setup\TrData\o51010.dat"
File "C:\Project\SMTrader\Setup\TrData\o51100.dat"
File "C:\Project\SMTrader\Setup\TrData\o51200.dat"
File "C:\Project\SMTrader\Setup\TrData\o51210.dat"
File "C:\Project\SMTrader\Setup\TrData\o51211.dat"
File "C:\Project\SMTrader\Setup\TrData\s10001.dat"
File "C:\Project\SMTrader\Setup\TrData\s20001.dat"
File "C:\Project\SMTrader\Setup\TrData\s20101.dat"
File "C:\Project\SMTrader\Setup\TrData\s21001.dat"
File "C:\Project\SMTrader\Setup\TrData\s31001.dat"
File "C:\Project\SMTrader\Setup\TrData\s90005.dat"
File "C:\Project\SMTrader\Setup\TrData\v90001.dat"
File "C:\Project\SMTrader\Setup\TrData\v90003.dat"
File "C:\Project\SMTrader\Setup\TrData\x00001.dat"
File "C:\Project\SMTrader\Setup\TrData\x00002.dat"
File "C:\Project\SMTrader\Setup\TrData\x00003.dat"
File "C:\Project\SMTrader\Setup\TrData\x00004.dat"
File "C:\Project\SMTrader\Setup\TrData\x00005.dat"
File "C:\Project\SMTrader\Setup\TrData\x00006.dat"
SetOutPath "$INSTDIR\system"
File "C:\Project\SMTrader\Setup\system\comms.ini"
File "C:\Project\SMTrader\Setup\system\Commsu.ini"
File "C:\Project\SMTrader\Setup\system\Realdata.dat"
SetOutPath "$INSTDIR\mst"
File "C:\Project\SMTrader\Setup\mst\afucode.cod"
File "C:\Project\SMTrader\Setup\mst\afuseriescode.cod"
File "C:\Project\SMTrader\Setup\mst\aopcode.cod"
File "C:\Project\SMTrader\Setup\mst\atmaprice.cod"
File "C:\Project\SMTrader\Setup\mst\atmjprice.cod"
File "C:\Project\SMTrader\Setup\mst\atmmprice.cod"
File "C:\Project\SMTrader\Setup\mst\atmprice.cod"
File "C:\Project\SMTrader\Setup\mst\BDAY.cod"
File "C:\Project\SMTrader\Setup\mst\bidcode.cod"
File "C:\Project\SMTrader\Setup\mst\cafcode.cod"
File "C:\Project\SMTrader\Setup\mst\cfucode.cod"
File "C:\Project\SMTrader\Setup\mst\clt1_futcode.cod"
File "C:\Project\SMTrader\Setup\mst\clt1_optcode.cod"
File "C:\Project\SMTrader\Setup\mst\clt1_usdcode.cod"
File "C:\Project\SMTrader\Setup\mst\cmecode.cod"
File "C:\Project\SMTrader\Setup\mst\efucode.cod"
File "C:\Project\SMTrader\Setup\mst\eopcode.cod"
File "C:\Project\SMTrader\Setup\mst\etcicode.cod"
File "C:\Project\SMTrader\Setup\mst\euxcode.cod"
File "C:\Project\SMTrader\Setup\mst\futcode.cod"
File "C:\Project\SMTrader\Setup\mst\FXCODE.cod"
File "C:\Project\SMTrader\Setup\mst\jangtime.cod"
File "C:\Project\SMTrader\Setup\mst\jfucode.cod"
File "C:\Project\SMTrader\Setup\mst\JMCODE.cod"
File "C:\Project\SMTrader\Setup\mst\jopcode.cod"
File "C:\Project\SMTrader\Setup\mst\kqfucode.cod"
File "C:\Project\SMTrader\Setup\mst\kqopcode.cod"
File "C:\Project\SMTrader\Setup\mst\ksdicode.cod"
File "C:\Project\SMTrader\Setup\mst\kspicode.cod"
File "C:\Project\SMTrader\Setup\mst\mccode.cod"
File "C:\Project\SMTrader\Setup\mst\mfucode.cod"
File "C:\Project\SMTrader\Setup\mst\MONTH.cod"
File "C:\Project\SMTrader\Setup\mst\monthcode.cod"
File "C:\Project\SMTrader\Setup\mst\mopcode.cod"
File "C:\Project\SMTrader\Setup\mst\MRKT.cod"
File "C:\Project\SMTrader\Setup\mst\OJMCODE.cod"
File "C:\Project\SMTrader\Setup\mst\OPMCODE.cod"
File "C:\Project\SMTrader\Setup\mst\OPTATM.cod"
File "C:\Project\SMTrader\Setup\mst\optcode.cod"
File "C:\Project\SMTrader\Setup\mst\OPTCSIZ.cod"
File "C:\Project\SMTrader\Setup\mst\OPTINFO.cod"
File "C:\Project\SMTrader\Setup\mst\PMCODE.cod"
File "C:\Project\SMTrader\Setup\mst\stkcode.cod"
File "C:\Project\SMTrader\Setup\mst\xfucode.cod"
SetOutPath "$INSTDIR\Data"
File "C:\Project\SMTrader\Setup\Data\hogadec.dat"
File "C:\Project\SMTrader\Setup\Data\국내_uamsg.dat"
File "C:\Project\SMTrader\Setup\Data\해외_uamsg.dat"

RegDLL "$INSTDIR\HDFCommAgent.ocx"


SectionEnd

######################################################################

Section -Icons_Reg
SetOutPath "$INSTDIR"
WriteUninstaller "$INSTDIR\uninstall.exe"

!ifdef REG_START_MENU
!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
CreateDirectory "$SMPROGRAMS\$SM_Folder"
CreateShortCut "$SMPROGRAMS\$SM_Folder\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
CreateShortCut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
CreateShortCut "$SMPROGRAMS\$SM_Folder\Uninstall ${APP_NAME}.lnk" "$INSTDIR\uninstall.exe"

!ifdef WEB_SITE
WriteIniStr "$INSTDIR\${APP_NAME} website.url" "InternetShortcut" "URL" "${WEB_SITE}"
CreateShortCut "$SMPROGRAMS\$SM_Folder\${APP_NAME} Website.lnk" "$INSTDIR\${APP_NAME} website.url"
!endif
!insertmacro MUI_STARTMENU_WRITE_END
!endif

!ifndef REG_START_MENU
CreateDirectory "$SMPROGRAMS\SMTrader"
CreateShortCut "$SMPROGRAMS\SMTrader\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
CreateShortCut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
CreateShortCut "$SMPROGRAMS\SMTrader\Uninstall ${APP_NAME}.lnk" "$INSTDIR\uninstall.exe"

!ifdef WEB_SITE
WriteIniStr "$INSTDIR\${APP_NAME} website.url" "InternetShortcut" "URL" "${WEB_SITE}"
CreateShortCut "$SMPROGRAMS\SMTrader\${APP_NAME} Website.lnk" "$INSTDIR\${APP_NAME} website.url"
!endif
!endif

WriteRegStr ${REG_ROOT} "${REG_APP_PATH}" "" "$INSTDIR\${MAIN_APP_EXE}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "DisplayName" "${APP_NAME}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "UninstallString" "$INSTDIR\uninstall.exe"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "DisplayIcon" "$INSTDIR\${MAIN_APP_EXE}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "DisplayVersion" "${VERSION}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "Publisher" "${COMP_NAME}"

!ifdef WEB_SITE
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "URLInfoAbout" "${WEB_SITE}"
!endif
SectionEnd

######################################################################

Section Uninstall
${INSTALL_TYPE}
Delete "$INSTDIR\accountlist.dat"
Delete "$INSTDIR\AxInterop.HDFCommAgentLib.dll"
Delete "$INSTDIR\chartdir.lic"
Delete "$INSTDIR\chartdir60.dll"
Delete "$INSTDIR\CodeMaster.dll"
Delete "$INSTDIR\CommApi.dll"
Delete "$INSTDIR\CppUnit.dll"
Delete "$INSTDIR\CreVolDBC.dll"
Delete "$INSTDIR\CreVolXMLSpy.dll"
Delete "$INSTDIR\fundlist.dat"
Delete "$INSTDIR\HDFCommAgent.ocx"
Delete "$INSTDIR\HDFCommClient.exe"
Delete "$INSTDIR\HDFCommClientCSharp.exe"
Delete "$INSTDIR\HDFComms.exe"
Delete "$INSTDIR\Interop.HDFCommAgentLib.dll"
Delete "$INSTDIR\LinkInfoMng.dll"
Delete "$INSTDIR\mfc100.dll"
Delete "$INSTDIR\MiraePKIX_v4.dll"
Delete "$INSTDIR\msvcp100.dll"
Delete "$INSTDIR\msvcr100.dll"
Delete "$INSTDIR\nsldap32v11.dll"
Delete "$INSTDIR\nsldap32v50.dll"
Delete "$INSTDIR\orderwndlist.dat"
Delete "$INSTDIR\PocoData.dll"
Delete "$INSTDIR\PocoDataMySQL.dll"
Delete "$INSTDIR\PocoFoundation.dll"
Delete "$INSTDIR\PocoJSON.dll"
Delete "$INSTDIR\PocoNet.dll"
Delete "$INSTDIR\PocoUtil.dll"
Delete "$INSTDIR\PocoXML.dll"
Delete "$INSTDIR\product.cod"
Delete "$INSTDIR\SFSmartCard.dll"
Delete "$INSTDIR\skcommcm.dll"
Delete "$INSTDIR\skcommem.dll"
Delete "$INSTDIR\skcommic.dll"
Delete "$INSTDIR\skcommif.dll"
Delete "$INSTDIR\skcommsc.dll"
Delete "$INSTDIR\SmTrader.cfg"
Delete "$INSTDIR\SMTrader.exe"
Delete "$INSTDIR\SMTrader.ini"
Delete "$INSTDIR\SrShare.dll"
Delete "$INSTDIR\systemlist.dat"
Delete "$INSTDIR\xcon30.dll"
Delete "$INSTDIR\XecureASN_v20.dll"
Delete "$INSTDIR\XecureCMP_v20.dll"
Delete "$INSTDIR\XecureCodec_v20.dll"
Delete "$INSTDIR\XecureCRL_v20.dll"
Delete "$INSTDIR\XecureCrypto_v20.dll"
Delete "$INSTDIR\XecureCSP_v20.dll"
Delete "$INSTDIR\XecureCTL_v20.dll"
Delete "$INSTDIR\XecureIO_v20.dll"
Delete "$INSTDIR\XecureLDAP_v20.dll"
Delete "$INSTDIR\XecureOCSP_v20.dll"
Delete "$INSTDIR\XecurePKCS12_v20.dll"
Delete "$INSTDIR\XecurePKCS5.dll"
Delete "$INSTDIR\XecurePKCS5_v20.dll"
Delete "$INSTDIR\XecurePKCS7_v20.dll"
Delete "$INSTDIR\XecurePKCS8_v20.dll"
Delete "$INSTDIR\XecurePKC_v20.dll"
Delete "$INSTDIR\XecurePVD_v20.dll"
Delete "$INSTDIR\XecureTSP_v20.dll"
Delete "$INSTDIR\xwcs_client.dll"
Delete "$INSTDIR\XWebCertMng.dll"
Delete "$INSTDIR\XWebCMP.dll"
Delete "$INSTDIR\XWebCS.dll"
Delete "$INSTDIR\XWebSSL.dll"
Delete "$INSTDIR\XWebUI.dll"
Delete "$INSTDIR\XWebUtil.dll"
Delete "$INSTDIR\★★API_OCX등록.bat"
Delete "$INSTDIR\Xecure3\License\gensysid.exe"
Delete "$INSTDIR\Xecure3\License\license.exp"
Delete "$INSTDIR\Xecure3\filter\xc_filter.txt"
Delete "$INSTDIR\Xecure3\conf\xc_conf.txt"
Delete "$INSTDIR\Xecure3\cert\kmCert.der"
Delete "$INSTDIR\Xecure3\cert\kmPri.key"
Delete "$INSTDIR\Xecure3\cert\signCert.der"
Delete "$INSTDIR\Xecure3\cert\signPri.key"
Delete "$INSTDIR\TrData\0051_국내선물호가.dat"
Delete "$INSTDIR\TrData\0052_국내옵션호가.dat"
Delete "$INSTDIR\TrData\0056_국내주식선물호가.dat"
Delete "$INSTDIR\TrData\0058_상품선물호가실시간.dat"
Delete "$INSTDIR\TrData\0059_상품옵션호가실시간.dat"
Delete "$INSTDIR\TrData\0065_국내선물체결.dat"
Delete "$INSTDIR\TrData\0066_국내옵션체결.dat"
Delete "$INSTDIR\TrData\0068_국내주식선물시세.dat"
Delete "$INSTDIR\TrData\0071_상품선물시세실시간.dat"
Delete "$INSTDIR\TrData\0073_상품옵션시세실시간.dat"
Delete "$INSTDIR\TrData\0075_CME선물호가.dat"
Delete "$INSTDIR\TrData\0076_해외선물호가.dat"
Delete "$INSTDIR\TrData\0077_CME선물체결.dat"
Delete "$INSTDIR\TrData\0078_CME옵션호가.dat"
Delete "$INSTDIR\TrData\0079_CME옵션체결.dat"
Delete "$INSTDIR\TrData\0082_해외선물체결.dat"
Delete "$INSTDIR\TrData\0101_지수실시간.dat"
Delete "$INSTDIR\TrData\0161_뉴스제목실시간.dat"
Delete "$INSTDIR\TrData\0171_FX 시세.dat"
Delete "$INSTDIR\TrData\0181_국내주문실시간접수.dat"
Delete "$INSTDIR\TrData\0182_국내주문실시간미체결.dat"
Delete "$INSTDIR\TrData\0183_국내주문실시간미결제.dat"
Delete "$INSTDIR\TrData\0184_국내주문실시간잔고.dat"
Delete "$INSTDIR\TrData\0185_국내주문실시간체결.dat"
Delete "$INSTDIR\TrData\0186_해외주문실시간미체결.dat"
Delete "$INSTDIR\TrData\0187_해외주문실시간미결제.dat"
Delete "$INSTDIR\TrData\0188_해외주문실시간잔고.dat"
Delete "$INSTDIR\TrData\0189_해외주문실시간체결.dat"
Delete "$INSTDIR\TrData\0190_해외순미결제실시간.dat"
Delete "$INSTDIR\TrData\0191_FX주문실시간미체결.dat"
Delete "$INSTDIR\TrData\0192_FX주문실시간미청산.dat"
Delete "$INSTDIR\TrData\0193_FX주문실시간청산.dat"
Delete "$INSTDIR\TrData\0194_FX실시간자산내역.dat"
Delete "$INSTDIR\TrData\0195_FX실시간요약.dat"
Delete "$INSTDIR\TrData\0196_해외주문실시간접수.dat"
Delete "$INSTDIR\TrData\0197_FX주문실시간접수.dat"
Delete "$INSTDIR\TrData\0202_현물_USD실시간체결.dat"
Delete "$INSTDIR\TrData\0208_영업일취득.dat"
Delete "$INSTDIR\TrData\0210_현물_금실시간체결.dat"
Delete "$INSTDIR\TrData\0211_CME주문실시간미체결.dat"
Delete "$INSTDIR\TrData\0212_CME주문실시간미결제.dat"
Delete "$INSTDIR\TrData\0213_CME주문실시간체결.dat"
Delete "$INSTDIR\TrData\0241_해외옵션호가.dat"
Delete "$INSTDIR\TrData\0242_해외옵션체결.dat"
Delete "$INSTDIR\TrData\0243_해외옵션체결차트.dat"
Delete "$INSTDIR\TrData\0244_해외옵션민감도.dat"
Delete "$INSTDIR\TrData\0261_국내주문실시간접수.dat"
Delete "$INSTDIR\TrData\0262_국내주문실시간미체결.dat"
Delete "$INSTDIR\TrData\0265_국내주문실시간체결.dat"
Delete "$INSTDIR\TrData\0271_CME주문실시간미체결.dat"
Delete "$INSTDIR\TrData\0273_CME주문실시간체결.dat"
Delete "$INSTDIR\TrData\0286_해외주문실시간미체결.dat"
Delete "$INSTDIR\TrData\0289_해외주문실시간체결.dat"
Delete "$INSTDIR\TrData\0296_해외주문실시간접수.dat"
Delete "$INSTDIR\TrData\0310_국내예상체결.dat"
Delete "$INSTDIR\TrData\g11002.DQ0104&.dat"
Delete "$INSTDIR\TrData\g11002.DQ0107&.dat"
Delete "$INSTDIR\TrData\g11002.DQ0110&.dat"
Delete "$INSTDIR\TrData\g11002.DQ0116&.dat"
Delete "$INSTDIR\TrData\g11002.DQ0119&.dat"
Delete "$INSTDIR\TrData\g11002.DQ0122&.dat"
Delete "$INSTDIR\TrData\g11002.DQ0125&.dat"
Delete "$INSTDIR\TrData\g11002.DQ0217&.dat"
Delete "$INSTDIR\TrData\g11002.DQ0242&.dat"
Delete "$INSTDIR\TrData\g11002.DQ0321&.dat"
Delete "$INSTDIR\TrData\g11002.DQ0502&.dat"
Delete "$INSTDIR\TrData\g11002.DQ0509&.dat"
Delete "$INSTDIR\TrData\g11002.DQ0521&.dat"
Delete "$INSTDIR\TrData\g11002.DQ0622&.dat"
Delete "$INSTDIR\TrData\g11002.DQ1211&.dat"
Delete "$INSTDIR\TrData\g11002.DQ1302&.dat"
Delete "$INSTDIR\TrData\g11002.DQ1303&.dat"
Delete "$INSTDIR\TrData\g11002.DQ1305&.dat"
Delete "$INSTDIR\TrData\g11002.DQ1306&.dat"
Delete "$INSTDIR\TrData\g11004.AQ0101%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0128%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0301%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0302%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0303%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0401%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0402%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0403%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0404%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0405%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0408%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0409%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0415%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0450%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0495%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0602%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0605%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0607%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0613%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0636%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0712%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0715%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0725%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0901%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0904%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0906%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0908%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0910%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0911%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0914%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0915%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0920%.dat"
Delete "$INSTDIR\TrData\g11004.AQ0923%.dat"
Delete "$INSTDIR\TrData\g11004.AS0129%.dat"
Delete "$INSTDIR\TrData\g11004.AS0132%.dat"
Delete "$INSTDIR\TrData\g11004.AS0134%.dat"
Delete "$INSTDIR\TrData\g12001.DO1601&.dat"
Delete "$INSTDIR\TrData\g12001.DO1701&.dat"
Delete "$INSTDIR\TrData\g12001.DO1901&.dat"
Delete "$INSTDIR\TrData\g12001.DO2001&.dat"
Delete "$INSTDIR\TrData\g12001.DO2101&.dat"
Delete "$INSTDIR\TrData\g12001.DO2201&.dat"
Delete "$INSTDIR\TrData\g12003.AO0401%.dat"
Delete "$INSTDIR\TrData\g12003.AO0402%.dat"
Delete "$INSTDIR\TrData\g12003.AO0403%.dat"
Delete "$INSTDIR\TrData\g12003.AO0501%.dat"
Delete "$INSTDIR\TrData\g12003.AO0502%.dat"
Delete "$INSTDIR\TrData\g12003.AO0503%.dat"
Delete "$INSTDIR\TrData\l41600.dat"
Delete "$INSTDIR\TrData\l41601.dat"
Delete "$INSTDIR\TrData\l41602.dat"
Delete "$INSTDIR\TrData\l41603.dat"
Delete "$INSTDIR\TrData\l41619.dat"
Delete "$INSTDIR\TrData\l41703.dat"
Delete "$INSTDIR\TrData\n51000.dat"
Delete "$INSTDIR\TrData\n51001.dat"
Delete "$INSTDIR\TrData\n51003.dat"
Delete "$INSTDIR\TrData\n51006.dat"
Delete "$INSTDIR\TrData\o44005.dat"
Delete "$INSTDIR\TrData\o44011.dat"
Delete "$INSTDIR\TrData\o51000.dat"
Delete "$INSTDIR\TrData\o51010.dat"
Delete "$INSTDIR\TrData\o51100.dat"
Delete "$INSTDIR\TrData\o51200.dat"
Delete "$INSTDIR\TrData\o51210.dat"
Delete "$INSTDIR\TrData\o51211.dat"
Delete "$INSTDIR\TrData\s10001.dat"
Delete "$INSTDIR\TrData\s20001.dat"
Delete "$INSTDIR\TrData\s20101.dat"
Delete "$INSTDIR\TrData\s21001.dat"
Delete "$INSTDIR\TrData\s31001.dat"
Delete "$INSTDIR\TrData\s90005.dat"
Delete "$INSTDIR\TrData\v90001.dat"
Delete "$INSTDIR\TrData\v90003.dat"
Delete "$INSTDIR\TrData\x00001.dat"
Delete "$INSTDIR\TrData\x00002.dat"
Delete "$INSTDIR\TrData\x00003.dat"
Delete "$INSTDIR\TrData\x00004.dat"
Delete "$INSTDIR\TrData\x00005.dat"
Delete "$INSTDIR\TrData\x00006.dat"
Delete "$INSTDIR\system\comms.ini"
Delete "$INSTDIR\system\Commsu.ini"
Delete "$INSTDIR\system\Realdata.dat"
Delete "$INSTDIR\mst\afucode.cod"
Delete "$INSTDIR\mst\afuseriescode.cod"
Delete "$INSTDIR\mst\aopcode.cod"
Delete "$INSTDIR\mst\atmaprice.cod"
Delete "$INSTDIR\mst\atmjprice.cod"
Delete "$INSTDIR\mst\atmmprice.cod"
Delete "$INSTDIR\mst\atmprice.cod"
Delete "$INSTDIR\mst\BDAY.cod"
Delete "$INSTDIR\mst\bidcode.cod"
Delete "$INSTDIR\mst\cafcode.cod"
Delete "$INSTDIR\mst\cfucode.cod"
Delete "$INSTDIR\mst\clt1_futcode.cod"
Delete "$INSTDIR\mst\clt1_optcode.cod"
Delete "$INSTDIR\mst\clt1_usdcode.cod"
Delete "$INSTDIR\mst\cmecode.cod"
Delete "$INSTDIR\mst\efucode.cod"
Delete "$INSTDIR\mst\eopcode.cod"
Delete "$INSTDIR\mst\etcicode.cod"
Delete "$INSTDIR\mst\euxcode.cod"
Delete "$INSTDIR\mst\futcode.cod"
Delete "$INSTDIR\mst\FXCODE.cod"
Delete "$INSTDIR\mst\jangtime.cod"
Delete "$INSTDIR\mst\jfucode.cod"
Delete "$INSTDIR\mst\JMCODE.cod"
Delete "$INSTDIR\mst\jopcode.cod"
Delete "$INSTDIR\mst\kqfucode.cod"
Delete "$INSTDIR\mst\kqopcode.cod"
Delete "$INSTDIR\mst\ksdicode.cod"
Delete "$INSTDIR\mst\kspicode.cod"
Delete "$INSTDIR\mst\mccode.cod"
Delete "$INSTDIR\mst\mfucode.cod"
Delete "$INSTDIR\mst\MONTH.cod"
Delete "$INSTDIR\mst\monthcode.cod"
Delete "$INSTDIR\mst\mopcode.cod"
Delete "$INSTDIR\mst\MRKT.cod"
Delete "$INSTDIR\mst\OJMCODE.cod"
Delete "$INSTDIR\mst\OPMCODE.cod"
Delete "$INSTDIR\mst\OPTATM.cod"
Delete "$INSTDIR\mst\optcode.cod"
Delete "$INSTDIR\mst\OPTCSIZ.cod"
Delete "$INSTDIR\mst\OPTINFO.cod"
Delete "$INSTDIR\mst\PMCODE.cod"
Delete "$INSTDIR\mst\stkcode.cod"
Delete "$INSTDIR\mst\xfucode.cod"
Delete "$INSTDIR\log\HDFlogfile_20190123.log"
Delete "$INSTDIR\log\HDFlogfile_20190124.log"
Delete "$INSTDIR\log\HDFlogfile_20190125.log"
Delete "$INSTDIR\log\037704e1\HDFlogfile_20190123.log"
Delete "$INSTDIR\log\037704e1\HDFlogfile_20190124.log"
Delete "$INSTDIR\log\037704e1\HDFlogfile_20190125.log"
Delete "$INSTDIR\log\037704e1\Order_20190123.log"
Delete "$INSTDIR\log\037704e1\Order_20190124.log"
Delete "$INSTDIR\log\037704e1\Order_20190125.log"
Delete "$INSTDIR\Data\hogadec.dat"
Delete "$INSTDIR\Data\국내_uamsg.dat"
Delete "$INSTDIR\Data\해외_uamsg.dat"
 
RmDir "$INSTDIR\Data"
RmDir "$INSTDIR\log\037704e1"
RmDir "$INSTDIR\log"
RmDir "$INSTDIR\mst"
RmDir "$INSTDIR\system"
RmDir "$INSTDIR\TrData"
RmDir "$INSTDIR\Xecure3\cert"
RmDir "$INSTDIR\Xecure3\conf"
RmDir "$INSTDIR\Xecure3\filter"
RmDir "$INSTDIR\Xecure3\License"
 
Delete "$INSTDIR\uninstall.exe"
!ifdef WEB_SITE
Delete "$INSTDIR\${APP_NAME} website.url"
!endif

RmDir "$INSTDIR"

!ifdef REG_START_MENU
!insertmacro MUI_STARTMENU_GETFOLDER "Application" $SM_Folder
Delete "$SMPROGRAMS\$SM_Folder\${APP_NAME}.lnk"
Delete "$SMPROGRAMS\$SM_Folder\Uninstall ${APP_NAME}.lnk"
!ifdef WEB_SITE
Delete "$SMPROGRAMS\$SM_Folder\${APP_NAME} Website.lnk"
!endif
Delete "$DESKTOP\${APP_NAME}.lnk"

RmDir "$SMPROGRAMS\$SM_Folder"
!endif

!ifndef REG_START_MENU
Delete "$SMPROGRAMS\SMTrader\${APP_NAME}.lnk"
Delete "$SMPROGRAMS\SMTrader\Uninstall ${APP_NAME}.lnk"
!ifdef WEB_SITE
Delete "$SMPROGRAMS\SMTrader\${APP_NAME} Website.lnk"
!endif
Delete "$DESKTOP\${APP_NAME}.lnk"

RmDir "$SMPROGRAMS\SMTrader"
!endif

DeleteRegKey ${REG_ROOT} "${REG_APP_PATH}"
DeleteRegKey ${REG_ROOT} "${UNINSTALL_PATH}"
SectionEnd

######################################################################

Function un.onInit
!insertmacro MUI_UNGETLANGUAGE
FunctionEnd

######################################################################

