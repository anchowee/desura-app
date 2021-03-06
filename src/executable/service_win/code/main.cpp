/*
Desura is the leading indie game distribution platform
Copyright (C) 2011 Mark Chandler (Desura Net Pty Ltd)

$LicenseInfo:firstyear=2014&license=lgpl$
Copyright (C) 2014, Linden Research, Inc.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation;
version 2.1 of the License only.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, see <http://www.gnu.org/licenses/>
or write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

Linden Research, Inc., 945 Battery Street, San Francisco, CA  94111  USA
$/LicenseInfo$
*/

#include "Common.h"
#include "serviceMain.h"
#include "Tracer.h"

void ControlHandler(DWORD request);

SERVICE_STATUS g_ServiceStatus; 
SERVICE_STATUS_HANDLE g_hStatus; 

CGCServiceApp g_ServiceApp;

class Color;

void LogMsg(MSG_TYPE type, std::string msg, Color* col, std::map<std::string, std::string> *mpArgs)
{
	if (type == MT_TRACE)
		g_Tracer.trace(msg, mpArgs);
	else
		fprintf(stdout, "[LogMsg] %s\n", msg.c_str());
}


#include "DesuraPrintFRedirect.h"




void SetCrashSettings(const wchar_t* user, bool upload)
{
	g_ServiceApp.setCrashSettings(user, upload);
}

void OnPipeDisconnect()
{
	g_ServiceApp.onDisconnect();
}

HANDLE g_WaitEvent = nullptr;
HANDLE g_StopEvent = nullptr;


void WINAPI ServiceMain(DWORD argc, char** argv)
{
	g_WaitEvent = CreateEvent(nullptr, false, false, nullptr);

	g_ServiceStatus.dwServiceType        = SERVICE_WIN32; 
	g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwServiceSpecificExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 0;
	g_ServiceStatus.dwWaitHint = 0;

	g_hStatus = RegisterServiceCtrlHandler(SERVICE_NAME, (LPHANDLER_FUNCTION)ControlHandler);

	if (!g_ServiceApp.start(argc, argv))
	{
		g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		g_ServiceStatus.dwWin32ExitCode = -1;
		SetServiceStatus(g_hStatus, &g_ServiceStatus);
	}
	else
	{
		g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
		SetServiceStatus(g_hStatus, &g_ServiceStatus);

		while (g_ServiceStatus.dwCurrentState == SERVICE_RUNNING)
			WaitForSingleObject(g_WaitEvent, INFINITE);
	}

	g_ServiceApp.stop();

	CloseHandle(g_WaitEvent);
	g_WaitEvent = nullptr;

	if (g_StopEvent)
		SetEvent(g_StopEvent);
}

int main(int argc, char** argv) 
{ 
#ifdef DEBUG
#if 0
	while (!IsDebuggerPresent())
		gcSleep( 500 );
#endif
#endif

	if (GetFileAttributes("desura_service_debug.txt") != 0xFFFFFFFF)
	{
		gcTraceS("Waiting for debugger");

		while (!IsDebuggerPresent())
			gcSleep( 500 );
	}

	if (argc >= 2 && strcmp(argv[1], "-exe")== 0)
	{
		if (g_ServiceApp.start(argc, argv))
		{
			printf("Service has started. Press any key to exit....\n");
			getchar();
		}
		else
		{
			printf("Failed to start service.\n");
		}

		g_ServiceApp.stop();
	}
	else
	{
		SERVICE_TABLE_ENTRY ServiceTable[2];
		ServiceTable[0].lpServiceName = SERVICE_NAME;
		ServiceTable[0].lpServiceProc = &ServiceMain;

		ServiceTable[1].lpServiceName = nullptr;
		ServiceTable[1].lpServiceProc = nullptr;

		g_StopEvent = CreateEvent(nullptr, false, false, nullptr);
		StartServiceCtrlDispatcher(ServiceTable);
		WaitForSingleObject(g_StopEvent, INFINITE);

		CloseHandle(g_StopEvent);
		g_StopEvent = nullptr;
	}
}

void ControlHandler(DWORD request) 
{ 
	switch(request) 
	{ 
	case SERVICE_CONTROL_STOP: 
		g_ServiceStatus.dwWin32ExitCode = 0; 
		g_ServiceStatus.dwCurrentState  = SERVICE_STOPPED; 
		break;
 
	case SERVICE_CONTROL_SHUTDOWN: 
		g_ServiceStatus.dwWin32ExitCode = 0; 
		g_ServiceStatus.dwCurrentState  = SERVICE_STOPPED;
		break;
	} 

	SetEvent(g_WaitEvent);
	SetServiceStatus(g_hStatus,  &g_ServiceStatus);
}