/*
 * main.cpp
 *
 *  Created on: Nov 30, 2020
 *      Author: kub0x
 */

#include "Registration/Headers/RegServer.hpp"
#include "Rendezvous/Headers/Rendezvous.hpp"
#include "Utils/Crypto/Headers/PKI.hpp"
#include <vector>

int main(int argc, char** argv){
	PKI::GetInstance()->GenSelfSigned();
	//RegServer *reg_server = new RegServer(44444);
	//reg_server->Listen();
	Rendezvous *rendez = new Rendezvous(44444);
	rendez->Listen();
	return 0;
}


