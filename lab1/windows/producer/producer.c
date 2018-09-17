#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

#define BUF_SIZE 256
TCHAR szName[] = TEXT("Global\\OsFileMappingObject");
TCHAR szProdSemName[] = TEXT("Global\\ProducerSemaphore");
TCHAR szConsSemName[] = TEXT("Global\\ConsumerSemaphore");
TCHAR szMsg[] = TEXT("Studying Operating Systems Is Fun!");

int _tmain() {
	DWORD dwWaitResult;
	HANDLE hProducerSem;
	HANDLE hConsumerSem;
	HANDLE hMapFile;
	LPCTSTR pBuf;

	// Create semaphores ////////////////////////////////////////////////////////////////////////////////////////////////////////

	hProducerSem = CreateSemaphoreA(NULL, 0, 1, szProdSemName);

	if (hProducerSem == NULL) {
		printf("CreateSemaphore (prod) error: %d\n", GetLastError());
		return 1;
	}

	hConsumerSem = CreateSemaphoreA(NULL, 0, 1, szConsSemName);

	if (hConsumerSem == NULL) {
		printf("CreateSemaphore (cons) error: %d\n", GetLastError());
		return 1;
	}

	// Setup shared memory /////////////////////////////////////////////////////////////////////////////////////////////////////

	// motsvarar linux shm_open?
	// INVALID_FILE_HANDLE creates a file mapping object of a specified size that is backed by the system paging file instead of by a file in the file system
	hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, BUF_SIZE, szName);

	if (hMapFile == NULL) {
		_tprintf(TEXT("Could not create file mapping object (%d).\n"), GetLastError());
		return 1;
	}

	// motsvarar linux mmap
	pBuf = (LPTSTR)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BUF_SIZE);

	if (pBuf == NULL) {
		_tprintf(TEXT("Could not map view of file (%d).\n"), GetLastError());
		CloseHandle(hMapFile);
		return 1;
	}

	CopyMemory((PVOID)pBuf, szMsg, (_tcslen(szMsg) * sizeof(TCHAR)));

	// Signal to consumer process and wait ///////////////////////////////////////////////////////////////////////////////////////

	_tprintf(TEXT("Press <enter> to signal consumer that data is ready: "));
	_getch();

	if (!ReleaseSemaphore(hProducerSem, 1, NULL)) {
		printf("ReleaseSemaphore error: %d\n", GetLastError());
	}

	// wait for consumer to finish processing
	dwWaitResult = WaitForSingleObject(hConsumerSem, 5000L);
	if (dwWaitResult == WAIT_TIMEOUT) {
		printf("WAIT TIMEOUT (cons semaphore)\n");
		return 1;
	}

	MessageBox(NULL, pBuf, TEXT("Process1"), MB_OK);

	// Cleanup //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	CloseHandle(hProducerSem);
	CloseHandle(hConsumerSem);

	UnmapViewOfFile(pBuf);

	CloseHandle(hMapFile);

	return 0;
}
