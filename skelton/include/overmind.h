/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_OVERMIND
#define _HEADER_OVERMIND

#include "track.h"
#include "array.h"
#include "types.h"
#include "inter.h"

class Module;
extern Inter* ecran;

class Executor {
	friend class Module;
	friend class Overmind;
	bool paused, self_destruct;
protected:
	Array<Module*> modules;
public:
	bool done;
	Executor(bool self_des=false);
	virtual ~Executor();
	void add(Module* m);
	void remove();
	void pause() {
	  paused = true;
	}
	void unpause() {
		paused = false;
	}
	virtual void step();
};

class Module {
	TRACKED;
	friend class Executor;
protected:
	Executor* parent;
	bool first_time;
	bool done;
public:
	Module();
	virtual ~Module();

	//will be called repeatedly by the executor until 'ret' is called
	virtual void step();

	//will be called once by the executor to initialise
	virtual void init();

	//'this' will be deleted by the executor
	//'module' will replace it and start executing
	void exec(Module* module);

	//'this' will be put on hold
	//'module' will start execution
	//'this' will resume after 'module' calls 'ret'
	void call(Module* module);

	//'this' will be deleted by the executor
	//the caller will resume execution
	//if there is no caller, the executor will terminate
	void ret();
};

class Module_thread: public Module {
public:
	Module_thread();
};

class Overmind {
protected:
	Array<Executor*> execs;
	bool paused;
public:
	bool done;
	Dword framecount;
	Overmind();
	virtual ~Overmind();
	void step();
	void start(Executor* e);
	void stop(Executor* e);
	void pause();
	void unpause();
	void clean_up();
};

class Menu: public Module {
	Inter* old_ecran;
protected:
	Inter* inter;
	Zone* result;
public:
	Palette pal;
	Menu(Inter* base=NULL);
	virtual ~Menu();
	virtual void init();
	virtual void step();
};

extern Overmind overmind;

#endif
