
#pragma once
#pragma warning( disable : 4996 )
#include "stdafx.h"
#include <Windows.h>
#include <vector>
#include <list>
#include <iostream>
#include <crtdbg.h>
#include <process.h>
#include "ReadDirectoryChanges.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
/* strftime example */
#include <stdio.h>      /* puts */
#include <time.h>       /* time_t, struct tm, time, localtime, strftime */
#include "VtOutSystemOrderManager.h"
using namespace rdc;

/// <summary>
/// 파일 감시 클래스
/// </summary>
class VtFileEventMonitor {
    /// <summary>
    /// 쓰레드 동기화 클래스
    /// </summary>
    class CriticalSection {
        CRITICAL_SECTION m_cs;
        CriticalSection(const CriticalSection&);
        CriticalSection& operator=(const CriticalSection&);
    public:
        CriticalSection() { ::InitializeCriticalSection(&m_cs); }
        ~CriticalSection() { ::DeleteCriticalSection(&m_cs); }
        void Lock() { ::EnterCriticalSection(&m_cs); }
        void Unlock() { ::LeaveCriticalSection(&m_cs); }
    };
    class AutoLock {
        CriticalSection& m_cs;
    public:
        AutoLock(CriticalSection& cs) : m_cs(cs) { m_cs.Lock(); }
        ~AutoLock() { m_cs.Unlock(); }
    };

	/// <summary>
	/// 최대 감시 핸들 갯수
	/// </summary>
    static const UINT MAX_WAIT_COUNT = 64;

public:
    VtFileEventMonitor()
        : m_sync()
        , m_shutdownevent(::CreateEvent(NULL, TRUE, FALSE, NULL))
        , m_htWorker(NULL)
        , m_uWorkerThreadId(0)
    {
		// 파일 모니터 객체 생성
		changes = new CReadDirectoryChanges(eIoModel, dwMaxChanges);
	}
    virtual ~VtFileEventMonitor()
    {
		// 파일 모니터 객체 종료후 삭제
		if (changes) {
			changes->Terminate();
			delete changes;
			changes = nullptr;
		}

		// 쓰레드 정지
        Stop();
    }

	/// <summary>
	/// 작업 쓰레드 구동 - 이벤트가 올 때 까지 블록된다.
	/// </summary>
	/// <returns></returns>
    bool Start()
    {
		// 작업 쓰레드 생성
        m_htWorker = reinterpret_cast<HANDLE>(::_beginthreadex(NULL,
            0,
            VtFileEventMonitor::_ThreadProc,
            this,
            0,
            &m_uWorkerThreadId));
        if (m_htWorker == NULL)
            return false;
        return true;
    }

	/// <summary>
	/// 작업 쓰레드를 종료하고 모든 관련된 자원을 해제한다.
	/// </summary>
    void Stop()
    {
		// 작업 쓰레드가 있다면 정지 이벤트를 보내고 그 이후에 쓰레드를 정지 시킨다.
        if (m_htWorker != NULL) {
            SetEvent(m_shutdownevent);
            ::WaitForSingleObject(m_htWorker, INFINITE);
            ::CloseHandle(m_htWorker); m_htWorker = NULL;
        }

		// 셧다운 이벤트 핸들 해제
        ::CloseHandle(m_shutdownevent); m_shutdownevent = NULL;
    }

	/// <summary>
	/// 작업 쓰레드의 핸들을 반환한다.
	/// </summary>
	/// <returns></returns>
    HANDLE GetThreadHandle()
    { 
		return m_htWorker; 
	}

	std::vector<std::string> split(const std::string& s, char delimiter)
	{
		std::vector<std::string> tokens;
		std::string token;
		std::istringstream tokenStream(s);
		while (std::getline(tokenStream, token, delimiter))
		{
			tokens.push_back(token);
		}
		return tokens;
	}

