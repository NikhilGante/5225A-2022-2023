#pragma once
#include "main.h"
#include "pros/colors.hpp"
#include "printing.hpp"

using pros::text_format_e_t;
using pros::Color;

//Forward-Declaration
class _Task_;
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
extern Button testing_button_1, testing_button_2;
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

namespace alert{
  // extern Timer timer;
  // extern std::uint32_t end_time;

  void update();
  void start(std::string fmt, term_colours colour = term_colours::ERROR, std::uint32_t time = 1000); //text + cols + time / text + cols / text
  
  template <typename... Params> void start(term_colours colour, std::uint32_t time, std::string fmt, Params... args); //text + cols + time
  template <typename... Params> void start(Color color, std::uint32_t time, std::string fmt, Params... args); //text + col + time
  template <typename... Params> void start(std::uint32_t time, std::string fmt, Params... args); //text + red + time
  template <typename... Params> void start(term_colours colour, std::string fmt, Params... args); //text + col + 1000
}

//All constructor args are in the format points, format, page, Text, Color

class GUI{
  template <typename V> friend class Text;
  friend class Page;
  friend class Button;
  friend class Slider;
  friend class Text_;
  friend void
    main_setup(),
    main_background(),
    util_setup(),
    util_background(),
    alert::update(),
    alert::start(std::string, term_colours, std::uint32_t);

  public:
    enum class Style{ //how the rect coords get evaluated
      CENTRE,
      CORNER,
      SIZE,
      CENTER=CENTRE
    };

  private:
    //Vars
      static constexpr bool
        prompt_enabled = true,
        testing_page_active = false;
      static const Page* current_page;
      static const GUI* current_gui;
      static _Task_ task;
      static bool touched;
      static int x, y;
      std::vector<Page*> pages;
      std::function<void()> setup, background;

    //Functions
      static void
        update(),
        update_screen_status(),
        go_next(), go_prev(),
        screen_terminal_fix(),
        clear_screen(Color=Color::black),
        clear_screen(std::uint32_t),
        draw_oblong(int, int, int, int, double, double);
      static Color get_colour(term_colours);
      static int get_height(text_format_e_t), get_width(text_format_e_t);
      static std::tuple<int, int, int, int> fix_points(int, int, int, int, Style);
      bool pressed() const;

  public:
    //Pages in the gui, init function, loop function
    GUI(const GUI&) = delete;
    GUI(std::vector<Page*>, std::function <void()>, std::function <void()>);

    //Functions
      static void
        aligned_coords (int, int, int, int, int = 480, int = 220),
        init(),
        go_to(int);
      static bool prompt(std::string, std::string = "", std::uint32_t=0); //Also prompts to controller
};

class Page{
  template <typename V> friend class Text;
  friend class GUI;
  friend class Button;
  friend class Slider;
  friend class Text_;
  friend void
    main_setup(),
    main_background(),
    util_setup(),
    util_background(),
    alert::update();

  private:

    //Vars
      std::function <void()> setup_func, loop_func;
      std::uint32_t b_col;
      std::string title;
      bool active=true;
      std::vector<GUI*> guis; //Parents
      std::vector<Button*> buttons; //Children
      std::vector<Slider*> sliders; //Children
      std::vector<Text_*> texts; //Children

    //Functions
      static Page* page_id(int);
      static int page_num(const Page*);
      bool pressed() const;
      void
        draw() const,
        update() const,
        go_to() const,
        set_setup_func(std::function <void()>), set_loop_func(std::function <void()>),
        set_active(bool = true);

  public:
    //Title, Bcolour
    Page(const Page&) = delete;
    explicit Page(std::string, Color = Color::black);
};

//Text parent class
class Text_{
  template <typename V> friend class Text;
  friend class GUI;
  friend class Page;
  friend class Button;
  friend class Slider;
  friend void
    main_setup(),
    main_background(),
    util_setup(),
    util_background();

  private:
    int x1 = USER_RIGHT, y1 = USER_DOWN, x2 = USER_LEFT, y2 = USER_UP;

  protected:
    //Vars
      int x, y;
      text_format_e_t txt_size;
      std::string label, text;
      std::uint32_t l_col, b_col = static_cast<std::uint32_t>(Color::black);
      GUI::Style type;
      Page* page;
      bool active = true;

    //Functions
      virtual void
        update() = 0,
        update_val() = 0;
      void
        draw(),
        set_background(int, int, Color), //Centre
        set_background(int, int), //Centre
        set_background(int, int, int, int, GUI::Style, Color),
        set_background(int, int, int, int, GUI::Style),
        set_background(Color),
        set_active(bool = true);
};

