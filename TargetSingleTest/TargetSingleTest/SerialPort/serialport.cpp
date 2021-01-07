//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "serialport.h"
#include <process.h>
#include <iostream>

/**thread is existed */ 
bool CSerialPort::s_bExit = false;
/**  */ 
const UINT SLEEP_TIME_INTERVAL = 5;

CSerialPort::CSerialPort(void)
: m_hListenThread(INVALID_HANDLE_VALUE)
{
	m_hComm = INVALID_HANDLE_VALUE;
	m_hListenThread = INVALID_HANDLE_VALUE;

	InitializeCriticalSection(&m_csCommunicationSync);

}

CSerialPort::~CSerialPort(void)
{
	CloseListenTread();
	ClosePort();
	DeleteCriticalSection(&m_csCommunicationSync);
}

bool CSerialPort::InitPort(CWnd *pPortOwner, UINT portNo /*= 1*/,UINT baud /*= CBR_9600*/,char parity /*= 'N'*/,
						    UINT databits /*= 8*/, UINT stopsbits /*= 1*/,DWORD dwCommEvents /*= EV_RXCHAR*/ )
{

	/**  */ 
	char szDCBparam[50];
	sprintf_s(szDCBparam, "baud=%d parity=%c data=%d stop=%d", baud, parity, databits, stopsbits);

	m_pOwner = pPortOwner;
	m_nPortNr = portNo;

	/***/ 
	if (!openPort(portNo))
	{
		return false;
	}




	/**  */ 
	EnterCriticalSection(&m_csCommunicationSync);

	/** */ 
	BOOL bIsSuccess = TRUE;

    /**
	 */
	/*if (bIsSuccess )
	{
		bIsSuccess = SetupComm(m_hComm,10,10);
	}*/

	/** */
	COMMTIMEOUTS  CommTimeouts;
	CommTimeouts.ReadIntervalTimeout         = 0;
	CommTimeouts.ReadTotalTimeoutMultiplier  = 0;
	CommTimeouts.ReadTotalTimeoutConstant    = 0;
	CommTimeouts.WriteTotalTimeoutMultiplier = 0;
	CommTimeouts.WriteTotalTimeoutConstant   = 0; 
	if ( bIsSuccess)
	{
		bIsSuccess = SetCommTimeouts(m_hComm, &CommTimeouts);
	}

	DCB  dcb;
	if ( bIsSuccess )
	{
		// 
		//DWORD dwNum = MultiByteToWideChar (CP_ACP, 0, szDCBparam, -1, NULL, 0);
		//wchar_t *pwText = new wchar_t[dwNum] ;
		//if (!MultiByteToWideChar (CP_ACP, 0, szDCBparam, -1, pwText, dwNum))
		//{
		//	bIsSuccess = TRUE;
		//}

		///**  */ 
		//bIsSuccess = GetCommState(m_hComm, &dcb) && BuildCommDCB(pwText, &dcb) ;
		///**  */ 
		//dcb.fRtsControl = RTS_CONTROL_ENABLE; 

		///** */ 
		//delete [] pwText;

		bIsSuccess = GetCommState(m_hComm, &dcb) && BuildCommDCB(szDCBparam, &dcb) ;
		/**  */ 
		dcb.fRtsControl = RTS_CONTROL_ENABLE; 



	}

	if ( bIsSuccess )
	{
		/**  */ 
		bIsSuccess = SetCommState(m_hComm, &dcb);
	}
		
	/**  */
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	/**  */ 
	LeaveCriticalSection(&m_csCommunicationSync);

	return bIsSuccess==TRUE;
}

bool CSerialPort::InitPort(CWnd *pPortOwner, UINT portNo ,const LPDCB& plDCB )
{
	/**  */ 
	m_pOwner = pPortOwner;
	m_nPortNr = portNo;

	if (!openPort(portNo))
	{
		return false;
	}
	
	/** */ 
	EnterCriticalSection(&m_csCommunicationSync);

	/**  */ 
	if (!SetCommState(m_hComm, plDCB))
	{
		return false;
	}

	/**   */
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	/**  */ 
	LeaveCriticalSection(&m_csCommunicationSync);

	return true;
}

void CSerialPort::ClosePort()
{
	/**  */
	if( m_hComm != INVALID_HANDLE_VALUE )
	{
		CloseHandle( m_hComm );
		m_hComm = INVALID_HANDLE_VALUE;
	}
}

bool CSerialPort::openPort( UINT portNo )
{
	/**  */ 
	EnterCriticalSection(&m_csCommunicationSync);

	/**  */ 
    char szPort[50];
	sprintf_s(szPort, "\\\\?\\COM%d", portNo);

	/**  */ 
	m_hComm = CreateFileA(szPort,		                /** �豸��,COM1,COM2�� */ 
						 GENERIC_READ | GENERIC_WRITE,  /** ����ģʽ,��ͬʱ��д */   
						 0,                             /** ����ģʽ,0��ʾ������ */ 
					     NULL,							/** ��ȫ������,һ��ʹ��NULL */ 
					     OPEN_EXISTING,					/** �ò�����ʾ�豸�������,���򴴽�ʧ�� */ 
						 0,    
						 0);    

	/** �����ʧ�ܣ��ͷ���Դ������ */ 
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		LeaveCriticalSection(&m_csCommunicationSync);
		return false;
	}

	/** �˳��ٽ��� */ 
	LeaveCriticalSection(&m_csCommunicationSync);

	return true;
}

