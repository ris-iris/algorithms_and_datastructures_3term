#include <iostream>
#include <vector>
#define DESK_SIZE 64
#define SIZE 8
enum WLD {
  WIN,
  LOSS,
  DRAW
};

class Play {
 private:
  enum STEP {
    FIRST = 0,
    SECOND = 1
  };

  struct State {
    std::pair<int, int> escapee_coord;
    std::pair<int, int> terminator_coord;
    enum STEP step;
  };

  std::vector<std::vector<int>> graph;
  std::vector<WLD> wld;
  std::vector<std::vector<bool>> obstackles;
  std::vector<bool> used;
  std::vector<int> degree;
  State start;

 public:
  Play(std::vector<std::string> &desk) {
    graph.assign(DESK_SIZE * DESK_SIZE * 2, std::vector<int>(0));
    wld.assign(DESK_SIZE * DESK_SIZE * 2, DRAW);
    obstackles.assign(SIZE, std::vector<bool>(SIZE));
    used.assign(DESK_SIZE * DESK_SIZE * 2, false);
    degree.assign(DESK_SIZE * DESK_SIZE * 2, 0);

    for (int i = 0; i < SIZE; ++i) {
      for (int j = 0; j < SIZE; ++j) {
        if (desk[i][j] - '0' == 1) {
          obstackles[i][j] = true;
        } else {
          obstackles[i][j] = false;
          if (desk[i][j] - '0' == 2) {
            start.escapee_coord = std::make_pair(i, j);
          } else if (desk[i][j] - '0' == 3) {
            start.terminator_coord = std::make_pair(i, j);
          }
        }
      }
    }
    start.step = FIRST;
    InitPlayGraph();
  }

  WLD WhoWin() {
    for (int node = 0; node < DESK_SIZE * DESK_SIZE * 2; ++node) {
      if (!used[node] && (wld[node] == LOSS || wld[node] == WIN))
        dfs(node);
    }
    return wld[StateToNode(start)];
  }

 private:
  static int StateToNode(State &s) {
    return ((s.escapee_coord.first * 8 + s.escapee_coord.second) * DESK_SIZE
        + (s.terminator_coord.first * 8 + s.terminator_coord.second)) * 2 + s.step;
  }

  static State NodeToState(int n) {
    STEP s = static_cast<STEP>(n % 2);
    n /= 2;
    std::pair<int, int> tc = std::make_pair((n % DESK_SIZE) / 8, (n % DESK_SIZE) % 8);
    n /= DESK_SIZE;
    std::pair<int, int> ec = std::make_pair(n / 8, n % 8);
    return (struct State) {.escapee_coord = ec, .terminator_coord = tc, .step = s};
  }

  bool CanShut(State &s) {
    if (s.escapee_coord.first == s.terminator_coord.first) {
      int min = std::min(s.escapee_coord.second, s.terminator_coord.second);
      int max = std::max(s.escapee_coord.second, s.terminator_coord.second);
      for (int i = min; i < max; ++i) {
        if (obstackles[s.escapee_coord.first][i]) return false;
      }
      return true;
    }

    if (s.escapee_coord.second == s.terminator_coord.second) {
      int min = std::min(s.escapee_coord.first, s.terminator_coord.first);
      int max = std::max(s.escapee_coord.first, s.terminator_coord.first);
      for (int i = min; i < max; ++i) {
        if (obstackles[i][s.escapee_coord.second]) return false;
      }
      return true;
    }

    if (s.escapee_coord.first + s.escapee_coord.second == s.terminator_coord.first + s.terminator_coord.second) {
      int sum = s.escapee_coord.second + s.escapee_coord.first;
      int min = std::min(s.escapee_coord.first, s.terminator_coord.first);
      int max = std::max(s.escapee_coord.first, s.terminator_coord.first);
      for (int i = min; i < max; ++i) {
        if (obstackles[i][sum - i]) return false;
      }
      return true;
    }

    if (s.escapee_coord.first - s.escapee_coord.second == s.terminator_coord.first - s.terminator_coord.second) {
      int sum = s.escapee_coord.second - s.escapee_coord.first;
      int min = std::min(s.escapee_coord.first, s.terminator_coord.first);
      int max = std::max(s.escapee_coord.first, s.terminator_coord.first);
      for (int i = min; i < max; ++i) {
        if (obstackles[i][sum + i]) return false;
      }
      return true;
    }

    return false;
  }

  WLD WinLossDraw(State &s) {
    if (CanShut(s)) {
      return s.step == SECOND ? WIN : LOSS;
    }
    return s.step == FIRST && s.escapee_coord.first == 7 ? WIN : DRAW;
  }

  void InitPlayGraph() {
    for (int node = 0; node < DESK_SIZE * DESK_SIZE * 2; ++node) {
      State temp = NodeToState(node);
      if (obstackles[temp.escapee_coord.first][temp.escapee_coord.second]
          || obstackles[temp.terminator_coord.first][temp.terminator_coord.second]
          || temp.terminator_coord.first == temp.escapee_coord.first
              && temp.terminator_coord.second == temp.escapee_coord.second)
        continue;
      wld[node] = WinLossDraw(temp);
      if (wld[node] == DRAW) {
        temp.step = static_cast<STEP>((temp.step + 1) % 2);
        auto temp_player_coord = temp.step == FIRST ? &temp.terminator_coord : &temp.escapee_coord;
        for (int i = -1; i < 2; ++i) {
          for (int j = -1; j < 2; ++j) {
            if (i == 0 && j == 0) continue;
            temp_player_coord->first += i;
            temp_player_coord->second += j;
            if (temp_player_coord->first < SIZE && temp_player_coord->first > -1 && temp_player_coord->second < SIZE
                && temp_player_coord->second > -1 && !obstackles[temp_player_coord->first][temp_player_coord->second]) {
              graph[StateToNode(temp)].push_back(node);
              ++degree[node];
            }
            temp_player_coord->first -= i;
            temp_player_coord->second -= j;
          }
        }
      }
    }
  }

  void dfs(int node) {
    used[node] = true;
    for (auto next: graph[node]) {
      if (!used[next]) {
        if (wld[node] == LOSS) {
          wld[next] = WIN;
        } else if (--degree[next] == 0) {
          wld[next] = LOSS;
        } else {
          continue;
        }
        dfs(next);
      }
    }
  }

};

int main() {
  std::vector<std::string> matrix(SIZE);
  for (int i = 0; i < SIZE; ++i) {
    getline(std::cin, matrix[i]);
  }

  Play play(matrix);
  if (play.WhoWin() == WIN) {
    std::cout << 1;
  } else {
    std::cout << -1;
  }
  return 0;
}