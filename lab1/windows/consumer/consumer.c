#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

#pragma comment(lib, "user32.lib")

#define BUF_SIZE 256
TCHAR szName[] = TEXT("Global\\OsFileMappingObject");
TCHAR szProdSemName[] = TEXT("Global\\ProducerSemaphore");
TCHAR szConsSemName[] = TEXT("Global\\ConsumerSemaphore");

int _tmain() {
	DWORD dwWaitResult;
	HANDLE hProducerSem;
	HANDLE hConsumerSem;
	HANDLE hMapFile;
	LPCTSTR pBuf;

	// Create semaphores ////////////////////////////////////////////////////////////////////////////////////////////////////////

	hProducerSem = CreateSemaphore(NULL, 0, 1, szProdSemName);

	if (hProducerSem == NULL) {
		printf("CreateSemaphore (prod) error: %d\n", GetLastError());
		return 1;
	}

	hConsumerSem = CreateSemaphore(NULL, 0, 1, szConsSemName);

	if (hConsumerSem == NULL) {
		printf("CreateSemaphore (cons) error: %d\n", GetLastError());
		return 1;
	}

	// Wait for producer to signal data ready for processing ///////////////////////////////////////////////////////////////////

	dwWaitResult = WaitForSingleObject(hProducerSem, 60000L);
	if (dwWaitResult == WAIT_TIMEOUT) {
		printf("WAIT TIMEOUT (prod semaphore)\n");
		return 1;
	}

	// Setup shared memory /////////////////////////////////////////////////////////////////////////////////////////////////////

	hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, szName);

	if (hMapFile == NULL) {
		_tprintf(TEXT("Could not open file mapping object (%d).\n"), GetLastError());
		return 1;
	}

	pBuf = (LPTSTR)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BUF_SIZE);

	if (pBuf == NULL) {
		_tprintf(TEXT("Could not map view of file (%d).\n"), GetLastError());
		CloseHandle(hMapFile);
		return 1;
	}

	// Process data in shared memory ////////////////////////////////////////////////////////////////////////////////////////////

	//togglecase((char *)pBuf, lstrlenA(pBuf));

	// Signal producer that data processing is complete /////////////////////////////////////////////////////////////////////////

	if (!ReleaseSemaphore(hConsumerSem, 1, NULL)) {
		printf("ReleaseSemaphore error: %d\n", GetLastError());
	}

	// Cleanup //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	CloseHandle(hProducerSem);
	CloseHandle(hConsumerSem);

	UnmapViewOfFile(pBuf);

	CloseHandle(hMapFile);

	return 0;
}
