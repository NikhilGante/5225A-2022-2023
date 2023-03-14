#include "gui.hpp"
#include "task.hpp"
#include "timer.hpp"
#include "../config.hpp"
#include "../Devices/controller.hpp"
#include "../util.hpp"

//GUI:: Static Variable Declarations
  const Page* GUI::current_page = nullptr;
  bool GUI::touched = false;
  int GUI::x = 0, GUI::y = 0; //try making a point again
  _Task GUI::task{"GUI"};

//Text Vars
  std::string prompt_string;
  namespace alert{
    Timer timer{"GUI Alert", none_log, false};
    std::uint32_t end_time;
  }

//Default pages
  Page perm {"PERM BTNS", Color::pink}; //Common page objects
    Button prev_page{{PAGE_LEFT, PAGE_UP, 75, 20, GUI::Style::SIZE}, Button::SINGLE, perm, "<-"};
    Button next_page{{PAGE_RIGHT, 0, -75, 20, GUI::Style::SIZE}, Button::SINGLE, perm, "->"};
    Button home{{100, 10, 18, 6, GUI::Style::CENTRE}, Button::SINGLE, perm, "Home"};

  Page testing {"Testing"}; //Blank page made so it already exists when quick tests are created
    Text<> testing_text_1 {{125, 50}, GUI::Style::CENTRE, TEXT_SMALL, testing, "BLANK TEXT 1"};
    Text<> testing_text_2 {{350, 50}, GUI::Style::CENTRE, TEXT_SMALL, testing, "BLANK TEXT 2"};
    Button testing_button_1 {{25, 70, 200, 80, GUI::Style::SIZE}, Button::SINGLE, testing, "BLANK BUTTON 1"};
    Button testing_button_2 {{250, 70, 200, 80, GUI::Style::SIZE}, Button::SINGLE, testing, "BLANK BUTTON 2"};
    Slider testing_slider {{MID_X, 200, 200, 20, GUI::Style::CENTRE}, Slider::HORIZONTAL, -100, 100, testing, "BLANK SLIDER"};

  Page prompt_sequence {"Prompt"};
    Button prompt_button {{300, MID_Y, 160, 90, GUI::Style::CENTRE}, Button::SINGLE, prompt_sequence};
    Button prompt_back_button {{20, USER_UP, 100, 50, GUI::Style::SIZE}, Button::SINGLE, prompt_sequence, "BACK"};
    Text prompt_button_text {{0, 0}, GUI::Style::CENTRE, TEXT_SMALL, prompt_sequence, "%s", prompt_string};

  Page screen_flash {"Alert"}; //Called screen_flash because there are a lot of things with the word alert
    Button screen_flash_back_button {{20, USER_UP, 100, 50, GUI::Style::SIZE}, Button::SINGLE, screen_flash, "BACK"};
    Text screen_flash_text {{MID_X, MID_Y}, GUI::Style::CENTRE, TEXT_MEDIUM, screen_flash, ""};
    Text screen_flash_time {{70, 85}, GUI::Style::CENTRE, TEXT_SMALL, screen_flash, "Time Left: %d", [](){return alert::end_time-alert::timer.getTime();}};

  Page terminal {"Screen Printing"};


namespace alert{
  const Page* page;
  Queue<std::pair<Params, std::string>, 25> queue{"Alert"};

  //rest are templates, so defined in header

  void update(){
    //If nothing is currently flashing and there is something to flash, starts new flash
    Params& params = std::get<Params>(queue.front());
    if(!timer.playing() && !queue.empty()){
      Colour colour = static_cast<Colour>(params.GUI_colour);

      screen_flash.b_col = colour;
      screen_flash_time.b_col = colour;
      screen_flash_text.b_col = colour;
      screen_flash_time.l_col = ~colour&0xFFFFFF;
      screen_flash_text.l_col = ~colour&0xFFFFFF;
      screen_flash_text.label = std::get<std::string>(queue.front());

      page = GUI::current_page;
      end_time = params.time;
      screen_flash.goTo();

      master.rumble("-.");

      system_log(params.colour, "%s\n", std::get<std::string>(queue.front()));
      timer.reset(); //Starts counting down
    }

    //If something is flashing and either it's done or interrupted, ends flash
    if(timer.playing() && (timer.getTime() >= end_time || screen_flash_back_button.pressed())){
      queue.pop();
      timer.reset(false);
      page->goTo();
      return;
    }
  }
}

