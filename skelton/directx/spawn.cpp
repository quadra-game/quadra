/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "types.h"
#include "error.h"
#include "spawn.h"

class ProcessWin32: public Process {
	PROCESS_INFORMATION pi;
	unsigned long exit_code;
public:
	ProcessWin32();
	virtual void init(const char *fn, int argc, char *argv[]);
	virtual bool done();
	virtual int get_exit_code();
};


Process *Process::alloc() {
	return new ProcessWin32();
}

ProcessWin32::ProcessWin32() {
	memset(&pi, 0, sizeof(pi));
}

void ProcessWin32::init(const char *fn, int argc, char *argv[]) {
	STARTUPINFO si;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	si.hStdInput = NULL;
	si.hStdOutput = NULL;
	si.hStdError = NULL;
	char full_cmd[2048];
	strcpy(full_cmd, fn);
	int i;
	for(i=0; i<argc; i++) {
		strcat(full_cmd, " ");
		strcat(full_cmd, argv[i]);
	}
	if(!CreateProcess(NULL, full_cmd, NULL, NULL, FALSE, DETACHED_PROCESS, NULL, NULL, &si, &pi)) {
		skelton_msgbox("Error creating external process: %d\n", GetLastError());
		return;
	}
	skelton_msgbox("Process started (PID: %d)\n", pi.dwProcessId);
}

bool ProcessWin32::done() {
	if(!GetExitCodeProcess(pi.hProcess, &exit_code)) {
		skelton_msgbox("Error getting ExitCode: %d\n", GetLastError());
		return false;
	}
	return exit_code != STILL_ACTIVE;
}

int ProcessWin32::get_exit_code() {
	return (int)exit_code;
}
