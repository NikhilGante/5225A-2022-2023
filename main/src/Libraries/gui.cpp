#include "gui.hpp"
#include "../config.hpp"
#include "../util.hpp"
#include "task.hpp"
#include "logging.hpp"

//GUI:: Static Variable Declarations
  const Page* GUI::current_page = nullptr;
  bool GUI::touched = false;
  int GUI::x = 0, GUI::y = 0; //try making a point again
  _Task_ GUI::task("GUI");

//Text Vars
  std::string prompt_string;

//Default pages
  Page perm ("PERM BTNS", Color::pink); //Common page objects
    Button prev_page(PAGE_LEFT, PAGE_UP, 75, 20, GUI::Style::SIZE, Button::SINGLE, perm, "<-");
    Button next_page(PAGE_RIGHT, 0, -75, 20, GUI::Style::SIZE, Button::SINGLE, perm, "->");
    Button home(100, 10, 18, 6, GUI::Style::CENTRE, Button::SINGLE, perm, "Home");

  Page testing ("Testing"); //Blank page made so it already exists when quick tests are created
    Text<> testing_text_1 (125, 50, GUI::Style::CENTRE, TEXT_SMALL, testing, "BLANK TEXT 1");
    Text<> testing_text_2 (350, 50, GUI::Style::CENTRE, TEXT_SMALL, testing, "BLANK TEXT 2");
    Button testing_button_1 (25, 70, 200, 80, GUI::Style::SIZE, Button::SINGLE, testing, "BLANK BUTTON 1");
    Button testing_button_2 (250, 70, 200, 80, GUI::Style::SIZE, Button::SINGLE, testing, "BLANK BUTTON 2");
    Slider testing_slider (MID_X, 200, 200, 20, GUI::Style::CENTRE, Slider::HORIZONTAL, -100, 100, testing, "BLANK SLIDER");

  Page prompt_sequence ("Prompt");
    Button prompt_button (300, MID_Y, 160, 90, GUI::Style::CENTRE, Button::SINGLE, prompt_sequence);
    Button prompt_back_button (20, USER_UP, 100, 50, GUI::Style::SIZE, Button::SINGLE, prompt_sequence, "BACK");
    Text prompt_button_text (0, 0, GUI::Style::CENTRE, TEXT_SMALL, prompt_sequence, "%s", prompt_string);

  Page screen_flash ("Alert"); //Called screen_flash because there are a lot of things with the word alert
    Button screen_flash_back_button (20, USER_UP, 100, 50, GUI::Style::SIZE, Button::SINGLE, screen_flash, "BACK");
    Text screen_flash_text (MID_X, MID_Y, GUI::Style::CENTRE, TEXT_LARGE, screen_flash, "");
    Text screen_flash_time (70, 85, GUI::Style::CENTRE, TEXT_SMALL, screen_flash, "Time Left: %d", std::function([](){return alert::end_time-alert::timer.get_time();}));

  Page terminal ("Screen Printing");

namespace alert{
  Timer timer("Flash Timer", false);
  std::uint32_t end_time;
  const Page* page;

  void start(std::string text, Color color, std::uint32_t time){
    std::uint32_t colour = static_cast<std::uint32_t>(color);

    screen_flash.b_col = colour;
    screen_flash_time.b_col = colour;
    screen_flash_text.b_col = colour;
    screen_flash_time.l_col = ~colour&0xFFFFFF;
    screen_flash_text.l_col = ~colour&0xFFFFFF;
    screen_flash_text.label = text;

    page = GUI::current_page;
    end_time = time;
    screen_flash.go_to();

    master.rumble(".-");

    log_d.print("\n\n%s\n", text);

    timer.reset(); //Starts counting
    if(time) printf2(term_colours::CYAN, "Showing for %dms.\n\n", time);
  }

  void start(std::string text, term_colours term_colour, std::uint32_t time){
    std::uint32_t colour = static_cast<std::uint32_t>(GUI::get_colour(term_colour));

    screen_flash.b_col = colour;
    screen_flash_time.b_col = colour;
    screen_flash_text.b_col = colour;
    screen_flash_time.l_col = ~colour&0xFFFFFF;
    screen_flash_text.l_col = ~colour&0xFFFFFF;
    screen_flash_text.label = text;

    page = GUI::current_page;
    end_time = time;
    screen_flash.go_to();

    master.rumble(".-");

    log_d.print(term_colour, "\n\n%s\n", text);

    timer.reset(); //Starts counting
    if(time) printf2(term_colours::CYAN, "Showing for %dms.\n\n", time);
  }

  //rest are templates, so defined in header

