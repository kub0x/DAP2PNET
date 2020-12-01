/*
 * main.cpp
 *
 *  Created on: Nov 30, 2020
 *      Author: kub0x
 */

#include "Registration/Headers/RegServer.hpp"
#include "Utils/Crypto/Headers/PKI.hpp"
#include <vector>

int main(int argc, char** argv){
	PKI::GetInstance()->GenSelfSigned();
	RegServer *reg_server = new RegServer(44444);
	reg_server->Listen();
	return 0;
}


