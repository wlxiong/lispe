/*
 *  LispE
 *
 * Copyright 2020-present NAVER Corp.
 * The 3-Clause BSD License
 */
//
//  lispe_gui.cxx
//

#ifndef lispe_gui_h
#define lispe_gui_h


class Doublewindow;

class Lispwidget : public Element {
public:
    LispE* lisp;
    Fl_Widget* widget;
    Element* function;
    Element* object;
    

    Lispwidget(LispE* lsp, short t, Element* f, Element* o) : Element(t) {
        lisp = lsp;
        widget = NULL;
        object = null_;
        function = null_;
        
        if (o != null_ && o != NULL) {
            object = new List;
            object->append(lisp->provideAtom(l_quote));
            object->append(o);
            lisp->garbaging(object);
        }

        if (f != null_ && f != NULL)
            function = f;
        
        //In case an error occurs, we use this mechanism to ensure a clean
        //deletion of FLTK objects
    }
    
    Element* labeltype(LispE*);
    Element* labelcolor(LispE*);
    Element* labelfont(LispE*);
    Element* labelsize(LispE*);
    Element* backgroundcolor(LispE*);
    Element* labelwindow(LispE*);
    Element* selectionColor(LispE* lisp);

    void align(LispE*);

    
    void hide(LispE*);
    void focus(LispE*);
    void show(LispE*);

    virtual void run() {}
    virtual void finalize(LispE*) {}
    virtual void  circle(LispE* lisp) {}
    virtual void  point(LispE* lisp) {}
    virtual void  pie(LispE* lisp) {}
    virtual void  rectangleFill(LispE* lisp) {}
    virtual void  rectangle(LispE* lisp) {}
    virtual void  drawText(LispE* lisp) {}
    virtual void  arc(LispE* lisp) {}
    virtual void  line(LispE* lisp) {}
    virtual void  lineShape(LispE* lisp) {}
    virtual void  close() {}
    
    virtual Element* coordinates(LispE* lisp) {return null_;}
    virtual Element* linerotation(LispE* lisp) {return null_;}
    virtual Element* transform_x(LispE* lisp) {return null_;}
    virtual Element* transform_y(LispE* lisp) {return null_;}
    virtual Element* transform_dx(LispE* lisp) {return null_;}
    virtual Element* transform_dy(LispE* lisp) {return null_;}
    virtual Element* plot(LispE* lisp) {return null_;}
    virtual Element* ask(LispE* lisp) {return null_;}
    virtual Element* plotcoords(LispE* lisp) {return null_;}
    virtual Element* textsize(LispE* lisp) {return null_;}
    virtual Element* rgbcolor(LispE* lisp) {return null_;};
        
    virtual void drawcolor(LispE* lisp) {}
    virtual void textfont(LispE* lisp) {}
    virtual void polygon(LispE* lisp) {}
    virtual void loop(LispE* lisp) {}
    virtual void scale(LispE* lisp) {}
    virtual void translate(LispE* lisp) {}
    virtual void rotate(LispE* lisp) {}
    virtual void multmatrix(LispE* lisp) {}
    virtual void transform_vertex(LispE* lisp) {}
    virtual void pushclip(LispE*) {}
    virtual void popclip(LispE*) {}
    virtual void alert(LispE*) {}

    virtual Element* value(LispE*) {
        return emptystring_;
    }
    
    virtual void insert(LispE*) {}
    virtual Element* selection(LispE* lisp) {
        return emptystring_;
    }
    
    virtual void wrap() {}
    virtual void boundaries() {}
    virtual void step() {}

    void redraw() {
        if (widget != NULL)
            widget->redraw();
    }
    
    bool check();

    virtual void clean() {
        if (widget != NULL) {
            delete widget;
            widget = NULL;
        }
    }
    virtual void push(Lispwidget* w) {}
};

class Lispwindow : public Lispwidget {
public:
    Doublewindow* window;
    string label;
    double time_value;
    bool update;
    bool finalized;
    vector<Lispwidget*> items;

    Lispwindow(LispE* lsp, short t,  int x, int y, int w, int h, string& label, Element* f, Element* o);
    ~Lispwindow();
    
    void push(Lispwidget* w) {
        items.push_back(w);
        w->incrementstatus(1,true);
    }
    
