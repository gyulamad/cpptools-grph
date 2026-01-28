#pragma once
// LCOV_EXCL_START
// Coverage excluded for mock/helper classes - not testing mock implementations

#include "../../misc/Canvas.hpp"
#include <string>
#include <stdexcept>

using namespace std;

class MockCanvas : public Canvas {
public:
    MockCanvas(int width = 800, int height = 600) : canvasWidth(width), canvasHeight(height), failLine {}
    {}
    
    void line(int left1, int top1, int left2, int top2, unsigned int color, int style = 0) override {
        if (failLine) throw runtime_error("MockCanvas line failed");
    }
    void circle(int left, int top, int radius, unsigned int color) override {}
    void circlef(int left, int top, int radius, unsigned int color) override {}
    void rect(int left, int top, int width, int height, unsigned int color) override {}
    void rectf(int left, int top, int width, int height, unsigned int color) override {}
    void text(int left, int top, const string& txt, unsigned int color, int font = 0, int size = 14) override {}
    void measure(const string& text, int& width, int& height, int& descent, int font = 0, int size = 14) override {
        width = 0;
        height = 0;
        descent = 0;
    }
    int width() override { return canvasWidth; }
    int height() override { return canvasHeight; }
    void clear() override {}
    
    bool failLine;
    
private:
    int canvasWidth;
    int canvasHeight;
};

// LCOV_EXCL_STOP
