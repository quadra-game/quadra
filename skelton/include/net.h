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

#ifndef _HEADER_NET
#define _HEADER_NET

#ifdef UGS_DIRECTX
#include "winsock.h"
#endif

#ifdef UGS_LINUX
#include <errno.h>
#ifndef __STRICT_ANSI__
#define I_SET_STRICT_ANSI
#define __STRICT_ANSI__
#endif
#include <sys/socket.h>
#ifdef I_SET_STRICT_ANSI
#undef I_SET_STRICT_ANSI
#undef __STRICT_ANSI__
#endif
#include <sys/time.h>
#include <netinet/in.h>
#define SOCKET_ERROR -1
#endif

#include "array.h"
#include "types.h"
#include "packet.h"
#include "notify.h"
#include "net_call.h"
#include "id.h"
#include "buf.h"

class IP_addr {
public:
	Dword ip, mask;
	IP_addr(const IP_addr& o);
	IP_addr(const char *addr);
	IP_addr(Dword pip, Dword pmask=0xFFFFFFFF);
	void print(char *st);
	bool operator>(const IP_addr& o);
	bool operator>=(const IP_addr& o);
	void set(const char *addr);
	void set(Dword pip, Dword pmask);
};

class Net_buf;
class Net_connection;
class Net_connection_tcp;

class Packet;

class Net_param {
public:
	virtual bool accept_connection(Net_connection *nc);
	virtual void server_deconnect() { } // handle event in case of server disconnection
	virtual void client_connect(Net_connection *adr) { }
	virtual void client_deconnect(Net_connection *adr) { } // handle event in case of a client disconnection (on server only)
	virtual int tcpport()=0;
	virtual int udpport() {
		return tcpport()+1;
	}
	virtual void print_packet(Packet *p2, char *st)=0;
	virtual Packet *alloc_packet(Word pt)=0;
	virtual bool is_dispatchable(Net_connection *nc, Packet *p) {
		return true;
	}
	virtual Dword magic()=0;
	virtual char *get_motd() {
		return "Hello!";
	}
};

class Exec_ping: public Net_callable {
	static Dword next_uid;
	Net_connection *dest;
	Word type;
	Dword uid;
	Net_callable *net_callable;
public:
	//default d to 0 for clients
	Exec_ping(Packet_ping *p, Word pt, Net_callable *netc, Net_connection *d=NULL);
	virtual ~Exec_ping();
	virtual void net_call(Packet *p2);
	virtual bool validate(Packet_ping *p2);
};

class Buf;

class Net_connection: public Identifyable {
public:
	enum Netstate {
		invalid, valid, waitingfordns, dnslookup, connecting, connected, disconnected
	};
	bool packet_based;
	bool joined;
	bool trusted;
	Dword incoming_inactive;
	Dword outgoing_inactive;
	Dword outgoing_size;
	Dword outgoing_min;
	Dword outgoing_max;
	Dword outgoing_total;
	Dword incoming_size;
	Dword incoming_min;
	Dword incoming_max;
	Dword incoming_total;
	Dword commit_count_in;
	Dword commit_count_out;
	Dword commit_count_both;
	Dword commit_count_total;
	Buf *incoming; //Set only when !packet_based
	virtual Netstate state();
	virtual void connect(Net_connection *dest);
	virtual void connect(Dword adr, int port);
	virtual void connect(const char* host, int port);
	virtual bool checktcp();
	virtual void receivetcp(Net_buf *p);
	virtual int receivetcp(Byte *buf, Dword size);
	virtual void sendtcp(Packet *p2);
	virtual void sendtcp(const Byte *buf, Dword size);
	virtual void commit();
	virtual void disconnect();
	virtual Dword address() const {
		return INADDR_LOOPBACK;
	}
	virtual Dword getdestaddr() const {
		return INADDR_LOOPBACK;
	}
	virtual int getdestport() const {
		return destport;
	}
	virtual Dword getbufsize() const;
	virtual int getFD() const {
		return -1;
	}
	Net_connection();
	virtual ~Net_connection();
protected:
	int destport;
	Netstate _state;
private:
	static int next_port;
	Buf *buf;
	Net_connection *connected_to;
};

