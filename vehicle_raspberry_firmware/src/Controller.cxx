#include "Controller.hpp"
#include <iostream>
#include "cpm/Parameter.hpp"
#include "cpm/Logging.hpp"


template<typename T>
std::unique_ptr<cpm::Reader<T>> make_reader(std::string name, uint8_t vehicle_id)
{
    cpm::VehicleIDFilteredTopic<T> topic(cpm::get_topic<T>(name), vehicle_id);
    return std::unique_ptr<cpm::Reader<T>>(new cpm::Reader<T>(topic));
}

Controller::Controller(uint8_t vehicle_id, std::function<uint64_t()> _get_time)
:mpcController(vehicle_id)
,m_get_time(_get_time)
{
    reader_CommandDirect = make_reader<VehicleCommandDirect>("vehicleCommandDirect", vehicle_id);
    reader_CommandSpeedCurvature = make_reader<VehicleCommandSpeedCurvature>("vehicleCommandSpeedCurvature", vehicle_id);

    asyncReader_vehicleCommandTrajectory = std::unique_ptr< cpm::AsyncReader<VehicleCommandTrajectory> >
    (
        new cpm::AsyncReader<VehicleCommandTrajectory>
        (
            [this](dds::sub::LoanedSamples<VehicleCommandTrajectory>& samples)
            {
                reveice_trajectory_callback(samples);
            }, 
            cpm::ParticipantSingleton::Instance(), 
            cpm::VehicleIDFilteredTopic<VehicleCommandTrajectory>(cpm::get_topic<VehicleCommandTrajectory>("vehicleCommandTrajectory"), vehicle_id)
        )
    );
}

void Controller::reveice_trajectory_callback(dds::sub::LoanedSamples<VehicleCommandTrajectory>& samples)
{
    std::lock_guard<std::mutex> lock(command_receive_mutex);
    for(auto sample : samples) 
    {
        if(sample.info().valid())
        {
            auto dds_trajectory_points = sample.data().trajectory_points();                                    
            for(auto trajectory_point : dds_trajectory_points) 
            {
                m_trajectory_points[trajectory_point.t().nanoseconds()] = trajectory_point;
            }
            state = ControllerState::Trajectory;
            latest_command_receive_time = m_get_time();
        }
    }

    // Erase trajectory points which are older than 1 second
    const uint64_t past_threshold_time = latest_command_receive_time - 1000000000ull;
    auto last_valid_it = m_trajectory_points.upper_bound(past_threshold_time);
    m_trajectory_points.erase(m_trajectory_points.begin(), last_valid_it);
}


void Controller::update_vehicle_state(VehicleState vehicleState) 
{
    m_vehicleState = vehicleState;
}


void Controller::receive_commands(uint64_t t_now)
{
    std::lock_guard<std::mutex> lock(command_receive_mutex);

    VehicleCommandDirect sample_CommandDirect;
    uint64_t sample_CommandDirect_age;

    VehicleCommandSpeedCurvature sample_CommandSpeedCurvature;
    uint64_t sample_CommandSpeedCurvature_age;

    reader_CommandDirect->get_sample(t_now, sample_CommandDirect, sample_CommandDirect_age);
    reader_CommandSpeedCurvature->get_sample(t_now, sample_CommandSpeedCurvature, sample_CommandSpeedCurvature_age);


    if(sample_CommandDirect_age < command_timeout)
    {
        m_vehicleCommandDirect = sample_CommandDirect;
        state = ControllerState::Direct;
        latest_command_receive_time = t_now;
    }
    else if(sample_CommandSpeedCurvature_age < command_timeout)
    {
        m_vehicleCommandSpeedCurvature = sample_CommandSpeedCurvature;  
        state = ControllerState::SpeedCurvature;
        latest_command_receive_time = t_now;
    }
}

