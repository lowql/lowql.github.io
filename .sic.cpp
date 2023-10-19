#include<bits/stdc++.h> 
using namespace std;

class SIC_Line;
vector<SIC_Line> SIC_Program;

int current_Address=0;
int start_Address;

ofstream output("output.txt", ios::out);
ofstream loc("loc.txt", ios::out);

unordered_map<string, int> SYMTAB;
//hash brown
unordered_map<string, string> Opcode = {
    {"ADD","18"},{"AND","40"},{"COMP","28"},{"DIV","24"},{"J","3C"},{"JEQ","30"},{"JGT","34"},{"JLT","38"},{"JSUB","48"},{"LDA","00"},{"LDCH","50"},{"LDL","08"},{"LDX","04"},{"MUL","20"},{"OR","44"},{"RD","D8"},{"RSUB","4C"},{"STA","0C"},{"STCH","54"},{"STL","14"},{"STSW","E8"},{"STX","10"},{"SUB","1C"},{"TD","E0"},{"TIX","2C"},{"WD","DC"}
};

class SIC_Line
{
public:
    //loc = address
    int address=0;
    //source statement
    string address_Label = "";
    string mnemonic_Opcode = "";
    string operands = "";
    //object code
    string object_Code="";

    bool isDirectives = true;

    SIC_Line(string line)
    {
        stringstream ss;
        ss << line;
        getline(ss, address_Label, '\t');
        getline(ss, mnemonic_Opcode, '\t');
        getline(ss, operands, '\t');
    }
    void print()
    {
        cout << hex << address;
        cout << "\t" << address_Label << "\t" << mnemonic_Opcode << "\t" << operands << "\t" << object_Code << endl;
    }
     int cal_Size()
    {
        cout<<"mnemonic  "<<mnemonic_Opcode<<endl;
        if (mnemonic_Opcode == "START")
        {
            stringstream ss;
            start_Address= stoi(operands, 0, 16);
            address = start_Address;
            ss << "H"<< setw(6) << setfill(' ') << left<<address_Label << setw(6) << setfill('0') << hex<<right << start_Address;
            getline(ss,object_Code);
            return stoi(operands, 0, 16);
        }

        if (mnemonic_Opcode == "END")
        {
            stringstream ss;
            ss << "E" << setw(6) << setfill('0') << hex << start_Address;
            ss >> object_Code;
            return 0;
        }
        if (mnemonic_Opcode == "BYTE")
        {
            if (operands[0] == 'X')
            {
                for (int i = 2; i < operands.length() - 1; i++)object_Code += operands[i];
            }
            if (operands[0] == 'C')
            {
                stringstream ss;
                for (int i = 2; i < operands.length() - 1; i++)
                {
                    ss << uppercase << setw(2) << hex << setfill('0') << (int)operands[i];
                }
                ss >> object_Code;
            }
            return object_Code.length() / 2;
        }
        if (mnemonic_Opcode == "WORD")
        {
            stringstream ss;
            ss << uppercase << hex<< setw(6) << setfill('0')  <<stoi(operands);
            ss >> object_Code;
            return 3;
        }
        if (mnemonic_Opcode == "RESB")//no object code 
        {
            return stoi(operands);
        }
        if (mnemonic_Opcode == "RESW")//no object code
        {
            return stoi(operands) * 3;
        }
        isDirectives = false;
        return 3;
    }
    void pass1()
    {
        stringstream ss;
        address = current_Address;
        current_Address += cal_Size();
        if (address_Label != "")
        {
            SYMTAB[address_Label]=address;
        }
        loc << uppercase << setw(4) << setfill('0') << hex << address<<"\t";
        loc << address_Label << "\t" << mnemonic_Opcode << "\t" << operands << endl;
    }
 
   
    void pass2()
    {
      
        if(isDirectives==false)
        {
            int addressing_Mode = 0;
            if (operands!="" && operands[operands.length() - 2] == ',')
            {
                stringstream ss;
                ss << operands;
                getline(ss, operands, ',');
                addressing_Mode = 0x8000;
            }
            stringstream ss;
            ss << hex << uppercase << setw(4) << setfill('0') << (SYMTAB[operands]+addressing_Mode);
            ss >> object_Code;
            object_Code = Opcode[mnemonic_Opcode]+ object_Code;
        }
        output <<uppercase<< setw(4) << setfill('0') << hex << address << "\t";
        output << address_Label << "\t" << mnemonic_Opcode << "\t" << operands;
        if (!isDirectives || mnemonic_Opcode == "BYTE" || mnemonic_Opcode == "WORD")output << "\t" << object_Code;
        output << endl;
    }
};

int main()
{
    ifstream input_File("input.txt",ios::in);
    if (input_File.is_open())
    {
        string input_Line;
        while (getline(input_File, input_Line))
        {//fscanf(input_File,"[^\n]%*c",my_char_array)
            if(input_Line[0]!='.')SIC_Program.push_back(SIC_Line(input_Line));
        }
        input_File.close();
    }
    for (auto& L : SIC_Program)L.pass1();
    for (auto& L : SIC_Program)L.print();
    for (auto& L : SIC_Program)L.pass2();
    for (auto& L : SIC_Program)L.print();
    ofstream objectcode("objectcode.txt", ios::out);
    if (objectcode.is_open())
    {
        objectcode << uppercase << SIC_Program[0].object_Code << hex << setw(6) << setfill('0') << current_Address - start_Address << endl;
        
        int line_Start_Address = -1;
        string line_Object_Code = "";
        for (int i = 1; i < SIC_Program.size() - 1; i++)
        {
            if (SIC_Program[i].object_Code == "")continue;
            if (line_Start_Address == -1)line_Start_Address = SIC_Program[i].address;
            line_Object_Code += SIC_Program[i].object_Code;
            if (line_Object_Code.length() +SIC_Program[i+1].object_Code.length()>60 || SIC_Program[i + 1].object_Code == "" || SIC_Program[i + 1].mnemonic_Opcode == "END")
            {
                objectcode 
                << "T" 
                << uppercase << setw(6) << setfill('0') << hex << line_Start_Address 
                << setw(2) << setfill('0') << hex << line_Object_Code.length() / 2 
                << line_Object_Code
                << endl;
                
                line_Start_Address = -1;
                line_Object_Code = "";
            }
        }
        objectcode << uppercase << SIC_Program[SIC_Program.size() - 1].object_Code;
        objectcode.close();
        output.close();
        loc.close();
    }
    return 0;
}
