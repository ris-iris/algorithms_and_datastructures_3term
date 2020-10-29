#include <iostream>
#include <algorithm>
#include <math.h>
#include <iomanip>
#include <list>
#include <vector>

struct Point {
  double x, y;
  Point() = default;
  Point(double start, double end) : x(start), y(end) {}
};

bool operator<(const Point &a, const Point &b) {
  return a.x < b.x || a.x == b.x && a.y < b.y;
}

bool operator==(const Point &a, const Point &b) {
  return a.x == b.x && a.y == b.y;
}

struct Vector {
  double x, y;

  Vector(Point a, Point b) {
    x = b.x - a.x;
    y = b.y - a.y;
  }

  double length() {
    return sqrt(x * x + y * y);
  }

  Vector &operator-=(const Vector &b) {
    this->x = this->x - b.x;
    this->y = this->y - b.y;
    return *this;
  }
};

int CrossProductSign(Vector a, Vector b) {
  if (a.x * b.y - a.y * b.x < 0) {
    return -1;
  } else if (a.x * b.y - a.y * b.x == 0) {
    return 0;
  } else {
    return 1;
  }
}

bool IsLeftRotation(Point a, Point b, Point c) {
  return CrossProductSign(Vector(a, b), Vector(b, c)) > 0;
}

bool OneLine(Point a, Point b, Point c) {
  return CrossProductSign(Vector(a, b), Vector(b, c)) == 0;
}

bool MidleIsMidle(Point a, Point b, Point c) {
  return (a.x < b.x && b.x < c.x || a.x > b.x && b.x > c.x) || (a.y < b.y && b.y < c.y || a.y > b.y && b.y > c.y)
      || c == b || a == b;
}

int Graham(std::vector<Point> &points) {
  auto least_point = points.begin();
  for (auto p = points.begin(); p != points.end(); ++p) {
    if (*p < *least_point) std::swap(*least_point, *p);
  }
  std::sort(++points.begin(), points.end(), [&points](Point a, Point b) {
    return IsLeftRotation(points[0], a, b) || OneLine(points[0], a, b) && MidleIsMidle(points[0], a, b);
  });
  int right_bound = 1;
  for (int i = 2; i < points.size(); ++i) {
    while (!IsLeftRotation(points[right_bound - 1], points[right_bound], points[i]) && right_bound > 0) --right_bound;
    std::swap(points[i], points[right_bound + 1]);
    ++right_bound;
  }
  return ++right_bound;
}

double MinConvexHullLength(std::vector<Point> &points, int border) {
  double len = 0;
  Point prev_point = points.front();
  for (int i = 0; i < border; ++i) {
    len += Vector(prev_point, points[i]).length();
    prev_point = points[i];
  }
  len += Vector(prev_point, points[0]).length();
  return len;
}

int main(int argc, char *argv[]) {
  int n;
  std::vector<Point> points;
  Point p;
  std::cin >> n;
  for (int i = 0; i < n; ++i) {
    std::cin >> p.x >> p.y;
    points.push_back(p);
  }
  std::cout << std::setprecision(std::numeric_limits<long double>::digits10 + 1)
            << MinConvexHullLength(points, Graham(points));

  return 0;
}