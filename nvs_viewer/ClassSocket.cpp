#include "stdafx.h"
#include "ClassSocket.h"
#include "ClassModbusTCP.h"


ClassSocket::ClassSocket()
{	
}


ClassSocket::~ClassSocket()
{
}

void ClassSocket::OnAccept(int nErrorCode)
{
	CAsyncSocket::OnAccept(nErrorCode);	
	ModbusTCP->OnAccept(nErrorCode);
}

void ClassSocket::OnClose(int nErrorCode)
{
	CAsyncSocket::OnClose(nErrorCode);
	ModbusTCP->OnSocketClose(nErrorCode);
}

void ClassSocket::OnConnect(int nErrorCode)
{
	CAsyncSocket::OnConnect(nErrorCode);
	ModbusTCP->OnConnect(nErrorCode);
}

void ClassSocket::OnReceive(int nErrorCode)
{
	CAsyncSocket::OnReceive(nErrorCode);
	ModbusTCP->OnReceive(nErrorCode);
}

#include "pch.h"