  void attempt_end(){
    //timer.playing() means flash is actually running
    //timer.get_time() >= end_time means time has run out
    //screen_flash_back_button.pressed() means user has exited the alert

    if (timer.playing() && (timer.get_time() >= end_time || screen_flash_back_button.pressed())){
      timer.reset(false);
      page->go_to();
    }
  }
}

//GUI Helpers
  //To get coordinates for aligned objects, (buttons, sliders...) of same size
  //Put in how many of buttons / sliders you want, and get properly spaced coords
  void GUI::aligned_coords (int x_objects, int y_objects, int x_size, int y_size, int x_range, int y_range){
    double x_space = (x_range-x_objects * x_size) / (x_objects+1.0);
    double y_space = (y_range-y_objects * y_size) / (y_objects+1.0);

    //Recalculates until it gets a nice multiple of 5
    while (fmod(x_space, 5)) x_space = ((--x_range) - x_objects * x_size) / (x_objects+1.0);
    while (fmod(y_space, 5)) y_space = ((--y_range) - y_objects * y_size) / (y_objects+1.0);

    newline(2);
    for (int y = 0; y < y_objects; y++){
      for (int x = 0; x < x_objects; x++){
        printf2("(%d, %d, %d, %d, GUI::Style::SIZE)", (x_space) * (x+1) + x_size*x, (y_space) * (y+1) + y_size*y + 20, x_size, y_size);
      }
      newline();
    }
    printf2("\nScreen Size: %d x %d", x_range, y_range);
    if ((x_space + x_size) * (x_objects) > 480) printf2("X out of bounds");
    if ((y_space + y_size) * (y_objects) > 220) printf2("Y out of bounds");
    newline(2);
  }

  bool GUI::prompt(std::string screen, std::string term, std::uint32_t delay_time){
    if(!prompt_enabled) return true;
    prompt_string = screen;
    if(term == "") term = screen;
    printf2(term_colours::BLUE, "\n\n%s\nPress the screen big button or the controller OK button when ready.", term);
    master.clear();
    master.print(0, 0, "Press OK btn");
    bool interrupted = false;
    const Page* page = GUI::current_page;
    prompt_sequence.go_to();

    //! Had to comment this out because our controller subclass has changed

    // //Wait for Release
    // WAIT_UNTIL(!(prompt_button.pressed() || master.get_digital(ok_button) || master.interrupt(false, true, false)) || interrupted){ //checks that no button is being pressed
    //   GUI::update_screen_status();
    //   if (prompt_back_button.pressed()) interrupted = true;
    // }

    // //Wait for Press
    // WAIT_UNTIL((prompt_button.pressed() || master.get_digital(ok_button)) || interrupted){ //waits for a press from prompt btn or ok btn. Interrupts with any controller digital btn
    //   GUI::update_screen_status();
    //   if (prompt_back_button.pressed() || master.interrupt(false, true, true)) interrupted = true;
    // }
    
    // //Wait for Release
    // WAIT_UNTIL(!(prompt_button.pressed() || master.get_digital(ok_button) || master.interrupt(false, true, false)) || interrupted){ //checks that no button is being pressed
    //   GUI::update_screen_status();
    //   if (prompt_back_button.pressed()) interrupted = true;
    // }


    // Alternative until Controller class is restored
      //Wait for Release
      WAIT_UNTIL(!prompt_button.pressed() || interrupted){ //checks that no button is being pressed
        GUI::update_screen_status();
        if (prompt_back_button.pressed()) interrupted = true;
      }

      //Wait for Press
      WAIT_UNTIL(prompt_button.pressed() || interrupted){ //waits for a press from prompt btn or ok btn. Interrupts with any controller digital btn
        GUI::update_screen_status();
        if (prompt_back_button.pressed()) interrupted = true;
      }
      
      //Wait for Release
      WAIT_UNTIL(!(prompt_button.pressed()) || interrupted){ //checks that no button is being pressed
        GUI::update_screen_status();
        if (prompt_back_button.pressed()) interrupted = true;
      }

    if (!interrupted){
      if(delay_time){
        printf2("\nWaiting for %dms before running.\n", delay_time);
        delay(delay_time);
      }
      printf2(term_colours::GREEN, "Running\n");
    }
    else printf2(term_colours::ERROR, "Interrupted\n");

    page->go_to();
    master.clear();
    return !interrupted;
  }

  void GUI::draw_oblong(int x1, int y1, int x2, int y2, double kS, double kR){ //ks and kr and scale values for shrink and radius
    int s = std::min(x2-x1, y2-y1) * kS; //Scale for shrinking button (pressed look)
    int r = std::min(x2-x1, y2-y1) * kR; //Scale for how rounded the button edges should be
    screen::fill_rect(x1+ s, y1+ s, x2-s, y2-s);
    screen::erase_rect(x1+ s, y1+ s, x1+ s + r, y1+ s + r);
    screen::erase_rect(x2-s, y1+ s, x2-s-r, y1+ s + r);
    screen::erase_rect(x1+ s, y2-s, x1+ s + r, y2-s-r);
    screen::erase_rect(x2-s, y2-s, x2-s-r, y2-s-r);
    screen::fill_circle(x1+ s + r, y1+ s + r, r);
    screen::fill_circle(x2-s-r, y1+ s + r, r);
    screen::fill_circle(x1+ s + r, y2-s-r, r);
    screen::fill_circle(x2-s-r, y2-s-r, r);
  }

  //Formats coordinates based on a GUI::Style (always in x1, y1, x2, y2)
  std::tuple<int, int, int, int> GUI::fix_points (int x1, int y1, int x2, int y2, Style type){
    switch(type){
      case GUI::Style::CENTRE:
        x1 -= x2;
        y1 -= y2;
        x2 += x1+ x2;
        y2 += y1+ y2;
        break;

      case GUI::Style::SIZE:
        x2 += x1;
        y2 += y1;
        break;
      case GUI::Style::CORNER:
        break;
    }
    
    //Arranging coordinates in a left-right up-down order.
    int temp;
    temp = std::max(x1, x2);
    x1 = std::min(x1, x2);
    x2 = temp;

    temp = std::max(y1, y2);
    y1 = std::min(y1, y2);
    y2 = temp;
    return {x1, y1, x2, y2}; //Gives back the fixed points as a tuple
  }

  int GUI::get_height(text_format_e_t size){
    switch(size){
      case TEXT_SMALL: return CHAR_HEIGHT_SMALL; break;
      case TEXT_MEDIUM: return CHAR_HEIGHT_MEDIUM; break;
      case TEXT_LARGE: return CHAR_HEIGHT_LARGE; break;
      default: return 0; break;
    }
  }

  int GUI::get_width(text_format_e_t size){
    switch(size){
      case TEXT_SMALL: return CHAR_WIDTH_SMALL; break;
      case TEXT_MEDIUM: return CHAR_WIDTH_MEDIUM; break;
      case TEXT_LARGE: return CHAR_WIDTH_LARGE; break;
      default: return 0; break;
    }
  }

  Color GUI::get_colour(term_colours colour){
    switch(colour){
      case term_colours::BLACK: return Color::black; break;
      case term_colours::ERROR:
      case term_colours::RED: return Color::red; break;
      case term_colours::NOTIF:
      case term_colours::GREEN: return Color::green; break;
      case term_colours::WARNING:
      case term_colours::YELLOW: return Color::yellow; break;
      case term_colours::BLUE: return Color::blue; break;
      case term_colours::MAGENTA: return Color::magenta; break;
      case term_colours::CYAN: return Color::cyan; break;
      case term_colours::WHITE: return Color::white; break;
      case term_colours::NONE: return Color::black; break;
    }
    return Color::black;
  }

  void GUI::screen_terminal_fix(){
    //Will only run if things are actually being printed
    if(terminal.texts.empty()){
      terminal.set_active(false);
      return;
    }

    //Sees how much space is user-requested
    int y = USER_UP;
    for (std::vector<Text_*>::const_iterator it = terminal.texts.begin(); it != terminal.texts.end(); it++){
      if ((*it)->txt_size != 4) y += get_height((*it)->txt_size) + 5;
      if(get_width(TEXT_SMALL) * (*it)->label.length() + 5 > 480){
          throw std::length_error("Item too long to print\n");
          return;
        }
    }

    int size = (PAGE_DOWN - y) / (std::count_if(terminal.texts.begin(), terminal.texts.end(), [](Text_* text){return text->txt_size == 4;})) - 5;
    
    text_format_e_t fmt;
    if(get_height(TEXT_LARGE) <= size) fmt = TEXT_LARGE;
    else if(get_height(TEXT_MEDIUM) <= size) fmt = TEXT_MEDIUM;
    else if(get_height(TEXT_SMALL) <= size) fmt = TEXT_SMALL;
    else{
      throw std::length_error("Too Many items being printed to screen\n");
      return;
    }

    for (std::vector<Text_*>::const_iterator it = terminal.texts.begin(); it != terminal.texts.end(); it++){
      if (get_width(fmt) * (*it)->label.length() + 5 > 480){
        if(fmt == TEXT_LARGE){
          if(get_width(TEXT_MEDIUM) * (*it)->label.length() + 5 < 480) (*it)->txt_size = TEXT_MEDIUM;
          else if(get_width(TEXT_SMALL) * (*it)->label.length() + 5 < 480) (*it)->txt_size = TEXT_SMALL;
        }

        else if(fmt == TEXT_MEDIUM) (*it)->txt_size = TEXT_SMALL;
      }
    }

    y = USER_UP;
    //Saves y-pos and txt_size
    for (std::vector<Text_*>::const_iterator it = terminal.texts.begin(); it != terminal.texts.end(); it++){
      (*it)->y = y;
      if ((*it)->txt_size == 4) (*it)->txt_size = fmt;
      y += get_height((*it)->txt_size) + 5;
    }
  }


