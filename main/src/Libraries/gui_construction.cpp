#include "gui.hpp"
#include "pros/apix.h"
#include "../Devices/piston.hpp"
#include "../Devices/motor.hpp"
#include "../config.hpp"
#include "../tracking.hpp"
#include "../drive.hpp"
#include "../util.hpp"
#include "../menu.hpp"
#include "../Subsystems/intake.hpp"
#include "../Subsystems/flywheel.hpp"
#include "../Subsystems/shooter.hpp"

#include "pros/misc.hpp"

//DO NOT MESS WITH INDENTATION IN THIS FILE //Actually maybe you can if you want. VS Code is mean :(

/*Field array*/ static std::vector<std::bitset<200>> field (200, std::bitset<200>{}); //Initializes to 200 blank bitsets
/*Temperature Alert Flag*/ static bool temp_flashed = false;

//Var init for text monitoring
  int enc_val;
  c::ext_adi_encoder_t ext_test_enc;
  c::adi_encoder_t test_enc;
  std::string motor_port_nums;
  std::string expander_port_nums;
  std::string no_pneumatic_port_nums;
  std::array<std::tuple<int, Button*, Button*, Text_*, int, std::string>, 8> motor_ports; //util: port, run, stop, stall counter, text(port and rpm)
  std::array<int, 21> expander_ports;
  std::array<Button*, 8> exp_pneum_btns;

