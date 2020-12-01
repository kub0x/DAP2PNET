/*
 * Singleton.cpp
 *
 *  Created on: Dec 1, 2020
 *      Author: kub0x
 */


#include "../Headers/Singleton.hpp"


template <class T>
T* Singleton<T>::GetInstance(){
	return Singleton<T>::myInstance;
}

template <class T>
T* Singleton<T>::myInstance = new T();
