#include "stdafx.h"
#include "ReadDirectoryRequest.h"
#include "ReadDirectoryServer.h"
#include "ReadDirectoryChanges.h"

namespace rdc
{

CReadDirectoryRequest::CReadDirectoryRequest(CReadDirectoryServer* pServer,
    const CString& strDirectory, BOOL bWatchSubTree, DWORD dwNotifyFilter, DWORD dwBufSize) :
    m_pServer(pServer),
    m_hDirectory(INVALID_HANDLE_VALUE),
    m_strDirectory(strDirectory),
    m_bWatchSubTree(bWatchSubTree),
    m_dwNotitfyFilter(dwNotifyFilter)
{
    ::ZeroMemory(this, sizeof(OVERLAPPED));
    m_vecBuffer.resize(dwBufSize);
    m_vecBackupBuffer.resize(dwBufSize);
}

CReadDirectoryRequest::~CReadDirectoryRequest()
{
    _ASSERT(m_hDirectory == INVALID_HANDLE_VALUE);
}

bool CReadDirectoryRequest::OpenDirectory()
{
    if (IsDirectoryOpened())
    {
        return true;
    }

    m_hDirectory = ::CreateFile(m_strDirectory,
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_DIRECTORY | FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        NULL);
    return (m_hDirectory != INVALID_HANDLE_VALUE);
}

void CReadDirectoryRequest::CancelRequest(void)
{
    if (IsDirectoryOpened())
    {
        ::CancelIo(m_hDirectory);
        ::CloseHandle(m_hDirectory);
        m_hDirectory = INVALID_HANDLE_VALUE;
    }
}

CReadDirectoryServer* CReadDirectoryRequest::GetServer(void)
{
    return m_pServer;
}

HANDLE CReadDirectoryRequest::GetDirectoryHandle(void)
{
    return m_hDirectory;
}

CString CReadDirectoryRequest::GetDirectory(void)
{
    return m_strDirectory;
}

void CReadDirectoryRequest::BackupBuffer(DWORD dwSize)
{
    _ASSERT(dwSize <= m_vecBackupBuffer.size());
    memcpy(&m_vecBackupBuffer[0], &m_vecBuffer[0], dwSize);
}

void CReadDirectoryRequest::ProcessNotification()
{
    _ASSERT(m_pServer);
    BYTE* pBase = m_vecBackupBuffer.data();
    for (;;)
    {
        FILE_NOTIFY_INFORMATION& fni = (FILE_NOTIFY_INFORMATION&)*pBase;
        CString strFileName(fni.FileName, fni.FileNameLength / sizeof(wchar_t));
        // Handle a trailing backslash, such as for a root directory.
        if (m_strDirectory.Right(1) != _T("\\"))
        {
            strFileName = m_strDirectory + _T("\\") + strFileName;
        }
        else
        {
            strFileName = m_strDirectory + strFileName;
        }

        // If it could be a short filename, expand it.
        LPCTSTR wszFileName = PathFindFileName(strFileName);
        int len = _tcsclen(wszFileName);
        // The maximum length of an 8.3 filename is twelve, including the dot.
        if (len <= 12 &&  _tcschr(wszFileName, _T('~')))
        {
            // Convert to the long filename form. Unfortunately, this
            // does not work for deletions, so it's an imperfect fix.
            TCHAR wbuf[MAX_PATH];
            if (::GetLongPathName(strFileName, wbuf, _countof(wbuf)) > 0)
            {
                strFileName = wbuf;
            }
        }

        m_pServer->GetHost()->Push(fni.Action, strFileName);
        if (!fni.NextEntryOffset)
        {
            break;
        }
        pBase += fni.NextEntryOffset;
    };
}

bool CReadDirectoryRequest::IsDirectoryOpened(void) const
{
    return (m_hDirectory != INVALID_HANDLE_VALUE);
}

void CReadDirectoryRequest::RemoveRequest(void)
{
    if (m_pServer)
    {
        m_pServer->RemoveRequest(this);
    }
}

BYTE* CReadDirectoryRequest::GetBuffer(void)
{
    return &m_vecBuffer[0];
}

DWORD CReadDirectoryRequest::GetBufferSize(void)
{
    return static_cast<DWORD>(m_vecBuffer.size());
}

BOOL CReadDirectoryRequest::IsWatchSubTree(void)
{
    return m_bWatchSubTree;
}

DWORD CReadDirectoryRequest::GetNotifyFilter(void)
{
    return m_dwNotitfyFilter;
}

}