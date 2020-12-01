/*
 * Server.hpp
 *
 *  Created on: Nov 30, 2020
 *      Author: kub0x
 */

#ifndef SRC_REGISTRATION_HEADERS_REGSERVER_HPP_
#define SRC_REGISTRATION_HEADERS_REGSERVER_HPP_

#include "../../Utils/Sockets/Headers/ServerSocket.hpp"

class RegServer : public ServerSocket {

public:
	RegServer(int port);
	void OnRead(ClientSocket* client, std::vector<char*>buf);

};



#endif /* SRC_REGISTRATION_HEADERS_REGSERVER_HPP_ */
