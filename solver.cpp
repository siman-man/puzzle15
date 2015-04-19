#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <limits.h>
#include <string>
#include <string.h>
#include <cstring>
#include <sstream>
#include <set>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <stack>
#include <queue>

using namespace std;

typedef long long ll;
typedef unsigned long long ull;

const int HEIGHT     = 4;                    // ボードの高さ
const int B_HEIGHT   = HEIGHT + 2;           // ボードの高さ(番兵付き)
const int WIDTH      = 4;                    // ボードの横
const int B_WIDTH    = WIDTH + 2;            // ボードの横(番兵付き)
const int BOARD_SIZE = B_HEIGHT * B_WIDTH;   // ボード全体の大きさ

const int E         = 16; // 空白地点
const int EMPTY     = 16; // 空白地点
const int W         = 17; // 壁
const int WALL      = 17; // 壁
const int STATE_NUM = 18; // 状態の数(0-17)

const int UNKNOWN   = -1;       // 未定
const int COMPLETE  = INT_MAX;  // 盤面完成

int MAX_DEPTH   = 70;       // 探索する深さの最大値
int BEAM_WIDTH  = 2000;     // ビーム幅

unsigned long long xor128(){
  static unsigned long long rx=123456789, ry=362436069, rz=521288629, rw=88675123;
  unsigned long long rt = (rx ^ (rx<<11));
  rx=ry; ry=rz; rz=rw;
  return (rw=(rw^(rw>>19))^(rt^(rt>>8)));
}

// 完成した盤面
char completeBoard[BOARD_SIZE] = {
  W,  W,  W,  W,  W, W,
  W,  1,  2,  3,  4, W,
  W,  5,  6,  7,  8, W,
  W,  9, 10, 11, 12, W,
  W, 13, 14, 15,  E, W,
  W,  W,  W,  W,  W, W
};

// (y,x)の2次元座標を1次元に変換するための対応表
const char z_map[B_HEIGHT][B_WIDTH] = {
  {  0,  1,  2,  3,  4,  5},
  {  6,  7,  8,  9, 10, 11},
  { 12, 13, 14, 15, 16, 17},
  { 18, 19, 20, 21, 22, 23},
  { 24, 25, 26, 27, 28, 29},
  { 30, 31, 32, 33, 34, 35}
};

// 本来のポジション(Y座標)
const char correctY[17] = {
  0,
  1,1,1,1,
  2,2,2,2,
  3,3,3,3,
  4,4,4,4
};

// 本来のポジション(X座標)
const char correctX[17] = {
  0,
  1,2,3,4,
  1,2,3,4,
  1,2,3,4,
  1,2,3,4
};

const int WD[HEIGHT][WIDTH] = {
  {  0,  1,  6, 11},
  {  1,  4,  7, 12},
  {  6,  7, 10, 15},
  { 11, 12, 15, 16},
};

// 2次元座標を1次元に変換
inline char getZ(int y, int x){
  return z_map[y][x];
}

// 盤面の「上, 右, 下, 左]」移動
const char dz[4] = {-B_WIDTH, 1, B_WIDTH, -1};

char board[BOARD_SIZE];      // 15パズル・ボード
char boardCopy[BOARD_SIZE];  // コピーボード

ull table[BOARD_SIZE][STATE_NUM];  // zoblist hash用テーブル
ull completeHash; // 完成形のボードのハッシュ値

int answer[81];

void initZobrist(){
  for(int y = 0; y < B_HEIGHT; ++y){
    for(int x = 0; x < B_WIDTH; ++x){
      int z = getZ(y,x);

      for(int i = 0; i < STATE_NUM; i++){
        table[z][i] = xor128();
      }
    }    
  }
}

// 探索用ノード
struct Node{
  char board[BOARD_SIZE];
  char depth;
  char z;
  char beforeZ;
  char emptyZ;
  int value;
  ull hashValue;

  Node(){
    this->depth   = 0;
    this->z       = UNKNOWN;
    this->beforeZ = UNKNOWN;
    this->value   = UNKNOWN;
  }

  bool operator >(const Node &e) const{
    return value < e.value;
  }    
};

class Puzzle15{
  public:
    /*
     * ソルバーの初期化
     *   input: 初期盤面
     */
    void init(char *input){
      fprintf(stderr,"init =>\n");
      // zoblist hashの初期化
      initZobrist();

      memcpy(board, completeBoard, sizeof(board));
      // 完成形のハッシュ値を生成
      completeHash = getBoardHash();

      // 初期盤面をコピー
      memcpy(board, input, sizeof(board));
      //showBoard();
    }

