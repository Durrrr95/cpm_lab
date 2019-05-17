#include "Controller.hpp"
#include <iostream>
#include "TrajectoryInterpolation.hpp"


void Controller::update_vehicle_state(VehicleState vehicleState) {
    m_vehicleState = vehicleState;
}

void Controller::update_command(VehicleCommandDirect vehicleCommand)
{
    m_vehicleCommandDirect = vehicleCommand;
    state = ControllerState::Direct;
}

void Controller::update_command(VehicleCommandSpeedCurvature vehicleCommand)
{
    m_vehicleCommandSpeedCurvature = vehicleCommand;
    state = ControllerState::SpeedCurvature;
}

void Controller::update_command(VehicleCommandTrajectory vehicleCommand)
{
    m_vehicleCommandTrajectory = vehicleCommand;
    state = ControllerState::Trajectory;
}

void Controller::vehicle_emergency_stop() 
{
    state = ControllerState::Stop;
}

double Controller::speed_controller(const double speed_measured, const double speed_target) {

    // steady-state curve, from curve fitting
    double motor_throttle = ((speed_target>=0)?(1.0):(-1.0)) * pow(fabs(0.152744 * speed_target),(0.627910));

    const double speed_error = speed_target - speed_measured;

    // PI controller for the speed
    const double integral_gain = 0.01;
    const double proportional_gain = 0.3;
    speed_throttle_error_integral += integral_gain * speed_error;

    speed_throttle_error_integral = fmin(0.5, fmax(-0.5, speed_throttle_error_integral)); // integral clamping

    motor_throttle += speed_throttle_error_integral;
    motor_throttle += proportional_gain * speed_error;
    return motor_throttle;
}


double steering_curvature_calibration(double curvature) 
{
    // steady-state curve, from curve fitting
    double steering_servo = (0.241857) * curvature;
    return steering_servo;
}


void Controller::get_control_signals(uint64_t stamp_now, double &motor_throttle, double &steering_servo) {

    if(state == ControllerState::Stop) {
        motor_throttle = 0;
        steering_servo = 0;
        return;
    }

    switch(state) {        

        case ControllerState::SpeedCurvature:
        {
            const double speed_target = m_vehicleCommandSpeedCurvature.speed();
            const double curvature    = m_vehicleCommandSpeedCurvature.curvature();
            const double speed_measured = m_vehicleState.speed();

            steering_servo = steering_curvature_calibration(curvature);
            motor_throttle = speed_controller(speed_measured, speed_target);
        }
        break;

        case ControllerState::Trajectory:
        {
            for(TrajectoryPoint trajectory_point : m_vehicleCommandTrajectory.trajectory_points()) {
                trajectory_points[trajectory_point.t().nanoseconds()] = trajectory_point;
            }            

            //std::cout << "trajectory_point stamp: " << trajectory_point.t().nanoseconds()
            //    << " --- total nodes: " << trajectory_points.size() << std::endl;

            // Find active segment
            auto iterator_segment_end = trajectory_points.lower_bound(stamp_now);
            if(iterator_segment_end != trajectory_points.end()
                && iterator_segment_end != trajectory_points.begin()) 
            {
                auto iterator_segment_start = iterator_segment_end;
                iterator_segment_start--;
                TrajectoryPoint start_point = (*iterator_segment_start).second;
                TrajectoryPoint end_point = (*iterator_segment_end).second;
                assert(stamp_now >= start_point.t().nanoseconds());
                assert(stamp_now <= end_point.t().nanoseconds());

                // We have a valid trajectory segment.
                // Interpolate for the current time.
                TrajectoryInterpolation trajectory_interpolation(stamp_now, start_point, end_point);


                const double x_ref = trajectory_interpolation.position_x;
                const double y_ref = trajectory_interpolation.position_y;
                const double yaw_ref = trajectory_interpolation.yaw;

                const double x = m_vehicleState.pose().x();
                const double y = m_vehicleState.pose().y();
                const double yaw = m_vehicleState.pose().yaw();

                double longitudinal_error =  cos(yaw_ref) * (x-x_ref)  + sin(yaw_ref) * (y-y_ref);
                double lateral_error      = -sin(yaw_ref) * (x-x_ref)  + cos(yaw_ref) * (y-y_ref);
                const double yaw_error = sin(yaw - yaw_ref);



                if(fabs(lateral_error) < 0.8 && fabs(longitudinal_error) < 0.8 && fabs(yaw_error) < 0.7)
                {
                    // Linear lateral controller
                    const double ref_curvature = fmin(0.5,fmax(-0.5,trajectory_interpolation.curvature));
                    //const double ref_curvature = trajectory_interpolation.curvature;
                    const double curvature = ref_curvature - 7.0 * lateral_error - 4.0 * yaw_error;

                    // Linear longitudinal controller
                    const double speed_target = trajectory_interpolation.speed - 0.5 * longitudinal_error;

                    const double speed_measured = m_vehicleState.speed();
                    steering_servo = steering_curvature_calibration(curvature);
                    motor_throttle = speed_controller(speed_measured, speed_target);


                    std::cout << 
                    "lateral_error " << lateral_error << "  " << 
                    "longitudinal_error " << longitudinal_error << "  " << 
                    "yaw_error " << yaw_error << "  " << 
                    "ref_curvature " << trajectory_interpolation.curvature << "  " << 
                    "curvature_cmd " << curvature << "  " << 
                    std::endl;
                }
            }
        }
        break;

        default: // Direct
        {
            motor_throttle = m_vehicleCommandDirect.motor_throttle();
            steering_servo = m_vehicleCommandDirect.steering_servo();
        }
        break;
    }

    motor_throttle = fmax(-1.0, fmin(1.0, motor_throttle));
    steering_servo = fmax(-1.0, fmin(1.0, steering_servo));
}


