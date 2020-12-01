/*
 * ClientSocket.hpp
 *
 *  Created on: Nov 30, 2020
 *      Author: kub0x
 */

#ifndef SRC_UTILS_HEADERS_CLIENTSOCKET_HPP_
#define SRC_UTILS_HEADERS_CLIENTSOCKET_HPP_

#include <string>
#include <functional>
#include <vector>

class ClientSocket{

private:
	const int PACKET_LENGTH=1024;
	int sockfd=0, port=0;
	std::string IP;
	std::function<void(ClientSocket*, std::vector<char*>)> ptr_read=0;
	std::function<void(ClientSocket*)> ptr_close=0;
public:
	//Two constructors, one for instantiating the socket with IP:Port.
	//The other having a valid socket descriptor that's already reserved
	ClientSocket(std::string IP, int port);
	ClientSocket(int sockfd);
	void SetCallBackRead(std::function<void(ClientSocket*, std::vector<char*>)> ptr_read);
	void SetCallBackClose(std::function<void(ClientSocket*)> ptr_close);
	void Connect();
	void Read();
	void Write();
	void Close();
	int GetID(){ return sockfd; }

};



#endif /* SRC_UTILS_HEADERS_CLIENTSOCKET_HPP_ */
