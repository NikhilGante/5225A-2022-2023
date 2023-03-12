#pragma once
#include "main.h"
#include "alert.hpp"
#include "tracker.hpp"

using pros::text_format_e_t;
using pros::Color;

//Forward-Declaration
class _Task;
class GUI;
class Page;
class Button;
class Slider;
class Text_; //parent (interface)
template <typename V = std::nullptr_t> class Text; //child (used)

//For main.cpp to switch between
extern GUI main_obj, util_obj;

//From gui.cpp
extern Page terminal, testing;
extern Button testing_button_1, testing_button_2, misc_checks;
extern Text<> testing_text_1, testing_text_2;
extern Slider testing_slider;

typedef std::uint32_t Colour;

constexpr int
  PAGE_LEFT = 0,
  PAGE_UP = 0,
  PAGE_RIGHT = 480,
  PAGE_DOWN = 240,
  USER_LEFT = 0,
  USER_UP = 25,
  USER_RIGHT = 479,
  USER_DOWN = 239,
  MID_X = 240,
  MID_Y = 120,
  CHAR_HEIGHT_SMALL = 12,
  CHAR_WIDTH_SMALL = 7,
  CHAR_HEIGHT_MEDIUM = 16,
  CHAR_WIDTH_MEDIUM = 9,
  CHAR_HEIGHT_LARGE = 32,
  CHAR_WIDTH_LARGE = 19;



//All constructor args are in the format points, format, page, Text, Color

class GUI: public ObjectTracker<GUI>{
  template <typename V> friend class Text;
  friend class Page;
  friend class Button;
  friend class Slider;
  friend class Text_;
  friend void
    mainSetup(),
    mainBackground(),
    utilSetup(),
    utilBackground(),
    alert::update();

  public:
    enum class Style{ //how the rect coords get evaluated
      CENTRE,
      CORNER,
      SIZE,
      CENTER=CENTRE
    };

    struct Coordinate{
      int x, y;
      Coordinate(int, int);
      Coordinate(){}
    };

    struct Box{
      int x1, y1, x2, y2;
      Box(int, int, int, int, GUI::Style = GUI::Style::CORNER);
      Box(Coordinate, int, int, GUI::Style = GUI::Style::CORNER);
      Box(int, int, Coordinate, GUI::Style = GUI::Style::CORNER);
      Box(Coordinate, Coordinate, GUI::Style = GUI::Style::CORNER);
      Box(){}
    };

  private:
    static constexpr bool
      prompt_enabled = true,
      testing_page_active = false;
    static const Page* current_page;
    constexpr static const GUI* current_gui = &main_obj;
    static _Task task;
    static bool touched;
    static int x, y;
    std::vector<Page*> pages;
    std::function<void()> setup, background;

    static void
      update(),
      update_screen_status(),
      goNext(), goPrev(),
      screen_terminal_fix(),
      clearScreen(Color=Color::black),
      clearScreen(std::uint32_t),
      drawOblong(int, int, int, int, double, double);
    static int get_height(text_format_e_t), get_width(text_format_e_t);
    bool pressed() const;

  public:
    //Pages in the gui, init function, loop function
    GUI(std::vector<Page*>, std::function<void()>, std::function<void()>);

    static void
      alignedCoords (int, int, int, int, int = 480, int = 220),
      init(),
      goTo(int);
    static bool prompt(std::string, std::string = "", std::uint32_t=0); //Also prompts to controller
};

Color getGUIColour(term_colours);

class Page: public ObjectTracker<Page>{
  template <typename V> friend class Text;
  friend class GUI;
  friend class Button;
  friend class Slider;
  friend class Text_;
  friend void alert::update();

  private:
  public:
    std::function<void()> setup_func, loop_func;
    std::uint32_t b_col;
    std::string title;
    bool active=true;
    std::vector<GUI*> guis; //Parents
    std::vector<Button*> buttons; //Children
    std::vector<Slider*> sliders; //Children
    std::vector<Text_*> texts; //Children

    static Page* page_id(int);
    static int page_num(const Page*);
    bool pressed() const;
    void
      draw() const,
      update() const;

