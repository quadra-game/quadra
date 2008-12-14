/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * 
 * Quadra, an action puzzle game
 * Copyright (C) 1998-2000  Ludus Design
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "overmind.h"
#include "input.h"

Overmind overmind;
Inter* ecran = NULL;

Overmind::Overmind() {
	framecount = 0;
	paused = false;
	done = false;
}

Overmind::~Overmind() {
	clean_up();
}

void Overmind::clean_up() {
	while(execs.size()) {
		Executor *e = execs.last();
		if(e && e->self_destruct)
			delete e;
		execs.removelast();
	}
}

void Overmind::pause() {
	paused = true;
}

void Overmind::unpause() {
	paused = false;
}

void Overmind::step() {
	if(paused)
		return;
	framecount++;
	for(int i=0; i<execs.size(); i++) {
		Executor *e = execs[i];
		if(!e) {
			execs.remove(i);
			i--;
		} else {
			e->step();
			if(e->done) {
				if(e->self_destruct)
					delete e;
				execs.remove(i);
				i--;
			}
		}
	}
	if(execs.size() == 0)
		done = true;
}

void Overmind::start(Executor* e) {
	execs.add(e);
	done = false;
}

void Overmind::stop(Executor* e) {
	for(int i=0; i<execs.size(); i++) {
		if(e == execs[i]) {
			if(e->self_destruct)
				delete e;
			execs.replace(i, NULL);
		}
	}
}

Executor::Executor(bool self_des) {
	done = paused = false;
	self_destruct = self_des;
}

Executor::~Executor() {
	while(!modules.empty())
		remove();
}

void Executor::remove() {
	delete modules.at(modules.size() - 1);
	modules.pop_back();
}

void Executor::step() {
	if(paused)
		return;
	if(!modules.empty()) {
		Module* last_module = modules.at(modules.size() - 1);
		if(!last_module->done) {
			if(last_module->first_time) {
				last_module->first_time = false;
				last_module->init();
			} else {
				last_module->step();
			}
		}
		while(!modules.empty() && modules.at(modules.size() - 1)->done) {
			remove();
		}
	}
	if(modules.empty())
		done=true;
}

void Executor::add(Module* m) {
	modules.push_back(m);
	m->parent = this;
}

Module::Module(): done(false) {
	first_time = true;
}

Module::~Module() {
}

void Module::step() {
}

void Module::init() {
}

void Module::exec(Module* module) {
	call(module);
	ret();
}

void Module::call(Module* module) {
	parent->add(module);
}

void Module::ret() {
	done=true;
}

Module_thread::Module_thread() {
	parent = new Executor(true);
	parent->add(this);
	overmind.start(parent);
}

Menu::Menu(Inter* base) {
	if(base)
		inter=new Inter(base);
	else
		inter=new Inter();
	result=NULL;
	old_ecran=ecran;
}

Menu::~Menu() {
	delete inter;
	ecran=old_ecran;
	if(ecran)
		ecran->dirt_all();
}

void Menu::init() {
	old_ecran=ecran;
	ecran = inter;
	input->allow_key_repeat(true);
}

void Menu::step() {
	ecran = inter;
	if(ecran) {
		result = ecran->do_frame();
	} else
		result = NULL;
}
