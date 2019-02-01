//---------------------------------------------------------------------------
// File Name : ESApiMessage.h
// Date      : 2015-11-04
// Creation  : 정 치영
// Update    : 
// Comment   : User Message의 기본값을 정의
//---------------------------------------------------------------------------


#ifndef __ESAPIMESSAGE_H__
#define __ESAPIMESSAGE_H__

enum
{
	// API Dll => user program
	WM_ESM_RECVDATA		= WM_USER + 0x8000,				// 서버에서 받은 데이터
														// wParam : 해당 tr 번호, lParam : 해당 데이터
	WM_ESM_ACCTLIST,									// 계좌 리스트
														// wParam : 계좌 갯수, lParam : 계좌 리스트의 포인트
	WM_ESM_CODELIST,									// 종목 정보 리스트
														// wParam : 종목 갯수, lParam : 종목 리스트의 포인트
	WM_ESM_CONNECT,										// 접속 완료 이벤트
	WM_ESM_DISCONNECT,									// 접속 종료 이벤트
	WM_ESM_CMEACCTLIST,									// 계좌 리스트 (국내만 사용)
};





#endif	// __MESSAGE_H__
