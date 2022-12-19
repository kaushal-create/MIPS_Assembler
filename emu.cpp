// Name: Kaushal Raj
// Roll No. - 2001CS36

#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <array>
using namespace std;

int Memory[1 << 24]; // It is the main memory of code
vector<string> code; // This stores the code
int A, B, PC, SP, idx, cont, exec;
pair<int, int> change;

// Functions to perform as per Mnemonic
// Load accumulator with the value specified
void ldc(int value)
{
  B = A;
  A = value;
}

void adc(int value)
{
  A += value;
}

void ldl(int offset)
{
  B = A;
  A = Memory[SP + offset];
  change = make_pair(SP + offset, 0);
  exec = 1;
}

void stl(int offset)
{
  change = make_pair(Memory[SP + offset], A);
  Memory[SP + offset] = A;
  exec = 2;
  A = B;
}

void ldnl(int offset)
{
  A = Memory[A + offset];
  change = make_pair(SP + offset, 0);
  exec = 1;
}

void stnl(int offset)
{
  change = make_pair(Memory[A + offset], B);
  Memory[A + offset] = B;
  exec = 2;
}

void add(int off)
{
  A += B;
}

void sub(int off)
{
  A = B - A;
}

void shl(int off)
{
  A = B << A;
}

void shr(int off)
{
  A = B >> A;
}

void adj(int value)
{
  SP = SP + value;
}

void a2sp(int off)
{
  SP = A;
  A = B;
}

void sp2a(int off)
{
  B = A;
  A = SP;
}

void call(int offset)
{
  B = A;
  A = PC;
  PC += offset;
}

void ret(int offset)
{
  PC = A;
  A = B;
}

void brz(int offset)
{
  if (A == 0)
    PC = PC + offset;
}

void brlz(int offset)
{
  if (A < 0)
    PC = PC + offset;
}

void br(int offset)
{
  PC = PC + offset;
}

string mnem[] = {"ldc", "adc", "ldl", "stl", "ldnl", "stnl", "add", "sub",
                 "shl", "shr", "adj", "a2sp", "sp2a", "call", "return", "brz", "brlz", "br", "HALT"};
void (*call_func[])(int) = {ldc, adc, ldl, stl, ldnl, stnl, add, sub, shl, shr, adj, a2sp, sp2a, call, ret, brz, brlz, br};

string decToHex(unsigned int num)
{
  // This function converts decimal to hex
  stringstream ss;
  ss << hex << num;
  string conv = ss.str();
  reverse(conv.begin(), conv.end());
  while ((int)conv.size() < 8)
  {
    conv += '0';
  }

  reverse(conv.begin(), conv.end());
  return conv;
}

void help()
{
  // Entry message
  cout << "Following functions are implemented:" << endl;
  cout << "1. Memory Dump using instruction: -dump" << endl;
  cout << "2. Emulate code one line at time: using instruction: -t" << endl;
  cout << "3. Emulate till the end using: -run" << endl;
  cout << "4. Show registers and SP values: -reg" << endl;
  cout << "5. Show instruction set using: -isa" << endl;
  cout << "6. Read operations using: -read" << endl;
  cout << "7. Write operations using: -write" << endl;
}

void showMem()
{
  // Prints the memory
  for (int i = 0; i < (int)code.size(); i++)
  {
    cout << decToHex(i) << " ";
    for (int j = i; j < min(i + 4, (int)code.size()); j++)
    {
      cout << decToHex(Memory[j]) << " ";
    }
    i += 3;
    cout << endl;
  }
}
void showReg()
{
  // Showa the register
  cout << "A: " << decToHex(A) << "     "
       << "B: " << decToHex(B) << "     "
       << "SP: " << decToHex(SP) << "     "
       << "PC: " << decToHex(PC + 1) << "     " << mnem[PC] << endl;
}
void readPrint()
{
  cout << "Reading memory[" << decToHex(PC) << "], has value: " << decToHex(change.first) << endl;
}
void writePrint()
{
  cout << "Writing memory[" << decToHex(PC) << "], from " << decToHex(change.first) << " to " << decToHex(change.second) << endl;
}
void inset()
{
  cout << "OpMachineCode Mnemonic Operand" << '\n';
  cout << "0      ldc      value " << endl;
  cout << "1      adc      value " << endl;
  cout << "2      ldl      value " << endl;
  cout << "3      stl      value " << endl;
  cout << "4      ldnl     value " << endl;
  cout << "5      stnl     value " << endl;
  cout << "6      add            " << endl;
  cout << "7      sub            " << endl;
  cout << "9      shr            " << endl;
  cout << "10     adj      value " << endl;
  cout << "11     a2sp           " << endl;
  cout << "12     sp2a           " << endl;
  cout << "13     call     offset" << endl;
  cout << "14     return         " << endl;
  cout << "15     brz      offset" << endl;
  cout << "16     brlz     offset" << endl;
  cout << "17     br       offset" << endl;
  cout << "18     HALT           " << endl;
}

bool runCode(int oper, int times = (1 << 25))
{
  // This runs the code or perform operations
  int iter = 0;
  while (times-- and PC < code.size())
  {
    cont++;
    if (PC >= code.size() or cont > (int)3e7)
    {
      cout << "Segmentation Fault" << endl;
      return false;
    }
    string cur = code[PC];
    int op = stoi(cur.substr(6, 2), 0, 16);
    if (op == 18)
    {
      cout << "HALT found" << endl;
      cout << cont << " Statements were executed total" << endl;
      return true;
    }
    int val = stoi(cur.substr(0, 6), 0, 16);
    if (val >= (1 << 23))
    {
      val -= (1 << 24);
    }
    if (times == 0)
      showReg();
    (call_func[op])(val);
    if (oper == 1 and exec == 1)
    {
      readPrint();
      exec = 0;
    }
    else if (oper == 2 and exec == 2)
    {
      writePrint();
      exec = 0;
    }
    PC++;
    idx++;
  }
  return true;
}

bool runEmu()
{
  // Interactive emylator run
  cout << "Enter command or 0 to exit:" << endl;
  string s;
  cin >> s;
  if (s == "0")
  {
    exit(0);
  }
  else if (s == "-dump")
  {
    showMem();
  }
  else if (s == "-reg")
  {
    showReg();
  }
  else if (s == "-t")
  {
    bool ret = runCode(0, 1);
    return ret;
  }
  else if (s == "-run")
  {
    bool ret = runCode(0);
    return ret;
  }
  else if (s == "-isa")
  {
    inset();
  }
  else if (s == "-read")
  {
    bool ret = runCode(1);
    return ret;
  }
  else if (s == "-write")
  {
    bool ret = runCode(2);
    return ret;
  }
  else
  {
    cout << "Enter correct instruction" << endl;
  }
  return true;
}

int main(int argc, char **argv)
{
  // Input function
  string fileName = argv[1];
  ifstream file(fileName, ios::in | ios::binary);
  unsigned int read;
  int pos = 0;
  while (file.read((char *)&read, sizeof(int)))
  {
    Memory[pos++] = read;
    code.push_back(decToHex(read));
    // cout << decToHex(read) << endl;
  }
  help();
  while (1)
  {
    runEmu();
  }
  return 0;
}