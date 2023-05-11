#include <sys/resource.h>
#include <sys/time.h>

#include "reversi.h"

typedef struct twoInt {
  int arr[26][26];
} twoInt;

typedef struct twoChar {
  char arr[26][26];
} twoChar;

typedef struct move {
  double bestVal;
  int row;
  int col;
} move;

void printBoard(char board[][26], int n) {}
bool positionInBounds(int n, int row, int col) {
  return (row < n && row >= 0 && col < n && col >= 0);
}
bool checkLegalInDirection(char board[][26], int n, int row, int col,
                           char colour, int deltaRow, int deltaCol) {}

bool checkder(twoChar node, int n, int row, int col, int deltaRow, int deltaCol,
              char colour) {
  bool same = false, diff = false;
  int possibleCol = col;
  int possibleRow = row;

  while (positionInBounds(n, row + deltaRow, col + deltaCol)) {
    row += deltaRow;
    col += deltaCol;
    if (node.arr[row][col] == 'U') {
      return false;
    } else if (node.arr[row][col] != colour) {
      diff = true;
    } else if (node.arr[row][col] == colour && diff) {
      same = true;
    } else {
      return false;
    }

    if (same && diff) break;
  }
  return (same && diff);
}

void updater(twoChar *node, int dim, int row, int col, char colour) {
  int stRow = row;
  int stCol = col;

  for (int deltaRow = -1; deltaRow <= 1; ++deltaRow) {
    for (int deltaCol = -1; deltaCol <= 1; ++deltaCol) {
      if (deltaRow != 0 || deltaCol != 0) {
        if (checkder(*node, dim, row, col, deltaRow, deltaCol, colour)) {
          while (node->arr[row + deltaRow][col + deltaCol] != colour) {
            row += deltaRow;
            col += deltaCol;
            node->arr[row][col] = (node->arr[row][col] == 'B') ? 'W' : 'B';
          }
          row = stRow;
          col = stCol;
          node->arr[row][col] = colour;
        }
      }
    }
  }
}

