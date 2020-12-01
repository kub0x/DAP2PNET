/*
 * Singleton.hpp
 *
 *  Created on: Dec 1, 2020
 *      Author: kub0x
 */

#ifndef SRC_UTILS_PATTERNS_SINGLETON_HEADERS_SINGLETON_HPP_
#define SRC_UTILS_PATTERNS_SINGLETON_HEADERS_SINGLETON_HPP_


template <class T>
class Singleton{

protected:
	static T* myInstance;
	Singleton(){}
public:
	static T* GetInstance();
};


#endif /* SRC_UTILS_PATTERNS_SINGLETON_HEADERS_SINGLETON_HPP_ */
