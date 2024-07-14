
#include "Language.hpp"

// Language Pack Management

int LineCount(const char *filename) {
    ifstream file(filename);
    int lineCount = 0;

    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            lineCount++;
        }

        file.close();
    } else {
        cerr << "Error: Unable to open the file." << endl;
    }

    return lineCount;
}

string LoadLine(const char *filename, int index) {
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return ""; // Return an empty string to indicate failure
    }

    string line;
    for (int currentLine = 0; currentLine <= index; ++currentLine) {
        if (!getline(file, line)) {
            // Return an empty string if the requested line index is out of bounds
            return "";
        }
    }

    file.close();
    return line;
}

void LoadLanguagePack(int Type, Pack *PackData) {
    int Lines;

    switch (Type) {
    case 0: // Eng
        Lines = LineCount("GameData/Global/LanguagePacks/English.lg");
        PackData->Size = Lines;
        PackData->Type = 0;

        if (Lines <= MaxPackSize) {
            for (int i = 0; i <= Lines; i++) {
                PackData->Lines[i] = LoadLine("GameData/Global/LanguagePacks/English.lg", i);
            }
        }

        break;
    case 1: // Russ
        Lines = LineCount("GameData/Global/LanguagePacks/Russian.lg");
        PackData->Size = Lines;
        PackData->Type = 1;

        if (Lines <= MaxPackSize) {
            for (int i = 0; i <= Lines; i++) {
                PackData->Lines[i] = LoadLine("GameData/Global/LanguagePacks/Russian.lg", i);
            }
        }

        break;
    case 2: // Span
        Lines = LineCount("GameData/Global/LanguagePacks/Spanish.lg");
        PackData->Size = Lines;
        PackData->Type = 2;

        if (Lines <= MaxPackSize) {
            for (int i = 0; i <= Lines; i++) {
                PackData->Lines[i] = LoadLine("GameData/Global/LanguagePacks/Spanish.lg", i);
            }
        }

        break;
    case 3: // Port
        Lines = LineCount("GameData/Global/LanguagePacks/Japanese.lg");
        PackData->Size = Lines;
        PackData->Type = 3;

        if (Lines <= MaxPackSize) {
            for (int i = 0; i <= Lines; i++) {
                PackData->Lines[i] = LoadLine("GameData/Global/LanguagePacks/Japanese.lg", i);
            }
        }

        break;
    case 4: // Franch
        Lines = LineCount("GameData/Global/LanguagePacks/French.lg");
        PackData->Size = Lines;
        PackData->Type = 4;

        if (Lines <= MaxPackSize) {
            for (int i = 0; i <= Lines; i++) {
                PackData->Lines[i] = LoadLine("GameData/Global/LanguagePacks/French.lg", i);
            }
        }

        break;
    }
}
