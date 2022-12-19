// Name: Kaushal Raj
// Roll No. - 2001CS36

// to compile code: g++ -o asm asm.cpp
// to run test file: ./asm test1.asm

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

struct table
{
  // The table will be store ASM code in following manner
  // ----------------------------------------------------------------------------------------------
  //  Label | Mnemonic | Operand | Operand Type (Hex, Oct etc) | Is label present in that line(Y/N)
  // ----------------------------------------------------------------------------------------------
  string label;
  string mnemonic;
  string operand;
  int oprType;
  bool labelPresent;
};
#define vp vector<pair<int, string> >
#define mp map<string, pair<string, int> >

vector<table> data(10000);  // Data vector in Table form
mp OPC;                     // Stores mnemonic and OPCode
vp errors;                  // Stores all the errors
vector<string> cleaned;     // Contains the code line in clean form w/o spaces
map<string, int> labels;    // Stores the labels and their declaration line
vp machineCode;             // Stores the machine code and extra info gor generating listCode
vector<int> programCounter; // This vector maintains Program counter on every line
bool haltPresent = false;   // Check if HALT is present in the Code
string fileName;            // Name of the file

// function prototype
void init();                  // Initialization of the Mnemonic, OPCode
bool isOctal(string s);       // These are helping functions and not related to assembler
bool isHexadecimal(string s); // Thus they are defined completely below main
bool isDecimal(string s);
bool validName(string cur);
void genErrors(int line, string type);
string clean(string s, int line);
void fillData(int i, string one, string two, string three, int type);
void implementSET();
void processLabel();
int calType(string s);
void tableForm();
void firstPass();
void makeDataSegment();
bool seeErrors();
void secondPass();
void writeToFile();

int main(int argc, char **argv)
{
  fileName = argv[1];

  // Calling required functions
  firstPass();

  // if first parsing passed sucessfully
  if (seeErrors())
  {
    secondPass();
    // write to all files
    writeToFile();
  }
  return 0;
}

void init()
{
  // Initialization of the Mnemonic, OPCode
  //  0 = Takes no operands like add
  //  1 = Takes 1 operand and is value, Ex: ldc 5
  //  2 = Takes 1 operand and is offset, Ex: call return
  //  No OPCODE for data and SET
  // SET is a pseudo instruction
  OPC["data"] = make_pair("", 1);
  OPC["ldc"] = make_pair("00", 1);
  OPC["adc"] = make_pair("01", 1);
  OPC["ldl"] = make_pair("02", 2);
  OPC["stl"] = make_pair("03", 2);
  OPC["ldnl"] = make_pair("04", 2);
  OPC["stnl"] = make_pair("05", 2);
  OPC["add"] = make_pair("06", 0);
  OPC["sub"] = make_pair("07", 0);
  OPC["shl"] = make_pair("08", 0);
  OPC["shr"] = make_pair("09", 0);
  OPC["adj"] = make_pair("0A", 1);
  OPC["a2sp"] = make_pair("0B", 0);
  OPC["sp2a"] = make_pair("0C", 0);
  OPC["call"] = make_pair("0D", 2);
  OPC["return"] = make_pair("0E", 0);
  OPC["brz"] = make_pair("0F", 2);
  OPC["brlz"] = make_pair("10", 2);
  OPC["br"] = make_pair("11", 2);
  OPC["HALT"] = make_pair("12", 0);
  OPC["SET"] = make_pair("", 1);
}

string decToHex(int number, int add = 24)
{
  // This functions converts decimal number to Hexadecimal
  if (add == 32)
  {
    unsigned int num = number;
    stringstream ss;
    ss << hex << num;
    return ss.str();
  }
  if (number < 0)
    number += (1 << add);
  stringstream ss;
  ss << hex << number;
  return ss.str();
}

void genErrors(int line, string type)
{
  // This functions stores the errors in errors vector
  errors.push_back(make_pair(line + 1, "Error at line no. : " + to_string(line) + " -- Type: " + type));
}

