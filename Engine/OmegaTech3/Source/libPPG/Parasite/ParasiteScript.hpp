#include "ParasiteScriptData.hpp"
#include "../Encoder/Encoder.hpp"

#define RedText "\033[4;31m"
#define GreenText "\033[1;32m"
#define BlueText "\033[1;34m"
#define FadeColorText "\033[1;37m"



string PMemKey = "9876543210zyxwvutsrqponmlkjihgfedcbaZYXWVUTSRQPONMLKJIHGFEDCBA";


auto ParasiteScriptLoadFile(const char* Path){
    std::ifstream Input(Path);
    std::stringstream sstr;

    while(Input >> sstr.rdbuf());

    return sstr.str();
}

void ParasiteScriptTFlagWipe(){
    for (int i = 0 ; i <= MaxTFlag - 1; i++){
        ToggleFlags[i].Value = 0;
    }
}

void ParasiteScriptInit(){ 
    ParasiteScriptCoreData.LesserFlag = false;
    ParasiteScriptCoreData.GreaterFlag = false;
    ParasiteScriptCoreData.EqualFlag = false;
    ParasiteScriptCoreData.NotEqualFlag = false;

    ParasiteScriptCoreData.CompareFlag = false;
    ParasiteScriptCoreData.ContinueFlag = true;
    ParasiteScriptCoreData.ErrorFlag = false;

    ParasiteScriptCoreData.ReturnLine = 0;
    ParasiteScriptCoreData.Line[0] = '\0';

    for (int i = 0; i <= ParasiteScriptCoreData.ProgramSize; i ++){
        ParasiteScriptCoreData.Line[i] = "";
    }
    
    ParasiteScriptCoreData.ProgramSize = 0;
    ParasiteScriptCoreData.TextSize = 20;

    if (TextureCounter != 0){
        for ( int i = 0; i != TextureCounter; i++)
        {
            UnloadTexture(TextureMemory[i].TextureValue);
            TextureMemory[i].Name = "";
        }
        
    }

    if (SFXCounter != 0){
        for ( int i = 0; i != SFXCounter; i++)
        {
            UnloadSound(SoundMemory[i].SoundValue);
            SoundMemory[i].Name = "";
        }
        
    }

    VaribleCounter = 0;
    TextureCounter = 0;
    PMemCounter = 0;
    JumpPointCounter = 0;
    ArrayCounter = 0;
    SFXCounter = 0;

    for (int i = 0 ; i <= MaxVaribles - 1; i++){
        VaribleMemory[i].IValue = 0;
    }
}

auto ScanVaribleMemory(string VaribleName){
    bool Found = false;
    for (int x = 0 ; x <= VaribleCounter; x++){
        if (VaribleMemory[x].Name == VaribleName){
            Found = true;
        }
    }
    for (int x = 0 ; x <= PMemCounter; x++){
        if (PermanentMemory[x].Name == VaribleName){
            Found = true;
        }
    }
    return Found;
}

int PullIntFromMemory(string VaribleName){
    bool Found = false;
    for (int x = 0 ; x <= VaribleCounter; x++){
        if (VaribleMemory[x].Name == VaribleName){
            Found = true;
            return VaribleMemory[x].IValue;
        }
    }
    for (int x = 0 ; x <= PMemCounter; x++){
        if (PermanentMemory[x].Name == VaribleName){
            Found = true;
            return PermanentMemory[x].IValue;
        }
    }
    if (!Found){
        cout << "Error Varible Not Declared: " << VaribleName << "\n";
        ParasiteScriptCoreData.ErrorFlag = true;
        return 0;
    }
}

Texture2D PullTextureFromMemory(string VaribleName){
    bool Found = false;

    for (int x = 0 ; x <= TextureCounter; x++){
        if (TextureMemory[x].Name == VaribleName){
            Found = true;
            return TextureMemory[x].TextureValue;
        }
    }

    if (!Found){
        cout << "Error Varible Not Declared: " << VaribleName << "\n";
        ParasiteScriptCoreData.ErrorFlag = true;
        Texture2D New;
        return New;
    }
}

Sound PullSoundFromMemory(string VaribleName){
    bool Found = false;

    for (int x = 0 ; x <= SFXCounter; x++){
        if (SoundMemory[x].Name == VaribleName){
            Found = true;
            return SoundMemory[x].SoundValue;
        }
    }

    if (!Found){
        cout << "Error Varible Not Declared: " << VaribleName << "\n";
        ParasiteScriptCoreData.ErrorFlag = true;
        Sound New;
        return New;
    }
}


string PullStrFromMemory(string VaribleName){
    bool Found = false;
    for (int x = 0 ; x <= VaribleCounter; x++){
        if (VaribleMemory[x].Name == VaribleName){
            Found = true;
            return VaribleMemory[x].Value;
        }
    }

    if (!Found){
        ParasiteScriptCoreData.ErrorFlag = true;
        return "";
    }
}