//Constructors
  void Button::construct(int x1, int y1, int x2, int y2, GUI::Style type, press_type form, Page* page, std::string text, Color b_col, Color l_col){

    //Saves params to class private vars
    this->b_col = static_cast<std::uint32_t>(b_col);
    this->b_col_dark = RGB2COLOR(static_cast<int>(COLOR2R(this->b_col) * 0.8), static_cast<int>(COLOR2G(this->b_col) * 0.8), static_cast<int>(COLOR2B(this->b_col) * 0.8));
    this->l_col = static_cast<std::uint32_t>(l_col);
    this->form = form;

    //Saves the buttons owning page
    this->page = page;
    (this->page->buttons).push_back(this);
    std::tie(this->x1, this->y1, this->x2, this->y2) = GUI::fix_points(x1, y1, x2, y2, type);


    std::size_t next_space = text.find(' ', text.length() / 2);
    std::size_t prev_space = text.find_last_of(' ', text.length() / 2);
    std::size_t space = (next_space-text.length() / 2 < text.length() / 2-prev_space) ? next_space : prev_space;

    if (space != std::string::npos && 8*text.length() + 5 > this->x2-this->x1){ //Spaces it if it's too long for one line
      this->label = text.substr(0, space);
      this->label1 = text.substr(space+1);

      this->text_x = (this->x1+ this->x2-(this->label.length() * CHAR_WIDTH_SMALL)) / 2;
      this->text_x1 = (this->x1+ this->x2-(this->label1.length() * CHAR_WIDTH_SMALL)) / 2;
      this->text_y = (this->y1+ this->y2-CHAR_HEIGHT_SMALL) / 2 - CHAR_HEIGHT_SMALL;
      this->text_y1 = (this->y1+ this->y2-CHAR_HEIGHT_SMALL) / 2 + CHAR_HEIGHT_SMALL;
    }
    else{
      this->text_x = (this->x1+ this->x2-(text.length() * CHAR_WIDTH_SMALL)) / 2;
      this->text_y = (this->y1+ this->y2-CHAR_HEIGHT_SMALL) / 2;
      this->label = text;
    }
  }

  GUI::GUI(std::vector<Page*> pages, std::function <void()> setup, std::function <void()> background){
    //Saves pages to gui
    this->pages.push_back(&perm);
    for (std::vector<Page*>::const_iterator it = pages.begin(); it != pages.end(); it++) this->pages.push_back(*it);
    this->pages.push_back(&testing);
    this->pages.push_back(&terminal);
    this->pages.push_back(&prompt_sequence);
    this->pages.push_back(&screen_flash);

    //Saves gui to pages
    for (std::vector<Page*>::const_iterator it = this->pages.begin(); it != this->pages.end(); it++) (*it)->guis.push_back(this);

    this->setup = setup;
    this->background = background;
  }

  Button::Button(int x1, int y1, int x2, int y2, GUI::Style type, press_type form, Page& page, std::string text, Color background_colour, Color label_colour){
    construct(x1, y1, x2, y2, type, form, &page, text, background_colour, label_colour);
  }

  Slider::Slider (int x1, int y1, int x2, int y2, GUI::Style type, direction dir, int min, int max, Page& page, std::string label, int increment, Color background_colour, Color label_colour){
    //Saves params to class private vars
    this->dir = dir;
    this->min = min;
    this->max = max;
    this->page = &page;
    this->val = inRange(0, min, max) ? 0 : (inRange(1, min, max) ? 1 : (min + max) / 2); //0 if that's the min, otherwise the average
    this->b_col = static_cast<std::uint32_t>(background_colour);
    this->l_col = static_cast<std::uint32_t>(label_colour);
    this->page->sliders.push_back(this);

    std::tie(this->x1, this->y1, this->x2, this->y2) = GUI::fix_points(x1, y1, x2, y2, type);

    switch(this->dir){
      case HORIZONTAL:
        text_x = (this->x1+ this->x2) / 2;
        text_y = this->y1-CHAR_HEIGHT_SMALL / 2-2;
        inc.construct(this->x2 + 5, this->y1, this->x2 + 25, this->y2, GUI::Style::CORNER, Button::SINGLE, this->page, ">", label_colour, background_colour);
        dec.construct(this->x1-25, this->y1, this->x1-5, this->y2, GUI::Style::CORNER, Button::SINGLE, this->page, "<", label_colour, background_colour);
        title.construct(text_x, text_y, GUI::Style::CENTRE, TEXT_SMALL, this->page, label + ":%d", [&](){return val;}, label_colour); //why not pass val by reference
        min_title.construct(this->x1, text_y, GUI::Style::CENTRE, TEXT_SMALL, this->page, "%d", [&](){return this->min;}, label_colour);
        max_title.construct(this->x2, text_y, GUI::Style::CENTRE, TEXT_SMALL, this->page, "%d", [&](){return this->max;}, label_colour);
        break;

      case VERTICAL:
        text_x = (this->x1+ this->x2) / 2;
        text_y = (this->y1+ this->y2) / 2;
        inc.construct(this->x1, this->y1-17, this->x2-this->x1, -20, GUI::Style::SIZE, Button::SINGLE, this->page, "^", label_colour, background_colour);
        dec.construct(this->x1, this->y2+17, this->x2-this->x1, 20, GUI::Style::SIZE, Button::SINGLE, this->page, "v", label_colour, background_colour);
        title.construct(text_x, this->y1-17-22-CHAR_HEIGHT_SMALL, GUI::Style::CENTRE, TEXT_SMALL, this->page, label + ":%d", [&](){return val;}, label_colour);
        min_title.construct(text_x, this->y2 + (CHAR_HEIGHT_SMALL + 3) / 2, GUI::Style::CENTRE, TEXT_SMALL, this->page, "%d", [&](){return this->min;}, label_colour);
        max_title.construct(text_x, this->y1-(CHAR_HEIGHT_SMALL + 3) / 2, GUI::Style::CENTRE, TEXT_SMALL, this->page, "%d", [&](){return this->max;}, label_colour);
        break;
    }

    //Buttons
    if(min > max) increment = -increment;
    dec.set_func([&, increment](){this->val-=increment; if(!inRange(this->val, this->min, this->max)) this->val = this->min;});
    inc.set_func([&, increment](){this->val +=increment; if(!inRange(this->val, this->min, this->max)) this->val = this->max;});
  }

  Page::Page(std::string title, Color background_colour){
    this->b_col = static_cast<std::uint32_t>(background_colour);
    this->title = title;
    if (!(title == "PERM BTNS" || title == "Prompt" || title == "Alert")){
      for (std::vector<Button*>::const_iterator it = perm.buttons.begin(); it != perm.buttons.end(); it++) buttons.push_back(*it);
    }
  }