// function removes unnecessary spaces
string clean(string s, int line)
{
  for (int i = 0; i < 2; i++)
  {
    reverse(s.begin(), s.end());
    while (s.back() == ' ' or s.back() == '\t')
    {
      s.pop_back();
    }
  }
  // cout<<s<<endl;
  string temp;
  for (int i = 0; i < (int)s.size(); i++)
  {
    // if comment found
    if (s[i] == ';')
      break;
    if (s[i] == ':')
    {
      temp += ":";
      if (i == (int)s.size() - 1 or s[i + 1] != ' ')
        temp += " ";
      continue;
    }
    if (s[i] != ' ' and s[i] != '\t')
    {
      temp += s[i];
      continue;
    }
    temp += " ";
    int j = i;
    while (s[i] == s[j] and j < (int)s.size())
      j++;
    i = j - 1;
  }
  while (!temp.empty() and (temp.back() == ' ' or temp.back() == '\t'))
    temp.pop_back();
  int spac = 0;
  for (int i = 0; i < temp.size(); i++)
  {
    spac += (temp[i] == ' ');
  }
  if (spac > 2)
    genErrors(line + 1, "Invalid syntax");
  // cout<<temp<<endl;
  return temp;
}

void implementSET()
{
  // This function implements SET mnemonic given.
  // Since SET is pseudo instruction, we implement it with other mnemonic
  vector<string> temp;
  for (int i = 0; i < (int)cleaned.size(); i++)
  {
    string cur;
    bool state = false;
    for (int j = 0; j < (int)cleaned[i].size(); j++)
    {
      cur += cleaned[i][j];
      if (cleaned[i][j] == ':')
      {
        cur.pop_back();
        if (cleaned[i].size() > j + 5 and cleaned[i].substr(j + 2, 3) == "SET")
        {
          state = true;
          if (abs(labels[cur]) == i)
          {
            // programCounter[i] = (int)stoi(cleaned[i].substr(j + 6, (int)cleaned[i].size() - (j + 6)));
            labels[cur] = (int)temp.size() - 1;
            temp.push_back(cleaned[i].substr(0, j + 1) + " data " + cleaned[i].substr(j + 6, (int)cleaned[i].size() - (j + 6)));
          }
          break;
        }
      }
    }
    if (!state and !cleaned[i].empty())
      temp.push_back(cleaned[i]);
  }
  cleaned = temp;
}

// This function processes Lables and stores the labels map, with the postion they are declared
// error whenever there is multiple decleration of labels except for data & set
void processLabel()
{
  for (int i = 0; i < (int)cleaned.size(); i++)
  {
    string cur;
    for (int j = 0; j < (int)cleaned[i].size(); j++)
    {
      if (cleaned[i][j] == ':')
      {
        // checking for alphanumeric label name starts with alphabet
        bool is = validName(cur);
        if (!is)
        {
          genErrors(i + 1, "Invalid label name");
          break;
        }
        if (labels.count(cur))
        {
          genErrors(i + 1, "Multiple declaration of label: " + cur);
        }
        labels[cur] = i;
        break;
      }
      cur += cleaned[i][j];
    }
  }
}

// Fills the data vector to reduce code size and make clean
void fillData(int i, string one, string two, string three, int type)
{
  data[i].label = one;
  data[i].mnemonic = two;
  data[i].operand = three;
  data[i].oprType = type;
}

// This functions process the data as:
// ----------------------------------------------------------
//  Label | Mnemonic | Operand | Operand Type (Hex, Oct etc)
// ----------------------------------------------------------
// Stores the data in above form in table vector
void tableForm()
{
  int pc = 0;
  for (int i = 0; i < (int)cleaned.size(); i++)
  {
    string word[10] = {"", "", "", ""}, cur = "";
    int ptr = 1;
    for (int j = 0; j < (int)cleaned[i].size(); j++)
    {
      if (cleaned[i][j] == ':')
      {
        word[0] = cur;
        cur = "";
        j++;
        continue;
      }
      else if (cleaned[i][j] == ' ')
      {
        word[ptr++] = cur;
        cur = "";
        continue;
      }
      cur += cleaned[i][j];
      if (j == (int)cleaned[i].size() - 1)
        word[ptr++] = cur;
    }
    if (!word[0].empty())
    {
      data[i].labelPresent = true;
    }
    else
    {
      data[i].labelPresent = false;
    }
    if (word[1] == "HALT")
    {
      haltPresent = true;
    }

    if (!word[0].empty())
    {
      labels[word[0]] = pc;
    }
    programCounter[i] = pc;
    if (ptr == 1)
    {
      fillData(i, word[0], "", "", 0);
      continue;
    }
    pc++;
    // if mnemonic is not present in the mnemonic table
    if (!OPC.count(word[1]))
    {
      genErrors(i + 1, "Invalid Mnemonic");
      continue;
    }
    // if required number of operand is not present
    if (min(OPC[word[1]].second, 1) != ptr - 2)
    {
      genErrors(i + 1, "Invalid OPCode-Syntax combination");
      continue;
    }
    fillData(i, word[0], word[1], word[2], calType(word[2]));

    // if label called but not defined earlier
    if (data[i].oprType == 1 and !labels.count(data[i].operand))
    {
      genErrors(i + 1, "No such label / data variable");
    }
    else if (data[i].oprType == -1)
    {
      genErrors(i + 1, "Invalid number");
    }
  }
}