//Constructors
  //Main GUI
    Page temps ("Temperature"); //Motor temps
      //Remaining elements created upon instantiation of _Motor objects

    Page checks("Competition");
      Button drive_motors (15, 45, 100, 75, GUI::Style::SIZE, Button::SINGLE, checks, "Drive Motors");
      Button pneums (130, 45, 100, 75, GUI::Style::SIZE, Button::SINGLE, checks, "Pneumatics");
      Button auton_selector (245, 140, 100, 75, GUI::Style::SIZE, Button::SINGLE, checks, "Select Autons");
      Button sensor_check (15, 140, 100, 75, GUI::Style::SIZE, Button::SINGLE, checks, "Distance");
      Button misc_checks (130, 140, 100, 75, GUI::Style::SIZE, Button::SINGLE, checks, "Misc");
      // Button save_pos (245, 140, 100, 75, GUI::Style::SIZE, Button::SINGLE, checks, "Save Position");

    Page track ("Tracking"); //Display tracking vals and reset btns
      Text track_x(50, 45, GUI::Style::CENTRE, TEXT_SMALL, track, "X:%.1f", std::function([](){return tracking.getPos().x;}));
      Text track_y(135, 45, GUI::Style::CENTRE, TEXT_SMALL, track, "Y:%.1f", std::function([](){return tracking.getPos().y;}));
      Text track_a(220, 45, GUI::Style::CENTRE, TEXT_SMALL, track, "A:%.1f", std::function([](){return std::fmod(tracking.getPos().a, 360);}));
      Text enc_l(50, 130, GUI::Style::CENTRE, TEXT_SMALL, track, "L:%d", std::function([](){return left_tracker.get_position() != std::numeric_limits<int32_t>::max() ? left_tracker.get_position() : 0;}));
      Text enc_r(135, 130, GUI::Style::CENTRE, TEXT_SMALL, track, "R:%d", std::function([](){return right_tracker.get_position() != std::numeric_limits<int32_t>::max() ? right_tracker.get_position() : 0;}));
      Text enc_b(220, 130, GUI::Style::CENTRE, TEXT_SMALL, track, "B:%d", std::function([](){return back_tracker.get_position() != std::numeric_limits<int32_t>::max() ? back_tracker.get_position() : 0;}));
      Button res_x(15, 60, 70, 55, GUI::Style::SIZE, Button::SINGLE, track, "Reset X");
      Button res_y(100, 60, 70, 55, GUI::Style::SIZE, Button::SINGLE, track, "Reset Y");
      Button res_a(185, 60, 70, 55, GUI::Style::SIZE, Button::SINGLE, track, "Reset A");
      Button res_all(15, 160, 110, 60, GUI::Style::SIZE, Button::SINGLE, track, "Reset All");
      Button res_target(135, 160, 110, 60, GUI::Style::SIZE, Button::SINGLE, track, "Reset to Target");

    Page moving ("Moving"); //Moves to target, home, or centre
      Slider x_val(35, 45, 250, 40, GUI::Style::SIZE, Slider::HORIZONTAL, 0, 144, moving, "X");
      Slider y_val(35, 110, 250, 40, GUI::Style::SIZE, Slider::HORIZONTAL, 0, 144, moving, "Y");
      Slider a_val(35, 175, 250, 40, GUI::Style::SIZE, Slider::HORIZONTAL, 0, 360, moving, "A");
      Button go_to_xya(320, 45, 150, 40, GUI::Style::SIZE, Button::SINGLE, moving, "Target");
      Button go_home(320, 110, 150, 40, GUI::Style::SIZE, Button::SINGLE, moving, "Home");
      Button go_centre(320, 175, 150, 40, GUI::Style::SIZE, Button::SINGLE, moving, "Centre");

    Page systems ("Subsystems"); //Activates Subsystems
      Text shoot_text(0, 0, GUI::Style::CENTRE, TEXT_SMALL, systems, "Shooter");
      Button shoot1(0, 0, 0, 0, GUI::Style::SIZE, Button::SINGLE, systems, "1");
      Button shoot2(0, 0, 0, 0, GUI::Style::SIZE, Button::SINGLE, systems, "2");
      Button shoot3(0, 0, 0, 0, GUI::Style::SIZE, Button::SINGLE, systems, "3");
      Text intake_text(0, 0, GUI::Style::CENTRE, TEXT_SMALL, systems, "Intake");
      Button intake_on(0, 0, 0, 0, GUI::Style::SIZE, Button::TOGGLE, systems, "On");
      Button intake_rev(0, 0, 0, 0, GUI::Style::SIZE, Button::TOGGLE, systems, "Off");
      Button intake_off(0, 0, 0, 0, GUI::Style::SIZE, Button::TOGGLE, systems, "Reverse");
      Slider flywheel_vel(0, 0, 0, 0, GUI::Style::CENTRE, Slider::HORIZONTAL, 0, 127, systems, "Flywheel Velocity");
      Button flywheel_set(0, 0, 0, 0, GUI::Style::SIZE, Button::SINGLE, systems, "Flywheel Set");


    Page tuning ("Tuning Tracking"); //Tests to tune tracking when on new base
      Text tuning_instructions_1(MID_X, 35, GUI::Style::CENTRE, TEXT_SMALL, tuning, "Press your desired tracking test and follow");
      Text tuning_instructions_2(MID_X, 50, GUI::Style::CENTRE, TEXT_SMALL, tuning, "the terminal for instructions and results");
      Button encoder_direction (15, 70, 100, 70, GUI::Style::SIZE, Button::SINGLE, tuning, "Encoder Direction");
      Button side_corkscrew (130, 70, 100, 70, GUI::Style::SIZE, Button::SINGLE, tuning, "Side Corkscrew");
      Button back_corkscrew (245, 70, 100, 70, GUI::Style::SIZE, Button::SINGLE, tuning, "Back Corkscrew");
      Button spin360 (360, 70, 100, 70, GUI::Style::SIZE, Button::SINGLE, tuning, "Spins");
      Button wheel_size (15, 155, 100, 70, GUI::Style::SIZE, Button::SINGLE, tuning, "Wheel Size");
      Button back_wheel_size (130, 155, 100, 70, GUI::Style::SIZE, Button::SINGLE, tuning, "Back Wheel Size");
      Button encoder_ticks (245, 155, 100, 70, GUI::Style::SIZE, Button::SINGLE, tuning, "Encoder Ticks");
      Button manual (410, 190, 35, 20, GUI::Style::CENTRE, Button::TOGGLE, tuning, "Manual");

    Page motors ("Motor Control"); //Motor Control for known ports
      Slider mot_speed_set (MID_X, 60, 180 , 15, GUI::Style::CENTRE, Slider::HORIZONTAL, -127, 127, motors, "Speed");
      //Remaining elements created upon instantiation of _Motor objects

    Page pneumatics ("Pneumatics"); //Pneumatic testing page for known ports
      //Remaining elements created upon instantiation of Piston objects

    Page logging ("Logging"); //Log printing page from file to terminal
      //Remaining elements created upon instantiation of Logging objects


  //Utility

    Page ports ("Ports"); //Shows what ports to use on builder util
      Text mot (10, 50, GUI::Style::CORNER, TEXT_MEDIUM, ports, "Motors: %s", motor_port_nums);
      Text expanders (10, 100, GUI::Style::CORNER, TEXT_MEDIUM, ports, "Expanders: %s", expander_port_nums);
      Text no_pneumatic (10, 150, GUI::Style::CORNER, TEXT_MEDIUM, ports, "No Pneumatics (Ignore): %s", no_pneumatic_port_nums);

    Page encoders ("Encoders"); //Display tracking vals and reset btns
      Slider expander_1 (30, 90, 30, 100, GUI::Style::SIZE, Slider::VERTICAL, 0, 21, encoders, "E1");
      Slider port_1 (115, 90, 30, 100, GUI::Style::SIZE, Slider::VERTICAL, 1, 8, encoders, "P1");
      Slider port_2 (200, 90, 30, 100, GUI::Style::SIZE, Slider::VERTICAL, 1, 8, encoders, "P2");
      Button enc_set (350, 60, 50, 20, GUI::Style::CENTRE, Button::SINGLE, encoders, "Set");
      Text enc (350, 100, GUI::Style::CENTRE, TEXT_SMALL, encoders, "%s", std::function([](){return sprintf2("%d:%c%c", expander_1.getValue(), port_1.getValue() + 64, port_2.getValue() + 64);}));
      Text enc_degs (350, 120, GUI::Style::CENTRE, TEXT_SMALL, encoders, "Degs: %d", enc_val);
      Text enc_rots (350, 140, GUI::Style::CENTRE, TEXT_SMALL, encoders, "Rots: %d", std::function([](){return int(enc_val / 360);}));
      Text enc_remain (350, 160, GUI::Style::CENTRE, TEXT_SMALL, encoders, "Remaining: %d", std::function([](){return std::abs(enc_val-360*int(std::round((enc_val + sgn(enc_val) * 180) / 360)));})); //replace with near_angle
      Button enc_res (350, 200, 50, 20, GUI::Style::CENTRE, Button::SINGLE, encoders, "Reset");

    Page motor ("Motor Control");
      Slider mot_speed (60, 45, 300 , 30, GUI::Style::SIZE, Slider::HORIZONTAL, -127, 127, motor, "Speed");
      Button mot_jam_detect (USER_RIGHT-20, 45, -60, 30, GUI::Style::SIZE, Button::TOGGLE, motor, "Jam");
      Text mot_text_1 (65, 115, GUI::Style::CENTRE, TEXT_SMALL, motor, "Port %s", std::get<5>(motor_ports[0]));
      Text mot_text_2 (180, 115, GUI::Style::CENTRE, TEXT_SMALL, motor, "Port %s", std::get<5>(motor_ports[1]));
      Text mot_text_3 (295, 115, GUI::Style::CENTRE, TEXT_SMALL, motor, "Port %s", std::get<5>(motor_ports[2]));
      Text mot_text_4 (410, 115, GUI::Style::CENTRE, TEXT_SMALL, motor, "Port %s", std::get<5>(motor_ports[3]));
      Text mot_text_5 (65, 180, GUI::Style::CENTRE, TEXT_SMALL, motor, "Port %s", std::get<5>(motor_ports[4]));
      Text mot_text_6 (180, 180, GUI::Style::CENTRE, TEXT_SMALL, motor, "Port %s", std::get<5>(motor_ports[5]));
      Text mot_text_7 (295, 180, GUI::Style::CENTRE, TEXT_SMALL, motor, "Port %s", std::get<5>(motor_ports[6]));
      Text mot_text_8 (410, 180, GUI::Style::CENTRE, TEXT_SMALL, motor, "Port %s", std::get<5>(motor_ports[7]));
      Button mot_update_1 (15, 125, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Run");
      Button mot_update_2 (130, 125, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Run");
      Button mot_update_3 (245, 125, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Run");
      Button mot_update_4 (360, 125, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Run");
      Button mot_update_5 (15, 190, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Run");
      Button mot_update_6 (130, 190, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Run");
      Button mot_update_7 (245, 190, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Run");
      Button mot_update_8 (360, 190, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Run");
      Button mot_stop_1 (70, 125, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Stop");
      Button mot_stop_2 (185, 125, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Stop");
      Button mot_stop_3 (300, 125, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Stop");
      Button mot_stop_4 (415, 125, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Stop");
      Button mot_stop_5 (70, 190, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Stop");
      Button mot_stop_6 (185, 190, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Stop");
      Button mot_stop_7 (300, 190, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Stop");
      Button mot_stop_8 (415, 190, 45, 30, GUI::Style::SIZE, Button::SINGLE, motor, "Stop");

    Page pneumatic ("Pneumatics"); //Pneumatic testing page for random ports
      Slider expander (MID_X, 60, 180 , 15, GUI::Style::CENTRE, Slider::HORIZONTAL, 0, 21, pneumatic, "Expander");
      Text ADI_a_text (65, 115, GUI::Style::CENTRE, TEXT_SMALL, pneumatic, "A");
      Text ADI_b_text (180, 115, GUI::Style::CENTRE, TEXT_SMALL, pneumatic, "B");
      Text ADI_c_text (295, 115, GUI::Style::CENTRE, TEXT_SMALL, pneumatic, "C");
      Text ADI_d_text (410, 115, GUI::Style::CENTRE, TEXT_SMALL, pneumatic, "D");
      Text ADI_e_text (65, 180, GUI::Style::CENTRE, TEXT_SMALL, pneumatic, "E");
      Text ADI_f_text (180, 180, GUI::Style::CENTRE, TEXT_SMALL, pneumatic, "F");
      Text ADI_g_text (295, 180, GUI::Style::CENTRE, TEXT_SMALL, pneumatic, "G");
      Text ADI_h_text (410, 180, GUI::Style::CENTRE, TEXT_SMALL, pneumatic, "H");
      Button ADI_a (15, 125, 100, 30, GUI::Style::SIZE, Button::TOGGLE, pneumatic, "On/Off");
      Button ADI_b (130, 125, 100, 30, GUI::Style::SIZE, Button::TOGGLE, pneumatic, "On/Off");
      Button ADI_c (245, 125, 100, 30, GUI::Style::SIZE, Button::TOGGLE, pneumatic, "On/Off");
      Button ADI_d (360, 125, 100, 30, GUI::Style::SIZE, Button::TOGGLE, pneumatic, "On/Off");
      Button ADI_e (15, 190, 100, 30, GUI::Style::SIZE, Button::TOGGLE, pneumatic, "On/Off");
      Button ADI_f (130, 190, 100, 30, GUI::Style::SIZE, Button::TOGGLE, pneumatic, "On/Off");
      Button ADI_g (245, 190, 100, 30, GUI::Style::SIZE, Button::TOGGLE, pneumatic, "On/Off");
      Button ADI_h (360, 190, 100, 30, GUI::Style::SIZE, Button::TOGGLE, pneumatic, "On/Off");


void mainSetup(){
  //Competition
    drive_motors.setFunc([](){
      if(GUI::prompt("Press to check drive motors", "", 1000)){
        moveDrive(60, 0);
        delay(1000);

        driveBrake();
        delay(250);

        moveDrive(-60, 0);
        delay(1000);

        moveDrive(0, 0);
      }
    });

    pneums.setFunc([](){
      for(auto piston: Piston::getList()){
        if (GUI::prompt("Press to check " + piston->getName())){
          piston->toggleState();
          delay(500);
          piston->toggleState();
        }
        else return;
      }
    });

    sensor_check.setFunc([](){
      for (_Motor* motor: _Motor::getList()) if (!motor->plugged()) alert::start("Motor %s unplugged", motor->getName());
      if(c::registry_get_plugged_type(roller_sensor.get_port()-1) != c::E_DEVICE_OPTICAL) alert::start("Roller sensor not plugged in port %d", roller_sensor.get_port());
      else if(c::registry_get_plugged_type(rotation_port-1) != c::E_DEVICE_ROTATION) alert::start("Roller sensor not plugged in port %d", rotation_port);

      else if(!inRange(r_reset_dist.get(), 20, 2000)) alert::start("Right Distance Sensor Out of Range");
      else if(!inRange(l_reset_dist.get(), 20, 2000)) alert::start("Left Distance Sensor Out of Range");

      else(alert::start("All Sensors Good", term_colours::GREEN));
    });

    misc_checks.setFunc([](){
      if (!usd::is_installed()) alert::start("No SD Card!");
      else if (battery::get_capacity() <= 60) alert::start("Battery is at %d%%", battery::get_capacity());
      else if(battery::get_voltage() < 12200) alert::start("Battery is at %dV",  battery::get_voltage() );

      else(alert::start("No Errors Found", term_colours::GREEN));
    });

    auton_selector.setFunc(Auton::select);

  //Tracking
    // for (int x = 0; x < 200; x++) field[x].reset(); //Should be able to get rid of this

    track.setSetupFunc([](){
      screen::set_pen(static_cast<std::uint32_t>(Color::white));
      screen::draw_rect(270, 30, 470, 230);
      screen::draw_line(370, 30, 370, 230);
      screen::draw_line(270, 130, 470, 130);
      for (int x = 0; x < 200; x++){
        for (int y = 0; y < 200; y++) if(field[x].test(y)) screen::draw_pixel(270 + x, 230-y); //Draws saved tracking values
      }
    });
    track.setLoopFunc([](){
      screen::set_pen(static_cast<std::uint32_t>(Color::red));
      screen::draw_pixel(270 + (200.0*tracking.getPos().x / 144.0), 230-(200.0*tracking.getPos().y / 144.0)); //Scales to screen

      if(main_obj.pressed()){
        int x = GUI::x-270, y = 230-GUI::y;
        if(inRange(x, 0, 200) && inRange(y, 0, 200)){
          x_val.setValue(x * 144.0 / 200.0);
          y_val.setValue(y * 144.0 / 200.0);
        }
      }
    });

    // res_y.setFunc([](){tracking.reset(tracking.getPos().x, 0.0, tracking.getPos().a);});
    // res_x.setFunc([](){tracking.reset(0.0, tracking.getPos().y, tracking.getPos().a);});
    // res_a.setFunc([](){tracking.reset(tracking.getPos().x, tracking.getPos().y, 0.0);});

    res_all.setFunc([](){
      right_tracker.reset();
      left_tracker.reset();
      back_tracker.reset();
      DEPRECATE;
      // tracking.reset();
    });

    res_target.setFunc([](){
      right_tracker.reset();
      left_tracker.reset();
      back_tracker.reset();
      DEPRECATE;
      // tracking.reset(Position(x_val.getValue(), y_val.getValue(), a_val.getValue()));
    });

  //Moving
    moving.setSetupFunc([](){
      Position pos = tracking.getPos();
      x_val.setValue(pos.x);
      y_val.setValue(pos.y);
      a_val.setValue(pos.a);
    });

    go_to_xya.setFunc([&](){
      Position target (x_val.getValue(), y_val.getValue(), a_val.getValue());
      if (GUI::prompt("Press to go to " + sprintf2("%d", target), "", 1000)) moveToTargetAsync(target);
    });
    go_home.setFunc([](){
      Position target (0, 0, tracking.getPos().a);
      if (GUI::prompt("Press to go to " + sprintf2("%d", target), "", 1000)) moveToTargetAsync(target);
    });
    go_centre.setFunc([](){
      Position target (72, 72, tracking.getPos().a);
      if (GUI::prompt("Press to go to " + sprintf2("%d", target), "", 1000)) moveToTargetAsync(target);
    });

  //Subsystems
    shoot1.setFunc([](){shoot(1);});
    shoot2.setFunc([](){shoot(2);});
    shoot3.setFunc([](){shoot(3);});
    intake_on.setFunc([](){intakeOn();});
    intake_rev.setFunc([](){intakeRev();});
    intake_off.setFunc([](){intakeOff();});
    flywheel_set.setFunc([](){setFlywheelVel(flywheel_vel.getValue());});

  //Tuning Tracking
    manual.select();
    //Figure out how to do these with the other left / right, front / back or moving in the other direction

    encoder_direction.setFunc([](){ //Encoder Direction
      if(manual.on){
        if(GUI::prompt("Press, then move the robot forward and to the right", "")){
          tracking.reset();

          if (GUI::prompt("Press when done movement")){
            printf2("The left tracking wheel is in the %s direction", left_tracker.get_position() > 0 ? "right" : "wrong");
            printf2("The right tracking wheel is in the %s direction", right_tracker.get_position() > 0 ? "right" : "wrong");
            printf2("The back tracking wheel is in the %s direction", back_tracker.get_position() > 0 ? "right" : "wrong");
          }
        }
      }
      else{
        if(GUI::prompt("Press, then clear the area", "", 1500)){
          tracking.reset();

          moveDrive(50.0, 20.0); //preferably strafe right instead of turning.
          tracking.waitForDistance(30);
          driveBrake();

          printf2("The left tracking wheel is in the %s direction", left_tracker.get_position() > 0 ? "right" : "wrong");
            printf2("The right tracking wheel is in the %s direction", right_tracker.get_position() > 0 ? "right" : "wrong");
            printf2("The back tracking wheel is in the %s direction", back_tracker.get_position() > 0 ? "right" : "wrong");
        }
      }
    });

    encoder_ticks.setFunc([](){ //Encoder missing ticks
      if(manual.on){
        if (GUI::prompt("Press, then spin the encoder", "Start the encoder in a known position. Press then button, then spin it wildly, and then stop it in that position again.")){
          tracking.reset();

          if (GUI::prompt("Press when stopped")){
            double left = left_tracker.get_position() / 360.0;
            double right = right_tracker.get_position() / 360.0;
            double back = back_tracker.get_position() / 360.0;
            int left_rot = round(left);
            int right_rot = round(right);
            int back_rot = round(back);
            left -= left_rot;
            right -= right_rot;
            back -= back_rot;
            term_colours left_col = std::abs(left) <= 0.01 ? term_colours::GREEN : term_colours::RED;
            term_colours right_col = std::abs(right) <= 0.01 ? term_colours::GREEN : term_colours::RED;
            term_colours back_col = std::abs(back) <= 0.01 ? term_colours::GREEN : term_colours::RED;

            if(left == 0) printf2(term_colours::GREEN, "The left encoder was perfect over %d rotations", left_rot);
            else printf2(left_col, "The left encoder %s %d ticks over %d rotations", left > 0 ? "gained" : "lost", std::abs(360.0 * left), left_rot);

            if(right == 0) printf2(term_colours::GREEN, "The right encoder was perfect over %d rotations", right_rot);
            else printf2(right_col, "The right encoder %s %d ticks over %d rotations", right > 0 ? "gained" : "lost", std::abs(360.0 * right), right_rot);

            if(back == 0) printf2(term_colours::GREEN, "The back encoder was perfect over %d rotations", back_rot);
            else printf2(back_col, "The back encoder %s %d ticks over %d rotations", back > 0 ? "gained" : "lost", std::abs(360.0 * back), back_rot);
          }
        }
      }
      else{
        alert::start("No Automatic Option");
      }
    });

    back_corkscrew.setFunc([](){ //Back Corkscrew
      if(manual.on){
        if(GUI::prompt("Press, then move along y", "Please slide the robot forward along a known surface in the y-direction")){
          tracking.reset();

          if (GUI::prompt("Press when stopped")){
            int back = back_tracker.get_position();
            if(std::abs(back) <= 2) printf2(term_colours::GREEN, "The back wheel is pretty accurate, it is at %d ticks.", back);
            else printf2(term_colours::RED, "The back wheel is corkscrewing. It is at %d ticks. Consider turning it %sclockwise.", back, sgn(left_tracker.get_position()) == sgn(back) ? "" : "counter-");
          }
        }
      }
      else{
        if(GUI::prompt("Push the left side against a surface", "Please push the robot's left side along a known surface, then press.", 1500)){
          tracking.reset();
          
          moveDrive(50.0, -20.0);
          tracking.waitForDistance(25);
          driveBrake();

          int back = back_tracker.get_position();
          if(std::abs(back) <= 2) printf2(term_colours::GREEN, "The back wheel is pretty accurate, it is at %d ticks.", back);
          else printf2(term_colours::RED, "The back wheel is corkscrewing. It is at %d ticks. Consider turning it %sclockwise.", back, sgn(left_tracker.get_position()) == sgn(back) ? "" : "counter-");
        }
      }
    });

    side_corkscrew.setFunc([](){ //Left-Right Corkscrew
      if(manual.on){
        if(GUI::prompt("Press, then move along x", "Please slide the robot along a known surface in the x-direction")){
          tracking.reset();

          if(GUI::prompt("Press when stopped")){
            int left = left_tracker.get_position();
            int right = right_tracker.get_position();

            if(std::abs(left) <= 2) printf2(term_colours::GREEN, "The left wheel is pretty accurate, it is at %d ticks.", left);
            else printf2(term_colours::RED, "The left wheel is corkscrewing. It is at %d ticks. Consider turning it %sclockwise.", left, sgn(left) == sgn(back_tracker.get_position()) ? "counter-" : "");

            if(std::abs(right) <= 2) printf2(term_colours::GREEN, "The right wheel is pretty accurate, it is at %d ticks.", right);
            else printf2(term_colours::RED, "The right wheel is corkscrewing. It is at %d ticks. Consider turning it %sclockwise.", right, sgn(right) == sgn(back_tracker.get_position()) ? "counter-" : "");
          }
        }
      }
      else{
        alert::start("I cannot strafe");
      }
    });

    wheel_size.setFunc([](){ //Left-Right wheel size
      if(manual.on){
        if(GUI::prompt("Press, then move the robot 30 inches", "Place the robot's side against a known surface. Press the button, then slide the robot forwards exactly 30 inches.")){
          tracking.reset();

          if (GUI::prompt("Press when completed 30 inches")){
            printf2("The left tracking wheel's diameter is %f", 60.0 / degToRad(left_tracker.get_position()));
            printf2("The right tracking wheel's diameter is %f", 60.0 / degToRad(right_tracker.get_position()));
          }
        }
      }
      else{
        if(GUI::prompt("Place the robot's side against a surface. Then press.", "", 1500)){
          tracking.reset();

          moveDrive(50.0, -20.0);
          tracking.waitForDistance(30); //This needs to brake at exactly 30, it can't just be past 30
          driveBrake();

          printf2("If the robot is far off of 30 inches, consider changing the wheel size constants.");
          printf2("Multiply the actual travelled distance by %f to get the left wheel diameter", std::abs(2.0 / degToRad(left_tracker.get_position())));
          printf2("Multiply the actual travelled distance by %f to get the right wheel diameter", std::abs(2.0 / degToRad(right_tracker.get_position())));
        }
      }
    });

    back_wheel_size.setFunc([](){ //Back wheel size
      if(manual.on){
        if(GUI::prompt("Press, then move the robot 30 inches", "Place the robot's front against a known surface. Press the button, then slide the robot sideways exactly 30 inches.")){
          tracking.reset();

          if (GUI::prompt("Press when completed 30 inches")){
            printf2("The back tracking wheel's diameter is %f", 60.0 / degToRad(back_tracker.get_position()));
          }
        }
      }
      else{
        alert::start("I cannot strafe");
      }
    });

    spin360.setFunc([](){ //Robot turn accuracy
      if(manual.on){
        if(GUI::prompt("Press, then spin the robot", "Place the robot's against a known surface. Press the button, then spin the robot. Return it to the starting point. (Half turns are ok)")){
          flattenAgainstWallSync();
          tracking.reset();

          if(GUI::prompt("Press when back at start position")){
            printf2("The robot is %.2f inches %s and %.2f inches %s off the starting point.", std::abs(tracking.getPos().x), tracking.getPos().x > 0 ? "right" : "left", std::abs(tracking.getPos().y), tracking.getPos().y > 0 ? "forward" : "back");
            
            double turned = tracking.getPos().a / 180.0;
            double rots = round(turned);
            turned = 180 * (turned-rots);
            rots /= 2;

            double dist = std::abs(left_tracker.get_position() - right_tracker.get_position()) / (360.0 * rots);

            if(std::abs(turned) <= 0.05) printf2(term_colours::GREEN, "This seems pretty accurate. It's %.4f degrees off over %.1f rotations.", turned, rots);
            else if(turned > 0) printf2(term_colours::RED, "However, the robot gained %.2f degrees over %.1f rotations. Consider decreasing the DistanceLR to %.3f.", turned, rots, dist);
            else printf2(term_colours::RED, "However, the robot lost %.2f degrees over %.1f rotations. Consider increasing the DistanceLR to %.3f.", -turned, rots, dist);
          }
        }
      }
      else{
        if(GUI::prompt("Press, then spin the robot", "Place the robot's front against a known surface. Press the button, then clear the area.", 1500)){
          flattenAgainstWallSync();
          tracking.reset();
          moveDrive(-60.0, 0.0);
          tracking.waitForDistance(10);
          driveBrake();
          moveDrive(0.0, 80.0);
          WAIT_UNTIL(std::abs(tracking.getPos().a) > 3200);
          moveDrive(0.0, 40.0);
          WAIT_UNTIL(std::abs(tracking.getPos().a) > 3600);
          driveBrake();
          moveDrive(60.0, 0.0);
          tracking.waitForDistance(5);
          flattenAgainstWallSync();
          driveBrake();

          printf2("The robot is %.2f inches %s and %.2f inches %s off the starting point.", std::abs(tracking.getPos().x), tracking.getPos().x > 0 ? "right" : "left", std::abs(tracking.getPos().y), tracking.getPos().y > 0 ? "forward" : "back");
            
          double turned = tracking.getPos().a / 180.0;
          double rots = round(turned);
          turned = 180 * (turned-rots);
          rots /= 2;

          double dist = std::abs(left_tracker.get_position() - right_tracker.get_position()) / (360.0 * rots);

          if(std::abs(turned) <= 0.05) printf2(term_colours::GREEN, "This seems pretty accurate. It's %.4f degrees off over %.1f rotations.", turned, rots);
          else if(turned > 0) printf2(term_colours::RED, "However, the robot gained %.2f degrees over %.1f rotations. Consider decreasing the DistanceLR to %.3f.", turned, rots, dist);
          else printf2(term_colours::RED, "However, the robot lost %.2f degrees over %.1f rotations. Consider increasing the DistanceLR to %.3f.", -turned, rots, dist);
        }
      }
    });
}

void mainBackground(){
  //Saving Field coords
  int x = 200*tracking.getPos().x / 144, y = 200*tracking.getPos().y / 144;
  if(inRange(x, 0, 199) && inRange(y, 0, 199)) field[x].set(y); //Saves position (x, y) to as tracked

  for (_Motor* motor: _Motor::getList()) motor->updateTemperatureText();
}

void utilSetup(){
  //Encoders
    enc_set.setFunc([](){
      int port = expander_1.getValue();
      if(std::abs(port_1.getValue() - port_2.getValue()) != 1 || std::min(port_1.getValue(), port_2.getValue()) % 2 == 0){
        alert::start(term_colours::ERROR, "Invalid Ports %c%c", port_1.getValue() + 64, port_2.getValue() + 64);
        return;
      }
      if(port){
        if(c::registry_get_plugged_type(port-1) != c::E_DEVICE_ADI){
          alert::start(term_colours::ERROR, "No Expander in port %d", port);
          return;
        }
        ext_test_enc = c::ext_adi_encoder_init(port, port_1.getValue(), port_2.getValue(), false);
      }
      else test_enc = c::adi_encoder_init(port_1.getValue(), port_2.getValue(), false);
    });

    enc_res.setFunc([](){
      int port = expander_1.getValue();
      if(port){
        c::ext_adi_encoder_reset(ext_test_enc);
      }
      else c::adi_encoder_reset(test_enc);
    });

    encoders.setLoopFunc([](){
      if(expander_1.getValue()) enc_val = c::ext_adi_encoder_get(ext_test_enc);
      else enc_val = c::adi_encoder_get(test_enc);

      if(enc_val == std::numeric_limits<int32_t>::max()) enc_val = 0;
    });


  //Motor Control
    // mot_jam_detect.select();
    
    motor_ports = {
      std::make_tuple(std::numeric_limits<int>::max(), &mot_update_1, &mot_stop_1, &mot_text_1, 0, ""),
      std::make_tuple(std::numeric_limits<int>::max(), &mot_update_2, &mot_stop_2, &mot_text_2, 0, ""),
      std::make_tuple(std::numeric_limits<int>::max(), &mot_update_3, &mot_stop_3, &mot_text_3, 0, ""),
      std::make_tuple(std::numeric_limits<int>::max(), &mot_update_4, &mot_stop_4, &mot_text_4, 0, ""),
      std::make_tuple(std::numeric_limits<int>::max(), &mot_update_5, &mot_stop_5, &mot_text_5, 0, ""),
      std::make_tuple(std::numeric_limits<int>::max(), &mot_update_6, &mot_stop_6, &mot_text_6, 0, ""),
      std::make_tuple(std::numeric_limits<int>::max(), &mot_update_7, &mot_stop_7, &mot_text_7, 0, ""),
      std::make_tuple(std::numeric_limits<int>::max(), &mot_update_8, &mot_stop_8, &mot_text_8, 0, ""),
    };

    for (int port=0, i = 0; port<21; port++){
      if (c::registry_get_plugged_type(port) == c::E_DEVICE_MOTOR && i < 8){
        std::get<0>(motor_ports[i]) = port+1;
        i++;
      }
    }

    for (const auto& mot_arr: motor_ports){

      if (std::get<0>(mot_arr) != std::numeric_limits<int>::max()) motor_port_nums.append(std::to_string(std::get<0>(mot_arr)) + ", ");
      else{
        std::get<1>(mot_arr)->setActive(false);
        std::get<2>(mot_arr)->setActive(false);
        std::get<3>(mot_arr)->setActive(false);
      }
      std::get<1>(mot_arr)->setFunc([&](){c::motor_move(std::get<0>(mot_arr), mot_speed.getValue());});
      std::get<2>(mot_arr)->setFunc([&](){c::motor_move(std::get<0>(mot_arr), 0);});
    }

    if (motor_port_nums.back() == ',') motor_port_nums.pop_back();


    motor.setLoopFunc([](){
      for (auto& tup: motor_ports){
        int port = std::get<0>(tup);
        Button* run_btn = std::get<1>(tup);
        Button* stop_btn = std::get<2>(tup);

        if (port != std::numeric_limits<int>::max()){ //? Works without this safety check
          switch(static_cast<int>(c::motor_get_temperature(port))){
            case 0:
            case 5:
              run_btn->setBackground(Color::white);
              stop_btn->setBackground(Color::white);
              break;
            case 10:
            case 15:
              run_btn->setBackground(Color::blue);
              stop_btn->setBackground(Color::blue);
              break;
            case 20:
              run_btn->setBackground(Color::dodger_blue);
              stop_btn->setBackground(Color::dodger_blue);
              break;
            case 25:
              run_btn->setBackground(Color::turquoise);
              stop_btn->setBackground(Color::turquoise);
            case 30:
              run_btn->setBackground(Color::medium_sea_green);
              stop_btn->setBackground(Color::medium_sea_green);
            case 35:
              run_btn->setBackground(Color::lawn_green);
              stop_btn->setBackground(Color::lawn_green);
              break;
            case 40:
              run_btn->setBackground(Color::lime_green);
              stop_btn->setBackground(Color::lime_green);
            case 45:
              run_btn->setBackground(Color::yellow);
              stop_btn->setBackground(Color::yellow);
              break;
            case 50:
              run_btn->setBackground(Color::orange_red);
              stop_btn->setBackground(Color::orange_red);
              break;
            case 55:
              run_btn->setBackground(Color::red);
              stop_btn->setBackground(Color::red);
              break;
            default:
              run_btn->setBackground(Color(rand()));
              stop_btn->setBackground(Color(rand()));
              break;
          }
        }
      }
    });


  //Pneumatic Control
    exp_pneum_btns = {&ADI_a, &ADI_b, &ADI_c, &ADI_d, &ADI_e, &ADI_f, &ADI_g, &ADI_h};

    for (int port=0; port<21; port++){
      if (c::registry_get_plugged_type(port) == c::E_DEVICE_ADI) expander_ports[port] = port+1;
      else expander_ports[port] = std::numeric_limits<int>::max();
    }

    for (int port: expander_ports){
      if (port != std::numeric_limits<int>::max()) expander_port_nums.append(std::to_string(port) + ", ");
    }

    if (expander_port_nums.back() == ',') expander_port_nums.pop_back();

    for (int i = 1; i <= 8; i++){
      if(c::adi_port_get_config(i) != ADI_DIGITAL_IN && c::adi_port_get_config(i) != ADI_DIGITAL_OUT){
        no_pneumatic_port_nums.push_back(char(i + 64));
        no_pneumatic_port_nums.push_back(',');
      }
      exp_pneum_btns[i-1]->setFunc([i](){
        int port = expander.getValue();
        if (port){
          if(c::registry_get_plugged_type(port-1) != c::E_DEVICE_ADI){
            alert::start(term_colours::ERROR, "No Expander in port %d", port);
            return;
          }
          c::ext_adi_port_set_config(port, i, ADI_DIGITAL_OUT);
          c::ext_adi_port_set_value(port, i, HIGH);
        }
        else{
          c::adi_port_set_config(i, ADI_DIGITAL_OUT);
          c::adi_port_set_value(i, HIGH);
        }
      });
      exp_pneum_btns[i-1]->setOffFunc([i](){
        int port = expander.getValue();
        if (port){
          if(c::registry_get_plugged_type(port-1) != c::E_DEVICE_ADI){
            alert::start(term_colours::ERROR, "No Expander in port %d", port);
            return;
          }
          c::ext_adi_port_set_config(port, i, ADI_DIGITAL_OUT);
          c::ext_adi_port_set_value(port, i, LOW);
        }
        else{
          c::adi_port_set_config(i, ADI_DIGITAL_OUT);
          c::adi_port_set_value(i, LOW);
        }
      });
    }

    if (no_pneumatic_port_nums.back() == ',') no_pneumatic_port_nums.pop_back();
}

void utilBackground(){
  //Motor Stalled
  for (auto& mot_tup: motor_ports){
    int port = std::get<0>(mot_tup);
    int& stall_count = std::get<4>(mot_tup);
    if (port != std::numeric_limits<int>::max()){
      int cur = c::motor_get_actual_velocity(port), target = c::motor_get_target_velocity(port), temp = c::motor_get_temperature(port);

      std::get<5>(mot_tup) = sprintf2("%d: %d", port, cur);
      if(mot_jam_detect.on){
        if (std::abs(target-cur) > 5) stall_count++;
        else stall_count = 0;
        if (stall_count >= mapValues(std::abs(target), 0, 250, 40, 2)){
          stall_count = 0;
          alert::start(5000, "Motor %d Jammed\n", port);
          c::motor_move(port, 0);
        }
      }

      if (target && temp > 40){
        alert::start(5000, "Motor %d Overheated to %d\n", port, temp);
        c::motor_move(port, 0);
      }
    }
  }
}

GUI main_obj ({&temps, &checks, &track, &moving, &tuning, &systems, &motors, &pneumatics, &logging}, &mainSetup, &mainBackground);

GUI util_obj ({&ports, &encoders, &motor, &pneumatic}, &utilSetup, &utilBackground);
