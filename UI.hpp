#pragma once

// DEPENDENCY: fltk


/**
The problem: ***FLTK is suxs!!***

This UI lib is supposed to be a usability layer that hides the FLTK 
while fixes the bugz and crep nonsenses that's still
after !!! 30+ years of development and keep continously coming up...

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

#include <FL/Fl_Widget.H>
#include <FL/Fl_Group.H>
#include "../misc/array_splice.hpp"
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl.H>
#include "TimePointSeries.hpp"
#include "Fl_ChartBox.hpp"
#include "../misc/safe.hpp"
#include "../misc/Logger.hpp"

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
    virtual void refresh() = 0;

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
        elems.clear();
    }

    template<typename T, typename... Args>
    T* create(Args... args) {
        T* elem = new T(args...);
        elems.push_back({ elem, [](void* elem) {
            if (elem) delete (T*)elem; 
            elem = nullptr;
        } });
        return elem;
    }

    template<typename T>
    size_t find(T* elem) {
        for (size_t n = 0; n < elems.size(); n++)
            if (elems[n].first == elem) return n;
        throw ERROR("UI Element is not found!");
    }

    template<typename T>
    void remove(T* elem) {
        size_t n = find(elem);
        elems[n].second(elems[n].first);
        array_splice(elems, n, 1);
    }

    void build(UI_Element* root) {
        SAFE(root)->build();
    }

protected:
    vector<pair<UI_Element*, function<void(void*)>>> elems;
};



class UI_Window: public UI_Element {
public:

    struct IdleData {
        void (*func)(void*);
        void* data;
        bool once;
        void (*wrap)(void*);
        bool first = true;
    };

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

    int run(void idle(void*) = nullptr, void* data = nullptr, bool once = false) {
        SAFE(window);
        if (argc && argv) window->show(argc, argv);
        else window->show();
        
        if (idle) {
            idle_data.func = idle;
            idle_data.data = data;
            idle_data.once = once;
            idle_data.wrap = [](void* idle_data_void) {
                IdleData* idle_data = (IdleData*)idle_data_void;
                if (idle_data->first) { // this is a hack since remove_idle doesn't really seems to be working
                    idle_data->func(idle_data->data);
                    if (idle_data->once) idle_data->first = false;
                }
            };
            Fl::add_idle(idle_data.wrap, (void*)&idle_data);
        }

        return Fl::run();
    }

    int run(
        int argc, char **argv, 
        void idle(void*) = nullptr, void* data = nullptr, bool once = false
    ) {
        this->argc = argc;
        this->argv = argv;
        return run(idle, data, once);
    }

    void refresh() override {
        // Process FLTK events to allow window to redraw
        // Fl::flush();  // or 
        // Fl::check(); // - both work
        // Optionally trigger redraw
        Fl::check();
        flwindow()->redraw();
    }

    // ============ ONLY FOR DEBUG, DO NOT USE!! ===============
    Fl_Window* flwindow() const { return SAFE(window); }
    Fl_Scroll* flscroll() const { return SAFE(scroll); }
    // =========================================================

protected:
    string title;
    int argc = 0;
    char **argv = nullptr;

    Fl_Window* window = nullptr;
    Fl_Scroll* scroll = nullptr;
    
private:

    static IdleData idle_data;

};
inline UI_Window::IdleData UI_Window::idle_data;

class UI_ScrollBox: public UI_Element {
public:
    UI_ScrollBox(
        int left, int top, int width, int height, 
        int scrollbarThickness = 15
    ):
        UI_Element(left, top, width, height),
        scrollbarThickness(scrollbarThickness)
    {}
    virtual ~UI_ScrollBox() {}

    Fl_Widget* build() override {
        checkBuilt(scroll);

        scroll = createFl<Fl_Scroll>(getAbsoluteLeft(), getAbsoluteTop(), width, height);

        Fl_Box* spacing = new Fl_Box(getAbsoluteLeft(), getAbsoluteTop(), width - scrollbarThickness, height);

        scroll->add(spacing);
        scroll->box(FL_DOWN_BOX);
        scroll->type(Fl_Scroll::BOTH_ALWAYS);
        scroll->scrollbar_size(scrollbarThickness);
        
        buildFl(scroll);
        return scroll;
    }

    Fl_Scroll* flscroll() const { return SAFE(scroll); }

protected:
    Fl_Scroll* scroll = nullptr;
    int scrollbarThickness;
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

    void addCandleSeries(const CandleSeries candleSeries, int pane = 0) {
        flchart()->addCandleSeries(candleSeries, pane);
    }

    void addBarSeries(const TimePointSeries barSeries, int pane = 0) {
        flchart()->addBarSeries(barSeries, pane);
    }

    void addPointSeries(const TimePointSeries pointSeries, int pane = 0) {
        flchart()->addPointSeries(pointSeries, pane);
    }

    void clearAllSerieses() {
        flchart()->clearAllSerieses();
    }

    void clearCandlesSerieses() {
        flchart()->clearCandlesSerieses();
    }

    void clearBarsSerieses() {
        flchart()->clearBarsSerieses();
    }

    void clearPointsSerieses() {
        flchart()->clearPointsSerieses();
    }

    void refresh() override {
        Fl::check();
        flchart()->redraw();
    }

    Fl_ChartBox* flchart() const { return SAFE(chart); }

protected:
    Fl_ChartBox* chart = nullptr;
};

class UI_MultiChart: public UI_ScrollBox {
public:
    UI_MultiChart(
        int left, int top, int width, int height,
        int scrollbarThickness,
        int spacing//, int chartHeight
    ): 
        UI_ScrollBox(left, top, width, height, scrollbarThickness),
        spacing(spacing), 
        nextChartTop(spacing),
        // chartHeight(chartHeight),
        chartWidth(width - spacing*2 - scrollbarThickness)
        
    {}

    virtual ~UI_MultiChart() {}

    void createChart(UI_Manager& ui, int n, int chartHeight) {
        for (int i = 0; i < n; i++) {
            UI_ChartBox* chartBox = ui.create<UI_ChartBox>(
                spacing, nextChartTop, chartWidth, chartHeight
            );
            chartBoxes.push_back(chartBox);
            add(chartBox);
            nextChartTop += chartHeight; // + (addSpacing ? spacing : 0);
        }
    }

    void addChartSpacing() {
        nextChartTop += spacing;
    }

    void removeCharts(UI_Manager& ui) {
        for (UI_ChartBox* chartBox: chartBoxes)
            ui.remove(chartBox);
        chartBoxes.clear();
        nextChartTop = spacing;
    }

    void addCandleSeries(
        const size_t chartno,
        const CandleSeries& candleSeries
    ) {
        chartBoxes.at(chartno)->addCandleSeries(candleSeries);
    }

    void clearChartsSeries() {
        for (UI_ChartBox* chartBox: chartBoxes)
            chartBox->clearAllSerieses();
    }

    void joinScroll() {
        int chartno = 0;
        for (UI_ChartBox* chartBox: chartBoxes) {
            chartBox->flchart()->scroll = [chartno, chartBox](int /*left*/, int /*top*/, int /*delta_x*/, int /*delta_y*/, int /*button*/) {
                DBG("TODO zoom all! " + to_string(chartno));
            };
            chartno++;
        }
    }
    
protected:
    int spacing, nextChartTop, /*chartHeight,*/ chartWidth;
    vector<UI_ChartBox*> chartBoxes;
};