    /*
     * z1と空白の座標を交換
     *   z1: 交換座標1
     */
    inline void moveBoard(int z1, int z2){
      swap(board[z1], board[z2]);
    }

    bool beforeUpperLine(){
      if(board[13] != 1) return false;
      if(board[7] != 2) return false;
      if(board[8] != 3) return false;
      if(board[16] != 4) return false;
      return true;
    }

    bool checkUpperLine(){
      if(board[7] != 1) return false;
      if(board[8] != 2) return false;
      if(board[9] != 3) return false;
      if(board[10] != 4) return false;
      return true;
    }

    bool checkUpperLine2(){
      if(board[13] != 5) return false;
      if(board[14] != 6) return false;
      if(board[15] != 7) return false;
      if(board[16] != 8) return false;
      return true;
    }

    bool checkUpperLine3(){
      if(board[19] != 9) return false;
      if(board[20] != 10) return false;
      if(board[21] != 11) return false;
      if(board[22] != 12) return false;
      return true;
    }

    bool checkLeftLine(){
      //if(board[7] != 1) return false;
      if(board[13] != 5) return false;
      if(board[19] != 9) return false;
      if(board[25] != 13) return false;
      return true;
    }

    bool checkLeftLine2(){
      //if(board[8] != 2) return false;
      if(board[14] != 6) return false;
      if(board[20] != 10) return false;
      if(board[26] != 14) return false;
      return true;
    }


    /*
     * 盤面全体のマンハッタン距離を求める
     */
    int calcMD(int ez){
      int dist = 0;
      int ey = ez / B_WIDTH + 1;
      int ex = ex % B_WIDTH + 1;

      for(int y = 1; y <= HEIGHT; y++){
        for(int x = 1; x <= WIDTH; x++){
          int z = getZ(y,x);
          int num = board[z];

          int dy = y - correctY[num];
          int dx = x - correctX[num];
          int ny = ey - correctY[num];
          int nx = ex - correctX[num];

          if(num == 1){
            dist += (dy*dy) + (dx*dx) + (ny*ny + nx*nx);
          }else if(num != EMPTY){
            dist += (dy*dy) + (dx*dx);
          }

          //dist += WD[dy][dx] + WD[ny][nx];
        }
      }

      return dist;
    }

    /*
     * 盤面から空白地点を探す
     */
    int searchEmpty(){
      for(int y = 1; y <= HEIGHT; y++){
        for(int x = 1; x <= WIDTH; x++){
          int z = getZ(y,x);

          if(board[z] == EMPTY) return z;
        }
      }

      return UNKNOWN;
    }

    /*
     * ノードの作成
     */
    Node createNode(){
      Node node;

      memcpy(node.board, board, sizeof(board));

      return node;
    }

    int calcEval(int emptyZ){
      int value = 0;

      ull hash = getBoardHash();

      if(hash == completeHash) return COMPLETE;

      value -= calcMD(emptyZ);

      if(board[28] <= 4) value -= 5;
      if(board[7] == 13 || board[7] == 14 || board[7] == 15) value -= 10;
      if(board[10] == 13 || board[10] == 14 || board[10] == 15) value -= 10;

      if(checkUpperLine()){
        value += 1000;

        if(checkLeftLine()){
          value += 1000;

          if(checkUpperLine2()){
            value += 1000;

            if(checkLeftLine2()){
              value += 1000;

              if(checkUpperLine3()){
                value += 1000;
              }
            }
          }else if(checkLeftLine2()){
            value += 1000;

            if(checkUpperLine2()){
              value += 1000;
            }
          }
        }else if(checkUpperLine2()){
          value += 1000;

          if(checkLeftLine()){
            value += 1000;

            if(checkLeftLine2()){
              value += 1000;
            }
          }
        }
      }

      return value;
    }

    /*
     * 盤面のハッシュ値を取得(zoblist hash)
     */
    inline ull getBoardHash(){
      ull value = 0;

      for(int y = 1; y <= HEIGHT; y++){
        for(int x = 1; x <= WIDTH; x++){
          int z = getZ(y,x);

          value ^= table[z][board[z]];
        }
      }

      return value;
    }

