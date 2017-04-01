//服务启动

#include "Common/Common.h"
#include "LicensePlateUnit.h"


#include <conio.h>

#pragma comment(lib, "advapi32.lib")

#define SVCNAME									_T("IS_LicensePlateUnit")
#define SVC_ERROR                       		((DWORD)0xC0020001L)


SERVICE_STATUS									g_SvcStatus;
SERVICE_STATUS_HANDLE							g_SvcStatusHandle;
HANDLE											g_hSvcStopEvent = NULL;

#define  	STOP_SERVICE_WAIT_INTERVAL			500
#define  	STOP_SERVICE_WAIT_COUNT				10

void 		SvcInstall(void);
void 		SvcUninstall(void);
void 		WINAPI SvcCtrlHandler(DWORD);
void 		WINAPI SvcMain(DWORD, LPTSTR *);
void 		SvcInit(DWORD, LPTSTR *);
void 		ReportSvcStatus(DWORD, DWORD, DWORD);

int Test_tmain(int argc, _TCHAR* argv[])
{
	CLicensePlateUnit	*plate = new CLicensePlateUnit();

	plate->Start();

	getchar();

	plate->Stop();
	delete plate;
	plate = NULL;

	return 0;
}



int __cdecl _tmain(int argc, TCHAR *argv[])
{
	SERVICE_TABLE_ENTRY DispatchTable[] =
	{
		{ SVCNAME, (LPSERVICE_MAIN_FUNCTION)SvcMain },
		{ NULL, NULL }
	};

	TCHAR ppu[MAX_PATH] = { 0 };
	GetPPUPath(ppu, MAX_PATH);
	_tcscat_s(ppu, _T("config\\config.ini"));

	ILog4zManager::getInstance()->config(ppu);
	ILog4zManager::getInstance()->start();

	if (argc == 2 && !strcmp(argv[1], "debug"))
	{
		Test_tmain(argc, argv);
		return 0;
	}

	if (lstrcmpi(argv[1], _T("install")) == 0)
	{
		LOGFMTA("install");
		SvcInstall();
		return 0;
	}

	if (lstrcmpi(argv[1], _T("uninstall")) == 0)
	{
		LOGFMTA("uninstall");
		SvcUninstall();
		return 0;
	}

	if (!StartServiceCtrlDispatcher(DispatchTable))
	{
		LOGFMTA("main: StartServiceCtrlDispatcher Failed, ErrorCode = %u", GetLastError());
	}

	return 0;
}

void SvcInstall()
{
	SC_HANDLE				schSCManager;
	SC_HANDLE				schService;
	TCHAR					szPath[MAX_PATH];
	SERVICE_FAILURE_ACTIONS	svcaction = { 0 };
	SERVICE_DELAYED_AUTO_START_INFO svcdelayed = { 0 };

	if (!GetModuleFileName(NULL, szPath, MAX_PATH))
	{
		LOGFMTA("SvcInstall: GetModuleFileName Failed, ErrorCode = %u", GetLastError());
		return;
	}

	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (NULL == schSCManager)
	{
		LOGFMTA("SvcInstall: OpenSCManager Failed, ErrorCode = %u", GetLastError());
		return;
	}

	schService = CreateService(schSCManager, SVCNAME, SVCNAME, SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS, SERVICE_AUTO_START,
		SERVICE_ERROR_NORMAL, szPath, NULL, NULL, NULL, NULL, NULL);
	if (schService == NULL)
	{
		LOGFMTA("SvcInstall: CreateService Failed, ErrorCode = %u", GetLastError());
		CloseServiceHandle(schSCManager);
		return;
	}

	svcaction.lpRebootMsg = NULL;
	svcaction.dwResetPeriod = 3600 * 24;

	SC_ACTION action[3];

	action[0].Delay = 60 * 1000;
	action[0].Type = SC_ACTION_RESTART;
	action[1].Delay = 60 * 1000;
	action[1].Type = SC_ACTION_RESTART;
	action[2].Delay = 60 * 1000;
	action[2].Type = SC_ACTION_NONE;

	svcaction.cActions = 3;
	svcaction.lpsaActions = action;
	svcaction.lpCommand = NULL;

	if (!ChangeServiceConfig2(schService, SERVICE_CONFIG_FAILURE_ACTIONS, &svcaction))
	{
		LOGFMTA("SvcInstall: ChangeServiceConfig2 svcaction Failed, ErrorCode = %u", GetLastError());
	}

	svcdelayed.fDelayedAutostart = TRUE;
	if (!ChangeServiceConfig2(schService, SERVICE_CONFIG_DELAYED_AUTO_START_INFO, &svcdelayed))
	{
		LOGFMTA("SvcInstall: ChangeServiceConfig2 svcdelayed Failed, ErrorCode = %u", GetLastError());
	}

	if (!StartService(schService, 0, NULL))
	{
		LOGFMTA("SvcInstall: StartService Failed, ErrorCode = %u", GetLastError());
	}

	LOGFMTA("Service %s Installed Successfully", SVCNAME);
	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
}

