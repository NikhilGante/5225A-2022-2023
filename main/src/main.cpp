#include "main.h"
#include "Libraries/async.hpp"
#include "Libraries/geometry.hpp"
#include "Libraries/pid.hpp"
#include "Libraries/piston.hpp"
#include "Libraries/task.hpp"
#include "Libraries/timer.hpp"
#include "Libraries/state.hpp"

#include "lift.hpp"

#include "config.hpp"


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {

}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
struct MoveToTargetParams{
	// replaces default constructor
	MoveToTargetParams(Position target,	bool brake,	int max_power = 127,	int exit_power = 0,	bool overshoot = false, double end_error_r = 0.5,	double end_error_a = 5.0):
		target(target), brake(brake), max_power(max_power), exit_power(exit_power), overshoot(overshoot), end_error_r(end_error_r), end_error_a(end_error_a)
		{}
  Position target;
  bool brake;
  int max_power = 127;
  int exit_power = 0;
  bool overshoot = false;
  double end_error_r = 0.5;
  double end_error_a = 5.0;
};

void moveDrive(double y, double a){
  front_l.move(y+a);
  front_r.move(y-a);
  back_l.move(y+a);
  back_r.move(y-a);
  center_l.move(y+a);
  center_r.move(y-a);
}

void moveToTargetFn(void* params){
  MoveToTargetParams* args_ptr = (MoveToTargetParams*)_Task::get_params(params); // stores args locally
	MoveToTargetParams args = *args_ptr;

  delete args_ptr;  // frees memory
  args_ptr = nullptr; // params shouldn't point to anything

  printf("started mtt, %lf %lf %lf, %d\n", args.target.x, args.target.y, args.target.angle, args.max_power);
	moveDrive(args.max_power, 0);
	while(true){
		delay(10);
	}
}

struct turnParams{
  double turn_power;
	string ignore;
};


void turnFn(void* params){
  turnParams* args_ptr = (turnParams*)_Task::get_params(params); // stores args locally
	turnParams args = *args_ptr;

  delete args_ptr;  // frees memory
  args_ptr = nullptr; // params shouldn't point to anything

  printf("started turnFn, %lf %s\n", args.turn_power, args.ignore.c_str());
	moveDrive(0, args.turn_power);
	while(true){
		delay(10);
	}
}

enum class angler_states{
	idle,
	top,
	bottom
};

class Angler: public Subsystem<angler_states>{
public:
	Angler(): Subsystem<angler_states>("angler", angler_states::idle, std::unordered_map<angler_states, const char*>{
		{angler_states::idle, "idle"},
		{angler_states::top, "top"},
		{angler_states::bottom, "bottom"},
	})
	{}
	void handle(){

	}
	void handle_state_change(angler_states state){

	}
};

Angler angler;

void opcontrol() {
	angler.start_task();
	while(true){
		master.update_buttons();
		if(master.is_rising(DIGITAL_R1) && lift_index < 4){
			liftMoveToTarget.async(++lift_index);
			printf("index:%d\n", lift_index);
		}
		if(master.is_rising(DIGITAL_R2) && lift_index > 0){
			liftMoveToTarget.async(--lift_index);
			printf("index:%d\n", lift_index);
		}
		printf("hi%d\n", millis());

		// printf("index:%d\n", lift_index);
		// printf("state:%d\n", lift_task.task_ptr->get_state());
		delay(10);
	}
	_Task move_task(moveToTargetFn);

	AsyncObj<MoveToTargetParams> moveToTarget("moveToTarget", moveToTargetFn, move_task);
	AsyncObj<turnParams> turn("turn", turnFn, move_task);
// {}, false, -30
	moveToTarget.async({{3.52,35.3,224.5}, false});
	delay(500);
	turn.async({60.32, "poggers"});

}