template <typename V>
class Text: public Text_{
  friend class GUI;
  friend class Page;
  friend class Button;
  friend class Slider;
  friend class Text_;
  friend void
    main_setup(),
    main_background(),
    util_setup(),
    util_background(),
    alert::update();

  private:
    Text(){};

    //Vars
      std::remove_const_t<V> prev_value;
      std::function<V() > value;

    //Functions
      void update() override;
      void update_val() override;
      void construct (int, int, GUI::Style, text_format_e_t, Page*, std::string, std::function<V()>, Color);

  public:
    V run_func();

    //Constructors (Points, Format, Page, Label, [var info], Lcolour)

      Text(const Text&) = delete;
      /*Terminal (var - no format)*/ Text (std::string, V&, Color = Color::white);
      /*Terminal (var - format)*/ Text (std::string, V&, text_format_e_t, Color = Color::white);
      /*Terminal (array - no format)*/ template <typename I> Text (std::string, V*, I&, Color = Color::white);
      /*Terminal (array - format)*/ template <typename I> Text (std::string, V*, I&, text_format_e_t, Color = Color::white);
      /*Terminal (function - no format)*/ Text (std::string, const std::function<V()>&, Color = Color::white);
      /*Terminal (function - format)*/ Text (std::string, const std::function<V()>&, text_format_e_t, Color = Color::white);
      /*No var*/ Text (int, int, GUI::Style, text_format_e_t, Page&, std::string, Color = Color::white);
      /*Variable*/ Text (int, int, GUI::Style, text_format_e_t, Page&, std::string, V&, Color = Color::white);
      /*Array*/ template <typename I> Text (int, int, GUI::Style, text_format_e_t, Page&, std::string, V*, I&, Color = Color::white);
      /*Function*/ Text (int, int, GUI::Style, text_format_e_t, Page&, std::string, const std::function<V() >&, Color = Color::white);
};

class Button{
  template <typename V> friend class Text;
  friend class GUI;
  friend class Page;
  friend class Slider;
  friend class Text_;
  friend void
    main_setup(),
    main_background(),
    util_setup(),
    util_background(),
    alert::update();

  public: enum press_type{
    SINGLE,
    LATCH,
    REPEAT,
    TOGGLE
  };

  private:
    Button (){};

    //Vars
      std::uint32_t l_col, b_col, b_col_dark;
      std::string label, label1 = "";
      int x1, y1, x2, y2, text_x, text_y, text_x1, text_y1;
      bool last_pressed = 0;
      press_type form; //What type of button
      std::function<void()> func, off_func;
      Text_* title = nullptr;
      bool active=true;
      Page* page;

      //For latch buttons
      bool on = 0; //on is for toggle
      std::vector<Button*> options;

    //Functions
      static void create_options(std::vector<Button*>);
      bool
        pressed() const,
        new_press(),
        new_release();
      void
        run_func() const,
        run_off_func() const,
        draw() const,
        draw_pressed()  const,
        construct (int, int, int, int, GUI::Style, press_type, Page*, std::string, Color, Color),
        update(),
        add_text (Text_&, bool = true),
        set_func(std::function <void()>), set_off_func(std::function <void()>),
        set_active(bool = true),
        set_background (Color);

  public:
    //Points, Format, Page, Label, Bcolour, Lcolour
    Button (const Button&) = delete;
    Button (int, int, int, int, GUI::Style, press_type, Page&, std::string = "", Color = Color::dark_orange, Color = Color::black);

    //Functions
      void select(), deselect();
};

class Slider{
  template <typename V> friend class Text;
  friend class GUI;
  friend class Page;
  friend class Button;
  friend class Text_;
  friend void
    main_setup(),
    main_background(),
    util_setup(),
    util_background();

  public: enum direction{
    VERTICAL,
    HORIZONTAL
  };

  private:
    //Vars
      int
        x1, y1, x2, y2, text_x, text_y,
        min, max,
        val, prev_val;
      std::uint32_t l_col, b_col;
      direction dir;
      bool active=true;
      Page* page;
      Button dec, inc;
      Text<int> title, min_title, max_title;

    //Functions
      void
        update(),
        set_active(bool = true),
        draw() const;
      bool pressed() const;

  public:
    //Points, Format, Min, Max, Page, Label, Bcolour, Lcolour
    Slider (int, int, int, int, GUI::Style, direction, int, int, Page&, std::string = "Value", int = 1, Color = Color::white, Color = Color::dark_orange);
    Slider (const Slider&) = delete;

    //Functions
      int get_value() const;
      void set_value(int);
};

