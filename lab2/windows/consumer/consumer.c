#include <stdio.h>
#include <windows.h>
#include <TCHAR.h>

#define BUFFER_SIZE 100

int main(VOID)
{
	HANDLE ReadHandle, WriteHandle;
	char buffer[BUFFER_SIZE];
	DWORD read;

	ReadHandle = GetStdHandle(STD_INPUT_HANDLE);
	WriteHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	/* have the child read from the pipe */
	if (ReadFile(ReadHandle, buffer, BUFFER_SIZE, &read, NULL))
		printf("child: >%s<", buffer);
	else
		fprintf(stderr, "Child: Error reading from pipe\n");

	togglecase(buffer, strlen(buffer));

	/* have the child write to the pipe */
	if (!WriteFile(WriteHandle, buffer, BUFFER_SIZE, &read, NULL))
		fprintf(stderr, "Error writing to pipe\n");

	return 0;
}