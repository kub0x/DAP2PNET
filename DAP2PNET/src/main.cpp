/*
 * main.cpp
 *
 *  Created on: Nov 30, 2020
 *      Author: kub0x
 */

#include "Utils/Sockets/Headers/ServerSocket.hpp"
#include "Utils/Crypto/Headers/PKI.hpp"

int main(int argc, char** argv){
	PKI::GetInstance()->GenSelfSigned();
	ServerSocket *server = new ServerSocket(44444);
	server->Listen();
	return 0;
}