	/// <summary>
	/// 파일 변화 이벤트를 처리한다.
	/// </summary>
	/// <param name="action">액션의 종류 : 생성, 삭제, 수정</param>
	/// <param name="fileName">액션이 일어난 파일 이름</param>
	void OnFileChanged(DWORD action, CString fileName)
	{
		const std::string filename = fileName;
		std::string lastline;
		std::ifstream fs;
		fs.open(filename.c_str(), std::fstream::in);
		if (fs.is_open()) {
			//Got to the last character before EOF
			fs.seekg(-1, std::ios_base::end);
			if (fs.peek() == '\n') {
				//Start searching for \n occurrences
				fs.seekg(-1, std::ios_base::cur);
				int i = fs.tellg();
				for (i; i > 0; i--) {
					if (fs.peek() == '\n') {
						//Found
						fs.get();
						break;
					}
					//Move one character back
					fs.seekg(i, std::ios_base::beg);
				}
			}
			getline(fs, lastline);
		}
		else {
			std::cout << "Could not find end line character" << std::endl;
		}
		fs.close();

		std::string msg;
		msg.append(filename);
		msg.append(_T(","));
		msg.append(lastline);
		
		VtOutSystemOrderManager* outSysMgr = VtOutSystemOrderManager::GetInstance();
		outSysMgr->OnOutSignal(msg);

		msg.append(_T("\n"));
		std::cout << msg << std::endl;
		TRACE(msg.c_str());
	}
	/// <summary>
	/// 감시할 디렉토리를 추가한다.
	/// </summary>
	/// <param name="filePath">파일이 있는 디렉토리 경로(파일이름 포함하지 않음)</param>
	/// <param name="includingSubDir">하위 디렉토리 감시여부. 참이면 하위 디렉토리 감시, 거짓이면 지정된 디렉토리만 감시</param>
	void AddMonDir(LPCTSTR filePath, bool includingSubDir)
	{
		changes->AddDirectory(filePath, true, dwNotificationFlags, dwBufSize);
	}
private:
    /// <summary>
    /// 작업 쓰레드 본체 
    /// </summary>
    /// <returns>셧다운 이벤트가 일어날때 쓰레드 루틴을 종료된다.</returns>
    unsigned int ThreadProc()
    {
		const HANDLE handles[] = { m_shutdownevent, changes->GetWaitHandle() };

		char buf[MAX_PATH] = { 0 };
		while (!bTerminate)
		{
			DWORD rc = ::WaitForMultipleObjectsEx(_countof(handles), handles, false, INFINITE, true);
			switch (rc) {
			case WAIT_OBJECT_0 + 0: // Shut down Event
				bTerminate = true;
				break;
			case WAIT_OBJECT_0 + 1: { // We've received a notification in the queue
				if (changes->CheckOverflow()) {
					std::string msg = _T("Queue overflowed.\n");
				}
				else {
					DWORD dwAction;
					CString strFilename;
					changes->Pop(dwAction, strFilename);
					OnFileChanged(dwAction, strFilename);
				}
			}
			break;
			case WAIT_IO_COMPLETION:
				// Nothing to do.
				break;
			}
		}

        return 0;
    }

	/// <summary>
	/// 액션의 종류
	/// </summary>
	/// <param name="dwAction"></param>
	/// <returns></returns>
	LPCWSTR ExplainAction(DWORD dwAction)
	{
		switch (dwAction) {
		case FILE_ACTION_ADDED:
			return L"[Add]";
		case FILE_ACTION_REMOVED:
			return L"[Delete]";
		case FILE_ACTION_MODIFIED:
			return L"[Modified]";
		case FILE_ACTION_RENAMED_OLD_NAME:
			return L"[Renamed From]";
		case FILE_ACTION_RENAMED_NEW_NAME:
			return L"[Renamed To]";
		default:
			return L"[BAD DATA]";
		}
	}

	/// <summary>
	/// 정적 쓰레드 함수 - 객체 멤버 함수를 부르기 위한 클래스 정적 쓰레드 함수
	/// </summary>
	/// <param name="p"></param>
	/// <returns></returns>
    static unsigned int __stdcall _ThreadProc(void* p)
    {
        _ASSERTE(p != NULL);
        return reinterpret_cast<VtFileEventMonitor*>(p)->ThreadProc();
    }

private:
	/// <summary>
	/// 이벤트 처리 모델
	/// </summary>
	EIoModelType eIoModel = eIoModel_CompletionRoutine;
	/// <summary>
	/// 최대 감시 이벤트 갯수
	/// </summary>
	DWORD dwMaxChanges = 4096;
	/// <summary>
	/// 버퍼 크기
	/// </summary>
	DWORD dwBufSize = 16 * 1024;
	/// <summary>
	/// 파일 감시 플래그
	/// </summary>
	const DWORD dwNotificationFlags = FILE_NOTIFY_CHANGE_LAST_WRITE
		| FILE_NOTIFY_CHANGE_CREATION
		| FILE_NOTIFY_CHANGE_FILE_NAME
		| FILE_NOTIFY_CHANGE_DIR_NAME;
	/// <summary>
	/// 파일 감시 클래스 객체 포인터
	/// </summary>
	CReadDirectoryChanges* changes = nullptr;

	/// <summary>
	/// 쓰레드 동기화 객체
	/// </summary>
	CriticalSection m_sync;
	/// <summary>
	/// 셧다운 이벤트 핸들
	/// </summary>
    HANDLE m_shutdownevent;
	/// <summary>
	/// 작업 쓰레드 핸들
	/// </summary>
    HANDLE m_htWorker;
	/// <summary>
	/// 작업 쓰레드 아이디
	/// </summary>
    unsigned m_uWorkerThreadId;
	/// <summary>
	/// 쓰레드 종료 여부 결정
	/// </summary>
	bool bTerminate = false;
};
