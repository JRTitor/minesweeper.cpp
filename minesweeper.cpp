#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <fstream>

using namespace std;

int intFRange (const int& begin, const int& end) {
  random_device rd; // obtain a random number from hardware
  mt19937 gen(rd()); // seed the generator
  uniform_int_distribution<> distr(begin, end); // define the range
  return distr(gen); // generate numbers
}

class Cell {
private:
  bool open;
  bool mine;
  bool flag;
  int mineNear;
public:
  Cell() {
    open = false;
    mine = false;
    flag = false;
    mineNear = 0;
  }
  void Open() {
    open = true;
  }
  void Flag() {
    if (!GetOpen()) flag = true;
    else cout << "Cell has already been opened\n";
  }
  string Write(){
    string out;
    if (GetOpen()) out += "1 ";
    else out += "0 ";

    if (GetMine()) out += "1 ";
    else out += "0 ";

    if (GetFlag()) out += "1 ";
    else out += "0 ";

    out += to_string(GetMineCnt());

    return out;
  }
  void MineCnt(){++mineNear;}
  void mineInput(const int& cnt) {mineNear = cnt;}
  bool GetOpen() const {return open;}
  bool GetMine() const {return mine;}
  void setMine() {mine = true;}
  void setFlag() {flag = true;}
  bool GetFlag() const {return flag;}
  int GetMineCnt() const {return mineNear;}
};

ostream& operator<< (ostream& stream, const Cell& cll) {
  if (!cll.GetOpen() && !cll.GetFlag()) stream << "*";
  else{
    if (cll.GetFlag() && !cll.GetOpen()) stream << "F";
    else if(cll.GetMine()) stream << "M";
    else stream << cll.GetMineCnt();
  }
  return stream;
}

istream& operator>> (istream& stream, Cell& cll) {
  int op, mine, flag, num;
  stream >> op >> mine >> flag >> num;

  if(op == 1) cll.Open();
  if(mine == 1) cll.setMine();
  if (flag == 1) cll.setFlag();
  cll.mineInput(num);
  return stream;
}

class Field {
private:
  int xsize;
  int ysize;
  int bombs;
  vector<vector<Cell>> fld;
  void setBombsCnt(const int& bombcnt){
    int xbomb, ybomb;
    for(int i = 0; i < bombcnt; ++i) {
      xbomb = intFRange(0, xsize-1);
      ybomb = intFRange(0, ysize-1);
      fld.at(xbomb).at(ybomb).setMine();
      for (int j = xbomb - 1; j <= xbomb + 1; ++j) {
        for (int k = ybomb - 1; k <= ybomb + 1; ++k){
            if(j > -1  && k > -1 && j < xsize && k < ysize){
              if( ! fld.at(j).at(k).GetMine()) {
                fld.at(j).at(k).MineCnt();
            }
          }
        }
      }
    }
  }
  void openAround(int x, int y){
    for(int i = x-1; i <= x+1; ++i) {
      for (int j = y-1; j <= y+1; ++j) {
        if(j > -1 &&  i > -1 && i < xsize && j < ysize) {
          if( ! (fld.at(i).at(j).GetMine() || fld.at(i).at(j).GetOpen())) {
            OpenCell(i, j);
            openAround(i, j);
          }else {
            break;
          }
        }
      }
    }
  }
  void showAllMines(){
    for(int i = 0; i < xsize; ++i) {
      for(int j = 0; j < ysize; ++j) {
        if(fld.at(i).at(j).GetMine()){
          OpenCell(i, j);
        }
      }
    }
  }
public:
  Field() {
    xsize = 5;
    ysize = 5;
    bombs = intFRange(2, 5);
    Reset(xsize, ysize);
    setBombsCnt(bombs);
  }
  Field(const int& x, const int& y, const int& b) {
    xsize = x;
    ysize = y;
    Reset(xsize, ysize);
    bombs = b;
    setBombsCnt(bombs);
  }
  void Reset(const int &rows, const int &cols) {
    fld.resize(rows);
    for (int i = 0; i < rows; ++i) {
      fld.at(i).resize(cols);
    }
  }
  void OpenCell(const int& x, const int& y) {
    fld.at(x).at(y).Open();
  }
  void writeCell(const int& x, const int& y, Cell cll) {
    fld.at(x).at(y) = cll;
  }
  void setXY(const int& x, const int& y){
    xsize = x;
    ysize = y;
  }

