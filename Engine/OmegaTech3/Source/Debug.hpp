#include "Data.hpp"

bool TextBox001EditMode = false;
char TextBox001Text[128] = "";
bool TextBox002EditMode = false;
char TextBox002Text[1024] = "OmegaConsole\n";

void PrintConsole(const char *Message) {
    strcat(TextBox002Text, Message);
}

void ConsoleParseCommand(const char *Command) {
    bool Found = false;

    string CommandValue(Command);

    if (CommandValue == "ExportTerrain") {
        PrintConsole("Exporting Terrain As Mesh\n");
        if (ExportMesh(TerrainData.HeightMap.meshes[0], "Terrain.obj")) {
            PrintConsole("Done.. Stored in Terrain.obj\n");
        }
        Found = true;
    }

    if (CommandValue == "SigGen") {
        string Data = WstringToString(WorldData);
        string NewSig = SignatureGen<string>(Data);

        cout << NewSig << "\n";

        PrintConsole("Done.\n");
        Found = true;
    }

    if (CommandValue == "Rumble") {
        RumblePulse();
        Found = true;
    }

    if (!Found) {
        PrintConsole("Command Not Found: ");
        PrintConsole(Command);
        PrintConsole("<--\n");
    }
}

int CommandCounter = 0;

void DrawConsole() {
    GuiPanel((Rectangle){0, 40, 616, 288}, NULL);

    if (GuiTextBox((Rectangle){8, 296, 600, 24}, TextBox001Text, 128, TextBox001EditMode))
        TextBox001EditMode = !TextBox001EditMode;

    if (GuiTextBox((Rectangle){8, 48, 600, 240}, TextBox002Text, 1024, TextBox002EditMode)) {
    }

    if (IsKeyPressed(KEY_ENTER)) {
        ConsoleParseCommand(TextBox001Text);
        TextBox001Text[0] = '\0';
        CommandCounter++;

        if (CommandCounter == 2) {
            TextBox002Text[0] = '\0';
            CommandCounter = 0;
        }
    }
}

void CheckKey() {
#ifdef Linux
    char hostname[100];
    gethostname(hostname, 100);

    cout << "USER: " << hostname << "\n";

    wstring encodedData = CharArrayToWString(hostname);
    encodedData = Encode(encodedData, MainKey);

    if (!IsPathFile("GameData/Key/Key.sig")) {
        ofstream SigFile("GameData/Key/Key.sig");

        if (SigFile.is_open()) {
            SigFile << TextFormat("%ls", encodedData.c_str());
            SigFile.close();
        } else {
            cerr << "Error opening file for writing.\n";
        }
    } else {
        wstring Data = LoadFile("GameData/Key/Key.sig");

        if (!(Data[3] == encodedData[3])) {
            wcout << Data << " != " << encodedData << "\n";
            exit(0);
        }
    }
#endif
}