#include <iostream>
#include <set>
#include <numeric>
#include <vector>

struct Point {
  int x, y;
};

std::istream& operator>> (std::istream& is, Point& p)
{
  is >> p.x >> p.y;
  return is;
}

struct Vector {
  int x, y;

  Vector(Point a, Point b) {
    x = b.x - a.x;
    y = b.y - a.y;
  }
};

struct Segment{
  Point start;
  Point end;
  Segment() = default;
  Segment(Point& a, Point& b) : start(a), end(b) {}
  Vector GetVector(){
    return Vector(start, end);
  }
};

std::istream& operator>> (std::istream& is, Segment& s)
{
  is >> s.start >> s.end;
  return is;
}


int CrossProductSign(Vector a, Vector b) {
  return (a.x * b.y - a.y * b.x < 0) ? -1 : 1;
}

bool AreCrossing(Segment a, Segment b) {
  bool different_parts_a = CrossProductSign(a.GetVector(), Vector(a.start, b.start)) != CrossProductSign(a.GetVector(),
                                                                                                Vector(a.start, b.end));
  bool different_parts_b = CrossProductSign(b.GetVector(), Vector(b.start, a.start)) != CrossProductSign(b.GetVector(),
                                                                                                Vector(b.start, a.end));
  return different_parts_a && different_parts_b;

}

std::pair<int, int> FindCrossingPlace(Segment A, Segment B) {
  std::pair<int, int> cross_a;
  cross_a.first = (A.start.y - B.start.y) * B.GetVector().x - (A.start.x - B.start.x) * B.GetVector().y;
  cross_a.second = B.GetVector().y * A.GetVector().x - B.GetVector().x * A.GetVector().y;
  int gcd = std::gcd(std::abs(cross_a.first), std::abs(cross_a.second));
  gcd = std::max(1, gcd);
  cross_a.first /= gcd;
  cross_a.second /= gcd;

  return cross_a;
}

std::vector<Segment> GetRivers(int num_of_rivers, std::istream& instream){
  Segment river;
  std::vector<Segment> rivers;
  for (int i = 0; i < num_of_rivers; ++i) {
    instream >> river;
    rivers.push_back(river);
  }
  return rivers;
}

int NumOfBridges(Point a, Point b, std::vector<Segment> rivers) {
  Point river_start, river_end;
  std::set<std::pair<int, int>> bridges;
  Segment road(a, b);
  for (auto river:rivers) {
    if (AreCrossing(road, river)) {
      std::pair<int, int> bridge = FindCrossingPlace(road, river);
      bridges.insert(bridge);
    }
  }
  return bridges.size();
}

int main() {
  int n;
  Point a, b;
  std::cin >> a >> b;
  std::cin >> n;
  std::cout << NumOfBridges(a, b, GetRivers(n, std::cin));
}