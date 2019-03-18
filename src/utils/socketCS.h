// SocketCS.h

#pragma once

/* 
   Socket.h

   Copyright (C) 2002-2004 Ren� Nyffenegger

   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this source code must not be misrepresented; you must not
      claim that you wrote the original source code. If you use this source code
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original source code.

   3. This notice may not be removed or altered from any source distribution.

   Ren� Nyffenegger rene.nyffenegger@adp-gmbh.ch
*/

//
// This library version has been altered from the original.
//

#ifndef SOCKET_H
#define SOCKET_H


#include <WinSock2.h>

#include <string>

#include "ofMain.h"

enum TypeSocket {BlockingSocket, NonBlockingSocket};

class NCSocket {
public:

  virtual ~NCSocket();
  NCSocket(const NCSocket&);
  NCSocket& operator=(NCSocket&);

  std::string ReceiveLine();
  std::string ReceiveBytes();

  int ReceiveBytesInt();

  ofBuffer ReceiveBytesofBuffer(int bytestoreceive);

  void   Close();

  // The parameter of SendLine is not a const reference
  // because SendLine modifes the std::string passed.
  void   SendLine (std::string);

  // The parameter of SendBytes is a const reference
  // because SendBytes does not modify the std::string passed 
  // (in contrast to SendLine).
  void   SendBytes(const char *buf, int len);

protected:
  friend class SocketServer;
  friend class SocketSelect;

  NCSocket(SOCKET s);
  NCSocket();


  SOCKET s_;

  int* refCounter_;

private:
  static void Start();
  static void End();
  static int  nofSockets_;
};

class SocketClient : public NCSocket {
public:
  SocketClient(const std::string& host, int port);
  bool connect();
  std::string host;
  int port;
};

class SocketServer : public NCSocket {
public:
  SocketServer(int port, int connections, TypeSocket type=BlockingSocket);

  NCSocket* Accept();
};

// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winsock/wsapiref_2tiq.asp
class SocketSelect {
  public:
    SocketSelect(NCSocket const * const s1, NCSocket const * const s2=NULL, TypeSocket type=BlockingSocket);

    bool Readable(NCSocket const * const s);

  private:
    fd_set fds_;
}; 



#endif