// This function, seprates the code in DATA segment, so the instructions are executed properly
void makeDataSegment()
{
  vector<string> instr, dataseg;
  for (int i = 0; i < (int)cleaned.size(); i++)
  {
    bool state = false;
    for (int j = 0; j < cleaned[i].size(); j++)
    {
      if (cleaned[i].substr(j, 4) == "data" and j + 4 < cleaned[i].size())
      {
        dataseg.push_back(cleaned[i]);
        state = true;
        break;
      }
      if (cleaned[i].back() == ':' and i + 1 < (int)cleaned.size() and cleaned[i + 1].substr(0, 4) == "data")
      {
        dataseg.push_back(cleaned[i]);
        state = true;
        break;
      }
    }
    if (!state)
      instr.push_back(cleaned[i]);
  }
  instr.insert(instr.end(), dataseg.begin(), dataseg.end());
  cleaned = instr;
}

// First pass of assembler which uses the functions declared above
void firstPass()
{
  ifstream infile;

  // checking valid input extension file
  if (fileName.substr(fileName.size() - 4) != ".asm")
  {
    cout << "File extension mismatched." << endl;
    exit(0);
  }

  infile.open(fileName);
  fileName = fileName.substr(0, fileName.size() - 4);
  // cout << fileName << endl;
  if (infile.fail())
  {
    cout << "Input file doesn't exist, please make sure file is in same directory as the code!" << endl;
    exit(0);
  }
  string s;
  // creating clean code
  while (getline(infile, s))
  {
    string cur = clean(s, (int)cleaned.size());
    // cout << cur << endl;
    cleaned.push_back(cur);
  }
  init();         // Initializing the mnemonics -> opcode -> number of operand(s)
  processLabel(); // Process labels in the code Ex: var1:
  if (errors.empty())
  {
    implementSET(); // Implementing SET mnemonic
  }
  data.resize((int)cleaned.size());           // Allocates memory for table
  programCounter.resize((int)cleaned.size()); // Allocates memory for programCounter array
  makeDataSegment();                          // Seprates the code in data segment and code
  tableForm();                                // Makes the code in table form as states in the problem
}

// Stores the errors or warnings in the file: filename.log
bool seeErrors()
{
  ofstream outErrors(fileName + ".log");
  outErrors << "Log code generated in: " << fileName << ".log" << endl;
  if (errors.empty())
  {
    outErrors << "No errors found!" << endl;
    cout << "No errors found!" << endl;
    if (haltPresent == false)
    {
      cout << "1 warning detected" << endl;
      outErrors << "Warning: HALT not present!" << endl;
    }
    outErrors << "Machine code generated in: " << fileName << ".o" << endl;
    outErrors << "Listing code generated in: " << fileName << ".l" << endl;
    outErrors.close();
    return true;
  }
  sort(errors.begin(), errors.end());
  cout << (int)errors.size() << " Errors encountered! See logFile.log" << endl;
  vp::iterator it;
  for (it = errors.begin(); it != errors.end(); it++)
  {
    outErrors << (*it).second << endl;
  }
  outErrors.close();
  return false;
}

// Small function to append zero at end to make it of 6 bit
string appZero(string s, int sz = 6)
{
  // Example: F changes to 00000F
  reverse(s.begin(), s.end());
  while ((int)s.size() < sz)
  {
    s += '0';
  }
  reverse(s.begin(), s.end());
  return s;
}

int calType(string s)
{
  // This functions return whether the operand is label/ Hex value/ Decimal value/ Octal value
  if (s.empty())
    return 0;
  if (s[0] == '+' or s[0] == '-')
  {
    reverse(s.begin(), s.end());
    s.pop_back();
    reverse(s.begin(), s.end());
  }
  if (s.empty())
    return -1;
  else if (isDecimal(s))
    return 10;
  else if (isOctal(s))
    return 8;
  else if (isHexadecimal(s))
    return 16;
  else if (validName(s))
    return 1;
  return -1;
}