double Controller::speed_controller(const double speed_measured, const double speed_target) 
{

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


void Controller::update_remote_parameters()
{
    //trajectory_controller_lateral_P_gain = cpm::parameter_double("trajectory_controller/lateral_P_gain");
    //trajectory_controller_lateral_D_gain = cpm::parameter_double("trajectory_controller/lateral_D_gain");
}


std::shared_ptr<TrajectoryInterpolation> Controller::interpolate_trajectory_command(uint64_t t_now)
{
    // Find active segment
    auto iterator_segment_end = m_trajectory_points.lower_bound(t_now);
    if(iterator_segment_end != m_trajectory_points.end()
        && iterator_segment_end != m_trajectory_points.begin()) 
    {
        auto iterator_segment_start = iterator_segment_end;
        iterator_segment_start--;
        TrajectoryPoint start_point = (*iterator_segment_start).second;
        TrajectoryPoint end_point = (*iterator_segment_end).second;
        assert(t_now >= start_point.t().nanoseconds());
        assert(t_now <= end_point.t().nanoseconds());

        // We have a valid trajectory segment.
        // Interpolate for the current time.
        return std::make_shared<TrajectoryInterpolation>(t_now, start_point, end_point);
    }
    return nullptr;
}


void Controller::trajectory_controller_linear(uint64_t t_now, double &motor_throttle_out, double &steering_servo_out)
{
    std::shared_ptr<TrajectoryInterpolation> trajectory_interpolation = 
        interpolate_trajectory_command(t_now);

    if(trajectory_interpolation) 
    {
        const double x_ref = trajectory_interpolation->position_x;
        const double y_ref = trajectory_interpolation->position_y;
        const double yaw_ref = trajectory_interpolation->yaw;

        const double x = m_vehicleState.pose().x();
        const double y = m_vehicleState.pose().y();
        const double yaw = m_vehicleState.pose().yaw();

        double longitudinal_error =  cos(yaw_ref) * (x-x_ref)  + sin(yaw_ref) * (y-y_ref);
        double lateral_error      = -sin(yaw_ref) * (x-x_ref)  + cos(yaw_ref) * (y-y_ref);
        const double yaw_error = sin(yaw - yaw_ref);



        if(fabs(lateral_error) < 0.8 && fabs(longitudinal_error) < 0.8 && fabs(yaw_error) < 0.7)
        {
            // Linear lateral controller
            const double ref_curvature = fmin(0.5,fmax(-0.5,trajectory_interpolation->curvature));
            //const double ref_curvature = trajectory_interpolation->curvature;
            const double curvature = ref_curvature 
                - trajectory_controller_lateral_P_gain * lateral_error 
                - trajectory_controller_lateral_D_gain * yaw_error;

            // Linear longitudinal controller
            const double speed_target = trajectory_interpolation->speed - 0.5 * longitudinal_error;

            const double speed_measured = m_vehicleState.speed();
            steering_servo_out = steering_curvature_calibration(curvature);
            motor_throttle_out = speed_controller(speed_measured, speed_target);


            /*std::cout << 
            "lateral_error " << lateral_error << "  " << 
            "longitudinal_error " << longitudinal_error << "  " << 
            "yaw_error " << yaw_error << "  " << 
            "ref_curvature " << trajectory_interpolation->curvature << "  " << 
            "curvature_cmd " << curvature << "  " << 
            std::endl;*/
        }
    }
}

static inline double square(double x) {return x*x;}

void Controller::trajectory_tracking_statistics_update(uint64_t t_now)
{
    std::shared_ptr<TrajectoryInterpolation> trajectory_interpolation = 
        interpolate_trajectory_command(t_now);

    if(trajectory_interpolation) 
    {
        const double x_ref = trajectory_interpolation->position_x;
        const double y_ref = trajectory_interpolation->position_y;
        const double yaw_ref = trajectory_interpolation->yaw;

        const double x = m_vehicleState.pose().x();
        const double y = m_vehicleState.pose().y();
        const double yaw = m_vehicleState.pose().yaw();

        double longitudinal_error =  cos(yaw_ref) * (x-x_ref)  + sin(yaw_ref) * (y-y_ref);
        double lateral_error      = -sin(yaw_ref) * (x-x_ref)  + cos(yaw_ref) * (y-y_ref);

        trajectory_tracking_statistics_longitudinal_errors[trajectory_tracking_statistics_index] = longitudinal_error;
        trajectory_tracking_statistics_lateral_errors     [trajectory_tracking_statistics_index] = lateral_error;

        trajectory_tracking_statistics_index = (trajectory_tracking_statistics_index+1) 
                                                % TRAJECTORY_TRACKING_STATISTICS_BUFFER_SIZE;

        if(trajectory_tracking_statistics_index == 0)
        {
            // output summary

            double longitudinal_error_sum = 0;
            double lateral_error_sum = 0;
            double longitudinal_error_max = 0;
            double lateral_error_max = 0;

            for (int i = 0; i < TRAJECTORY_TRACKING_STATISTICS_BUFFER_SIZE; ++i)
            {
                longitudinal_error_sum += trajectory_tracking_statistics_longitudinal_errors[i];
                lateral_error_sum += trajectory_tracking_statistics_lateral_errors[i];

                longitudinal_error_max = fmax(longitudinal_error_max, fabs(trajectory_tracking_statistics_longitudinal_errors[i]));
                lateral_error_max = fmax(lateral_error_max, fabs(trajectory_tracking_statistics_lateral_errors[i]));
            }

            const double longitudinal_error_mean = longitudinal_error_sum / TRAJECTORY_TRACKING_STATISTICS_BUFFER_SIZE;
            const double lateral_error_mean = lateral_error_sum / TRAJECTORY_TRACKING_STATISTICS_BUFFER_SIZE;


            double longitudinal_error_variance_sum = 0;
            double lateral_error_variance_sum = 0;

            for (int i = 0; i < TRAJECTORY_TRACKING_STATISTICS_BUFFER_SIZE; ++i)
            {
                longitudinal_error_variance_sum += square(trajectory_tracking_statistics_longitudinal_errors[i] - longitudinal_error_mean);
                lateral_error_variance_sum += square(trajectory_tracking_statistics_lateral_errors[i] - lateral_error_mean);
            }


            const double longitudinal_error_variance = longitudinal_error_variance_sum / (TRAJECTORY_TRACKING_STATISTICS_BUFFER_SIZE - 1);
            const double lateral_error_variance = lateral_error_variance_sum / (TRAJECTORY_TRACKING_STATISTICS_BUFFER_SIZE - 1);

            const double longitudinal_error_std = sqrt(longitudinal_error_variance);
            const double lateral_error_std = sqrt(lateral_error_variance);

            cpm::Logging::Instance().write(
                "Vehicle Controller Tracking Errors:"
                "long,mean: %f  "
                "long,std: %f  "
                "long,max: %f  "
                "lat,mean: %f  "
                "lat,std: %f  "
                "lat,max: %f  ",
                longitudinal_error_mean, 
                longitudinal_error_std,
                longitudinal_error_max,
                lateral_error_mean,
                lateral_error_std,
                lateral_error_max
            );
        }
    }
}

void Controller::get_control_signals(uint64_t t_now, double &motor_throttle, double &steering_servo) 
{
    receive_commands(t_now);

    update_remote_parameters();

    if(latest_command_receive_time + command_timeout < t_now
        && state != ControllerState::Stop)
    {
        cpm::Logging::Instance().write(
            "Warning: Vehicle Controller: "
            "No new commands received. Stopping.");
        state = ControllerState::Stop;
    }

    if(m_vehicleState.IPS_update_age_nanoseconds() > 3000000000ull 
        && state == ControllerState::Trajectory)
    {
        cpm::Logging::Instance().write(
            "Warning: Vehicle Controller: "
            "Lost IPS position reference. Stopping.");
        state = ControllerState::Stop;
    }

    if(state == ControllerState::Stop) 
    {
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
            std::lock_guard<std::mutex> lock(command_receive_mutex);
            trajectory_tracking_statistics_update(t_now);

            // Run controller
            mpcController.update(
                t_now, m_vehicleState, m_trajectory_points,
                motor_throttle, steering_servo);
            //trajectory_controller_linear(t_now, motor_throttle, steering_servo);
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

