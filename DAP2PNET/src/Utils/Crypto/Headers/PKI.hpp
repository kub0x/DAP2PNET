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

	void ComputeIDFromCSR(std::string& id, X509_REQ *cert);

public:

	std::string X509ToPEM(X509 *cert);
	void GenSelfSigned();
	void GenSSLX509();
	void GenerateKeyPair(std::string file_name);
	X509* CSRtoX509(std::string strCsr);
	X509* SignCert(X509 *cert, std::string file_name);

};



#endif /* SRC_UTILS_CRYPTO_HEADERS_PKI_HPP_ */