//Methods
  Page* Page::page_id(int page_num){
    return GUI::current_gui->pages.at(page_num);
  }

  int Page::page_num(const Page* page_id){
    const std::vector<Page*>::const_iterator it = std::find(GUI::current_gui->pages.begin(), GUI::current_gui->pages.end(), page_id);
    if (it == GUI::current_gui->pages.end()){
      throw std::domain_error(sprintf2("Page %p does not exist!\n", page_id));
      return -1;
    }
    return it-GUI::current_gui->pages.begin();
  }

  void GUI::go_next(){
    std::vector<Page*>::const_iterator it = std::find(GUI::current_gui->pages.begin(), GUI::current_gui->pages.end(), current_page);
    do{
      it++;
      if (it == current_gui->pages.end()) it = current_gui->pages.begin();
    } while(!(*it)->active);

    (*it)->go_to();
  }

  void GUI::go_prev(){
    std::vector<Page*>::const_iterator it = std::find(GUI::current_gui->pages.begin(), GUI::current_gui->pages.end(), current_page);
    do{
      if (it == current_gui->pages.begin()) it = current_gui->pages.end();
      it--;
    } while(!(*it)->active);

    (*it)->go_to();
  }

  void GUI::go_to(int page_num){
    Page::page_id(page_num)->go_to();
  }

  void Page::go_to() const{
    if(page_num(this) == -1) return;
    GUI::current_page = this; //Saves new page then draws all the buttons on the page
    draw();
    if(setup_func) setup_func();
    WAIT_UNTIL(!GUI::touched) GUI::update_screen_status();
  }

  void GUI::clear_screen(Color color){
    clear_screen(static_cast<std::uint32_t>(color));
  }

  void GUI::clear_screen(std::uint32_t colour){
    screen::set_pen(colour);
    screen::fill_rect(PAGE_LEFT, PAGE_UP, PAGE_RIGHT, PAGE_DOWN);
  }

  void Button::create_options(std::vector<Button*> buttons){
    std::vector<Button*>::const_iterator it, it2; //For clarity

    for (it = buttons.begin(); it != buttons.end(); it++){
      Button* btn_id = *it;
      if (btn_id->form != LATCH && btn_id->form != TOGGLE){
        throw std::invalid_argument(sprintf2("Option Feature is only available for latch and toggle buttons! Failed on \"%s\" button.\n", btn_id->label));
        return;
      }
    }

    for (it = buttons.begin(); it != buttons.end(); it++){ //For each button in the list
      for (it2 = buttons.begin(); it2 != buttons.end(); it2++){ //Go through the list and save each button
        if (*it != *it2) (*it)->options.push_back(*it2);
      }
    }
  }

  int Slider::get_value() const{
    return val;
  }

  void Slider::set_value(int val){
    int old_val = this->val;
    this->val = std::clamp(val, min, max);
    if(this->val != old_val) draw();
  }

  void Button::add_text(Text_& text_ref, bool overwrite){
    if(page != text_ref.page){
      throw std::invalid_argument(sprintf2("Text can only be linked to a button on the same page! Failed on \"%s\" button and \"%s\" text.", label, text_ref.label));
      return;
    }
    title = &text_ref;
    text_ref.l_col = l_col;
    text_ref.b_col = b_col;
    text_ref.active = active;
    text_ref.type = GUI::Style::CENTRE;
    text_ref.y = (y1+ y2) / 2;
    text_ref.x = (x1+ x2) / 2;
    text_ref.x1 = USER_RIGHT;
    text_ref.y1 = USER_DOWN;
    text_ref.x2 = USER_LEFT;
    text_ref.y2 = USER_UP;

    if (overwrite){
      label = "";
      label1 = "";
    }
    else{
      text_ref.y += GUI::get_height(text_ref.txt_size);
      text_y -= GUI::get_height(text_ref.txt_size);
    }
  }

  void Button::select(){
    switch(form){
      case Button::LATCH:
      case Button::TOGGLE:
        on = true;

        for (std::vector<Button*>::const_iterator option_it = options.begin(); option_it != options.end(); option_it++) (*option_it)->deselect();

      case Button::SINGLE:
      case Button::REPEAT:
        draw_pressed();
        run_func();
        break;
    }
  }

  void Button::deselect(){
    switch(form){
      case Button::LATCH:
      case Button::TOGGLE:
        on = false;
      case Button::SINGLE:
      case Button::REPEAT:
        draw();
        run_off_func();
        break;
    }
  }

  void Button::set_background (Color color){
    if (title) title->set_background(color);
    if (b_col != static_cast<std::uint32_t>(color)){
      b_col = static_cast<std::uint32_t>(color);
      b_col_dark = RGB2COLOR(static_cast<int>(COLOR2R(b_col) * 0.8), static_cast<int>(COLOR2G(b_col) * 0.8), static_cast<int>(COLOR2B(b_col) * 0.8));
      draw();
    }
  }

  void Text_::set_background (int x1, int y1, Color color){ //Centre
    std::tie(this->x1, this->y1, this->x2, this->y2) = GUI::fix_points(this->x, this->y, x1, y1, GUI::Style::CENTRE);
    set_background(color);
  }

  void Text_::set_background (int x1, int y1, int x2, int y2, GUI::Style type, Color color){
    std::tie(this->x1, this->y1, this->x2, this->y2) = GUI::fix_points(x1, y1, x2, y2, type);
    set_background(color);
  }

  void Text_::set_background (int x1, int y1){ //Centre
    std::tie(this->x1, this->y1, this->x2, this->y2) = GUI::fix_points(this->x, this->y, x1, y1, GUI::Style::CENTRE);
  }

  void Text_::set_background (int x1, int y1, int x2, int y2, GUI::Style type){
    std::tie(this->x1, this->y1, this->x2, this->y2) = GUI::fix_points(x1, y1, x2, y2, type);
  }

  void Text_::set_background (Color color){
    if (b_col != static_cast<std::uint32_t>(color)){
      b_col = static_cast<std::uint32_t>(color);
      draw();
    }
  }

  void Page::set_active(bool active){
    this->active = active;
    if (!active && this == GUI::current_page) GUI::go_next();
  }

  void Button::set_active(bool active){
    this->active = active;
    if (title) title->set_active(active);
    if (page == GUI::current_page){
      if (active) draw();
      else{
        screen::set_pen(page->b_col);
        screen::fill_rect(x1, y1, x2, y2);
      }
    }
  }

  void Text_::set_active(bool active){
    this->active = active;
    if (page == GUI::current_page){
      if (active) draw();
      else{
        screen::set_pen(page->b_col);
        screen::fill_rect(x1, y1, x2, y2);
      }
    }
  }

  void Slider::set_active(bool active){
    this->active = active;
    if (page == GUI::current_page){
      if (active) draw();
      else{
        screen::set_pen(page->b_col);
        screen::fill_rect(x1, y1, x2, y2);
      }
    }
  }


