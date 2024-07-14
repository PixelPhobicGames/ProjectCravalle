
#ifndef SIGCHECK_H
#define SIGCHECK_H

#pragma once

#include <iostream>
#include <string>

using namespace std;

//#define USESIG

template <typename Type> bool VerifyData(Type Data , Type Signature);
template <typename Type> Type SignatureGen(Type Data);

extern string Signatures[20];

void InitSig();

#endif