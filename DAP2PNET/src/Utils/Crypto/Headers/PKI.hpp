/*
 * PKI.hpp
 *
 *  Created on: Dec 1, 2020
 *      Author: kub0x
 */

#ifndef SRC_UTILS_CRYPTO_HEADERS_PKI_HPP_
#define SRC_UTILS_CRYPTO_HEADERS_PKI_HPP_

#include <openssl/x509.h>
#include <string>
#include "../../Patterns/Singleton/Includes/Singleton.cpp"

class PKI : public Singleton<PKI>{

private:
	friend class Singleton<PKI>;
	PKI() : Singleton<PKI>(){}

public:

	void SaveCert(X509* cert);
	void GenSelfSigned();
	void GenerateKeyPair();
	X509* CSRtoX509(std::string strCsr);
	X509* SignCert(X509 *cert, int id);

};



#endif /* SRC_UTILS_CRYPTO_HEADERS_PKI_HPP_ */