//Drawing
  void Page::draw() const{
    GUI::clear_screen(b_col);
    screen::set_pen(b_col);
    screen::set_eraser(b_col);
    screen::fill_rect(PAGE_LEFT, PAGE_UP, PAGE_RIGHT, 20);
    screen::set_pen(static_cast<std::uint32_t>(Color::white));
    screen::print(TEXT_SMALL, MID_X-(title.length() + 3 + std::to_string(page_num(this)).length()) * CHAR_WIDTH_SMALL / 2, 5, "%s - %d", title, page_num(this));
    for (std::vector<Button*>::const_iterator it = buttons.begin(); it != buttons.end(); it++) (*it)->draw();
    for (std::vector<Slider*>::const_iterator it = sliders.begin(); it != sliders.end(); it++) (*it)->draw();
    for (std::vector<Text_*>::const_iterator it = texts.begin(); it != texts.end(); it++) (*it)->draw();
  }

  void Button::draw() const{
    if (!(active && (page == GUI::current_page || page == &perm))) return;
    if (on){ //on buttons must be drawn in a pressed state
      draw_pressed();
      return;
    }
    screen::set_pen(b_col);
    screen::set_eraser(GUI::current_page->b_col); //It's current_page not just page because perm has a different colour
    screen::fill_rect(x1, y1, x2, y2);
    GUI::draw_oblong(x1, y1, x2, y2, 0, 0.15);
    screen::set_pen(l_col);
    screen::set_eraser(b_col);
    screen::print(TEXT_SMALL, text_x, text_y, "%s", label);
    screen::print(TEXT_SMALL, text_x1, text_y1, "%s", label1);
    if(title){
      title->b_col = b_col;
      title->draw();
    }
  }

  void Button::draw_pressed() const{
    if (!(active && (page == GUI::current_page || page == &perm))) return;
    screen::set_eraser(page->b_col); //Erases button
    screen::erase_rect(x1, y1, x2, y2);

    screen::set_pen(b_col_dark);
    screen::set_eraser(GUI::current_page->b_col);
    GUI::draw_oblong(x1, y1, x2, y2, 0.04, 0.2);
    screen::set_pen(l_col);
    screen::set_eraser(b_col_dark);
    screen::print(TEXT_SMALL, text_x, text_y, "%s", label);
    screen::print(TEXT_SMALL, text_x1, text_y1, "%s", label1);
    if(title){
      title->b_col = b_col_dark;
      title->draw();
    }
  }

  void Slider::draw() const{
    if (!(active && (page == GUI::current_page || page == &perm))) return;
    screen::set_pen(b_col);
    screen::fill_rect(x1, y1, x2, y2);
    screen::set_pen(l_col);
    if(dir == HORIZONTAL) screen::fill_rect(x1+1, y1+1, mapValues(std::clamp(val, min, max), min, max, x1, x2), y2-1);
    else screen::fill_rect(x1+1, mapValues(std::clamp(val, min, max), min, max, y2, y1), x2-1, y2-1);
  }

  void Text_::draw(){
    if (!(active && (page == GUI::current_page || page == &perm))) return;
    update_val();

    if (x2 != 0 && y2 != 0){ //If background box exists. Should be able to get rid of this as now it always exists
      screen::set_eraser(page->b_col);
      screen::erase_rect(x1, y1, x2, y2);

      screen::set_pen(b_col);
      GUI::draw_oblong(x1, y1, x2, y2, 0, 0.15);
    }
    screen::set_pen(l_col);
    screen::set_eraser(b_col);

    int x_coord = x, y_coord = y;
    if (type == GUI::Style::CENTRE){
      x_coord -= GUI::get_width(txt_size) / 2.0*text.length();
      y_coord -= GUI::get_height(txt_size) / 2.0;
    }
    
    //Resizes the background so it won't have overwriting issues
    x1 = std::min(x1, x_coord);
    x2 = std::max(x2, x_coord + (GUI::get_width(txt_size)+1) *static_cast<int>(text.length()));
    y1 = std::min(y1, y_coord);
    y2 = std::max(y2, y_coord + GUI::get_height(txt_size));

    screen::print(txt_size, x_coord, y_coord, "%s", text);
  }


