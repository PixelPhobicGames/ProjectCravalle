#include "PPGIO.hpp"

wstring CharArrayToWString(const char *charArray) {
    locale utf8Locale(locale(), new codecvt_utf8<wchar_t>);
    wstring_convert<codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(charArray);
}

string WstringToString(wstring wideStr) {

    locale utf8Locale(locale(), new codecvt_utf8<wchar_t>);

    wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.to_bytes(wideStr);
}

wstring LoadFile(const char *Path) {
    wstring Data;
    wifstream OutFile;
    OutFile.open(Path);
    OutFile >> Data;
    return Data;
}

float ToFloat(wstring Data) {
    string s(Data.begin(), Data.end());
    float Out = stof(s);
    return Out;
}

int GetWDLSize(wstring WData, wstring Extra) {
    int Out = 0;
    const size_t totalSize = WData.size() + Extra.size();

    for (size_t i = 0; i <= totalSize; ++i) {
        if (i < WData.size() && WData[i] == ':') {
            ++Out;
        } else if (i >= WData.size() && Extra[i - WData.size()] == ':') {
            ++Out;
        }
    }

    return Out;
}

wstring WReadValue(wstring Data, int Start, int End) {
    wstring ReadValueOut;

    if (End >= Data.size()) {
        End = Data.size();
    }

    for (int i = Start; i <= End; i++) {
        ReadValueOut += Data[i];
    }

    return ReadValueOut;
}

wstring WSplitValue(wstring Data, int Place) {
    SVPlaceCounter = 0;
    SVStart = 0;
    SVEnd = 0;

    if (Place == 0) {
        for (int i = 0; i <= Data.size(); i++) {
            if (Data[i] == L':' || i == Data.size()) {
                SVEnd = i - 1;
                break;
            }
        }
    } else {
        Place = Place;
        SVPlaceCounter = 1;
        for (int i = 0; i <= Data.size(); i++) {
            if (SVPlaceCounter == Place) {
                SVStart = i + 1;
                for (int x = i + 1; x <= Data.size(); x++) {
                    if (Data[x] == L':' || x == Data.size()) {
                        SVEnd = x - 1;
                        break;
                    }
                }
            }
            if (Data[i] == L':') {
                SVPlaceCounter++;
            }
        }
    }

    return WReadValue(Data, SVStart, SVEnd);
}

int PullConfigValue(const char *Path, int ValueIndex) {
    ifstream configFile(Path);

    string line;
    getline(configFile, line);

    configFile.close();

#ifdef Windows
    stringstream ss(line);
#endif

#ifdef Linux
    stringstream ss(line);
#endif

    string token;
    int index = 0;
    while (getline(ss, token, '|')) {
        if (index == ValueIndex) {
            try {
                int intValue = stoi(token);
                return intValue;
            } catch (const invalid_argument &ia) {
                cerr << "Error: InvalidIntegerValueInTheConfigurationFile" << endl;
                return -1;
            }
        }
        index++;
    }

    cerr << "Error: ValueNotFoundAtIndex" << ValueIndex << endl;
    return -1;
}

bool GetCollision(int x, int y, int w, int h, int x2, int y2, int w2, int h2) {
    if (y + h <= y2) {
        return false;
    }

    if (y2 + h2 >= y) {
        return false;
    }

    if (x + w <= x2) {
        return false;
    }

    if (x2 + w2 >= x) {
        return false;
    }

    return true;
}

string ReadValue(string Data, int Start,
                 int End) { // Reads Value from (Start to End)
    string Out;
    for (int i = Start; i <= End; i++) {
        Out += Data[i];
    }
    return Out;
}

string SplitValue(string Data, int Place) {
    string Out;
    int PlaceCounter = 0;

    int Start = 0;
    int End = 0;

    if (Place == 0) {
        for (int i = 0; i <= Data.size(); i++) {
            if (Data[i] == ' ' || i == Data.size()) {
                End = i - 1;
                break;
            }
        }
    } else {
        Place = Place;
        PlaceCounter = 1;
        for (int i = 0; i <= Data.size(); i++) {
            if (PlaceCounter == Place) {
                Start = i + 1;
                for (int x = i + 1; x <= Data.size(); x++) {
                    if (Data[x] == ' ' || x == Data.size()) {
                        End = x - 1;
                        break;
                    }
                }
            }
            if (Data[i] == ' ') {
                PlaceCounter++;
            }
        }
    }

    Out = ReadValue(Data, Start, End);

    return Out;
}

string SplitValueEx(string Data, char Character, int Place) {
    string Out;
    int PlaceCounter = 0;

    int Start = 0;
    int End = 0;

    if (Place == 0) {
        for (int i = 0; i <= Data.size(); i++) {
            if (Data[i] == Character || i == Data.size()) {
                End = i - 1;
                break;
            }
        }
    } else {
        Place = Place;
        PlaceCounter = 1;
        for (int i = 0; i <= Data.size(); i++) {
            if (PlaceCounter == Place) {
                Start = i + 1;
                for (int x = i + 1; x <= Data.size(); x++) {
                    if (Data[x] == Character || x == Data.size()) {
                        End = x - 1;
                        break;
                    }
                }
            }
            if (Data[i] == Character) {
                PlaceCounter++;
            }
        }
    }

    Out = ReadValue(Data, Start, End);

    return Out;
}

int StringToInt(const std::string &Data) {
    int Out = 0;
    int Place = 1;
    int i = Data.size() - 1; // Start from the End of the string

    while (i >= 0) {
        char c = Data[i];
        if (c == '-') {
            Out = -Out; // Handle negative sign
        } else if (c >= '0' && c <= '9') {
            int digit = c - '0';
            Out += digit * Place;
            Place *= 10;
        }
        i--;
    }

    return Out;
}

float StringToFloat(string Data) {
    float Out = 0.0f;
    float Place = 1.0f;
    int i = Data.size() - 1; // Start from the end of the string

    bool isNegative = false;

    while (i >= 0) {
        char c = Data[i];
        if (c == '-') {
            isNegative = true; // Handle negative sign
        } else if (c == '.') {
            // Handle decimal point
            Out /= Place;
            Place = 1.0f;
        } else if (c >= '0' && c <= '9') {
            int digit = c - '0';
            Out += digit * Place;
            Place *= 10.0f;
        }
        i--;
    }

    return isNegative ? -Out : Out;
}