  public:
    //Title, Bcolour
    explicit Page(std::string, Color = Color::black);

    void
      setSetupFunc(std::function<void()>),
      setLoopFunc(std::function<void()>),
      setActive(bool = true),
      goTo() const;
};

//Text parent class
class Text_: public ObjectTracker<Text_>{
  template <typename V> friend class Text;
  friend class GUI;
  friend class Page;
  friend class Button;
  friend class Slider;

  private:
    GUI::Box box;

  protected:
    GUI::Coordinate coord;
    text_format_e_t txt_size;
    std::string label, text;
    std::uint32_t l_col, b_col = static_cast<std::uint32_t>(Color::black);
    GUI::Style type;
    Page* page;
    bool active = true;

    virtual void
      update() = 0,
      updateVal() = 0;
    void draw();

    Text_(): ObjectTracker{"Text"} {}

  public:
    void
      setBackground(int, int, Color), //Centre
      setBackground(int, int), //Centre
      setBackground(int, int, int, int, GUI::Style, Color),
      setBackground(int, int, int, int, GUI::Style),
      setBackground(Color),
      setActive(bool = true);
};

template <typename V>
class Text: public Text_{
  friend class GUI;
  friend class Page;
  friend class Button;
  friend class Slider;
  friend class Text_;
  friend class _Motor;
  friend class Piston;
  friend void alert::update();

  private:
    Text(){};

    std::remove_const_t<V> prev_value;
    std::function<V() > value;

    void update() override {if (active && value && prev_value != value()) draw();}
    void updateVal() override{
      if(value){
        prev_value = value();
        text = sprintf2(label, prev_value);
      }
      else text = label;
    }
    void construct (GUI::Coordinate coord, GUI::Style type, text_format_e_t txt_size, Page* page, std::string label, std::function<V()> value, Color l_col){
      //static_assert(!std::is_same_v<V, std::string>, "Text variable cannot be std::string, it causes unknown failures"); //Keep this for the painful memories
      updateName(label);
      box.x1 = USER_RIGHT;
      box.y1 = USER_DOWN;
      box.x2 = USER_LEFT;
      box.y2 = USER_UP;
      this->coord = coord;
      this->type = type;
      this->txt_size = txt_size;
      this->page = page;
      this->label = label;
      this->value = value;
      this->l_col = static_cast<std::uint32_t>(l_col);
      this->b_col = page->b_col;
      page->texts.push_back(this);
    } 

  public:
    V runFunc() {if(value) return value();}

    //Constructors (Points, Format, Page, Label, [var info], Lcolour)

      
      //Terminal - no format      
      Text(std::string text, V& value_obj, Color label_colour = Color::white){ //Var
        construct ({5, 0}, GUI::Style::CORNER, E_TEXT_LARGE_CENTER, &terminal, text, [&](){return value_obj;}, label_colour);
      }

      Text(std::string text, V* value_arr, auto& index, Color label_colour = Color::white){ //Array
        construct ({5, 0}, GUI::Style::CORNER, E_TEXT_LARGE_CENTER, &terminal, text, [value_arr, &index](){return value_arr[static_cast<int>(index) ];}, label_colour);
      }

      Text(std::string text, std::invocable auto&& func, Color label_colour = Color::white){ //Function
        construct ({5, 0}, GUI::Style::CORNER, E_TEXT_LARGE_CENTER, &terminal, text, func, label_colour);
      }

      //Terminal - format
      Text(std::string text, V& value_obj, text_format_e_t size, Color label_colour = Color::white){ //Var
        construct ({5, 0}, GUI::Style::CORNER, size, &terminal, text, [&](){return value_obj;}, label_colour);
      }

      Text(std::string text, V* value_arr, auto& index, text_format_e_t size, Color label_colour = Color::white){ //Array
        construct ({5, 0}, GUI::Style::CORNER, size, &terminal, text, [value_arr, &index](){return value_arr[static_cast<int>(index) ];}, label_colour);
      }

      Text(std::string text, std::invocable auto&& func, text_format_e_t size, Color label_colour = Color::white){ //Function
        construct ({5, 0}, GUI::Style::CORNER, size, &terminal, text, func, label_colour);
      }

