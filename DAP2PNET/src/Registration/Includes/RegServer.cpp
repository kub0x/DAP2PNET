/*
 * RegServer.cpp
 *
 *  Created on: Dec 2, 2020
 *      Author: kub0x
 */
#include <iostream>
#include <string>
#include "../../Utils/Crypto/Headers/PKI.hpp"
#include "../Headers/RegServer.hpp"

//WHETHER USE SSL OR NOT!
RegServer::RegServer(int port) : ServerSocket(port,true){ }

void RegServer::OnRead(ClientSocket *client, std::vector<char*> buff){
	std::cout << "IN REGISTRATION CALLBACK!" << std::endl;
	std::string strCsr;
	for(auto it = buff.begin(); it != buff.end(); ++it)
		strCsr+=*it;
	std::cout << strCsr << std::endl;
	X509 *recv_csr_cert = PKI::GetInstance()->CSRtoX509(strCsr);
	if (recv_csr_cert)
		PKI::GetInstance()->SignCert(recv_csr_cert, std::to_string(client->GetID()));
}