void StoreIntToMemory(string VaribleName, int Value){
    if (ScanVaribleMemory(VaribleName)){
        for (int x = 0 ; x <= VaribleCounter; x++){
            if (VaribleMemory[x].Name == VaribleName){
                VaribleMemory[x].IValue = Value;
            }
        }
    }
}

void StoreIntToPMemory(string VaribleName, int Value){
    if (ScanVaribleMemory(VaribleName)){
        for (int x = 0 ; x <= PMemCounter; x++){
            if (PermanentMemory[x].Name == VaribleName){
                PermanentMemory[x].IValue = Value;
            }
        }
    }
}

void StoreStrToMemory(string VaribleName, string Value){
    for (int x = 0 ; x <= VaribleCounter; x++){
        if (VaribleMemory[x].Name == VaribleName){
            VaribleMemory[x].Value = Value;
        }
    }
}

string RemoveSpaces(string Data)
{
    string OutData = "";

    for (int i = 0 ; i <= Data.size() ; i ++){
        if (Data[i] != ' '){
            OutData[i] = Data[i];
        }
    }

    return OutData;
}

int ScanSpaces(string Data){
    int Spaces = 0;

    for (int i = 0 ; i <= Data.size() ; i ++){
        if (Data[i] != ' '){
            Spaces ++;
        }
    }

    return Spaces;
}

bool IsNumber(const std::string& s) {
    std::istringstream iss(s);
    double value;
    iss >> std::noskipws >> value;  // Try to read a double without skipping white spaces

    // Check the entire string was consumed and the stream is in a good state
    return iss.eof() && !iss.fail();
}

void LoadScript(const char *ScriptPath){ // Loads Script
    istringstream ProgramData(ParasiteScriptLoadFile(ScriptPath));
    string CurrentLine;

    while (getline(ProgramData, CurrentLine)) {

        if (ReadValue(CurrentLine, 0 , 0) == ":"){            
            JumpPoints[JumpPointCounter].Name = ReadValue(CurrentLine, 1 , CurrentLine.size() - 1);
            JumpPoints[JumpPointCounter].LineNumber = ParasiteScriptCoreData.LineCounter;
            JumpPointCounter ++;
        }
        if (ReadValue(CurrentLine, 0 , 0) == " "){
            int SpaceCounter = 0;
            for (int i = 0 ; i <= CurrentLine.size() ; i ++){
                if (ReadValue(CurrentLine, i , i) == " ")SpaceCounter ++;
                else {
                    break;
                }
            }
            string Temp = ReadValue(CurrentLine, SpaceCounter , CurrentLine.size() - 1);
            CurrentLine = Temp;
        }
        ParasiteScriptCoreData.Line[ParasiteScriptCoreData.LineCounter] = CurrentLine;
        ParasiteScriptCoreData.LineCounter ++;
        

    } 
    ParasiteScriptCoreData.ProgramSize = ParasiteScriptCoreData.LineCounter;

    ParasiteScriptCoreData.LineCounter = 0;

} 


string ReplaceValue(int start, int end, std::string original, const std::string newValue) {
    return original.replace(start, end - start + 1, newValue);
}

string ReadLineByIndex(const std::string& filename, int index) {
    std::ifstream inputFile(filename); // Open the file for reading

    if (!inputFile.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return ""; // Return an empty string on failure
    }

    std::string line;
    int currentLineIndex = 0;

    while (currentLineIndex < index && std::getline(inputFile, line)) {
        currentLineIndex++; // Keep track of the current line index
    }

    inputFile.close(); // Close the file when done

    if (currentLineIndex == index) {
        return line; // Return the line at the specified index
    } else {
        return ""; // Line index not found
    }
}

void WritePMemPage(){
    ofstream PmemFile("GameData/Saves/PMEM");

    PmemFile << NEncode(to_string(PMemCounter), PMemKey) << endl;

    for (int i = 0; i <= PMemCounter - 1; i ++){
        PmemFile << NEncode(PermanentMemory[i].Name, PMemKey) << " " << NEncode(to_string(PermanentMemory[i].IValue), PMemKey) << endl;
    }

    PmemFile.close();
}

void RestorePMemPage(){
    PMemCounter = StringToInt(NEncode(ReadLineByIndex("GameData/Saves/PMEM" , 1), PMemKey));

    for (int i = 0; i <= PMemCounter - 1; i ++){
        string PmemValue = NEncode(ReadLineByIndex("GameData/Saves/PMEM" , 2 + i), PMemKey);
    }
}

static bool FoundMacroOperation = false;

