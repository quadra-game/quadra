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

#include "error.h"
#include "main.h"
#include "sound.h"

RCSID("$Id$")

Sound *sound = NULL;

Sound::Sound() {
	active=false;
	lpds=NULL;
	if(FAILED(DirectSoundCreate(NULL, &lpds, NULL))) {
		return;
	} else {
		if(FAILED(lpds->SetCooperativeLevel(hwnd, DSSCL_PRIORITY))) {
			skelton_msgbox("Sound::Sound: Failed to obtain DSSCL_PRIORITY. Trying DSSCL_NORMAL...\n");
			if(FAILED(lpds->SetCooperativeLevel(hwnd, DSSCL_NORMAL))) {
				skelton_msgbox("Sound::Sound: Failed DSSCL_NORMAL!\n");
				return;
			}
		}
	}

	DSBUFFERDESC dsBD;
	IDirectSoundBuffer *lpPrimary;
  ZeroMemory(&dsBD, sizeof(DSBUFFERDESC));
  dsBD.dwSize = sizeof(dsBD);
  dsBD.dwFlags = DSBCAPS_PRIMARYBUFFER;
  lpds->CreateSoundBuffer(&dsBD, &lpPrimary, NULL);

	WAVEFORMATEX format;
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = 2; // Assume stereo.
	format.nBlockAlign = 2*format.nChannels;
	format.wBitsPerSample = 16; // Assume 16-bit
	format.nSamplesPerSec = 44100; // Assume 44.1 kHz
	format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
	format.cbSize = 0;
	if(FAILED(lpPrimary->SetFormat(&format)))
		skelton_msgbox("Sound::Sound: Failed to SetFormat() to 16bit 44kHz stereo. Ignoring\n");
	lpPrimary->Play(0, 0, DSBPLAY_LOOPING);

	active=true;
}

void Sound::delete_sample(Sample *sam) {
}

Sound::~Sound() {
	if(lpds)
		lpds->Release();
}

Sample::Sample(Res& re, int nb) {
	iAlloc=0;
	if(!sound)
		return;
	LPWAVEFORMATEX pWaveHeader;
	BYTE *pbData;
	UINT cbData;
	void *buf = (void*)re.buf();
	DSParseWaveResource(buf, &pWaveHeader, &pbData, (DWORD *) &cbData);
	int i;
	iCurrent = 0;
	iAlloc = nb;
	pbWaveData = pbData;
	cbWaveSize = cbData;
	buffers[0] = DSLoadSoundBuffer(buf);
	for(i=1; i<iAlloc; i++) {
		calldx(sound->lpds->DuplicateSoundBuffer(buffers[0], &buffers[i]));
		buffers[i] = DSLoadSoundBuffer(buf);
	}
}

Sample::~Sample() {
	int i;
	for (i=0; i<iAlloc; i++)  {
		if(buffers[i])
			buffers[i]->Release();
	}
}

/*
void Sample::DSReloadSoundBuffer(IDirectSoundBuffer *pDSB, LPCTSTR lpName) {
	BYTE *pbWaveData;
	DWORD cbWaveSize;
	DSGetWaveResource(lpName, NULL, &pbWaveData, &cbWaveSize);
	calldx(IDirectSoundBuffer_Restore(pDSB));
	DSFillSoundBuffer(pDSB, pbWaveData, cbWaveSize);
}
*/
IDirectSoundBuffer *Sample::DSLoadSoundBuffer(void *res) {
	IDirectSoundBuffer *pDSB = NULL;
	DSBUFFERDESC dsBD = {0};
	BYTE *pbWaveData;

	DSParseWaveResource(res, &dsBD.lpwfxFormat, &pbWaveData, &dsBD.dwBufferBytes);
	dsBD.dwSize = sizeof(dsBD);
	dsBD.dwFlags = DSBCAPS_STATIC | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2;
	calldx(sound->lpds->CreateSoundBuffer(&dsBD, &pDSB, NULL));
	if(pDSB == NULL)
		new Error("Can't create sound buffer");
	DSFillSoundBuffer(pDSB, pbWaveData, dsBD.dwBufferBytes);
	return pDSB;
}

void Sample::DSFillSoundBuffer(IDirectSoundBuffer *pDSB, BYTE *pbWaveData, DWORD cbWaveSize) {
	if(pbWaveData && cbWaveSize) {
		LPVOID pMem1, pMem2;
		DWORD dwSize1, dwSize2;
		calldx(pDSB->Lock(0, cbWaveSize, &pMem1, &dwSize1, &pMem2, &dwSize2, 0));
		CopyMemory(pMem1, pbWaveData, dwSize1);
		if(0 != dwSize2)
			CopyMemory(pMem2, pbWaveData+dwSize1, dwSize2);
		pDSB->Unlock(pMem1, dwSize1, pMem2, dwSize2);
	} else
		new Error("Can't fill sound buffer");
}