  void Open(const int& row, const int& col) {
    OpenCell(row, col);
    if(fld.at(row).at(col).GetMine()){
      cout << "You've got mine and lost\n";
      showAllMines();
      return;
    }
    openAround(row, col);
  }
  void Flag(const int& row, const int& col) {
    if (col > -1 && row > -1 && col < ysize && row < xsize) {
      fld.at(row).at(col).setFlag();
    }else cout << "invalid range \n";
  }
  void Finish() {
    for(int i = 0; i < xsize; ++i) {
      for (int j = 0; j < ysize; ++j) {
        if (getCell(i, j).GetMine() && !getCell(i, j).GetFlag()) {
          cout << "You've have not flag all mines\n";
          showAllMines();
          return;
        }
      }
    }
    cout << "You've won\n";
    showAllMines();
  }
  void Save(const string& fileName){
    ofstream file (fileName);
    file << xsize << " " << ysize << " " <<bombs <<"\n";
    for(int i = 0; i < xsize; ++i) {
      for (int j = 0; j < ysize; ++j) {
        file << fld.at(i).at(j).Write();
        file <<"\n";
      }
    }
    file.close();
  }
  void setBoms (const int& mines) {bombs = mines;}
  int getX() const {return xsize;}
  int getY() const {return ysize;}
  int getBomb() const {return bombs;}
  Cell getCell(const int& x, const int& y) const {return fld.at(x).at(y);}
  void setCell(const int& x, const int& y, const Cell& cll) {
    fld.at(x).at(y) = cll;
  }
  void Load(const string& fileName){
    ifstream file (fileName);
    int x, y, minecnt;
    file >> x >> y >> minecnt;
    setXY(x, y);
    setBoms(minecnt);
    Reset(x, y);
    for (int i = 0; i < x; ++i) {
      for (int j = 0; j < y; ++j) {
        Cell cll;
        file >> cll;
        setCell(i, j, cll);
      }
    }
    file.close();
  }
};

ostream& operator<< (ostream& stream, const Field& fld) {

  for(int i = 0; i < fld.getX(); ++i) {
    if (i == 0) stream << "    ";
    stream << i << " ";
  }
  stream << "\n\n";

  for (int i = 0; i < fld.getX(); ++i) {
    for (int j = 0; j < fld.getY(); ++j) {
      if(j == 0){
        stream << i << "   ";
      }
      stream << fld.getCell(i, j) << " ";
    }
    stream << '\n';
  }
  return stream;
}

int main(int argc, char const *argv[]) {
  int a, x, y, g = 1;
  string action;
  Field f;
  cout <<"This is Mine Sweeper game\nYou can play default field configuration or customized field size and mine amount\n"<<
  "To play default game input: 1\nTo play customized game input: 2\nTo load game input:3\n";
  cin >> a;
  if(a == 1){
    f = Field();
  }else if (a == 2){
    int m;
    cout << "Input: rows columns number_of_mines\n";
    cin >> x >> y >> m;
    f = Field(x, y, m);
  }else if(a == 3) {
    cout << "Input file name:";
    cin >> action;
    f.Load(action);
  }else return 1;
  printf("\n");
  cout << f;
  printf("\n\n" );
  while (g == 1) {
    cout << "To open the sell input: row column Open\nTo Flag the cell input:row column Flag\n"
    <<"To save game: input -1 -1 SAVE\nTo end game and check mines input: -1 -1 FINISH\n";

    cin >> x >> y >> action;

    if (x!= -1 && y!= -1) {
      if (action == "Open") {
        f.Open(x, y);
        cout << f;
        if (f.getCell(x, y).GetMine()) break;
        continue;
      }else if (action == "Flag"){
        f.Flag(x, y);
        cout << f;
        continue;
      }
    }
    else if (x == -1 && y == -1) {
      if (action == "SAVE"){
        cout << "Input save name:";
        cin >> action;
        f.Save(action);
        cout << "save:"<<action<<" is made\n";
        break;
      }
      if (action == "FINISH") {
        f.Finish();
        cout << f;
        break;
      }
    }
  }
  return 0;
}