string SearchMacroOperation(string CurrentInstruction){ // Macro OP
    int Start = 0;
    int End = 0;

    bool Found = false;

    string Instruction = CurrentInstruction;

    for (int i = 0; i <= Instruction.size() ; i ++){
        if (Instruction[i] == '('){
            Start = i;
        }
        if (Instruction[i] == ')'){
            End = i;
            if (Start != 0){
                Found = true;
            }
            break;
        }
    }

    if (Found){
        string *MacroOperation = NULL; 
        MacroOperation = new string;   
        *MacroOperation = ReadValue(Instruction, Start , End);
        

        if (CodeLogging) cout << "Found Macro Operation -->" << *MacroOperation << endl;

        if (CodeLogging) cout << SplitValue(*MacroOperation , 1) << endl;

        if (SplitValue(*MacroOperation , 1) == "TFlag" && SplitValue(*MacroOperation , 2) == "-->"){
            int *TValue = NULL;
            TValue = new int;
            *TValue = ToggleFlags[StringToInt(SplitValue(Instruction, 3 ))].Value;

            Instruction = ReplaceValue(Start, End, Instruction , to_string(*TValue) );

            delete TValue;
            
        }

        if (SplitValue(*MacroOperation , 1) == "ValueOf" && SplitValue(*MacroOperation , 2) == "-->"){
            int Value = PullIntFromMemory(SplitValue(*MacroOperation , 3));
            Instruction = ReplaceValue(Start, End , Instruction , to_string(Value) );
        }

        if (SplitValue(*MacroOperation , 1) == "LanguagePack" && SplitValue(*MacroOperation , 2) == "-->"){
            string *LPackValue = NULL;
            LPackValue = new string;
            *LPackValue = GlobalPackData.Lines[StringToInt(SplitValue(Instruction, 3))];

            Instruction = ReplaceValue(Start, End, Instruction , *LPackValue );

            delete LPackValue;
            
        }
        

        delete MacroOperation;
    }

    FoundMacroOperation = Found;

    return Instruction;
}

static int TimeDelay = 0;

