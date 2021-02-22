/*
 * Rendezvous.hpp
 *
 *  Created on: Dec 8, 2020
 *      Author: kub0x
 */

#ifndef SRC_RENDEZVOUS_HEADERS_RENDEZVOUS_HPP_
#define SRC_RENDEZVOUS_HEADERS_RENDEZVOUS_HPP_

#include "../../Utils/Sockets/Headers/ServerSocket.hpp"

class Rendezvous : public ServerSocket{

private:
	const int MAX_PEERS = 5, MAX_TIMEOUT = 30000;
	std::vector<std::string> peers;
	void UpdatePeerList();
	bool OnUpdatePeerList();

public:

	Rendezvous(int port);
	void OnRead(ClientSocket *client, std::vector<char*> buff);
	//void OnClose(ClientSocket *client);
	ClientSocket* OnAccept(int clientfd, struct sockaddr_in *client_addr);

};



#endif /* SRC_RENDEZVOUS_HEADERS_RENDEZVOUS_HPP_ */
