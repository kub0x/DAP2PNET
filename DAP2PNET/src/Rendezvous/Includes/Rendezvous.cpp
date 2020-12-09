/*
 * Rendezvous.cpp
 *
 *  Created on: Dec 9, 2020
 *      Author: kub0x
 */


#include "../Headers/Rendezvous.hpp"
#include <iostream>

Rendezvous::Rendezvous(int port) : ServerSocket(port,true){

}

void Rendezvous::OnRead(ClientSocket *client, std::vector<char*> buff){

}

ClientSocket* Rendezvous::OnAccept(int clientfd, struct sockaddr_in *client_addr){
	ClientSocket *client = ServerSocket::OnAccept(clientfd, client_addr);
	if (client){//non-repeated connection object's alive
		std::cout << "New client!" << std::endl;
		unconn_peers.push_back(client->GetSourceIP());
	}else{
		std::cout << "Object Destroyed!" << std::endl;
	}
	return client;
}