auto CycleInstruction(){
    if (TimeDelay == 0){
        if (ParasiteScriptCoreData.LineCounter != ParasiteScriptCoreData.ProgramSize){

            bool FoundInstruction = false;
            
            string Instruction = "";
            Instruction += ParasiteScriptCoreData.Line[ParasiteScriptCoreData.LineCounter];

            if (SplitValue(Instruction, 0 ) == "end"){
                FoundInstruction = true;
                ParasiteScriptCoreData.ContinueFlag = true;
            }

            if (ParasiteScriptCoreData.ContinueFlag && SplitValue(Instruction, 0) != "//" && SplitValue(Instruction, 0) != ""){

                if (CodeLogging)cout << "Pre Macro:" << Instruction << "\n";

                bool MacroLoop = true;

                while (MacroLoop){
                    Instruction = SearchMacroOperation(Instruction);
                    MacroLoop = FoundMacroOperation;
                }

                Instruction += " ";

                if (CodeLogging)cout << "Post Macro:" << Instruction << "\n";

                for (int i = 0 ; i <= ScanSpaces(Instruction); i ++){

                    if (SplitValue(Instruction, i) == "+"){
                        int Value1 = 0;
                        int Value2 = 0;
                        
                        if (IsNumber(SplitValue(Instruction, i-1))){
                            Value1 = StringToInt(SplitValue(Instruction, i-1));
                        }
                        else {
                            Value1 = PullIntFromMemory(SplitValue(Instruction, i-1));
                        }
                        if (IsNumber(SplitValue(Instruction, i+1))){
                            Value2 = StringToInt(SplitValue(Instruction, i+1));
                        }
                        else {
                            Value2 = PullIntFromMemory(SplitValue(Instruction, i+1));
                        }

                        int Start = 0;
                        int End = 0;

                        int AdditionStart = 0;

                        for (int x = 0 ; x <= Instruction.size(); x++){
                            if (Instruction[x] == '+'){
                                AdditionStart = x;
                                break;
                            }
                        }

                        for (int x = AdditionStart - 2; x >= 0; x--){
                            if (Instruction[x] == ' '){
                                Start = x;
                                break;
                            }
                        }

                        for (int x = AdditionStart + 2; x <= Instruction.size(); x++){
                            if (Instruction[x] == ' '){
                                End = x;
                                break;
                            }
                        }

                        Instruction = ReplaceValue(Start, End, Instruction , ' ' + to_string(Value1 + Value2) + ' ');
                    }
                    if (SplitValue(Instruction, i) == "-"){
                        int Value1 = 0;
                        int Value2 = 0;
                        
                        if (IsNumber(SplitValue(Instruction, i-1))){
                            Value1 = StringToInt(SplitValue(Instruction, i-1));
                        }
                        else {
                            Value1 = PullIntFromMemory(SplitValue(Instruction, i-1));
                        }
                        if (IsNumber(SplitValue(Instruction, i+1))){
                            Value2 = StringToInt(SplitValue(Instruction, i+1));
                        }
                        else {
                            Value2 = PullIntFromMemory(SplitValue(Instruction, i+1));
                        }

                        int Start = 0;
                        int End = 0;

                        int AdditionStart = 0;

                        for (int x = 0 ; x <= Instruction.size(); x++){
                            if (Instruction[x] == '+'){
                                AdditionStart = x;
                                break;
                            }
                        }

                        for (int x = AdditionStart - 2; x >= 0; x--){
                            if (Instruction[x] == ' '){
                                Start = x;
                                break;
                            }
                        }

                        for (int x = AdditionStart + 2; x <= Instruction.size(); x++){
                            if (Instruction[x] == ' '){
                                End = x;
                                break;
                            }
                        }

                        Instruction = ReplaceValue(Start, End, Instruction , ' ' + to_string(Value1 - Value2) + ' ');
                    }
                    if (SplitValue(Instruction, i) == "*"){
                        int Value1 = 0;
                        int Value2 = 0;
                        
                        if (IsNumber(SplitValue(Instruction, i-1))){
                            Value1 = StringToInt(SplitValue(Instruction, i-1));
                        }
                        else {
                            Value1 = PullIntFromMemory(SplitValue(Instruction, i-1));
                        }
                        if (IsNumber(SplitValue(Instruction, i+1))){
                            Value2 = StringToInt(SplitValue(Instruction, i+1));
                        }
                        else {
                            Value2 = PullIntFromMemory(SplitValue(Instruction, i+1));
                        }

                        int Start = 0;
                        int End = 0;

                        int AdditionStart = 0;

                        for (int x = 0 ; x <= Instruction.size(); x++){
                            if (Instruction[x] == '+'){
                                AdditionStart = x;
                                break;
                            }
                        }

                        for (int x = AdditionStart - 2; x >= 0; x--){
                            if (Instruction[x] == ' '){
                                Start = x;
                                break;
                            }
                        }

                        for (int x = AdditionStart + 2; x <= Instruction.size(); x++){
                            if (Instruction[x] == ' '){
                                End = x;
                                break;
                            }
                        }

                        Instruction = ReplaceValue(Start, End, Instruction , ' ' + to_string(Value1 * Value2) + ' ');
                    }
                    if (SplitValue(Instruction, i) == "/"){
                        int Value1 = 0;
                        int Value2 = 0;
                        
                        if (IsNumber(SplitValue(Instruction, i-1))){
                            Value1 = StringToInt(SplitValue(Instruction, i-1));
                        }
                        else {
                            Value1 = PullIntFromMemory(SplitValue(Instruction, i-1));
                        }
                        if (IsNumber(SplitValue(Instruction, i+1))){
                            Value2 = StringToInt(SplitValue(Instruction, i+1));
                        }
                        else {
                            Value2 = PullIntFromMemory(SplitValue(Instruction, i+1));
                        }

                        int Start = 0;
                        int End = 0;

                        int AdditionStart = 0;

                        for (int x = 0 ; x <= Instruction.size(); x++){
                            if (Instruction[x] == '+'){
                                AdditionStart = x;
                                break;
                            }
                        }

                        for (int x = AdditionStart - 2; x >= 0; x--){
                            if (Instruction[x] == ' '){
                                Start = x;
                                break;
                            }
                        }

                        for (int x = AdditionStart + 2; x <= Instruction.size(); x++){
                            if (Instruction[x] == ' '){
                                End = x;
                                break;
                            }
                        }

                        Instruction = ReplaceValue(Start, End, Instruction , ' ' + to_string(Value1 - Value2) + ' ');
                         
                    }
                }

                if (CodeLogging)cout << "Post Math:" << Instruction << "\n";

                if (SplitValue(Instruction, 1) == "="){
                    FoundInstruction = true;
                    if (IsNumber(SplitValue(Instruction, 2))){
                        StoreIntToMemory(SplitValue(Instruction, 0) , StringToInt(SplitValue(Instruction, 2)));
                    }
                    else {
                        int Value = PullIntFromMemory(SplitValue(Instruction, 2));
                        StoreIntToMemory(SplitValue(Instruction, 0) , Value);
                    }
                }

                if (SplitValue(Instruction, 1) == "+="){
                    FoundInstruction = true;
                    if (IsNumber(SplitValue(Instruction, 2))){
                        int Value = StringToInt(SplitValue(Instruction, 2));
                        StoreIntToMemory(SplitValue(Instruction, 0) , PullIntFromMemory(SplitValue(Instruction, 0)) + Value);
                    }
                    else{
                        int Value = PullIntFromMemory(SplitValue(Instruction, 2));
                        StoreIntToMemory(SplitValue(Instruction, 0) , PullIntFromMemory(SplitValue(Instruction, 0)) + Value);
                    }
                }

                if (SplitValue(Instruction, 1) == "-="){
                    FoundInstruction = true;
                    if (IsNumber(SplitValue(Instruction, 2))){
                        int Value = StringToInt(SplitValue(Instruction, 2));
                        StoreIntToMemory(SplitValue(Instruction, 0) , PullIntFromMemory(SplitValue(Instruction, 0)) - Value);
                    }
                    else{
                        int Value = PullIntFromMemory(SplitValue(Instruction, 2));
                        StoreIntToMemory(SplitValue(Instruction, 0) , PullIntFromMemory(SplitValue(Instruction, 0)) - Value);
                    }
                }

                if (SplitValue(Instruction, 1) == "*="){
                    FoundInstruction = true;
                    if (IsNumber(SplitValue(Instruction, 2))){
                        int Value = StringToInt(SplitValue(Instruction, 2));
                        StoreIntToMemory(SplitValue(Instruction, 0) , PullIntFromMemory(SplitValue(Instruction, 0)) * Value);
                    }
                    else{
                        int Value = PullIntFromMemory(SplitValue(Instruction, 2));
                        StoreIntToMemory(SplitValue(Instruction, 0) , PullIntFromMemory(SplitValue(Instruction, 0)) * Value);
                    }
                }

                if (SplitValue(Instruction, 1) == "/="){
                    FoundInstruction = true;
                    if (IsNumber(SplitValue(Instruction, 2))){
                        int Value = StringToInt(SplitValue(Instruction, 2));
                        StoreIntToMemory(SplitValue(Instruction, 0) , PullIntFromMemory(SplitValue(Instruction, 0)) / Value);
                    }
                    else{
                        int Value = PullIntFromMemory(SplitValue(Instruction, 2));
                        StoreIntToMemory(SplitValue(Instruction, 0) , PullIntFromMemory(SplitValue(Instruction, 0)) / Value);
                    }
                }

                if (SplitValue(Instruction, 0 ) == "if"){
                    FoundInstruction = true;

                    ParasiteScriptCoreData.ContinueFlag = true;
                    ParasiteScriptCoreData.ElseFlag = false;

                    string Operator = SplitValue(Instruction, 2 );

                    int Value1 = 0;
                    int Value2 = 0;

                    if (IsNumber(SplitValue(Instruction, 1))){
                        Value1 = StringToInt(SplitValue(Instruction, 1 ));
                    }
                    else {
                        Value1 = PullIntFromMemory(SplitValue(Instruction, 1 ));
                    }

                    if (IsNumber(SplitValue(Instruction, 3))){
                        Value2 = StringToInt(SplitValue(Instruction, 3 ));
                    }
                    else {
                        Value2 = PullIntFromMemory(SplitValue(Instruction, 3 ));
                    }

                    if (CodeLogging) cout << "if " << Value1 << " " + Operator << " " << Value1 << "\n";

                    if (Operator == "=="){
                        if (Value1 == Value2){
                            ParasiteScriptCoreData.ContinueFlag = true;
                        }
                        else {
                            ParasiteScriptCoreData.ContinueFlag = false;
                            ParasiteScriptCoreData.ElseFlag = true;
                        }
                    }

                    if (Operator == ">"){
                        if (Value1 > Value2){
                            ParasiteScriptCoreData.ContinueFlag = true;
                        }
                        else {
                            ParasiteScriptCoreData.ContinueFlag = false;
                            ParasiteScriptCoreData.ElseFlag = true;
                        }
                    }

                    if (Operator == "<"){
                        if (Value1 < Value2){
                            ParasiteScriptCoreData.ContinueFlag = true;
                        }
                        else {
                            ParasiteScriptCoreData.ContinueFlag = false;
                            ParasiteScriptCoreData.ElseFlag = true;
                        }
                    }

                    if (Operator == ">="){
                        if (Value1 >= Value2){
                            ParasiteScriptCoreData.ContinueFlag = true;
                        }
                        else {
                            ParasiteScriptCoreData.ContinueFlag = false;
                            ParasiteScriptCoreData.ElseFlag = true;
                        }
                    }

                    if (Operator == "<="){
                        if (Value1 >= Value2){
                            ParasiteScriptCoreData.ContinueFlag = true;
                        }
                        else {
                            ParasiteScriptCoreData.ContinueFlag = false;
                            ParasiteScriptCoreData.ElseFlag = true;
                        }
                    }

                    if (Operator == "!="){
                        if (Value1 != Value2){
                            ParasiteScriptCoreData.ContinueFlag = true;
                        }
                        else {
                            ParasiteScriptCoreData.ContinueFlag = false;
                            ParasiteScriptCoreData.ElseFlag = true;
                        }
                    }
                }

                if (SplitValue(Instruction, 0 ) == "else"){
                    FoundInstruction = true;
                    if (!ParasiteScriptCoreData.ElseFlag){
                        ParasiteScriptCoreData.ContinueFlag = false;
                    }
                }

                if (SplitValue(Instruction, 0 ) == "stop"){
                    FoundInstruction = true;
                    exit(0);
                }

                if (SplitValue(Instruction, 0 ) == "JumpLabel"){   
                    FoundInstruction = true;
                    for (int i = 0 ; i <= JumpPointCounter ; i ++){
                        if (JumpPoints[i].Name == SplitValue(Instruction, 1)){
                            ParasiteScriptCoreData.ReturnLine = ParasiteScriptCoreData.LineCounter;
                            ParasiteScriptCoreData.LineCounter = JumpPoints[i].LineNumber;
                        }
                    }
                }

                if (SplitValue(Instruction, 0 ) == "Return"){  
                    FoundInstruction = true;
                    ParasiteScriptCoreData.LineCounter = ParasiteScriptCoreData.ReturnLine;
                }

                if (SplitValue(Instruction, 0 ) == "var"){
                    FoundInstruction = true;
                    VaribleMemory[VaribleCounter].Name = SplitValue(Instruction, 1 );

                    string VaribleData = SplitValue(Instruction, 2 );
                    int VaribleType = 0;

                    if (ReadValue(VaribleData , 0 , 0) == "'"){
                        VaribleType = 1;
                    }
                    else {
                        VaribleType = 0;
                    }
                    
                    if (VaribleType == 1){
                        string StoredData = "";
                        for (int i = 0 ; i <= VaribleData.size() ; i ++){
                            if (ReadValue(VaribleData , i , i) != "'"){
                                StoredData += ReadValue(VaribleData , i , i);
                            }
                        }
                        VaribleMemory[VaribleCounter].Value = StoredData;
                        VaribleMemory[VaribleCounter].IValue = 0;
                    }
                    else {
                        VaribleMemory[VaribleCounter].IValue = StringToInt(VaribleData);
                        VaribleMemory[VaribleCounter].Value = " ";
                    }

                    VaribleCounter++;   

                }

                if (SplitValue(Instruction, 0 ) == "PMem"){
                    FoundInstruction = true;

                    PermanentMemory[PMemCounter].Name = SplitValue(Instruction, 1 );

                    string VaribleData = SplitValue(Instruction, 2 );

                    if (IsNumber(VaribleData)){
                        PermanentMemory[PMemCounter].IValue = StringToInt(VaribleData);

                        PMemCounter++;   
                    }

                    else {
                        cout << "Error Cannot Load Strings Into PMEM" << endl;
                        ParasiteScriptCoreData.ErrorFlag = true;
                    }
                }

                if (SplitValue(Instruction, 0 ) == "Texture"){
                    FoundInstruction = true;

                    TextureMemory[TextureCounter].Name = SplitValue(Instruction, 1 );

                    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Scripts/Textures/%s.png", SceneIDMirror , SplitValue(Instruction, 2 ).c_str() ))){
                        TextureMemory[TextureCounter].TextureValue = LoadTexture(TextFormat("GameData/Worlds/World%i/Scripts/Textures/%s.png", SceneIDMirror , SplitValue(Instruction, 2 ).c_str() ));
                        TextureCounter ++;
                    }

                    else {
                        cout << "Texture Does Not Exist:" << TextFormat("GameData/Worlds/World%i/Scripts/Textures/%s.png", SceneIDMirror , SplitValue(Instruction, 2 ).c_str() ) << endl;
                        ParasiteScriptCoreData.ErrorFlag = true;
                    }

                }

                if (SplitValue(Instruction, 0 ) == "Sound"){
                    FoundInstruction = true;
                    SoundMemory[SFXCounter].Name = SplitValue(Instruction, 1 );

                    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Scripts/Textures/%s.png", SceneIDMirror , SplitValue(Instruction, 2 ).c_str() ))){
                        SoundMemory[SFXCounter].SoundValue = LoadSound(TextFormat("GameData/Worlds/World%i/Scripts/Sounds/%s.mp3", SceneIDMirror , SplitValue(Instruction, 2 ).c_str() ));
                        SFXCounter ++;
                    }

                    else {
                        cout << "SFX Does Not Exist:" << TextFormat("GameData/Worlds/World%i/Scripts/Sounds/%s.mp3", SceneIDMirror , SplitValue(Instruction, 2 ).c_str() ) << endl;
                        ParasiteScriptCoreData.ErrorFlag = true;
                    }
                }

                if (SplitValue(Instruction, 0 ) == "PlaySound"){
                    PlaySound(PullSoundFromMemory(SplitValue(Instruction, 1 )));
                    FoundInstruction = true;
                }


                if (SplitValue(Instruction, 0 ) == "MapClipping"){
                    FoundInstruction = true;

                    if (StringToInt(SplitValue(Instruction, 1 )) == 0){
                        MapClipping = false;
                    }
                    else {
                        MapClipping = true;
                    }
                }

                if (SplitValue(Instruction, 0 ) == "WriteTFlag"){
                    ToggleFlags[StringToInt(SplitValue(Instruction, 1 ))].Value = StringToInt(SplitValue(Instruction, 2));
                    FoundInstruction = true;
                }


                if (SplitValue(Instruction, 0 ) == "ReadTFlag"){
                    int Value = ToggleFlags[StringToInt(SplitValue(Instruction, 1 ))].Value;
                    StoreIntToMemory(SplitValue(Instruction, 2), Value);
                    FoundInstruction = true;
                }

                if (SplitValue(Instruction, 0 ) == "WriteDialogue"){
                    string Arg = ReadValue(Instruction , 14, Instruction.size() - 1);

                    if (IsNumber(Arg)){ // Pull from LG Langauge Pack
                        OmegaTechTextSystem.Write(GlobalPackData.Lines[StringToInt(SplitValue(Instruction, 1))]);
                    }
                    else {
                        OmegaTechTextSystem.Write(Arg);
                    }
                    FoundInstruction = true;
                }

                if (SplitValue(Instruction, 0 ) == "AddWDLInstruction"){
                    string WDLString = SplitValue(Instruction, 1 );
                    wstring ws(WDLString.begin(), WDLString.end());
                    ExtraWDLInstructions += ws;
                    FoundInstruction = true;
                }

                if (SplitValue(Instruction, 0 ) == "WipeExtraWDL"){
                    ExtraWDLInstructions = L"";
                    FoundInstruction = true;
                }

                if (SplitValue(Instruction, 0 ) == "SetScene"){
                    SetSceneFlag = true;
                    SetSceneId = StringToInt(SplitValue(Instruction, 1 ));
                    FoundInstruction = true;
                }

                if (SplitValue(Instruction, 0 ) == "Kill"){
                    ParasiteRunning = false;
                    FoundInstruction = true;
                }
                if (SplitValue(Instruction, 0 ) == "SetCameraPosition"){
                    SetCameraFlag = true;

                    int X = StringToInt(SplitValue(Instruction, 1 ));
                    int Y = StringToInt(SplitValue(Instruction, 2 ));
                    int Z = StringToInt(SplitValue(Instruction, 3 ));

                    SetCameraPos = {float(X),float(Y),float(Z)};

                    FoundInstruction = true;
                }

                if (SplitValue(Instruction, 0 ) == "SetMainLightPosition"){

                    int X = StringToInt(SplitValue(Instruction, 1 ));
                    int Y = StringToInt(SplitValue(Instruction, 2 ));
                    int Z = StringToInt(SplitValue(Instruction, 3 ));

                    SunPos = {float(X) ,float(Y),float(Z)};
                    
                    FoundInstruction = true;
                }

                if (SplitValue(Instruction, 0 ) == "CineFlowExec"){
                    FoundInstruction = true;
                    LoadCineFlowScript(TextFormat("GameData/Worlds/World%i/CineFlow/%s.cf", SceneIDMirror , SplitValue(Instruction, 1 ).c_str()));
                    UsingCineFlow = true;
                }

                if (SplitValue(Instruction, 0 ) == "GrassEnabled"){
                    FoundInstruction = true;
                    GrassEnabled = StringToInt(SplitValue(Instruction, 1 ));
                }

                if (SplitValue(Instruction, 0 ) == "PullMouse"){
                    FoundInstruction = true;

                    if (IsGamepadAvailable(0)){
                        VirtualMouseY += GetGamepadAxisMovement(0 , GAMEPAD_AXIS_LEFT_Y);
                        VirtualMouseX += GetGamepadAxisMovement(0 , GAMEPAD_AXIS_LEFT_X);

                        StoreIntToMemory( SplitValue(Instruction, 1 ), VirtualMouseX);
                        StoreIntToMemory( SplitValue(Instruction, 2 ), VirtualMouseY);
                    }
                    else {
                        StoreIntToMemory( SplitValue(Instruction, 1 ), GetMouseX());
                        StoreIntToMemory( SplitValue(Instruction, 2 ), GetMouseY());
                    }
                }

                if (SplitValue(Instruction, 0 ) == "PullInputs"){
                    FoundInstruction = true;

                    int InpValue = 0;

                    if (IsKeyDown(KEY_LEFT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)){
                        InpValue = 4;
                    }
                    if (IsKeyDown(KEY_RIGHT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)){
                        InpValue = 2;
                    }
                    if (IsKeyDown(KEY_UP) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP)){
                        InpValue = 1;
                    }
                    if (IsKeyDown(KEY_DOWN) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN)){
                        InpValue = 3;
                    }    
                    if (IsKeyDown(KEY_SPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)){
                        InpValue = 5;
                    }
                    if (IsKeyDown(KEY_A) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)){
                        InpValue = 6;
                    }
                    if (IsKeyDown(KEY_S) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT)){
                        InpValue = 7;
                    }

                    if (IsMouseButtonPressed(0) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1)){
                        InpValue = 8;
                    }
                    if (IsMouseButtonPressed(1) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)){
                        InpValue = 9;
                    }
                    
                    StoreIntToMemory(SplitValue(Instruction, 1 ) , InpValue);
                }

                if (SplitValue(Instruction, 0 ) == "BeginScene"){
                    FoundInstruction = true;
                    UsingParasiteDraw = true;
                }

                if (SplitValue(Instruction, 0 ) == "EndScene"){
                    FoundInstruction = true;
                    UsingParasiteDraw = false;
                }

                if (SplitValue(Instruction, 0 ) == "DrawFrame"){
                    FoundInstruction = true;

                    BeginTextureMode(ParasiteTarget);
                    ClearBackground(BLACK);

                    if (IsKeyPressed(KEY_ESCAPE)){
                        exit(0);
                    }

                }

                if (SplitValue(Instruction, 0 ) == "MouseState"){
                    FoundInstruction = true;

                    if (StringToInt(SplitValue(Instruction, 1 )) == 1){
                        ShowCursor();
                        EnableCursor();
                        VirtualMouseX = GetScreenWidth() / 2;
                        VirtualMouseY = GetScreenHeight() / 2;
                    }
                    else {
                        HideCursor();
                        DisableCursor();
                    }

                }

                if (SplitValue(Instruction, 0 ) == "DrawTexture"){
                    FoundInstruction = true;
                    int X = StringToInt(SplitValue(Instruction, 2 ));
                    int Y = StringToInt(SplitValue(Instruction, 3 ));
                    int Scale = StringToInt(SplitValue(Instruction, 4 ));

                    DrawTextureEx(PullTextureFromMemory(SplitValue(Instruction, 1 )) , {X , Y} , 0 , Scale, WHITE);
                }

                if (SplitValue(Instruction, 0 ) == "DrawLine"){
                    FoundInstruction = true;

                    int X = 0 ;
                    int Y = 0 ;
                    int DestX = 0 ;
                    int DestY = 0 ;

                    if (ScanVaribleMemory(SplitValue(Instruction, 1 ))){
                        X = PullIntFromMemory(SplitValue(Instruction, 1 ));
                    }
                    else {
                        X = StringToInt(SplitValue(Instruction, 1 ));
                    }

                    if (ScanVaribleMemory(SplitValue(Instruction, 2 ))){
                        Y = PullIntFromMemory(SplitValue(Instruction, 2));
                    }
                    else {
                        Y = StringToInt(SplitValue(Instruction, 2 ));
                    }

                    if (ScanVaribleMemory(SplitValue(Instruction, 3 ))){
                        DestX = PullIntFromMemory(SplitValue(Instruction, 3 ));
                    }
                    else {
                        DestX = StringToInt(SplitValue(Instruction, 3 ));
                    }

                    if (ScanVaribleMemory(SplitValue(Instruction, 4 ))){
                        DestY = PullIntFromMemory(SplitValue(Instruction, 4));
                    }
                    else {
                        DestY = StringToInt(SplitValue(Instruction, 4 ));
                    }

                    DrawLine(X , Y , DestX, DestY, WHITE);

                }

                if (SplitValue(Instruction, 0 ) == "DrawRectangle"){
                    FoundInstruction = true;

                    int X = 0 ;
                    int Y = 0 ;
                    int DestX = 0 ;
                    int DestY = 0 ;

                    if (ScanVaribleMemory(SplitValue(Instruction, 1 ))){
                        X = PullIntFromMemory(SplitValue(Instruction, 1 ));
                    }
                    else {
                        X = StringToInt(SplitValue(Instruction, 1 ));
                    }

                    if (ScanVaribleMemory(SplitValue(Instruction, 2 ))){
                        Y = PullIntFromMemory(SplitValue(Instruction, 2));
                    }
                    else {
                        Y = StringToInt(SplitValue(Instruction, 2 ));
                    }

                    if (ScanVaribleMemory(SplitValue(Instruction, 3 ))){
                        DestX = PullIntFromMemory(SplitValue(Instruction, 3 ));
                    }
                    else {
                        DestX = StringToInt(SplitValue(Instruction, 3 ));
                    }

                    if (ScanVaribleMemory(SplitValue(Instruction, 4 ))){
                        DestY = PullIntFromMemory(SplitValue(Instruction, 4));
                    }
                    else {
                        DestY = StringToInt(SplitValue(Instruction, 4 ));
                    }

                    DrawRectangle(X , Y , DestX, DestY, WHITE);

                }

                if (SplitValue(Instruction, 0 ) == "EndFrame"){
                    FoundInstruction = true;
                    EndTextureMode();

                    BeginDrawing();
                        DrawTexturePro(ParasiteTarget.texture, (Rectangle){ 0, 0, ParasiteTarget.texture.width, -ParasiteTarget.texture.height }, (Rectangle){ 0, 0, float(GetScreenWidth()), float(GetScreenHeight())}, (Vector2){ 0, 0 } , 0.f , WHITE);
                    EndDrawing();
                }

                if (CodeLogging)cout << Instruction << "\n";
            }
        }
    }
}
