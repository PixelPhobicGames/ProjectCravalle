#include "SigCheck.hpp"


template <typename Type>
bool VerifyData(Type Data, Type Signature) {
    int SizeOfData = Data.size();

    int Divisor = 4;

    bool Check = true;

    if (int((Signature.size())) < SizeOfData / Divisor)
        return false;

    for (int i = 0; i <= SizeOfData; i += Divisor) {
        if (Data[i] != Signature[i / Divisor]) {
            Check = false;
        }
    }

    return Check;
}

template bool VerifyData<std::string>(std::string Data, std::string Signature);

template <typename Type>
Type SignatureGen(Type Data) {
    int SizeOfData = Data.size();

    int Divisor = 4;

    Type Signature;

    for (int i = 0; i <= SizeOfData; i += Divisor) {
        Signature += Data[i];
    }

    return Signature;
}
template std::string SignatureGen<std::string>(std::string Data);

string Signatures[20];

void InitSig() {
    
}