//Function Handling
  void Page::set_setup_func(std::function <void()> function){setup_func = function;}
  void Page::set_loop_func(std::function <void()> function){loop_func = function;}
  void Button::set_func(std::function <void()> function){func = function;}
  void Button::set_off_func(std::function <void()> function){off_func = function;}
  void Button::run_func() const {if (func) func();}
  void Button::run_off_func() const {if (off_func) off_func();}


//Data Updates
  void GUI::init(){
    screen_terminal_fix();
    testing.set_active(testing_page_active);
    perm.set_active(false);
    prompt_sequence.set_active(false);
    screen_flash.set_active(false);

    prev_page.set_func(&go_prev);
    next_page.set_func(&go_next);
    home.set_func([](){go_to(1);});

    prompt_button.add_text(prompt_button_text);

    current_gui->setup();

    //! Do not touch this section. I do not fully understand, but any change here will break things
    go_to(0);
    if(terminal.active) terminal.go_to();
    else go_to(1); //! Sets it to page 1 for program start. Don't delete this. If you want to change the starting page, call GUI::go_to(Page Number) in initialize()

    GUI::task.start(update);
  }

  void GUI::update(){
    while(true){
      current_gui->background();
      update_screen_status();
      const Page& cur_p = * current_page;
      /*Page*/ cur_p.update();
      /*Button*/ for (std::vector<Button*>::const_iterator it = cur_p.buttons.begin(); it != cur_p.buttons.end(); it++){(*it)->update(); if(&cur_p != current_page) continue;}
      /*Slider*/ for (std::vector<Slider*>::const_iterator it = cur_p.sliders.begin(); it != cur_p.sliders.end(); it++) (*it)->update();
      /*Text*/ for (std::vector<Text_*>::const_iterator it = cur_p.texts.begin(); it != cur_p.texts.end(); it++) (*it)->update();
      /*Flash*/ alert::attempt_end();

      _Task_::delay(10);
    }
  }

  void GUI::update_screen_status(){
    screen_touch_status_s_t status = screen::touch_status();
    touched = status.touch_status == TOUCH_PRESSED || status.touch_status == TOUCH_HELD;
    x = status.x;
    y = status.y;
  }

  bool GUI::pressed() const{
    return (touched && this == GUI::current_gui);
  }

  bool Page::pressed() const{
    if (this == GUI::current_page || this == &perm){
      for (std::vector<GUI*>::const_iterator it = guis.begin(); it != guis.end(); it++){
        if ((*it)->pressed()) return true; //If any of it's owning gui's are pressed
      }
    }
    return false;
  }

  bool Slider::pressed() const{
    return (page->pressed() && active && inRange(GUI::x, x1, x2) && inRange(GUI::y, y1, y2));
  }

  bool Button::pressed() const{
    return (page->pressed() && active && inRange(GUI::x, x1, x2) && inRange(GUI::y, y1, y2));
  }

  bool Button::new_press(){
    if(!last_pressed){
      last_pressed = pressed();
      return last_pressed;
    }
    return false;
  }

  bool Button::new_release(){
    if(last_pressed){
      last_pressed = pressed();
      return !last_pressed;
    }
    return false;
  }

  void Page::update() const{
    if (loop_func) loop_func();
  }

  void Button::update(){
    if (!active) return;

    switch(form){
      case Button::LATCH:
        if (new_press()){
          on = !on; //Toggles the latch
          if (on) select();
          else deselect();
        }
        new_release(); //Must be here even if there is nothing to do.

        if(on) run_func();
        else run_off_func();
        break;
      
      case Button::TOGGLE:
        if (new_press()){
          on = !on; //Toggles the latch
          if (on) select();
          else deselect();
        }
        new_release(); //Must be here even if there is nothing to do.
        break;

      case Button::SINGLE:
        if (new_press()) select();
        else if (new_release()) deselect();

        break;

      case Button::REPEAT:
        if (new_press()) select();
        else if (new_release()) deselect();

        if (pressed()) run_func();

        break;
    }
  }

  void Slider::update(){
    if (!active) return;
    if (pressed()){
      switch (dir){
        case HORIZONTAL:
          val = mapValues(GUI::x, x1, x2, min, max); //Gets val based on press location
          break;

        case VERTICAL:
          val = mapValues(GUI::y, y2, y1, min, max);
          break;
      }
    }
    if (val != prev_val){
      draw();
      prev_val = val;
    }
  }
