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

#include "main.h"
#include <mmsystem.h>
#include "types.h"
#include "error.h"
#include "music.h"
#include "command.h"

RCSID("$Id$")

//#define MAX_TRACKS 100

class MusicWin32: public Music {
	MCI_OPEN_PARMS mci_open;
	int playing;
	bool loop_all;
public:
	MusicWin32();
	virtual void open();
	virtual void close();
	virtual void play(int quel, bool loop=false);
	virtual void replay();
	virtual void stop();
};

Music *music=NULL;

Music* Music::alloc() {
  if(!command.token("nocd"))
    return new MusicWin32;
  else
    return new MusicNull;
}

MusicWin32::MusicWin32() {
	//m_nNumberOfTracks = 0;
	open();
}

void MusicWin32::open() {
	active = false;
	playing = -1;
	mci_open.wDeviceID = 0;
	mci_open.lpstrDeviceType = (LPCTSTR)MCI_DEVTYPE_CD_AUDIO;
	if(mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_TYPE|MCI_OPEN_TYPE_ID, (DWORD)&mci_open))
		return;

	MCI_SET_PARMS	mciSet;
	mciSet.dwTimeFormat = MCI_FORMAT_TMSF;
	if(mciSendCommand(mci_open.wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD)&mciSet))
		return;

	active = true;
}

void MusicWin32::close() {
	if(!active)
		return;
	mciSendCommand(mci_open.wDeviceID, MCI_CLOSE, NULL, NULL);
	active = false;
}

/*
short	Music::Read(void)
{
	int i;
	short nTrackLength;

	m_nNumberOfTracks = 0;
	mci_open.lpstrDeviceType = (LPCTSTR)MCI_DEVTYPE_CD_AUDIO;

	if(mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_TYPE|MCI_OPEN_TYPE_ID, (DWORD)(LPVOID)&mci_open))
	{
		return 0;
	}

	m_MCIStatus.dwItem = MCI_STATUS_NUMBER_OF_TRACKS;
	if(mciSendCommand(mci_open.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM|MCI_WAIT, (DWORD)(LPVOID)&m_MCIStatus))
	{
		mciSendCommand(mci_open.wDeviceID, MCI_CLOSE, NULL, NULL);
		return 0;
	}

	m_nNumberOfTracks = (short)m_MCIStatus.dwReturn;
	if(m_nNumberOfTracks > MAX_TRACKS) m_nNumberOfTracks = MAX_TRACKS;
	m_MCIStatus.dwItem = MCI_STATUS_LENGTH;

	for(i = 0; i < m_nNumberOfTracks; i++)
	{
		m_MCIStatus.dwTrack = i + 1;
		mciSendCommand(mci_open.wDeviceID, MCI_STATUS, MCI_TRACK|MCI_STATUS_ITEM|MCI_WAIT,	(DWORD)(LPVOID)&m_MCIStatus);
		nTrackLength = (short)(MCI_MSF_MINUTE(m_MCIStatus.dwReturn)*60 + MCI_MSF_SECOND(m_MCIStatus.dwReturn));
		m_nTrackLength[i] = nTrackLength;
	}

	mciSendCommand(mci_open.wDeviceID, MCI_CLOSE, NULL, NULL);

	return m_nNumberOfTracks;
}

short	Music::GetTrackLength(short nTrack)
{
	if(nTrack > 0 && nTrack <= m_nNumberOfTracks) return m_nTrackLength[nTrack-1];
	else return 0;
}

void Music::SetTrackLength(short nTrack, short nNewLength)
{
	if(nTrack > 0 && nTrack <= m_nNumberOfTracks)
		m_nTrackLength[nTrack-1] = nNewLength;
}

short	Music::GetTotalLength(void)
{
	short nTotalLength = 0;
	short nTrack;

	for(nTrack = 0; nTrack < m_nNumberOfTracks; nTrack++)
		nTotalLength = (short)(nTotalLength + m_nTrackLength[nTrack]);

	return nTotalLength;
}
*/
void MusicWin32::play(int quel, bool loop) {
	if(!active)
		return;
	if(loop)
		playing = 1;
	else
		playing = quel;
	loop_all = loop;
	MCI_PLAY_PARMS mciPlay;

	mciPlay.dwCallback = (DWORD) hwnd;
	mciPlay.dwFrom = MCI_MAKE_TMSF(quel, 0, 0, 0);
	mciPlay.dwTo = MCI_MAKE_TMSF(quel+1, 0, 0, 0);
	if(loop)
		mciSendCommand(mci_open.wDeviceID, MCI_PLAY, MCI_FROM /*| MCI_NOTIFY*/, (DWORD)&mciPlay);
	else
		mciSendCommand(mci_open.wDeviceID, MCI_PLAY, MCI_FROM | MCI_TO/* | MCI_NOTIFY*/, (DWORD)&mciPlay);
	time_control = TIME_FREEZE;
}

void MusicWin32::replay() {
	play(playing, loop_all);
}

void MusicWin32::stop() {
	if(!active || playing == -1)
		return;
	playing = -1;
	MCI_PLAY_PARMS mciPlay;
	mciPlay.dwCallback = 0;
	mciSendCommand(mci_open.wDeviceID, MCI_STOP, NULL, (DWORD)&mciPlay);
}