    /*
     * 15パズルソルバー(幅優先型)
     *   input: 初期盤面
     */
    vector<int> solve(char *input){
      ull hash;
      vector<int> result;
      vector<ull> mapHistory;

      init(input);

      int moveCount = 0;
      int beforeEmpty = UNKNOWN;

      while(true){
        map<ull, bool> checkList;

        for(int i = 0; i < moveCount-1; i++){
          checkList[mapHistory[i]] = true;
        }

        Node root = createNode();
        root.emptyZ = searchEmpty();

        // 現在の盤面を保存
        memcpy(boardCopy, board, sizeof(board));

        queue<Node> que;
        int maxValue = INT_MIN;
        int bestZ = UNKNOWN;
        priority_queue< Node, vector<Node>, greater<Node> > pque;
        pque.push(root);

        while(!pque.empty()){
          map<int, int> directCount;

          for(int i = 0; i < BEAM_WIDTH && !pque.empty(); i++){
            Node n = pque.top(); pque.pop();

            if(n.depth > MAX_DEPTH) continue;
            if(directCount[n.z] > BEAM_WIDTH/2) continue;
            directCount[n.z] += 1;

            que.push(n);
          }

          if(!pque.empty()){
            priority_queue< Node, vector<Node>, greater<Node> > temp_pque;
            pque = temp_pque;
          }

          while(!que.empty()){
            Node node = que.front(); que.pop();
            memcpy(board, node.board, sizeof(board));

            int z = node.emptyZ;
            int value = node.value;

            // 評価値が更新された場合
            if(maxValue < value && node.depth > 0 && beforeEmpty != node.z){
              maxValue = value;
              bestZ = node.z;
            }else if(maxValue > 0 && node.depth > 0 && value < 0){
              continue;
            }else if(maxValue == COMPLETE){
              break;
            }

            hash = getBoardHash();

            if(checkList[hash]) continue;
            checkList[hash] = true;

            for(int i = 0; i < 4; i++){
              int nz = z + dz[i];

              // 移動先が壁ならスキップ
              if(board[nz] == WALL) continue;
              // 前の移動先と一緒であれば探索しない
              if(node.beforeZ == nz) continue;
              // 前の局面と同じにならないように
              if(node.z == UNKNOWN && nz == beforeEmpty) continue;

              moveBoard(z, nz);

              Node nnode = createNode();
              nnode.depth = node.depth + 1;
              nnode.beforeZ = z;
              nnode.emptyZ = nz;
              nnode.value = calcEval(nz);

              nnode.z = (node.z == UNKNOWN)? nz : node.z;

              pque.push(nnode);

              moveBoard(nz, z);
            }
          }
        }

        // 盤面を元に戻す
        memcpy(board, boardCopy, sizeof(boardCopy));

        // 盤面が完成しない
        if(bestZ == UNKNOWN){
          fprintf(stderr,"Err! maxValue = %d\n", maxValue);
          showBoard();
          moveCount = 9999;
          break;
        }

        hash = getBoardHash();
        mapHistory.push_back(hash);
        beforeEmpty = searchEmpty();

        fprintf(stderr,"bestZ = %d, num = %d\n", bestZ, board[bestZ]);
        result.push_back(board[bestZ]);
        moveBoard(beforeEmpty, bestZ);
        moveCount++;
        hash = getBoardHash();

        // 盤面が完成したらループから抜ける
        if(hash == completeHash){
          fprintf(stderr,"Complete!\n");
          break;
        }

        //showBoard();
      }

      //fprintf(stdout,"move_count:%d\n", moveCount);
      //fprintf(stderr,"move count = %d\n", moveCount);
      return result;
    }

    /*
     * 盤面の表示(デバッグ用)
     */
    void showBoard(){
      for(int y = 1; y <= HEIGHT; y++){
        for(int x = 1; x <= WIDTH; x++){
          int z = getZ(y,x);

          fprintf(stderr, "%02d ", board[z]);
        }
        fprintf(stderr,"\n");
      }
    }
};

int main(){
  Puzzle15 pz15;
  char input[BOARD_SIZE];
  string str;

  // WALLで初期化
  for(int i = 0; i < BOARD_SIZE; i++){
    input[i] = WALL;
  }

  for(int y = 1; y <= HEIGHT; y++){
    for(int x = 1; x <= WIDTH; x++){
      cin >> str;

      int z = getZ(y,x);

      if(str != "*"){
        input[z] = atoi(str.c_str());
        //input[z] = stoi(str);
      }else{
        input[z] = EMPTY;
      }
    }
  }

  vector<int> answer;
  answer = pz15.solve(input);

  for(int i = 0; i < answer.size(); i++){
    cout << answer[i] << endl;
  }

  return 0;
}
