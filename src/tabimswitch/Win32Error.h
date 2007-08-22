#ifndef _WIN32_ERROR_H_
#define _WIN32_ERROR_H_

#include <windows.h>
#include <wchar.h>

class Win32Error
{
public:
    ~Win32Error(void) { reset(); }

    Win32Error(void) : m_errCode(::GetLastError()), m_pszErrorMsg(NULL), m_pwszErrorMsg(NULL) {}
    Win32Error(DWORD error) : m_errCode(error), m_pszErrorMsg(NULL), m_pwszErrorMsg(NULL) { }
    Win32Error(HRESULT hr) : m_errCode(HRESULT_TO_WIN32ERROR(hr)), m_pszErrorMsg(NULL), m_pwszErrorMsg(NULL) {}
    Win32Error(int winsockError) : m_errCode(winsockError), m_pszErrorMsg(NULL), m_pwszErrorMsg(NULL) {}

    void setErrorCode(DWORD win32Error) { m_errCode = win32Error; reset(); }
    void setHResult(HRESULT hr) { m_errCode = HRESULT_TO_WIN32ERROR(hr); reset(); }

    DWORD getErrorCode(void) const { return m_errCode; }
    HRESULT getHResult(void) const { return HRESULT_FROM_WIN32(m_errCode); }

    char const* getString()
    {
        if ( m_pszErrorMsg == NULL )
        {
            DWORD ret = FormatMessageA(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                NULL, m_errCode, 
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                (LPSTR)&m_pszErrorMsg, 0, NULL );
            if ( ret == 0 )
                m_pszErrorMsg = NULL;
            else
            {
                char* pCRLF = strrchr(m_pszErrorMsg, '\r');
                if ( ! pCRLF )
                    pCRLF = strrchr(m_pszErrorMsg, '\n');
                if ( ! pCRLF )
                    *pCRLF = '\0';
            }
        }

        return m_pszErrorMsg;
    }

    wchar_t const* getWString()
    {
        if ( m_pwszErrorMsg == NULL )
        {
            DWORD ret = FormatMessageW(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                NULL, m_errCode, 
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                (LPWSTR)&m_pwszErrorMsg, 0, NULL );
            if ( ret == 0 )
                m_pwszErrorMsg = NULL;
            else
            {
              char* pCRLF = strrchr(m_pszErrorMsg, L'\r');
              if ( ! pCRLF )
                  pCRLF = strrchr(m_pszErrorMsg, L'\n');
              if ( ! pCRLF )
                  *pCRLF = L'\0';
            }
        }

        return m_pwszErrorMsg;
    }

private:
    DWORD m_errCode;
    LPSTR m_pszErrorMsg;
    LPWSTR m_pwszErrorMsg;

private:
    //
    // An "It just works" solution, refer to
    // http://blogs.msdn.com/oldnewthing/archive/2006/11/03/942851.aspx
    //
    static DWORD HRESULT_TO_WIN32ERROR(HRESULT hRes)
    {
        DWORD win32error;
        if ( WIN32_FROM_HRESULT(hRes, &win32error) )
            return win32error;
        else
            return hRes;
    }

    static BOOL WIN32_FROM_HRESULT(HRESULT hr, OUT DWORD *pdwWin32)
    {
        if ((hr & 0xFFFF0000) == MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0)) {
            // Could have come from many values, but we choose this one
            *pdwWin32 = HRESULT_CODE(hr);
            return TRUE;
        }
        if (hr == S_OK) {
            *pdwWin32 = HRESULT_CODE(hr);
            return TRUE;
        }
        // otherwise, we got an impossible value
        return FALSE;
    }

    void reset(void)
    {
        if ( m_pszErrorMsg )
            ::LocalFree((HLOCAL)m_pszErrorMsg);
        m_pszErrorMsg = NULL;

        if ( m_pwszErrorMsg )
            ::LocalFree((HLOCAL)m_pwszErrorMsg);
        m_pwszErrorMsg = NULL;
    }
};

#endif // _WIN32_ERROR_H_