void Sample::DSParseWaveResource(void *res, WAVEFORMATEX **ppWaveHeader, BYTE **ppbWaveData,DWORD *pcbWaveSize) {
	DWORD *pdw;
	DWORD *pdwEnd;
	DWORD dwRiff;
	DWORD dwType;
	DWORD dwLength;
	if(ppWaveHeader)
		*ppWaveHeader = NULL;
	if(ppbWaveData)
		*ppbWaveData = NULL;
	if(pcbWaveSize)
		*pcbWaveSize = 0;
	pdw = (DWORD *)res;
	dwRiff = *pdw++;
	dwLength = *pdw++;
	dwType = *pdw++;

	if(dwRiff != mmioFOURCC('R', 'I', 'F', 'F'))
		new Error("WAV file not 'RIFF' format");

	if(dwType != mmioFOURCC('W', 'A', 'V', 'E'))
		new Error("Not a WAV file!");

	pdwEnd = (DWORD *)((BYTE *)pdw + dwLength-4);

	while(pdw < pdwEnd) {
		dwType = *pdw++;
		dwLength = *pdw++;
		switch(dwType) {
			case mmioFOURCC('f', 'm', 't', ' '):
				if(ppWaveHeader && !*ppWaveHeader) {
					if(dwLength < sizeof(WAVEFORMAT))
						new Error("Not a WAV file! (2)");

					*ppWaveHeader = (WAVEFORMATEX *)pdw;
					if((!ppbWaveData || *ppbWaveData) && (!pcbWaveSize || *pcbWaveSize))
						return;
				}
				break;

			case mmioFOURCC('d', 'a', 't', 'a'):
				if((ppbWaveData && !*ppbWaveData) || (pcbWaveSize && !*pcbWaveSize)) {
					if(ppbWaveData)
						*ppbWaveData = (LPBYTE)pdw;
					if(pcbWaveSize)
						*pcbWaveSize = dwLength;
					if(!ppWaveHeader || *ppWaveHeader)
						return;
				}
				break;
		}
		pdw = (DWORD *)((BYTE *)pdw + ((dwLength+1)&~1));
	}
	new Error("Error parsing WAV file");
}

IDirectSoundBuffer *Sample::getfreebuffer() {
	IDirectSoundBuffer *pDSB = buffers[iCurrent];
	if(pDSB)  {
		DWORD dwStatus;
		calldx(pDSB->GetStatus(&dwStatus));
		if(dwStatus & DSBSTATUS_PLAYING) {
			if(iAlloc > 1)  {
				if(++iCurrent >= iAlloc)
					iCurrent = 0;
				pDSB = buffers[iCurrent];
				calldx(pDSB->GetStatus(&dwStatus));
				if(dwStatus & DSBSTATUS_PLAYING) {
					pDSB->Stop();
					pDSB->SetCurrentPosition(0);
				}
			}
			if(pDSB && (dwStatus & DSBSTATUS_BUFFERLOST)) {
				calldx(pDSB->Restore());
				DSFillSoundBuffer(pDSB, pbWaveData, cbWaveSize);
			}
		}
	}
	return pDSB;
}

void Sample::stop() {
	int i;
	for (i=0; i<iAlloc; i++)  {
		buffers[i]->Stop();
		buffers[i]->SetCurrentPosition(0);
	}
}

Sfx::Sfx(Sample *sam, Dword dwPlayFlags, int vo, int pa, int f, int pos) {
	if(!sound || !sam || !sound->active)
		return;
	buf = sam->getfreebuffer();
	if(buf != NULL) {
		if(pos != -1)
			position(pos);
		if(f != -1)
			freq(f);
		if(vo != -1)
			volume(vo);
		if(pa != -1)
			pan(pa);
		calldx(buf->Play(0, 0, dwPlayFlags));
	}
}

void Sfx::pan(int pa) {  //-4000=left 0=center 4000=right
	calldx(buf->SetPan(pa));
}

void Sfx::freq(int pa) { //200=low  60000=very high
	calldx(buf->SetFrequency(pa));
}

void Sfx::volume(int pa) { //0=full .. -4000=nil
	calldx(buf->SetVolume(pa));
}

void Sfx::position(int pa) {
	calldx(buf->SetCurrentPosition(pa));
}