//GUI Helpers
  GUI::Box::Box(int x1, int y1, int x2, int y2, GUI::Style type){
    switch(type){
      case GUI::Style::CENTRE:
        x1 -= x2;
        y1 -= y2;
        x2 += x1+x2;
        y2 += y1+y2;
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

    this->x1 = x1;
    this->y1 = y1;
    this->x2 = x2;
    this->y2 = y2;
  }
  GUI::Box::Box(Coordinate coord1, int x2, int y2, GUI::Style type): Box{coord1.x, coord1.y, x2, y2, type} {}
  GUI::Box::Box(int x1, int y1, Coordinate coord2, GUI::Style type): Box{x1, y1, coord2.x, coord2.y, type} {}
  GUI::Box::Box(Coordinate coord1, Coordinate coord2, GUI::Style type): Box{coord1.x, coord1.y, coord2.x, coord2.y, type} {}

  GUI::Coordinate::Coordinate(int x, int y): x{x}, y{y} {}


  //To get coordinates for aligned objects, (buttons, sliders...) of same size
  //Put in how many of buttons / sliders you want, and get properly spaced coords
  void GUI::alignedCoords (int x_objects, int y_objects, int x_size, int y_size, int x_range, int y_range){
    double x_space = (x_range-x_objects * x_size) / (x_objects+1.0);
    double y_space = (y_range-y_objects * y_size) / (y_objects+1.0);

    //Recalculates until it gets a nice multiple of 5
    while (std::fmod(x_space, 5)) x_space = ((--x_range) - x_objects * x_size) / (x_objects+1.0);
    while (std::fmod(y_space, 5)) y_space = ((--y_range) - y_objects * y_size) / (y_objects+1.0);

    newline(2);
    for (int y = 0; y < y_objects; y++){
      for (int x = 0; x < x_objects; x++){
        printf2("(%d, %d, %d, %d, GUI::Style::SIZE)\n", (x_space) * (x+1) + x_size*x, (y_space) * (y+1) + y_size*y + 20, x_size, y_size);
      }
      newline();
    }
    printf2("\nScreen Size: %d x %d\n", x_range, y_range);
    if ((x_space + x_size) * (x_objects) > 480) printf2("X out of bounds\n");
    if ((y_space + y_size) * (y_objects) > 220) printf2("Y out of bounds\n");
    newline(2);
  }

  bool GUI::prompt(std::string screen, std::string term, std::uint32_t delay_time){
    if(!prompt_enabled) return true;
    prompt_string = screen;
    if(term == "") term = screen;
    printf2(term_colours::BLUE, "\n\n%s\nPress the screen big button or the controller OK button when ready.\n", term);
    master.clear();
    master.print(0, "Press OK btn");
    bool interrupted = false;
    Page const * page = GUI::current_page;
    prompt_sequence.goTo();

    //Wait for Release
    WAIT_UNTIL(!(prompt_button.pressed() || master.getDigital(okBtn) || master.interrupt(false, true, false)) || interrupted){ //checks that no button is being pressed
      GUI::update_screen_status(); //Need this because prompt is run from checking a button press which is in the GUI thread
      if (prompt_back_button.pressed()) interrupted = true;
    }

    //Wait for Press
    WAIT_UNTIL((prompt_button.pressed() || master.getDigital(okBtn)) || interrupted){ //waits for a press from prompt btn or ok btn. Interrupts with any controller digital btn
      GUI::update_screen_status(); //Need this because prompt is run from checking a button press which is in the GUI thread
      if (prompt_back_button.pressed() || master.interrupt(false, true, true)) interrupted = true;
    }
    
    //Wait for Release
    WAIT_UNTIL(!(prompt_button.pressed() || master.getDigital(okBtn) || master.interrupt(false, true, false)) || interrupted){ //checks that no button is being pressed
      GUI::update_screen_status(); //Need this because prompt is run from checking a button press which is in the GUI thread
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

    page->goTo();
    master.clear();
    return !interrupted;
  }

  void GUI::drawOblong(int x1, int y1, int x2, int y2, double kS, double kR){ //ks and kr and scale values for shrink and radius
    int s = std::min(x2-x1, y2-y1) * kS; //Scale for shrinking button (pressed look)
    int r = std::min(x2-x1, y2-y1) * kR; //Scale for how rounded the button edges should be
    screen::fill_rect  (x1+s  , y1+s  , x2-s  , y2-s  );
    screen::erase_rect (x1+s  , y1+s  , x1+s+r, y1+s+r);
    screen::erase_rect (x2-s  , y1+s  , x2-s-r, y1+s+r);
    screen::erase_rect (x1+s  , y2-s  , x1+s+r, y2-s-r);
    screen::erase_rect (x2-s  , y2-s  , x2-s-r, y2-s-r);
    screen::fill_circle(x1+s+r, y1+s+r, r);
    screen::fill_circle(x2-s-r, y1+s+r, r);
    screen::fill_circle(x1+s+r, y2-s-r, r);
    screen::fill_circle(x2-s-r, y2-s-r, r);
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

  void GUI::screen_terminal_fix(){
    //Will only run if things are actually being printed
    if(terminal.texts.empty()){
      terminal.setActive(false);
      return;
    }

    //Sees how much space is user-requested
    int y = USER_UP;
    for (Text_* text_ptr: terminal.texts){
      if (text_ptr->txt_size != 4) y += get_height(text_ptr->txt_size) + 5;
      if(get_width(TEXT_SMALL) * text_ptr->label.length() + 5 > 480){
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

    for (Text_* text_ptr: terminal.texts){
      if (get_width(fmt) * text_ptr->label.length() + 5 > 480){
        if(fmt == TEXT_LARGE){
          if(get_width(TEXT_MEDIUM) * text_ptr->label.length() + 5 < 480) text_ptr->txt_size = TEXT_MEDIUM;
          else if(get_width(TEXT_SMALL) * text_ptr->label.length() + 5 < 480) text_ptr->txt_size = TEXT_SMALL;
        }

        else if(fmt == TEXT_MEDIUM) text_ptr->txt_size = TEXT_SMALL;
      }
    }

    y = USER_UP;
    //Saves y-pos and txt_size
    for (Text_* text_ptr: terminal.texts){
      text_ptr->coord.y = y;
      if (text_ptr->txt_size == 4) text_ptr->txt_size = fmt;
      y += get_height(text_ptr->txt_size) + 5;
    }
  }


//Constructors
  void Button::construct(GUI::Box coord, press_type form, Page* page, std::string text, Color b_col, Color l_col){
    //Saves params to class private vars
    this->coord = coord;
    this->b_col = static_cast<std::uint32_t>(b_col);
    this->b_col_dark = RGB2COLOR(static_cast<int>(COLOR2R(this->b_col) * 0.8), static_cast<int>(COLOR2G(this->b_col) * 0.8), static_cast<int>(COLOR2B(this->b_col) * 0.8));
    this->l_col = static_cast<std::uint32_t>(l_col);
    this->form = form;

    //Saves the buttons owning page
    this->page = page;
    this->page->buttons.push_back(this);
    updateName(text);


    std::size_t next_space = text.find(' ', text.length() / 2);
    std::size_t prev_space = text.find_last_of(' ', text.length() / 2);
    std::size_t space = (next_space-text.length() / 2 < text.length() / 2-prev_space) ? next_space : prev_space;

    if (space != std::string::npos && 8*text.length() + 5 > coord.x2-coord.x1){ //Spaces it if it's too long for one line
      this->label = text.substr(0, space);
      this->label1 = text.substr(space+1);
      
      text_coord = GUI::Coordinate{
        static_cast<int>(coord.x1 + coord.x2-(this->label.length() * CHAR_WIDTH_SMALL)) / 2,
        (coord.y1 + coord.y2-CHAR_HEIGHT_SMALL) / 2 - CHAR_HEIGHT_SMALL
      };

      text_coord1 = GUI::Coordinate{
        static_cast<int>(coord.x1 + coord.x2-(this->label1.length() * CHAR_WIDTH_SMALL)) / 2,
        (coord.y1 + coord.y2-CHAR_HEIGHT_SMALL) / 2 + CHAR_HEIGHT_SMALL
      };
    }
    else{
      text_coord = GUI::Coordinate(
        (coord.x1 + coord.x2-(text.length() * CHAR_WIDTH_SMALL)) / 2,
        (coord.y1 + coord.y2-CHAR_HEIGHT_SMALL) / 2
      );
      this->label = text;
    }
  }

  GUI::GUI(std::vector<Page*> pages, std::function<void()> setup, std::function<void()> background):
  ObjectTracker{"GUI"}, setup{setup}, background{background} {
    //Saves pages to gui
    this->pages.push_back(&perm);
    for (Page* page_ptr: pages) this->pages.push_back(page_ptr);
    this->pages.push_back(&testing);
    this->pages.push_back(&terminal);
    this->pages.push_back(&prompt_sequence);
    this->pages.push_back(&screen_flash);

    //Saves gui to pages
    for (Page* page_ptr: this->pages) page_ptr->guis.push_back(this);
  }

  Button::Button(GUI::Box coord, press_type form, Page& page, std::string text, Color background_colour, Color label_colour): ObjectTracker{"Button", text} {
    construct(coord, form, &page, text, background_colour, label_colour);
  }

  Slider::Slider (GUI::Box coord, direction dir, double min, double max, Page& page, std::string label, double increment, Color background_colour, Color label_colour):
  ObjectTracker{"Slider", label}, coord{coord}, dir{dir}, min{min}, max{max}, page{&page}, val{inRangeIncl(0, min, max) ? 0 : (inRangeIncl(1, min, max) ? 1 : (min + max) / 2)}, b_col{static_cast<std::uint32_t>(background_colour)}, l_col{static_cast<std::uint32_t>(label_colour)} {
    this->page->sliders.push_back(this);

    switch(this->dir){
      case HORIZONTAL:
        text_coord = {(coord.x1+ coord.x2) / 2, coord.y1-CHAR_HEIGHT_SMALL / 2-2};
        inc.construct({coord.x2+ 5, coord.y1, coord.x2+25, coord.y2, GUI::Style::CORNER}, Button::SINGLE, this->page, ">", label_colour, background_colour);
        dec.construct({coord.x1-25, coord.y1, coord.x1- 5, coord.y2, GUI::Style::CORNER}, Button::SINGLE, this->page, "<", label_colour, background_colour);
        title.construct(text_coord, GUI::Style::CENTRE, TEXT_SMALL, this->page, label + ":%d", [&](){return val;}, label_colour); //why not pass val by reference
        min_title.construct({coord.x1, text_coord.y}, GUI::Style::CENTRE, TEXT_SMALL, this->page, "%d", [&](){return this->min;}, label_colour);
        max_title.construct({coord.x2, text_coord.y}, GUI::Style::CENTRE, TEXT_SMALL, this->page, "%d", [&](){return this->max;}, label_colour);
        break;

      case VERTICAL:
        text_coord = {(coord.x1+ coord.x2) / 2, (coord.y1+ coord.y2) / 2};
        inc.construct({coord.x1, coord.y1-17, coord.x2-coord.x1, -20, GUI::Style::SIZE}, Button::SINGLE, this->page, "^", label_colour, background_colour);
        dec.construct({coord.x1, coord.y2+17, coord.x2-coord.x1, 20, GUI::Style::SIZE}, Button::SINGLE, this->page, "v", label_colour, background_colour);
        title.construct({text_coord.x, coord.y1-17-22-CHAR_HEIGHT_SMALL}, GUI::Style::CENTRE, TEXT_SMALL, this->page, label + ":%d", [&](){return val;}, label_colour);
        min_title.construct({text_coord.x, coord.y2 + (CHAR_HEIGHT_SMALL + 3) / 2}, GUI::Style::CENTRE, TEXT_SMALL, this->page, "%d", [&](){return this->min;}, label_colour);
        max_title.construct({text_coord.x, coord.y1-(CHAR_HEIGHT_SMALL + 3) / 2}, GUI::Style::CENTRE, TEXT_SMALL, this->page, "%d", [&](){return this->max;}, label_colour);
        break;
    }

    //Buttons
    if(min > max) increment = -increment;
    dec.setFunc([&, increment](){this->val-=increment; if(!inRangeIncl(this->val, this->min, this->max)) this->val = this->min;});
    inc.setFunc([&, increment](){this->val+=increment; if(!inRangeIncl(this->val, this->min, this->max)) this->val = this->max;});
  }

  Page::Page(std::string title, Color background_colour):
  ObjectTracker{"Page", title}, b_col{static_cast<std::uint32_t>(background_colour)}, title{title} {
    if (!(title == "PERM BTNS" || title == "Prompt" || title == "Alert")){
      for (Button* btn_ptr: perm.buttons) buttons.push_back(btn_ptr);
    }
  }


//Methods
  Page* Page::page_id(int page_num) {return GUI::current_gui->pages.at(page_num);}

  int Page::page_num(const Page* page_id){
    const std::vector<Page*>::const_iterator it = std::find(GUI::current_gui->pages.begin(), GUI::current_gui->pages.end(), page_id);
    if (it == GUI::current_gui->pages.end()){
      throw std::domain_error(sprintf2("Page %p does not exist!\n", page_id));
      return -1;
    }
    return it-GUI::current_gui->pages.begin();
  }

  void GUI::goNext(){
    std::vector<Page*>::const_iterator it = std::find(GUI::current_gui->pages.begin(), GUI::current_gui->pages.end(), current_page);
    do{
      it++;
      if (it == current_gui->pages.end()) it = current_gui->pages.begin();
    } while(!(*it)->active);

    (*it)->goTo();
  }

  void GUI::goPrev(){
    std::vector<Page*>::const_iterator it = std::find(GUI::current_gui->pages.begin(), GUI::current_gui->pages.end(), current_page);
    do{
      if (it == current_gui->pages.begin()) it = current_gui->pages.end();
      it--;
    } while(!(*it)->active);

    (*it)->goTo();
  }

  void GUI::goTo(int page_num) {Page::page_id(page_num)->goTo();}

  void Page::goTo() const{
    if(page_num(this) == -1) return;
    GUI::current_page = this; //Saves new page then draws all the buttons on the page
    draw();
    if(setup_func) setup_func();
    WAIT_UNTIL(!GUI::touched) GUI::update_screen_status();
  }

  void GUI::clearScreen(Color color) {clearScreen(static_cast<std::uint32_t>(color));} 

  void GUI::clearScreen(std::uint32_t colour){
    screen::set_pen(colour);
    screen::fill_rect(PAGE_LEFT, PAGE_UP, PAGE_RIGHT, PAGE_DOWN);
  }

  void Button::create_options(std::vector<Button*> buttons){
    std::vector<Button*>::const_iterator it, it2; //For clarity

    for (Button* btn_ptr: buttons){
      if (btn_ptr->form != LATCH && btn_ptr->form != TOGGLE){
        throw std::invalid_argument(sprintf2("Option Feature is only available for latch and toggle buttons! Failed on \"%s\" button.\n", btn_ptr->label));
        return;
      }
    }

    for (Button* btn1: buttons){ //For each button in the list
      for (Button* btn2: buttons){ //Go through the list and save each button
        if (btn1 != btn2) btn1->options.push_back(btn2);
      }
    }
  }

  void Slider::setValue(double val){
    // double old_val = this->val;
    // this->val = std::clamp<double>(val, min, max);
    // if(this->val != old_val) draw();

    val = std::clamp<double>(val, min, max);
    if(this->val != val){
      this->val = val;
      draw();
    }
  }

  void Button::addText(Text_& text_ref, bool overwrite){
    if(page != text_ref.page){
      throw std::invalid_argument(sprintf2("Text can only be linked to a button on the same page! Failed on \"%s\" button and \"%s\" text.", label, text_ref.label));
      return;
    }
    title = &text_ref;
    text_ref.l_col = l_col;
    text_ref.b_col = b_col;
    text_ref.active = active;
    text_ref.type = GUI::Style::CENTRE;
    text_ref.coord.y = (coord.y1 + coord.y2) / 2;
    text_ref.coord.x = (coord.x1 + coord.x2) / 2;
    text_ref.box.x1 = USER_RIGHT;
    text_ref.box.y1 = USER_DOWN;
    text_ref.box.x2 = USER_LEFT;
    text_ref.box.y2 = USER_UP;

    if (overwrite){
      label = "";
      label1 = "";
    }
    else{
      text_ref.coord.y += GUI::get_height(text_ref.txt_size);
      text_coord.y -= GUI::get_height(text_ref.txt_size);
    }
  }

  void Button::select(){
    switch(form){
      case Button::LATCH:
      case Button::TOGGLE:
        on = true;

        for (Button* option: options) option->deselect();

      case Button::SINGLE:
      case Button::REPEAT:
        drawPressed();
        runFunc();
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
        runOffFunc();
        break;
    }
  }

  void Button::setBackground (Color color){
    if (title) title->setBackground(color);
    if (b_col != static_cast<std::uint32_t>(color)){
      b_col = static_cast<std::uint32_t>(color);
      b_col_dark = RGB2COLOR(static_cast<int>(COLOR2R(b_col) * 0.8), static_cast<int>(COLOR2G(b_col) * 0.8), static_cast<int>(COLOR2B(b_col) * 0.8));
      draw();
    }
  }

  void Text_::setBackground (int x1, int y1, Color color){ //Centre
    box = {coord, x1, y1, GUI::Style::CENTRE};
    setBackground(color);
  }

  void Text_::setBackground (int x1, int y1, int x2, int y2, GUI::Style type, Color color){
    box = {x1, y1, x2, y2, type};
    setBackground(color);
  }

  void Text_::setBackground (int x1, int y1){ //Centre
    box = {coord, x1, y1, GUI::Style::CENTRE};
  }

  void Text_::setBackground (int x1, int y1, int x2, int y2, GUI::Style type){
    box = {x1, y1, x2, y2, type};
  }

  void Text_::setBackground (Color color){
    if (b_col != static_cast<std::uint32_t>(color)){
      b_col = static_cast<std::uint32_t>(color);
      draw();
    }
  }

  void Page::setActive(bool active){
    this->active = active;
    if (!active && this == GUI::current_page) GUI::goNext();
  }

  void Button::setActive(bool active){
    this->active = active;
    if (title) title->setActive(active);
    if (page == GUI::current_page){
      if (active) draw();
      else{
        screen::set_pen(page->b_col);
        screen::fill_rect(coord.x1, coord.y1, coord.x2, coord.y2);
      }
    }
  }

  void Text_::setActive(bool active){
    this->active = active;
    if (page == GUI::current_page){
      if (active) draw();
      else{
        screen::set_pen(page->b_col);
        screen::fill_rect(box.x1, box.y1, box.x2, box.y2);
      }
    }
  }

  void Slider::setActive(bool active){
    this->active = active;
    if (page == GUI::current_page){
      if (active) draw();
      else{
        screen::set_pen(page->b_col);
        screen::fill_rect(coord.x1, coord.y1, coord.x2, coord.y2);
      }
    }
  }


//Drawing
  void Page::draw() const{
    GUI::clearScreen(b_col);
    screen::set_pen(b_col);
    screen::set_eraser(b_col);
    screen::fill_rect(PAGE_LEFT, PAGE_UP, PAGE_RIGHT, 20);
    screen::set_pen(static_cast<std::uint32_t>(Color::white));
    screen::print(TEXT_SMALL, MID_X-(title.length() + 3 + std::to_string(page_num(this)).length()) * CHAR_WIDTH_SMALL / 2, 5, "%s - %d", title, page_num(this));
    for (Button* button: buttons) button->draw();
    for (Slider* slider: sliders) slider->draw();
    for (Text_*  text  : texts  ) text  ->draw();
  }

extern Page logging;

  void Button::draw() const{
    if (!(active && (page == GUI::current_page || page == &perm))) return;
    if (on){ //on buttons must be drawn in a pressed state
      drawPressed();
      return;
    }
    screen::set_pen(b_col);
    screen::set_eraser(GUI::current_page->b_col); //It's current_page not just page because perm has a different colour
    screen::fill_rect(coord.x1, coord.y1, coord.x2, coord.y2);
    GUI::drawOblong(coord.x1, coord.y1, coord.x2, coord.y2, 0, 0.15);
    screen::set_pen(l_col);
    screen::set_eraser(b_col);
    screen::print(TEXT_SMALL, text_coord.x, text_coord.y, "%s", label);
    screen::print(TEXT_SMALL, text_coord1.x, text_coord1.y, "%s", label1);
    if(title){
      title->b_col = b_col;
      title->draw();
    }
  }

  void Button::drawPressed() const{
    if (!(active && (page == GUI::current_page || page == &perm))) return;
    screen::set_eraser(page->b_col); //Erases button
    screen::erase_rect(coord.x1, coord.y1, coord.x2, coord.y2);

    screen::set_pen(b_col_dark);
    screen::set_eraser(GUI::current_page->b_col);
    GUI::drawOblong(coord.x1, coord.y1, coord.x2, coord.y2, 0.04, 0.2);
    screen::set_pen(l_col);
    screen::set_eraser(b_col_dark);
    screen::print(TEXT_SMALL, text_coord.x, text_coord.y, "%s", label);
    screen::print(TEXT_SMALL, text_coord1.x, text_coord1.y, "%s", label1);
    if(title){
      title->b_col = b_col_dark;
      title->draw();
    }
  }

  void Slider::draw() const{
    if (!(active && (page == GUI::current_page || page == &perm))) return;
    screen::set_pen(b_col);
    screen::fill_rect(coord.x1, coord.y1, coord.x2, coord.y2);
    screen::set_pen(l_col);
    if(dir == HORIZONTAL) screen::fill_rect(coord.x1+1, coord.y1+1, remapRange(std::clamp(val, min, max), min, max, coord.x1, coord.x2), coord.y2-1);
    else screen::fill_rect(coord.x1+1, remapRange(std::clamp(val, min, max), min, max, coord.y2, coord.y1), coord.x2-1, coord.y2-1);
  }

  void Text_::draw(){
    if (!(active && (page == GUI::current_page || page == &perm))) return;
    updateVal();

    if(box.x2 != 0){ //Hasn't been drawn yet
      screen::set_eraser(page->b_col);
      screen::erase_rect(box.x1, box.y1, box.x2, box.y2);
      screen::set_pen(b_col);
      GUI::drawOblong(box.x1, box.y1, box.x2, box.y2, 0, 0.15);
    }
    screen::set_pen(l_col);
    screen::set_eraser(b_col);

    int x_coord = coord.x, y_coord = coord.y;
    if (type == GUI::Style::CENTRE){
      x_coord -= GUI::get_width(txt_size) / 2.0*text.length();
      y_coord -= GUI::get_height(txt_size) / 2.0;
    }
    
    //Resizes the background so it won't have overwriting issues
    box.x1 = std::min(box.x1, x_coord);
    box.x2 = std::max(box.x2, x_coord + (GUI::get_width(txt_size)+1) *static_cast<int>(text.length()));
    box.y1 = std::min(box.y1, y_coord);
    box.y2 = std::max(box.y2, y_coord + GUI::get_height(txt_size));

    screen::print(txt_size, x_coord, y_coord, "%s", text);
  }


//Function Handling
  void Page::setSetupFunc(std::function<void()> function) {setup_func = function;}
  void Page::setLoopFunc (std::function<void()> function) { loop_func = function;}
  void Button::setFunc   (std::function<void()> function) {      func = function;}
  void Button::setOffFunc(std::function<void()> function) {  off_func = function;}
  void Button::runFunc() const {if (func) func();}
  void Button::runOffFunc() const {if (off_func) off_func();}
  bool Button::isOn() const {return on;}
  double Slider::getValue() const {return val;}

//Data Updates
  void GUI::init(){
    screen_terminal_fix();
    testing.setActive(testing_page_active);
    perm.setActive(false);
    prompt_sequence.setActive(false);
    screen_flash.setActive(false);

    prev_page.setFunc(&goPrev);
    next_page.setFunc(&goNext);
    home.setFunc([](){goTo(1);});

    prompt_button.addText(prompt_button_text);

    current_gui->setup();

    //! Do not touch this section. I do not fully understand, but any change here will break things
    goTo(0);
    if(terminal.active) terminal.goTo();
    else goTo(1); //! Sets it to page 1 for program start. Don't delete this. If you want to change the starting page, call GUI::goTo(Page Number) in initialize()

    GUI::task.start(update);
  }

  void GUI::update(){
    while(true){
      current_gui->background();
      update_screen_status();
      const Page& cur_p = *current_page;
      /*Page*/ cur_p.update();
      /*Button*/ for (Button* button: cur_p.buttons){button->update(); if(&cur_p != current_page) continue;}
      /*Slider*/ for (Slider* slider: cur_p.sliders) slider->update();
      /*Text*/   for (Text_*  text  : cur_p.texts  ) text  ->update();
      /*Flash*/ alert::update();

      _Task::delay(10);
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
      for (GUI* gui: guis){
        if (gui->pressed()) return true; //If any of it's owning gui's are pressed
      }
    }
    return false;
  }

  bool Slider::pressed() const{
    return (page->pressed() && active && inRangeIncl(GUI::x, coord.x1, coord.x2) && inRangeIncl(GUI::y, coord.y1, coord.y2));
  }

  bool Button::pressed() const{
    return (page->pressed() && active && inRangeIncl(GUI::x, coord.x1, coord.x2) && inRangeIncl(GUI::y, coord.y1, coord.y2));
  }

  bool Button::newPress(){
    if(!last_pressed){
      last_pressed = pressed();
      return last_pressed;
    }
    return false;
  }

  bool Button::newRelease(){
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
        if (newPress()){
          on = !on; //Toggles the latch
          if (on) select();
          else deselect();
        }
        newRelease(); //Must be here even if there is nothing to do.

        if(on) runFunc();
        else runOffFunc();
        break;
      
      case Button::TOGGLE:
        if (newPress()){
          on = !on; //Toggles the latch
          if (on) select();
          else deselect();
        }
        newRelease(); //Must be here even if there is nothing to do.
        break;

      case Button::SINGLE:
        if (newPress()) select();
        else if (newRelease()) deselect();
        break;

      case Button::REPEAT:
        if (newPress()) select();
        else if (newRelease()) deselect();

        if (pressed()) runFunc();
        break;
    }
  }

  void Slider::update(){
    if (!active) return;
    if (pressed()){
      switch (dir){
        case HORIZONTAL:
          val = remapRange(GUI::x, coord.x1, coord.x2, min, max); //Gets val based on press location
          break;

        case VERTICAL:
          val = remapRange(GUI::y, coord.y2, coord.y1, min, max);
          break;
      }
    }
    if (val != prev_val){
      draw();
      prev_val = val;
    }
  }