void SvcUninstall()
{
	SC_HANDLE				schSCManager = NULL;
	SC_HANDLE				schService = NULL;
	SERVICE_STATUS_PROCESS	ssp;
	DWORD					dwBytesNeeded;
	UINT32					i;

	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (NULL == schSCManager)
	{
		LOGFMTA("SvcUninstall: OpenSCManager Failed, ErrorCode = %u", GetLastError());
		goto Cleanup;
	}

	schService = OpenService(schSCManager, SVCNAME, SERVICE_ALL_ACCESS);
	if (schService == NULL)
	{
		LOGFMTA("SvcUninstall: OpenService Failed, ErrorCode = %u", GetLastError());
		goto Cleanup;
	}

	if (ControlService(schService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&ssp))
	{
		for (i = 0; i < STOP_SERVICE_WAIT_COUNT; i++)
		{
			if (!QueryServiceStatusEx(schService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded))
			{
				LOGFMTA("SvcUninstall: QueryServiceStatusEx Failed, ErrorCode = %u", GetLastError());
				break;
			}

			if (ssp.dwCurrentState == SERVICE_STOPPED)
			{
				break;
			}
			Sleep(STOP_SERVICE_WAIT_INTERVAL);
		}
	}
	else
	{
		LOGFMTA("SvcUninstall: ControlService Failed, ErrorCode = %u", GetLastError());
	}

	if (!DeleteService(schService))
	{
		LOGFMTA("SvcUninstall: DeleteService Failed, ErrorCode = %u", GetLastError());
	}
	else
	{
		LOGFMTA("Delete Service %s Successfully", SVCNAME);
	}

Cleanup:
	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
}



void WINAPI SvcMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
	//初始化服务
	g_SvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS;
	g_SvcStatus.dwServiceSpecificExitCode = 0;
	g_SvcStatus.dwCheckPoint = 0;
	g_SvcStatus.dwWaitHint = 0;

	g_SvcStatusHandle = RegisterServiceCtrlHandler(SVCNAME, SvcCtrlHandler);
	if (!g_SvcStatusHandle)
	{
		LOGFMTA("SvcMain: RegisterServiceCtrlHandler Failed, ErrorCode = %u", GetLastError());
		return;
	}
	SvcInit(dwArgc, lpszArgv);
}


void SvcInit(DWORD dwArgc, LPTSTR *lpszArgv)
{
	CLicensePlateUnit	*pMgr = NULL;

	//开始初始化
	g_SvcStatus.dwCurrentState = SERVICE_START_PENDING;
	g_SvcStatus.dwControlsAccepted = 0;
	SetServiceStatus(g_SvcStatusHandle, &g_SvcStatus);

	g_hSvcStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (g_hSvcStopEvent == NULL)
	{
		LOGFMTA("SvcInit: CreateEvent Failed, ErrorCode = %u", GetLastError());
		goto Cleanup;
	}

	pMgr = new CLicensePlateUnit();
	if (pMgr == NULL)
	{
		LOGFMTA("SvcInit: new CLicensePlateUnit Failed");
		goto Cleanup;
	}

	if (!pMgr->Start())
	{
		LOGFMTA("SvcInit: MgrStart Failed");
		goto Cleanup;
	}

	//初始化完成，等待退出
	g_SvcStatus.dwCurrentState = SERVICE_RUNNING;
	g_SvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	SetServiceStatus(g_SvcStatusHandle, &g_SvcStatus);

	//等待退出
	WaitForSingleObject(g_hSvcStopEvent, INFINITE);

Cleanup:
	if (g_hSvcStopEvent != NULL)
	{
		CloseHandle(g_hSvcStopEvent);
		g_hSvcStopEvent = NULL;
	}

	pMgr->Stop();

	if (pMgr != NULL)
	{
		delete pMgr;
		pMgr = NULL;
	}

	g_SvcStatus.dwCurrentState = SERVICE_STOPPED;
	g_SvcStatus.dwControlsAccepted = 0;
	SetServiceStatus(g_SvcStatusHandle, &g_SvcStatus);
}



void WINAPI SvcCtrlHandler(DWORD dwCtrl)
{
	switch (dwCtrl)
	{
	case SERVICE_CONTROL_STOP:
	case SERVICE_CONTROL_SHUTDOWN:
		g_SvcStatus.dwCurrentState = SERVICE_STOP_PENDING;
		g_SvcStatus.dwControlsAccepted = 0;
		SetServiceStatus(g_SvcStatusHandle, &g_SvcStatus);
		SetEvent(g_hSvcStopEvent);
		break;

	default:
		break;
	}
}

