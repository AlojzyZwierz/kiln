
#pragma once
struct Point {
    unsigned long x; // czas w ms
    float y;         // temperatura w °C
  };

  struct Line {
    float a;  // nachylenie
    float b;  // przesunięcie

    // Konstruktor z a i b
    Line(float aIn, float bIn) : a(aIn), b(bIn) {}

    // Konstruktor z dwóch punktów
    Line(Point p1, Point p2) {
        a = (p2.y - p1.y) / (p2.x - p1.x);
        b = p1.y - a * p1.x;
    }
    Line(int x1, int y1, int x2, int y2) {
        a = (y2 - y1) / (x2 - x1);
        b = y1 - a * x1;
    }

    Line(int x1, float y1, int x2, float y2) {
        a = (y2 - y1) / (x2 - x1);
        b = y1 - a * x1;
    }
    Line(){a=0;b=0;}
    // Zwróć y dla podanego x
    float y(unsigned long x) const {
        return a * x + b;
    }

    // Zwróć x dla podanego y
    unsigned long x(float y) const {
        return (y - b) / a;
    }
};