bool terminal(twoChar node, char colour, twoInt available, int n) {
  int colCount = 0, nullCount = 0, avail = 0;

  for (int k = 0; k < n; ++k) {
    for (int m = 0; m < n; ++m) {
      if (node.arr[k][m] == colour) colCount++;
      if (node.arr[k][m] == 'U') nullCount++;
      if (available.arr[k][m] == 1) avail++;
    }
  }

  if (colCount == 0 || nullCount == 0 || avail == 0) return true;

  return false;
}
twoInt search(twoChar node, int n, char turn) {
  twoInt available;

  for (int i = 0; i < n; ++i) {
    memset(available.arr[i], 0, n * sizeof(int));
  }

  int row = 0;
  int col = 0;

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      if (node.arr[i][j] == 'U') {
        for (int deltaRow = -1; deltaRow <= 1; ++deltaRow) {
          for (int deltaCol = -1; deltaCol <= 1; ++deltaCol) {
            if (deltaRow != 0 || deltaCol != 0) {
              bool same = false, diff = false;

              int possibleRow = i;
              int possibleCol = j;

              row = i;
              col = j;

              while (positionInBounds(n, row + deltaRow, col + deltaCol)) {
                row += deltaRow;
                col += deltaCol;
                if (node.arr[row][col] == 'U') {
                  break;
                } else if (node.arr[row][col] != turn) {
                  diff = true;
                } else if ((node.arr[row][col] == turn) && diff) {
                  same = true;
                } else {
                  break;
                }

                if (same && diff) {
                  available.arr[possibleRow][possibleCol] = 1;
                  break;
                }
              }
            }
          }
        }
      }
    }
  }

  return available;
}
move eval(twoChar node, int n, char colour) {
  move f;
  f.bestVal = 0;
  twoInt available = search(node, n, colour);
  twoInt ga = search(node, n, (colour == 'B') ? 'W' : 'B');

  double b = 0, w = 0;

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      if (available.arr[i][j] == 1) {
        (colour == 'B') ? b++ : w++;
      }

      if (ga.arr[i][j] == 1) {
        (colour == 'B') ? w++ : b++;
      }
    }
  }

  double mobility = 0, max = 0, min = 0, moveGain = 0;

  twoInt unocc;
  double bfrontier = 0, wfrontier = 0, front = 0;

  for (int k = 0; k < n; ++k) {
    memset(unocc.arr[k], 0, n * sizeof(int));
  }

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      if (node.arr[i][j] == 'B') {
        for (int k = -1; k <= 1; ++k) {
          for (int l = -1; l <= 1; ++l) {
            if (positionInBounds(n, i + l, j + k) &&
                node.arr[i + l][j + k] == 'U' && unocc.arr[i + l][j + k] != 1) {
              unocc.arr[i + l][j + k] = 1;
              bfrontier++;
            }
          }
        }
      }
    }
  }

  for (int k = 0; k < n; ++k) {
    memset(unocc.arr[k], 0, n * sizeof(int));
  }

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      if (node.arr[i][j] == 'W') {
        for (int k = -1; k <= 1; ++k) {
          for (int l = -1; l <= 1; ++l) {
            if (positionInBounds(n, i + l, j + k) &&
                node.arr[i + l][j + k] == 'U' && unocc.arr[i + l][j + k] != 2) {
              unocc.arr[i + l][j + k] = 2;
              wfrontier++;
            }
          }
        }
      }
    }
  }

  // Parity
  double blacktiles = 0;
  double whitetiles = 0;
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      if (node.arr[i][j] == 'B') {
        blacktiles++;
      } else if (node.arr[i][j] == 'W') {
        whitetiles++;
      }
    }
  }

  // Edges
  double wedges = 0, bedges = 0, edgescap = 0;
  for (int i = 1; i < 7; ++i) {
    (node.arr[0][i] == 'B') ? bedges++ : wedges++;
    (node.arr[i][0] == 'B') ? bedges++ : wedges++;
    (node.arr[7][i] == 'B') ? bedges++ : wedges++;
    (node.arr[i][7] == 'B') ? bedges++ : wedges++;
  }

  // Corners
  double bcorners = 0, wcorners = 0, corncap = 0;
  (node.arr[0][0] == 'B') ? bcorners++ : wcorners++;
  (node.arr[0][7] == 'B') ? bcorners++ : wcorners++;
  (node.arr[7][0] == 'B') ? bcorners++ : wcorners++;
  (node.arr[7][7] == 'B') ? bcorners++ : wcorners++;

  double bq = 0, wq = 0, q = 0;
  (node.arr[1][1] == 'B') ? bq++ : wq++;
  (node.arr[1][6] == 'B') ? bq++ : wq++;
  (node.arr[6][1] == 'B') ? bq++ : wq++;
  (node.arr[6][6] == 'B') ? bq++ : wq++;

  double sign = 1;

  if (colour == 'W') sign = -1;

  double parity =
      (sign * (blacktiles - whitetiles)) / (blacktiles + whitetiles) * 100;

  if (b + w != 0) {
    mobility = (sign * (b - w)) / (b + w) * 100;
  }
  if (bedges + wedges != 0) {
    edgescap = (sign * (bedges - wedges)) / (bedges + wedges) * 100;
  }
  if (bcorners + wcorners != 0) {
    corncap = (sign * (bcorners - wcorners)) / (bcorners + wcorners) * 100;
  }
  if (bfrontier + wfrontier != 0) {
    front = (sign * (bfrontier - wfrontier)) / (bfrontier + wfrontier) * 100;
  }

  double dead = ((colour == 'B') ? bq : wq);
  f.bestVal += (corncap * 5000);
  f.bestVal += (mobility * 3000);
  f.bestVal += (edgescap * 2000);
  f.bestVal += (front * 1000);
  f.bestVal += (parity * 10);
  f.bestVal -= 50 * dead;
  f.bestVal += ((colour == 'B') ? b : w) * 100;

  if (colour == 'B' && w == 0) {
    f.bestVal += 100;
  } else if (colour == 'W' && b == 0) {
    f.bestVal += 100;
  }

  return (f);
}
move minimax(twoChar node, int depth, bool max, char colour, double alpha,
             double beta, int n) {
  move p = {0, 0, 0};

  twoChar copy;
  twoInt available;

  available = search(node, n, colour);

  double v = 0;

  if (depth == 0 || terminal(node, colour, available, n)) {
    return eval(node, n, colour);
  } else if (max) {
    p.bestVal = -9999999999;

    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
        if (available.arr[i][j] == 1) {
          // Reset
          for (int k = 0; k < n; ++k) {
            memcpy(copy.arr[k], node.arr[k], n * sizeof(char));
          }

          updater(&copy, n, i, j, colour);

          v = minimax(copy, depth - 1, false, (colour == 'B') ? 'W' : 'B',
                      alpha, beta, n)
                  .bestVal;

          if (v > p.bestVal) {
            p.bestVal = v;
            p.row = i;
            p.col = j;
          }

          alpha = fmax(alpha, v);
          //   printf("%f", alpha);

          if (beta <= alpha) {
            p.row = i;
            p.col = j;
            return (p);
          }
        }
      }
    }
  } else {
    p.bestVal = 9999999999;

    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
        if (available.arr[i][j] == 1) {
          // Reset
          for (int k = 0; k < n; ++k) {
            memcpy(copy.arr[k], node.arr[k], n * sizeof(char));
          }

          updater(&copy, n, i, j, colour);

          v = minimax(copy, depth - 1, true, (colour == 'B') ? 'W' : 'B', alpha,
                      beta, n)
                  .bestVal;

          if (v < p.bestVal) {
            p.bestVal = v;
            p.row = i;
            p.col = j;
          }

          beta = fmin(beta, v);

          if (beta <= alpha) {
            p.row = i;
            p.col = j;
            return (p);
          }
        }
      }
    }
  }
  return (p);
}

int makeMove(char board[26][26], int n, char turn, int *row, int *col) {
  twoChar good;

  for (int k = 0; k < n; ++k) {
    memcpy(good.arr[k], board[k], n * sizeof(char));
  }

  twoInt available = search(good, n, turn);

  move v;
  struct rusage usage;
  struct timeval start, end;

  double time = 0, timeStart = 0;

  for (int i = 1; i < 5; ++i) {
    if (time < 1) {
      getrusage(RUSAGE_SELF, &usage);
      start = usage.ru_utime;
      timeStart = start.tv_sec + start.tv_usec / 1000000.0;  // in seconds

      v = minimax(good, i, true, turn, -99999999, 99999999, n);

      *row = v.row;
      *col = v.col;

      getrusage(RUSAGE_SELF, &usage);
      end = usage.ru_utime;
      time += (end.tv_sec + end.tv_usec / 1000000.0) - (timeStart);
    } else {
      break;
    }
  }
}

int main(void) { return 0; }