class Net_connection_tcp: public Net_connection {
private:
	friend class Net;
	int tcpsock;
	Dword from;
	sockaddr_in tcpsin;
	Byte tcpbuf[1024];
	Dword tcpbufsize;
	Dword tcppacsize;
	char desthost[256];
	Dword destaddr;
	int destport;
	Buf outgoing_buf;
public:
	Net_connection_tcp(int p=0, bool ppacket_based=true);
	Net_connection_tcp(int sock, Dword adr, int port, bool ppacket_based=true);
	virtual ~Net_connection_tcp();
	virtual Netstate state();
	virtual void connect(Dword adr, int port);
	virtual void connect(const char* host, int port);
	virtual bool checktcp();
	virtual void receivetcp(Net_buf *p);
	virtual int receivetcp(Byte *buf, Dword size);
	virtual void sendtcp(Packet *p);
	virtual void sendtcp(const Byte *buf, Dword size);
	virtual void commit();
	virtual Dword address() const {
		return from;
	}
	virtual Dword getdestaddr() const {
		return destaddr;
	}
	virtual int getdestport() const {
		return destport;
	}
	virtual Dword getbufsize() const {
		return tcpbufsize;
	}
	virtual int getFD() const;
};

class Http_request;

class Net: public Observable {
public:
	Array<Net_connection *> connections;
	bool active;
	char *last_error;
	Dword name_resolve;
	int port_resolve;
	char host_name[1024];
	//All the IP addresses of this machine
	Array<Dword> host_adr;
	//Those IP addresses that look like public internet addresses
	Array<Dword> host_adr_pub;

	Net_param *net_param;

	Net(Net_param *np);
	virtual ~Net();

	//Client+Serveur
	Net_connection *server_addr();
	void step(bool loop_only=false);
	void addwatch(Word id, Net_callable *nc);
	void removewatch(Word id, Net_callable *nc);
	void sendudp(Dword to, Packet *p);

	//Server
	void start_server(bool sock);
	void stop_server();
	void suspend_server();
	void sendtcp(Net_connection *nc, Packet *p);
	void dispatch(Packet *p, Dword pt, Net_connection *nc=NULL);

	//Client
	Net_connection *start_loopback_client();
	void start_client(Dword adr, int port=0);
	void stop_client();
	bool connected();
	void sendtcp(Packet *p);

	//All-purpose as long as TCP/IP is there :)
	Net_connection_tcp *start_other(Dword adr, int port);
	Net_connection_tcp *start_other(const char *host, int port);

	//Utils
	Packet *net_buf2packet(Net_buf *nb, bool tcp);

	static Dword dotted2addr(const char *host);
	static void stringaddress(char *st, Dword adr);
	static void stringaddress(char *st, Dword adr, int port);

	Dword getaddress(const char *host);
	void gethostbyname_completed(bool success);
	void gethostbyname_cancel();
	char *failed();
	void init_local_addresses();
	void init_all_udp();
	void close_all_udp();
private:
	friend class Net_connection_tcp;
	friend class Net_connection;
	Net_connection *server_connection;
	Net_connection *client_connection;

	friend class Packet_udp;

	class Net_receive_cb {
	public:
		Word id;
		Net_callable *net_callable;
		Net_receive_cb(Word i, Net_callable *nc) {
			id = i;
			net_callable = nc;
		}
	};
	Array<Net_receive_cb *> callbacks;

	int udpsock[32], udpport, udpnum;
	sockaddr_in udpsin;

	void packetreceived(Net_buf *nb, bool tcp);

	int checkreceive(int s);
	void receiveudp(int sock, Net_buf *p);
	bool accept();
	int open_udpsock(Dword adr);
	void verify_connections();
	void verify_server_connection();

	/* true si erreur (message dans last_error) */
	bool checkerror(int quel);
	/* verifie si erreur etendu dans WSALasterror ou errno */
	bool getlasterror(int quel);
	/* quit avec message d'erreur */
	void callwsa(int quel);

	#ifdef UGS_DIRECTX
		char name_buf[MAXGETHOSTSTRUCT];
		HANDLE name_handle;
	#endif
};

extern Net* net;

#endif
