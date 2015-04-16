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
const int MAX_DEPTH = 13;       // 探索する深さの最大値
const int COMPLETE  = INT_MAX;  // 盤面完成

unsigned long long xor128(){
  static unsigned long long rx=123456789, ry=362436069, rz=521288629, rw=88675123;
  unsigned long long rt = (rx ^ (rx<<11));
  rx=ry; ry=rz; rz=rw;
  return (rw=(rw^(rw>>19))^(rt^(rt>>8)));
}

// 完成した盤面
int completeBoard[BOARD_SIZE] = {
  W,  W,  W,  W,  W, W,
  W,  1,  2,  3,  4, W,
  W,  5,  6,  7,  8, W,
  W,  9, 10, 11, 12, W,
  W, 13, 14, 15,  E, W,
  W,  W,  W,  W,  W, W
};

// (y,x)の2次元座標を1次元に変換するための対応表
const int z_map[B_HEIGHT][B_WIDTH] = {
  {  0,  1,  2,  3,  4,  5},
  {  6,  7,  8,  9, 10, 11},
  { 12, 13, 14, 15, 16, 17},
  { 18, 19, 20, 21, 22, 23},
  { 24, 25, 26, 27, 28, 29},
  { 30, 31, 32, 33, 34, 35}
};

// 本来のポジション
const int correctY[17] = {
  0,
  1,1,1,1,
  2,2,2,2,
  3,3,3,3,
  4,4,4,4
};

const int correctX[17] = {
  0,
  1,2,3,4,
  1,2,3,4,
  1,2,3,4,
  1,2,3,4
};

// 2次元座標を1次元に変換
int getZ(int y, int x){
  return z_map[y][x];
}

// 盤面の「上, 右, 下, 左]」移動
const int dz[4] = {-B_WIDTH, 1, B_WIDTH, -1};

int board[BOARD_SIZE];      // 15パズル・ボード
int boardCopy[BOARD_SIZE];  // コピーボード

ull table[BOARD_SIZE][STATE_NUM];  // zoblist hash用テーブル
ull completeHash; // 完成形のボードのハッシュ値

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
  ull hashValue;
  int depth;
  int z;
  int beforeZ;
  int board[BOARD_SIZE];

  Node(){
    this->depth   = 0;
    this->z       = UNKNOWN;
    this->beforeZ = UNKNOWN;
  }
};

class Puzzle15{
  public:
    /*
     * ソルバーの初期化
     *   input: 初期盤面
     */
    void init(int *input){
      fprintf(stderr,"init =>\n");
      // zoblist hashの初期化
      initZobrist();

      memcpy(board, completeBoard, sizeof(board));
      // 完成形のハッシュ値を生成
      completeHash = getBoardHash();

      memcpy(board, input, sizeof(board));
      //showBoard();
    }

    /*
     * z1と空白の座標を交換
     *   z1: 交換座標1
     */
    void moveBoard(int z1){
      for(int i = 0; i < 4; i++){
        int z2 = z1 + dz[i];

        if(board[z2] == EMPTY){
          swap(board[z1], board[z2]);
          break;
        }
      }
    }

    /*
     * 盤面全体のマンハッタン距離を求める
     */
    int calcND(){
      int dist = 0;

      for(int y = 1; y <= HEIGHT; y++){
        for(int x = 1; x <= WIDTH; x++){
          int z = getZ(y,x);

          int dy = abs(y - correctY[board[z]]);
          int dx = abs(x - correctX[board[z]]);

          dist += 3 * min(dy, dx) + 5 * (max(dy, dx) - min(dy, dx));
        }
      }

      return dist;
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
      if(board[7] != 1) return false;
      if(board[13] != 5) return false;
      if(board[19] != 9) return false;
      if(board[25] != 13) return false;
      return true;
    }

    bool checkLeftLine2(){
      if(board[8] != 2) return false;
      if(board[14] != 6) return false;
      if(board[20] != 10) return false;
      if(board[26] != 14) return false;
      return true;
    }


