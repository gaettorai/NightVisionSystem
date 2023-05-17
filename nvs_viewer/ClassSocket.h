#pragma once

#include <afxsock.h>


class ClassSocket : public CAsyncSocket
{
public:
	ClassSocket();
	virtual ~ClassSocket();

	virtual void OnAccept(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
};