      //GUI Objects
      Text(GUI::Coordinate coord, GUI::Style rect_type, text_format_e_t size, Page& page, std::string text, Color label_colour = Color::white){ //No var
        construct (coord, rect_type, size, &page, text, nullptr, label_colour);
      }

      Text(GUI::Coordinate coord, GUI::Style rect_type, text_format_e_t size, Page& page, std::string text, V& value_obj, Color label_colour = Color::white){ //Variable
        construct (coord, rect_type, size, &page, text, [&](){return value_obj;}, label_colour);
      }

      Text(GUI::Coordinate coord, GUI::Style rect_type, text_format_e_t size, Page& page, std::string text, V* value_arr, auto& index, Color label_colour = Color::white){ //Array
        construct (coord, rect_type, size, &page, text, [value_arr, &index](){return value_arr[static_cast<int>(index) ];}, label_colour);
      }

      Text(GUI::Coordinate coord, GUI::Style rect_type, text_format_e_t size, Page& page, std::string text, std::invocable auto&& func, Color label_colour = Color::white){ //Function
        construct (coord, rect_type, size, &page, text, func, label_colour);
      }
};

//Deduction Guides
Text(std::string, std::invocable auto&& func,                  Color = Color::white) -> Text<decltype(func())>;
Text(std::string, std::invocable auto&& func, text_format_e_t, Color = Color::white) -> Text<decltype(func())>;
Text(GUI::Coordinate, GUI::Style, text_format_e_t, Page&, std::string, std::invocable auto&& func, Color = Color::white) -> Text<decltype(func())>;


class Button: public ObjectTracker<Button>{
  template <typename V> friend class Text;
  friend class GUI;
  friend class Page;
  friend class Slider;
  friend class Text_;
  friend class _Motor;
  friend class Piston;
  friend class Logging;
  friend void alert::update();

  public: enum press_type{
    SINGLE,
    LATCH,
    REPEAT,
    TOGGLE
  };

  private:
    Button (): ObjectTracker{"Button"} {}

    std::uint32_t l_col, b_col, b_col_dark;
    std::string label, label1 = "";
    GUI::Box coord;
    GUI::Coordinate text_coord, text_coord1;
    bool last_pressed = 0;
    press_type form; //What type of button
    std::function<void()> func, off_func;
    Text_* title = nullptr;
    bool active=true;
    Page* page;

    //For latch buttons
    bool on = 0; //on is for toggle
    std::vector<Button*> options;

    static void create_options(std::vector<Button*>);
    bool
      pressed() const,
      newPress(),
      newRelease();
    void
      runFunc() const,
      runOffFunc() const,
      draw() const,
      drawPressed()  const,
      construct (GUI::Box, press_type, Page*, std::string, Color, Color),
      update();

  public:
    //Points, Format, Page, Label, Bcolour, Lcolour
    Button (GUI::Box, press_type, Page&, std::string = "", Color = Color::dark_orange, Color = Color::black);

    void
      select(),
      deselect(),
      addText (Text_&, bool = true),
      setFunc(std::function<void()>), setOffFunc(std::function<void()>),
      setActive(bool = true),
      setBackground (Color);
    bool isOn() const;
};

class Slider: public ObjectTracker<Slider>{
  template <typename V> friend class Text;
  friend class GUI;
  friend class Page;
  friend class Button;
  friend class Text_;

  public: enum direction{
    VERTICAL,
    HORIZONTAL
  };

  private:
    GUI::Box coord;
    GUI::Coordinate text_coord;
    double
      min, max,
      val, prev_val;
    std::uint32_t l_col, b_col;
    direction dir;
    bool active=true;
    Page* page;
    Button dec, inc;
    Text<double> title, min_title, max_title;

    void
      update(),
      setActive(bool = true),
      draw() const;
    bool pressed() const;

  public:
    //Points, Format, Min, Max, Page, Label, Bcolour, Lcolour
    Slider (GUI::Box, direction, double, double, Page&, std::string = "Value", double = 1, Color = Color::white, Color = Color::dark_orange);

    double getValue() const;
    void setValue(double);
};