    /*
     * 盤面全体のマンハッタン距離を求める
     */
    int calcMD(){
      int dist = 0;
      int ez = searchEmpty();
      int ey = ez / B_WIDTH + 1;
      int ex = ex % B_WIDTH + 1;

      for(int y = 1; y <= HEIGHT; y++){
        for(int x = 1; x <= WIDTH; x++){
          int z = getZ(y,x);
          int num = board[z];

          int dy = abs(y - correctY[num]);
          int dx = abs(x - correctX[num]);
          int ny = abs(ey - correctY[num]);
          int nx = abs(ex - correctX[num]);

          if(num == 1){
            dist += 2 * (dy*dy) + (dx*dx) + (ny*ny + nx*nx);
          }else{
            dist += (dy*dy) + (dx*dx) + (ny*ny + nx*nx);
          }
        }
      }

      return dist;
    }

    /*
     * IDの値を求める
     */
    int calcID(){
      int dist = 0;  

      for(int y = 1; y <= HEIGHT; y++){
        for(int x = 1; x <= WIDTH; x++){
          int z = getZ(y,x);

          int dy = correctY[board[z]];
          int dx = correctX[board[z]];

          int d = 3 * abs(y-dy) + abs(x-dx);
          dist += d / 3 + d % 3;
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

    int calcEval(){
      int value = 0;

      ull hash = getBoardHash();

      if(hash == completeHash) return COMPLETE;

      value -= calcMD();
      //value -= calcND();

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
    ull getBoardHash(){
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
     * 15パズルソルバー
     *   input: 初期盤面
     */
    vector<int> solve(int *input){
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

        // 現在の盤面を保存
        memcpy(boardCopy, board, sizeof(board));

        queue<Node> que;
        int maxValue = INT_MIN;
        int bestZ = UNKNOWN;
        que.push(root);

        while(!que.empty()){
          Node node = que.front(); que.pop();

          if(node.depth > MAX_DEPTH) continue;

          memcpy(board, node.board, sizeof(board));

          int z = searchEmpty();
          int value = calcEval();

          // 評価値が更新された場合
          if(maxValue < value && node.depth > 0 && beforeEmpty != node.z){
            maxValue = value;
            //fprintf(stderr,"Update maxValue = %d, z = %d, depth = %d\n", maxValue, node.z, node.depth);
            bestZ = node.z;
          }else if(node.depth > 0 && maxValue > 0 && value < 0){
            //fprintf(stderr,"diff maxValue = %d, value = %d\n", maxValue, value);
            continue;
          }

          hash = getBoardHash();

          if(checkList[hash]) continue;
          checkList[hash] = true;

          for(int i = 0; i < 4; i++){
            int nz = z + dz[i];

            // 移動先が壁ならスキップ
            if(board[nz] == WALL) continue;
            // 前の移動先と一緒であれば却下
            if(node.beforeZ == nz) continue;
            if(node.z == UNKNOWN && nz == beforeEmpty) continue;

            moveBoard(nz);

            Node nnode = createNode();
            nnode.depth = node.depth + 1;
            nnode.beforeZ = z;

            if(node.z == UNKNOWN){
              nnode.z = nz;
              que.push(nnode);
            }else{
              nnode.z = node.z;
              que.push(nnode);
            }

            moveBoard(z);
          }
        }

        // 盤面を元に戻す
        memcpy(board, boardCopy, sizeof(boardCopy));

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
        moveBoard(bestZ);
        hash = getBoardHash();

        // 盤面が完成したらループから抜ける
        if(hash == completeHash){
          fprintf(stderr,"Complete!\n");
          break;
        }

        moveCount++;
        //showBoard();

        //if(moveCount > 120) break;
      }

      fprintf(stdout,"move_count:%d\n", moveCount);
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
  int input[BOARD_SIZE];
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

  vector<int> answer = pz15.solve(input);

  for(int i = 0; i < answer.size(); i++){
    //cout << answer[i] << endl;
  }

  return 0;
}
