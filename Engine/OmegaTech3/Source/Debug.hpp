#include "Data.hpp"

class OTConsole {
    public:
        bool ConsoleBoxEditMode = false;
        char ConsoleTextBuffer[128] = "";

        bool MainConsoleEditMode = false;
        char MainConsoleBuffer[1024] = "OmegaConsole\n";

        void PrintConsole(const char *Message) {
            strcat(MainConsoleBuffer, Message);
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
            
            if (!Found) {
                PrintConsole("Command Not Found: ");
                PrintConsole(Command);
                PrintConsole("<--\n");
            }
        }

        int CommandCounter = 0;

        void Draw() {
            GuiPanel((Rectangle){0, 40, 616, 288}, NULL);

            if (GuiTextBox((Rectangle){8, 296, 600, 24}, ConsoleTextBuffer, 128, ConsoleBoxEditMode))
                ConsoleBoxEditMode = !ConsoleBoxEditMode;

            if (GuiTextBox((Rectangle){8, 48, 600, 240}, MainConsoleBuffer, 1024, MainConsoleEditMode)) {

            }

            if (IsKeyPressed(KEY_ENTER)) {
                ConsoleParseCommand(ConsoleTextBuffer);
                ConsoleTextBuffer[0] = '\0';
                CommandCounter++;

                if (CommandCounter == 2) {
                    MainConsoleBuffer[0] = '\0';
                    CommandCounter = 0;
                }
            }
        }
};

static OTConsole OTDebugConsole;