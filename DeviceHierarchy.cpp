// DeviceHierarchy.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <cfgmgr32.h>
#include <io.h>
#include <fcntl.h>

void Padding(_In_ int pad)
{
	for (int c = 0; c < pad; c++) 
	{
		_fputts(TEXT("    "), stdout);
	}
}

#define BUFFER_SIZE 1024

void printNodeInfo(DEVINST& node, int padding)
{
	TCHAR buf[BUFFER_SIZE];
	if (CR_SUCCESS == CM_Get_Device_ID(node, buf, BUFFER_SIZE, 0)) {
		Padding(padding);
		_tprintf(TEXT("ID: %s\n"), buf);
	}

	DWORD dataType;
	DWORD size = BUFFER_SIZE;
	int ret = CM_Get_DevNode_Registry_Property(node, CM_DRP_FRIENDLYNAME, &dataType, buf, &size, 0);
	size = BUFFER_SIZE;
	if (ret != CR_SUCCESS) {
		ret = CM_Get_DevNode_Registry_Property(node, CM_DRP_DEVICEDESC, &dataType, buf, &size, 0);
		if (ret != CR_SUCCESS) {
			TCHAR name[] = TEXT("Unknown Device");
			_tcsncpy_s(buf, name, sizeof(name));
		}
	}

	Padding(padding);
	_tprintf(TEXT("NAME: %s\n"), buf);
}

void printNodeTree(DEVINST& root, int padding)
{
	DEVINST dInst = 0;

	int ret = CM_Get_Child(&dInst, root, 0);
	if (ret != CR_SUCCESS) return;

	printNodeInfo(dInst, padding);
	printNodeTree(dInst, padding + 1);

	while ((ret = CM_Get_Sibling(&dInst, dInst, 0)) == CR_SUCCESS)
	{
		printNodeInfo(dInst, padding);
		printNodeTree(dInst, padding + 1);
	}
}

int _tmain(_In_ int argc, _In_reads_(argc) PWSTR* argv)
{
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);

	DEVINST dInst = 0;
	int ret = CM_Locate_DevNode(&dInst, NULL, CM_LOCATE_DEVNODE_NORMAL);
	if (ret == CR_SUCCESS) {
		printNodeInfo(dInst, 0);
		printNodeTree(dInst, 1);
	}

	return 0;
}