bool CSerialPort::OpenListenThread()
{
	/** ����߳��Ƿ��Ѿ������� */ 
	if (m_hListenThread != INVALID_HANDLE_VALUE)
	{
		/** �߳��Ѿ����� */ 
		return false;
	}

	s_bExit = false;
	/** �߳�ID */ 
	UINT threadId;
	/** �����������ݼ����߳� */ 
	m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, ListenThread, this, 0, &threadId);
	if (!m_hListenThread)
	{
		return false;
	}
	/** �����̵߳����ȼ�,������ͨ�߳� */ 
	if (!SetThreadPriority(m_hListenThread, THREAD_PRIORITY_ABOVE_NORMAL))
	{
		return false;
	}

	return true;
}

bool CSerialPort::CloseListenTread()
{	
	if (m_hListenThread != INVALID_HANDLE_VALUE)
	{
		/** ֪ͨ�߳��˳� */ 
		s_bExit = true;

		/** �ȴ��߳��˳� */ 
		Sleep(10);

		/** ���߳̾����Ч */ 
		CloseHandle( m_hListenThread );
		m_hListenThread = INVALID_HANDLE_VALUE;
	}
	return true;
}

UINT CSerialPort::GetBytesInCOM()
{
	DWORD dwError = 0;	/** ������ */ 
	COMSTAT  comstat;   /** COMSTAT�ṹ��,��¼ͨ���豸��״̬��Ϣ */ 
	memset(&comstat, 0, sizeof(COMSTAT));

	UINT BytesInQue = 0;
	/** �ڵ���ReadFile��WriteFile֮ǰ,ͨ�������������ǰ�����Ĵ����־ */ 
	if ( ClearCommError(m_hComm, &dwError, &comstat) )
	{
		BytesInQue = comstat.cbInQue; /** ��ȡ�����뻺�����е��ֽ��� */ 
	}

	return BytesInQue;
}

UINT WINAPI CSerialPort::ListenThread( void* pParam )
{
	/** �õ������ָ�� */ 
	CSerialPort *pSerialPort = reinterpret_cast<CSerialPort*>(pParam);

	// �߳�ѭ��,��ѯ��ʽ��ȡ��������
	while (!pSerialPort->s_bExit) 
	{
		UINT BytesInQue = pSerialPort->GetBytesInCOM();
		/** ����������뻺������������,����Ϣһ���ٲ�ѯ */ 
		if ( BytesInQue == 0 )
		{
			Sleep(SLEEP_TIME_INTERVAL);
			continue;
		}

		/** ��ȡ���뻺�����е����ݲ������ʾ */
		char cRecved = 0x00;
		do
		{
			cRecved = 0x00;
			if(pSerialPort->ReadChar(cRecved) == true)
			{
				//std::cout << cRecved ; 

				::SendMessage((pSerialPort->m_pOwner)->m_hWnd, WM_COMM_RXCHAR, (WPARAM) cRecved, (LPARAM) pSerialPort->m_nPortNr);

				continue;
			}
		}while(--BytesInQue);
	}

	return 0;
}

bool CSerialPort::ReadChar( char &cRecved )
{
	BOOL  bResult     = TRUE;
	DWORD BytesRead   = 0;
	if(m_hComm == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	/** �ٽ������� */ 
	EnterCriticalSection(&m_csCommunicationSync);

	/** �ӻ�������ȡһ���ֽڵ����� */ 
	bResult = ReadFile(m_hComm, &cRecved, 1, &BytesRead, NULL);
	if ((!bResult))
	{ 
		/** ��ȡ������,���Ը��ݸô�����������ԭ�� */ 
		DWORD dwError = GetLastError();

		/** ��մ��ڻ����� */ 
		PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
		LeaveCriticalSection(&m_csCommunicationSync);

		return false;
	}

	/** �뿪�ٽ��� */ 
	LeaveCriticalSection(&m_csCommunicationSync);

	return (BytesRead == 1);

}

bool CSerialPort::WriteData( unsigned char* pData, unsigned int length )
{
	CString strTemp;
	for(int i=0; i<length; i++)
	{
		strTemp.Format(_T("%02X "),pData[i]);
		OutputDebugString(strTemp);
	}
	OutputDebugString(_T("\n"));

	BOOL   bResult     = TRUE;
	DWORD  BytesToSend = 0;
	if(m_hComm == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	/** �ٽ������� */ 
	EnterCriticalSection(&m_csCommunicationSync);

	/** �򻺳���д��ָ���������� */ 
	bResult = WriteFile(m_hComm, pData, length, &BytesToSend, NULL);
	if (!bResult)  
	{
		DWORD dwError = GetLastError();
		/** ��մ��ڻ����� */ 
		PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
		LeaveCriticalSection(&m_csCommunicationSync);

		return false;
	}

	/** �뿪�ٽ��� */ 
	LeaveCriticalSection(&m_csCommunicationSync);

	return true;
}