// Second pass of assembler
void secondPass()
{
  // Its converts the code to machine code and also generates Listing code
  for (int i = 0; i < (int)data.size(); i++)
  {
    if (cleaned[i].empty())
    {
      continue;
    }
    string location = appZero(decToHex(programCounter[i]));

    // if label is only in the line
    if (data[i].mnemonic == "")
    {
      string curMacCode = "        ";
      machineCode.push_back(make_pair(i, curMacCode));
      continue;
    }
    // if operand is label
    if (data[i].oprType == 1)
    {
      int decForm;
      if (OPC[data[i].mnemonic].second == 2)
      {
        int val = labels[data[i].operand];
        decForm = val - (programCounter[i] + 1);
      }
      else
      {
        decForm = labels[data[i].operand];
      }
      string curMacCode = appZero(decToHex(decForm)) + OPC[data[i].mnemonic].first;
      machineCode.push_back(make_pair(i, curMacCode));
    }
    // if there is no operand
    else if (data[i].oprType == 0)
    {
      string curMacCode = "000000" + OPC[data[i].mnemonic].first;
      machineCode.push_back(make_pair(i, curMacCode));
    }
    else
    {
      int sz = 6, add = 24;
      if (data[i].mnemonic == "data")
        sz = 8, add = 32;
      int decForm = stoi(data[i].operand, 0, data[i].oprType);
      string curMacCode = appZero(decToHex(decForm, add), sz) + OPC[data[i].mnemonic].first;
      machineCode.push_back(make_pair(i, curMacCode));
    }
  }
}

void writeToFile()
{
  // THis functions writes machine code to the file:
  // 1. Listing Code: listCode.txt
  // 2. Machine code: machineCode.o
  ofstream outList(fileName + ".l");
  vp::iterator it;
  for (it = machineCode.begin(); it != machineCode.end(); it++)
  {
    outList << appZero(decToHex(programCounter[(*it).first]),8) << " " << (*it).second << " " << cleaned[(*it).first] << endl;
  }
  outList.close();
  ofstream outMachineCode;

  outMachineCode.open(fileName + ".o", ios::binary | ios::out);

  for (it = machineCode.begin(); it != machineCode.end(); it++)
  {
    unsigned int x;
    if ((*it).second.empty() or (*it).second == "        ")
      continue;
    stringstream ss;
    ss << hex << (*it).second;
    // cout << ss.str() << endl;
    ss >> x; // output it as a signed type
    int b = static_cast<int>(x);
    outMachineCode.write((const char *)&x, sizeof(unsigned int));
  }
  outMachineCode.close();
  cout << "Log code generated in: " << fileName << ".log" << endl;
  cout << "Machine code generated in: " << fileName << ".o" << endl;
  cout << "Listing code generated in: " << fileName << ".l" << endl;
}

bool isOctal(string s)
{
  // Funtion to check whether the string is Octal or not
  if ((int)s.size() < 2)
    return false;
  bool ok = true;
  for (int i = 1; i < (int)s.size(); i++)
  {
    ok &= (s[i] >= '0' and s[i] <= '7');
  }
  return ok & (s[0] == '0');
}

bool isHexadecimal(string s)
{
  // Funtion to check whether the string is in HexaDecimal form or not
  bool state = true;
  if ((int)s.size() < 3)
    return false;
  state &= (s[0] == '0') & ((s[1] == 'x' or s[1] == 'X'));
  for (int i = 2; i < (int)s.size(); i++)
  {
    bool st = (s[i] >= '0' and s[i] <= '9');
    st |= ((s[i] >= 'a' and s[i] <= 'f') or ((s[i] >= 'A' and s[i] <= 'F')));
    state &= st;
  }
  return state;
}

bool isDecimal(string s)
{
  // Funtion to check whether the string is in decimal form or not
  bool flag = true;
  for (int i = 0; i < (int)s.size(); i++)
  {
    flag &= (s[i] >= '0' and s[i] <= '9');
  }
  return flag;
}

bool validName(string cur)
{
  // Check whether the label name is valid or not
  for (int i = 0; i < cur.size(); i++)
  {
    if ((cur[i] >= 'a' and cur[i] <= 'z') or (cur[i] >= 'A' and cur[i] <= 'Z') or (cur[i] >= '0' and cur[i] <= '9') or (cur[i] == '_'))
      continue;
    return false;
  }
  if ((cur[0] >= 'a' and cur[0] <= 'z') or (cur[0] >= 'A' and cur[0] <= 'Z') or (cur[0] == '_'))
    return true;
  return false;
}