#include <iostream>
#include <set>
#include <numeric>

struct Point {
  int x, y;
};

struct Vector {
  int x, y;

  Vector(Point a, Point b) {
    x = b.x - a.x;
    y = b.y - a.y;
  }
};

int CrossProductSign(Vector a, Vector b) {
  return (a.x * b.y - a.y * b.x < 0) ? -1 : 1;
}

bool AreCrossing(Point a1, Point a2, Point b1, Point b2) {
  bool different_parts_a = CrossProductSign(Vector(a1, a2), Vector(a1, b1)) != CrossProductSign(Vector(a1, a2),
                                                                                                Vector(a1, b2));
  bool different_parts_b = CrossProductSign(Vector(b1, b2), Vector(b1, a1)) != CrossProductSign(Vector(b1, b2),
                                                                                                Vector(b1, a2));
  return different_parts_a && different_parts_b;

}

std::pair<int, int> FindCrossingPlace(Point a, Vector A, Point b, Vector B) {
  std::pair<int, int> cross_a;
  cross_a.first = (a.y - b.y) * B.x - (a.x - b.x) * B.y;
  cross_a.second = B.y * A.x - B.x * A.y;
  int gcd = std::gcd(std::abs(cross_a.first), std::abs(cross_a.second));
  gcd = std::max(1, gcd);
  cross_a.first /= gcd;
  cross_a.second /= gcd;

  return cross_a;
}

int NumOfBridges(Point a, Point b, int num_of_rivers, std::istream& instream) {
  Point river_start, river_end;
  std::set<std::pair<int, int>> bridges;
  Vector road(a, b);
  for (int i = 0; i < num_of_rivers; ++i) {
    instream >> river_start.x >> river_start.y >> river_end.x >> river_end.y;
    Vector river(river_start, river_end);
    if (AreCrossing(a, b, river_start, river_end)) {
      std::pair<int, int> bridge = FindCrossingPlace(a, road, river_start, river);
      bridges.insert(bridge);
    }
  }
  return bridges.size();
}

int main() {
  int n;
  Point a, b;
  std::cin >> a.x >> a.y >> b.x >> b.y;
  std::cin >> n;
  std::cout << NumOfBridges(a, b, n, std::cin);
}