    void run();
    void finalize(LispE*);
    void circle(LispE* lisp);
    void point(LispE* lisp);
    void pie(LispE* lisp);
    void rectangleFill(LispE* lisp);
    void rectangle(LispE* lisp);
    void drawText(LispE* lisp);
    void arc(LispE* lisp);
    void line(LispE* lisp);
    void lineShape(LispE* lisp);
    void close();
    
    Element* coordinates(LispE*);
    Element* linerotation(LispE*);
    Element* transform_x(LispE*);
    Element* transform_y(LispE*);
    Element* transform_dx(LispE*);
    Element* transform_dy(LispE*);
    Element* plot(LispE*);
    Element* ask(LispE*);
    Element* plotcoords(LispE*);
    Element* textsize(LispE*);
    Element* rgbcolor(LispE*);
        
    void drawcolor(LispE*);
    void textfont(LispE*);
    void polygon(LispE*);
    void loop(LispE*);
    void scale(LispE*);
    void translate(LispE*);
    void rotate(LispE*);
    void multmatrix(LispE*);
    void transform_vertex(LispE*);
    void pushclip(LispE*);
    void popclip(LispE*);
    void alert(LispE*);

    void clean() {
        close();
    }
};

//------------------------------------------------------------------------------------

class Lispinput : public Lispwidget {
public:
    string text;
    string buf;
    
    Lispinput(LispE* lsp, short t,  int x, int y, int w, int h, bool mltline, string& label, Element* f, Element* o);
    
    Element* value(LispE*);
    void insert(LispE*);
    Element* selection(LispE*);
};

//------------------------------------------------------------------------------------

class Lispoutput : public Lispwidget {
public:
    string text;
    string buf;
    
    Lispoutput(LispE* lsp, short t,  int x, int y, int w, int h, bool mltline, string& label);
    
    Element* value(LispE*);
    void wrap();

};

//------------------------------------------------------------------------------------

class Lispbutton : public Lispwidget {
public:

    string text;
    
    Lispbutton(LispE* lsp, short t,  int x, int y, int w, int h, int thetype, int shape, string& label, Element* f, Element* o);
    Element* value(LispE*);
    
};

//------------------------------------------------------------------------------------

class Lispslider : public Lispwidget {
public:

    string text;
    
    Lispslider(LispE* lsp, short t,  int x, int y, int w, int h, int align, bool value_slider, string& label, Element* f, Element* o);
    Element* value(LispE*);
    void boundaries();
    void step();
};

//------------------------------------------------------------------------------------

class Doublewindow : public Fl_Double_Window {
public:
    LispE* lisp;
    Lispwindow* window;
    long iwindow;

    Doublewindow(LispE* lsp, int x, int y, int w, int h, const char* l, Lispwindow* wn);

    ~Doublewindow();

    void draw();
};


typedef enum {fltk_create, fltk_input, fltk_output, fltk_button, fltk_slider,
    fltk_run, fltk_end, fltk_close, fltk_value, fltk_insert, fltk_selection,
    fltk_redraw, fltk_circle, fltk_drawtext, fltk_rectangle, fltk_wrap, fltk_step,
    fltk_rectanglefill, fltk_arc, fltk_pie, fltk_point, fltk_line, fltk_boundaries,
    fltk_textfont, fltk_rgbcolor, fltk_show, fltk_focus, fltk_align, fltk_coordinates,
    fltk_selectioncolor, fltk_labelwindow, fltk_labeltype, fltk_labelcolor, fltk_labelfont, fltk_labelsize, fltk_drawcolor, fltk_polygon,
    fltk_loop, fltk_linerotation, fltk_scale, fltk_translate, fltk_rotate, fltk_multmatrix, fltk_transform_x, fltk_transform_y,
    fltk_transform_dx, fltk_transform_dy, fltk_transform_vertex, fltk_pushclip, fltk_popclip, fltk_textsize, fltk_hide,
    fltk_backgroundcolor, fltk_plot, fltk_plotcoords, fltk_ask, fltk_alert, fltk_lineshape
} fltk_action;

class Lispe_gui : public Element {
public:
    fltk_action action;
    short fltk_widget;
    
    Lispe_gui(LispE*, short idgui, short idwn, fltk_action a);

    Element* eval(LispE* lisp);

    //We use this instruction to return a description of the instruction
    //Indeed, just do: (print getenv) to get this information
    wstring asString(LispE* lisp);
};

#endif

