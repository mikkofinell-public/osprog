#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <TCHAR.h>

#define BUFFER_SIZE 100

int main(VOID)
{
	HANDLE ReadHandle, WriteHandle;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	char message[BUFFER_SIZE] = ("Greetings from Novia");
	DWORD written, read;

	/* set up security attributes so that pipe handles are inherited */
	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL,TRUE };

	/* allocate memory */
	ZeroMemory(&pi, sizeof(pi));

	/* create the pipe */
	if (!CreatePipe(&ReadHandle, &WriteHandle, &sa, 0))
	{
		fprintf(stderr, "Create Pipe Failed\n");
		return 1;
	}

	/* establish the START_INFO structure for the child process */
	GetStartupInfo(&si);
	si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

	/* redirect the standard io to the pipe */
	si.hStdInput = ReadHandle;
	si.hStdOutput = WriteHandle;
	si.dwFlags = STARTF_USESTDHANDLES;

	/* create the child process */
	if (!CreateProcessA(TEXT("consumer.exe"), NULL, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) 
	{
		fprintf(stderr, "Process Creation Failed\n");
		return -1;
	}

	/* the parent now wants to write to the pipe */
	if (!WriteFile(WriteHandle, message, BUFFER_SIZE, &written, NULL))
		fprintf(stderr, "Error writing to pipe\n");

	/* close the write end of the pipe */
	CloseHandle(WriteHandle);

	/* wait for the child to exit */
	WaitForSingleObject(pi.hProcess, INFINITE);

	/* have the parent read from the pipe */
	if (ReadFile(ReadHandle, message, BUFFER_SIZE, &read, NULL))
		printf("parent: >%s<", message);
	else
		fprintf(stderr, "Parent: Error reading from pipe\n");

	/* close all handles */
	CloseHandle(ReadHandle);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}