//Text Methods
  template <typename V>
  void Text<V>::update() {
    if (active && value && prev_value != value()) draw();
  }

  template <typename V>
  void Text<V>::update_val() {
    if(value){
      prev_value = value();
      text = sprintf2(label, prev_value);
    }
    else text = label;
  }

  template <typename V>
  void Text<V>::construct (int x, int y, GUI::Style type, text_format_e_t txt_size, Page* page, std::string label, std::function<V() > value, Color l_col){
    //static_assert(!std::is_same_v<V, std::string>, "Text variable cannot be std::string, it causes unknown failures"); //Keep this for memories
    this->x = x;
    this->y = y;
    this->type = type;
    this->txt_size = txt_size;
    this->page = page;
    this->label = label;
    this->value = value;
    this->l_col = static_cast<std::uint32_t>(l_col);
    this->b_col = page->b_col;
    page->texts.push_back(this);
  } 

  template <typename V>
  V Text<V>::run_func() {
    if (value) return value();
  }

//Screen Flash Definitions
  namespace alert{
    template <typename... Params>
    void start(term_colours colour, std::uint32_t time, std::string fmt, Params... args){
      start(sprintf2(fmt, args...), colour, time);
    }
    template <typename... Params>
    void start(Color color, std::uint32_t time, std::string fmt, Params... args){
      start(sprintf2(fmt, args...), color, 1000);
    }
    template <typename... Params>
    void start(std::uint32_t time, std::string fmt, Params... args){
      start(sprintf2(fmt, args...), term_colours::ERROR, time);
    }
    template <typename... Params>
    void start(term_colours colour, std::string fmt, Params... args){
      start(sprintf2(fmt, args...), colour, 1000);
    }
  }

//Text Constructors

  //Terminal (var - no format)
  template <typename V>
  Text<V>::Text (std::string text, V& value_obj, Color label_colour){
    construct (5, 0, GUI::Style::CORNER, E_TEXT_LARGE_CENTER, &terminal, text, [&](){return value_obj;}, label_colour);
  }

  //Terminal (var - format)
  template <typename V>
  Text<V>::Text (std::string text, V& value_obj, text_format_e_t size, Color label_colour){
    construct (5, 0, GUI::Style::CORNER, size, &terminal, text, [&](){return value_obj;}, label_colour);
  }

  //Terminal (array - no format)
  template <typename V>
  template <typename I>
  Text<V>::Text (std::string text, V* value_arr, I& index, Color label_colour){
    construct (5, 0, GUI::Style::CORNER, E_TEXT_LARGE_CENTER, &terminal, text, [value_arr, &index](){return value_arr[static_cast<int>(index) ];}, label_colour);
  }

  //Terminal (array - format)
  template <typename V>
  template <typename I>
  Text<V>::Text (std::string text, V* value_arr, I& index, text_format_e_t size, Color label_colour){
    construct (5, 0, GUI::Style::CORNER, size, &terminal, text, [value_arr, &index](){return value_arr[static_cast<int>(index) ];}, label_colour);
  }

  //Terminal (function - no format)
  template <typename V>
  Text<V>::Text (std::string text, const std::function<V()>& func, Color label_colour){
    construct (5, 0, GUI::Style::CORNER, E_TEXT_LARGE_CENTER, &terminal, text, func, label_colour);
  }

  //Terminal (function - format)
  template <typename V>
  Text<V>::Text (std::string text, const std::function<V()>& func, text_format_e_t size, Color label_colour){
    construct (5, 0, GUI::Style::CORNER, size, &terminal, text, func, label_colour);
  }



  //No var
  template <typename V>
  Text<V>::Text (int x, int y, GUI::Style rect_type, text_format_e_t size, Page& page, std::string text, Color label_colour){
    construct (x, y, rect_type, size, &page, text, nullptr, label_colour);
  }

  //Variable
  template <typename V>
  Text<V>::Text (int x, int y, GUI::Style rect_type, text_format_e_t size, Page& page, std::string text, V& value_obj, Color label_colour){
    construct (x, y, rect_type, size, &page, text, [&](){return value_obj;}, label_colour);
  }

  //Array
  template <typename V>
  template <typename I>
  Text<V>::Text (int x, int y, GUI::Style rect_type, text_format_e_t size, Page& page, std::string text, V* value_arr, I& index, Color label_colour){
    construct (x, y, rect_type, size, &page, text, [value_arr, &index](){return value_arr[static_cast<int>(index) ];}, label_colour);
  }

  //Function
  template <typename V>
  Text<V>::Text (int x, int y, GUI::Style rect_type, text_format_e_t size, Page& page, std::string text, const std::function<V() >& func, Color label_colour){
    construct (x, y, rect_type, size, &page, text, func, label_colour);
  }
