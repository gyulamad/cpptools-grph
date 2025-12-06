#pragma once

/**
The problem: (FLTK is suxs!!)

This UI lib is supposed to be a usability layer that hides the FLTK 
while fixes the bugz and crep nonsenses that's still after 30+ years development
and keep continously coming up...

The goal:

Users no need to even know that it uses the FLTK as a backend but still
provides a seemles GUI integration and way less painfull coding experience
by abstracting away the FLTK nonsences to the hell!

Example:
```
    // Create a UI manager and the UI elements..

    UI_Manager ui;
    UI_Window* window = ui.create<UI_Window>(0, 0, 800, 500, "Testing");
    UI_ScrollBox* scroll = ui.create<UI_ScrollBox>(50, 50, 200, 200);
    UI_ChartBox* chart = ui.create<UI_ChartBox>(10, 10, 300, 200);

    // Insert the elements to each other 
    // (it will positioning the elements relative to the parents)

    scroll.add(chart);
    window.add(scroll);
    
    // Build the Window and run the event loop:

    window->build();

    // ... 
    // Here, after the build, 
    // you have a last chance to make UI related stuff
    // before the UI event loop takes over...

    return window->run(argc, argv);
```

Note:

This is an ad-hock project and it's still in development,
meaning it does not covers all the FLTK elements and "features".
In case if you need to cover your own custom FLTK widget or component,
see the UI_ChartBox element as an example, it covers my custom Chart view.

If you need or happened to have covered a "standard" FLTK bug or something,
feel free to contribute with an issue or pull request to this repository!
*/


#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Scroll.H>

#include <string>
#include <vector>
#include <functional>

#include "Fl_ChartBox.hpp"

using namespace std;

class UI_Element {
public:
    UI_Element(int left, int top, int width, int height):
        left(left), top(top), width(width), height(height) {}

    virtual ~UI_Element() {}

    int getLeft() const { return left; }
    int getTop() const { return top; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    int getAbsoluteLeft() const { return left + (parent ? parent->getAbsoluteLeft() : 0); }
    int getAbsoluteTop() const { return top + (parent ? parent->getAbsoluteTop() : 0); }

    void add(UI_Element* child) {
        SAFE(child);
        children.push_back(child);
        child->parent = this;
    }

    virtual Fl_Widget* build() = 0;

protected:

    template<typename T, typename... Args>
    T* createFl(Args... args) {
        T* fl_widget = new T(args...);       
        return fl_widget;
    }
    
    void buildFl(Fl_Group* fl_group) {
        fl_group->begin();
        for (UI_Element* child: children)
            fl_group->add(child->build());
        fl_group->end();
    }

    void buildFl(Fl_Widget* /*fl_widget*/) {
        // Do nothing with non-container widgets!
    }

    void checkBuilt(void* fl) {
        if (fl)
            throw ERROR("FL element already built");
    }

    int left, top, width, height;
    UI_Element* parent = nullptr;
    vector<UI_Element*> children;
};

class UI_Manager {
public:
    UI_Manager() {}
    
    virtual ~UI_Manager() {
        for (auto elem: elems) 
            elem.second(elem.first);
    }

    template<typename T, typename... Args>
    T* create(Args... args) {
        T* elem = new T(args...);
        elems.push_back({ elem, [](void* elem) { delete (T*)elem; } });
        return elem;
    }

protected:
    vector<pair<UI_Element*, function<void(void*)>>> elems;
};

class UI_Window: public UI_Element {
public:
    UI_Window(int left, int top, int width, int height, string title = ""):
        UI_Element(top, left, width, height), title(title) {}
    
    UI_Window(int left, int top, int width, int height, const char* title = nullptr):
        UI_Window(left, top, width, height, string(title ? title : "")) {}

    UI_Window(int width, int height, const char* title = nullptr):
        UI_Window(0, 0, width, height, string(title ? title : "")) {}

    virtual ~UI_Window() {}

    void setFlArgs(int argc, char **argv) {
        this->argc = argc;
        this->argv = argv;
    }

    Fl_Widget* build() override {
        checkBuilt(window);
        window = createFl<Fl_Window>(getAbsoluteLeft(), getAbsoluteTop(), width, height, title.c_str());
        window->begin();
        scroll = new Fl_Scroll(0, 0, width, height);
        scroll->type(Fl_Scroll::BOTH);
        Fl_Box* spacing = new Fl_Box(getAbsoluteLeft(), getAbsoluteTop(), 0, 0);
        scroll->add(spacing);
        window->add(scroll);    
        window->resizable(scroll);
        window->end();
        buildFl(scroll);
        return window;
    }

    int run() {
        SAFE(window);
        if (argc) window->show(argc, argv);
        else window->show();
        return Fl::run();
    }

    int run(int argc, char **argv) {
        this->argc = argc;
        this->argv = argv;
        return run();
    }

    Fl_Window* flwindow() const { return SAFE(window); }
    Fl_Scroll* flscroll() const { return SAFE(scroll); }

protected:
    string title;
    int argc = 0;
    char **argv;

    Fl_Window* window = nullptr;
    Fl_Scroll* scroll = nullptr;
};

class UI_ScrollBox: public UI_Element {
public:
    using UI_Element::UI_Element;
    virtual ~UI_ScrollBox() {}

    Fl_Widget* build() override {
        // checkBuilt(group);
        checkBuilt(scroll);

        scroll = createFl<Fl_Scroll>(getAbsoluteLeft(), getAbsoluteTop(), width, height);

        Fl_Box* spacing = new Fl_Box(getAbsoluteLeft(), getAbsoluteTop(), 0, 0);
        
        // spacing->box(FL_FLAT_BOX);
        // spacing->color(EGA_RED);
        // spacing->color2(EGA_RED);

        scroll->add(spacing);
        scroll->box(FL_DOWN_BOX);
        scroll->type(Fl_Scroll::BOTH);
        
        buildFl(scroll);
        return scroll;
    }

    // Fl_Group* flgroup() const { return SAFE(group); }
    Fl_Scroll* flscroll() const { return SAFE(scroll); }

protected:
    Fl_Group* group = nullptr;
    Fl_Scroll* scroll = nullptr;
};

class UI_ChartBox: public UI_Element {
public:
    using UI_Element::UI_Element;
    virtual ~UI_ChartBox() {}

    Fl_Widget* build() override {
        checkBuilt(chart);
        chart = createFl<Fl_ChartBox>(getAbsoluteLeft(), getAbsoluteTop(), width, height);
        return chart;
    }

    void addCandles(
        const vector<Candle>& candles, 
        const string& interval,
        unsigned int bullishColor = CHART_COLOR_BULLISH,
        unsigned int bearishColor = CHART_COLOR_BEARISH,
        double shoulderSpacing = 0.1
    ) {
        flchart()->addCandles(
            candles, 
            interval, 
            bullishColor, 
            bearishColor, 
            shoulderSpacing
        );
    }

    void addPoints(
        const vector<TimePoint>& points, 
        unsigned int color = CHART_COLOR_PLOTTER
    ) {
        flchart()->addPoints(points, color);
    }

    Fl_ChartBox* flchart() const { return SAFE(chart); }

protected:
    Fl_ChartBox* chart = nullptr;
};