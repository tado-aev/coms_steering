#ifndef BRAKE_CONTROLLER_H_
#define BRAKE_CONTROLLER_H_

#include <serial/serial.h>

#include <cstdio>
#include <string>
#include <utility>

class SteeringController {
public:
    using pulse_t = long long int;

    static const unsigned READ_WRITE_TIMEOUT_MS = 250;

    // Default speed for Cool Muscle (pulse/s)
    static const int DEFAULT_S = 40;
    // Default acceleration for Cool Muscle (pulse/s^2)
    static const int DEFAULT_A = 50;
    // Default torque for Cool Muscle
    static const int DEFAULT_M = 20;

    /* Constructors, Destructor, and Assignment operators {{{ */
    SteeringController(const std::string& port, const unsigned baudrate,
                                       const std::pair<double, pulse_t>& limit_ccw,
                                       const std::pair<double, pulse_t>& limit_cw,
                                       const pulse_t origin_offset = 0);

    // Copy constructor
    SteeringController(const SteeringController& other) = delete;

    // Move constructor
    SteeringController(SteeringController&& other) = delete;

    // Destructor
    ~SteeringController();

    // Assignment operator
    SteeringController&
    operator=(const SteeringController& other) = delete;

    // Move assignment operator
    SteeringController&
    operator=(SteeringController&& other) = delete;
    /* }}} */

    void
    connect();

    void
    init();

    void
    on();

    void
    off();

    /**
     * Rotates the steering wheel to an angle at the given angular velocity
     *
     * 0 rad is when the steering is at the origin (straight). Limits in the
     * CW and CCW direction is determined by the limit_plus_ccw and
     * limit_minus_cw member variables.
     *
     * Also note that positive angles are in the CCW direction, and negative
     * angles are in the CW direction. Sign of the angular velocity is
     * ignored.
     *
     * \param[in] ang the target angle in radians
     *
     * \param[in] ang_vel the target angular velocity in rad/s
     *
     * \param[in] ang_acc the target angular acceleration in rad/s^2
     */
    void
    set(const double ang, const double ang_vel, const double ang_acc);

    /**
     * Uses the default acceleration
     */
    void
    set(const double ang, const double ang_vel);

    /**
     * Uses the default velocity and acceleration
     */
    void
    set(const double ang);

    /**
     * Sends the emergency stop command to the actuator
     */
    void
    emergency();

    /**
     * Sends the release emergency command to the actuator
     */
    void
    release_emergency();

    /**
     * Returns the raw pulse count obtained from the actuator
     */
    pulse_t
    get_pulse_count();

    /**
     * Returns the current angle of the steering wheel in radians
     *
     * 0 is when the steering wheel is at the center, positive angles are in
     * the CCW direction, and negative angles are in the CW direction.
     */
    double
    get_rad();

    /**
     * Sets the port to communicate through
     */
    void
    set_port(const std::string& port);

    /**
     * Sets the baud rate for the Cool Muscle actuator
     */
    void
    set_baudrate(const unsigned baudrate);

private:
    // Cool Muscle actuator for braking
    serial::Serial to_cool_muscle;

    // Rotation limits in [radians, pulse counts]
    std::pair<double, pulse_t> limit_plus_ccw;
    std::pair<double, pulse_t> limit_minus_cw;

    // Offset distance between mechanical and electrical origins
    pulse_t origin_offset;

    /**
     * Converts pulse count to radians
     */
    double
    pulse2rad(const pulse_t pulse);

    /**
     * Converts radians to pulse counts
     */
    pulse_t
    rad2pulse(const double rad);

    /**
     * Writes a line to the cool muscle after appending a CRLF
     */
    void
    writeline(const std::string& line);

    /**
     * Writes a line to the cool muscle after appending a CRLF
     *
     * The line written is in the format: "{line}{val}\r\n"
     */
    template<typename T>
    inline
    void
    writeline(const std::string& line, const T val) {
        writeline(line + std::to_string(val));
    }

    /**
     * Reads one line from the cool muscle
     *
     * Given an argument, populates that variable according to the format.
     * This method loops and keeps on reading line-by-line until a string with
     * the given format is matched and the variable is populated.
     *
     * \param[in] fmt format of the line. Same as what you give to scanf
     *
     * \param[in] var variable to be populated
     */
    template<typename T>
    void
    readline(const std::string& fmt, T& var);
};

#endif /* end of include guard */
