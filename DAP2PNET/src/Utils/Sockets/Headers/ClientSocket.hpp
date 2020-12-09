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

	typedef std::function<void(ClientSocket*, std::vector<char*>)> fnOnRead;
	typedef std::function<void(ClientSocket*)> fnOnClose;

protected:
	const int PACKET_LENGTH=1024;
	int sockfd=0, port=0;
	std::string remote_IP, source_IP;
	fnOnRead ptr_read=0;
	fnOnClose ptr_close=0;
	bool repeated_conn=false;
public:
	//Two constructors, one for instantiating the socket with IP:Port.
	//The other having a valid socket descriptor that's already reserved
	ClientSocket(std::string remote_IP, int port);
	ClientSocket(int sockfd, std::string source_IP);
	virtual ~ClientSocket(){}
	//member functions declared virtual in case of needing an override at SSLClientSocket class
	virtual void SetCallBackRead(fnOnRead ptr_read);
	virtual void SetCallBackClose(fnOnClose ptr_close);
	virtual void Connect();
	virtual void Read();
	virtual void Write();
	virtual void Close();
	virtual int GetID(){ return sockfd; }
	std::string GetSourceIP() { return source_IP; }
	void SetRepeatedConnection(bool repeated_conn) { this->repeated_conn=repeated_conn; }
	bool GetRepeatedConnection() { return repeated_conn; }

};



#endif /* SRC_UTILS_HEADERS_CLIENTSOCKET_HPP_ */
