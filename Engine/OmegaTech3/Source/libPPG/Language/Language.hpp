#ifndef LANGUAGE_HPP
#define LANGUAGE_HPP


#include <string>
#include <iostream>
#include <fstream>
#include <vector>


using namespace std;

#define MaxPackSize 1000

static int EnglishPack = 0;
static int SpanishPack = 2;
static int JapanesePack = 3;
static int FrenchPack = 4;
static int RussianPack = 1;


class Pack {
    public:
        int Size = 0;
        int Type = 0;
        string Lines[MaxPackSize];


        void DumpPack(){
            for (int i = 0; i <= Size; i ++){
                cout << "Index " << Size << " "<< Lines[i] << "\n";
            }
        }
};

void LoadLanguagePack(int Type, Pack* PackData);

#endif