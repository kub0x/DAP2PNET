/*
 * ServerSocket.hpp
 *
 *  Created on: Nov 30, 2020
 *      Author: kub0x
 */

#ifndef SRC_UTILS_HEADERS_SERVERSOCKET_HPP_
#define SRC_UTILS_HEADERS_SERVERSOCKET_HPP_

#include "../Headers/ClientSocket.hpp"
#include <vector>

class ServerSocket {

protected:
	const int MAX_CONNECTIONS=5;
	int serverfd, port, number_conn=0;
	int enabled_SSL = false;
	void InitSocket();

public:

	ServerSocket(int port);
	ServerSocket(int port, bool enable_SSL);
	virtual ~ServerSocket(){}
	virtual void OnRead(ClientSocket *client, std::vector<char*> buff);
	virtual void OnClose(ClientSocket *client );
	void Listen();
	void Write(int sockfd);
	void Close();
};



#endif /* SRC_UTILS_HEADERS_SERVERSOCKET_HPP_ */
