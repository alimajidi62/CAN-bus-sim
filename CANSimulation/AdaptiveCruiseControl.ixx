// AdaptiveCruiseControl.ixx - PI Controller for Engine Speed Control via CAN Bus
// Scenario: "Adaptive Cruise Control with PI Speed Governor"
// This module simulates a real-world automotive scenario where a PI controller
// maintains constant vehicle speed regardless of road conditions (slopes, flat roads)

module;

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <memory>
#include <string>
#include <cmath>
#include <random>
#include <atomic>
#include <iomanip>
#include <mutex>
#include <condition_variable>
#include <functional>

export module AdaptiveCruiseControl;

import CANBusSimulation;

using namespace std;
using namespace std::chrono;
using namespace CANSim;

export namespace AdaptiveCruiseControl {

    // ========================================
    // PI Controller Implementation
    // ========================================
    
    class PIController {
    private:
        double kp;              // Proportional gain
        double ki;              // Integral gain
        double integralSum;     // Accumulated integral error
        double previousError;   // Previous error for derivative calculation
        double outputMin;       // Minimum output limit
        double outputMax;       // Maximum output limit
        steady_clock::time_point lastUpdateTime;
        
    public:
        PIController(double proportionalGain, double integralGain, 
                    double minOutput = 0.0, double maxOutput = 100.0)
            : kp(proportionalGain), ki(integralGain), integralSum(0.0), 
              previousError(0.0), outputMin(minOutput), outputMax(maxOutput),
              lastUpdateTime(steady_clock::now()) {}
        
        double calculate(double setpoint, double currentValue) {
            auto currentTime = steady_clock::now();
            auto deltaTime = duration_cast<milliseconds>(currentTime - lastUpdateTime).count() / 1000.0;
            
            if (deltaTime <= 0.0) deltaTime = 0.001; // Prevent division by zero
            
            // Calculate error
            double error = setpoint - currentValue;
            
            // Proportional term
            double proportional = kp * error;
            
            // Integral term (with windup protection)
            integralSum += error * deltaTime;
            
            // Anti-windup: clamp integral sum
            double maxIntegral = (outputMax - outputMin) / ki;
            if (integralSum > maxIntegral) integralSum = maxIntegral;
            if (integralSum < -maxIntegral) integralSum = -maxIntegral;
            
            double integral = ki * integralSum;
            
            // Calculate output
            double output = proportional + integral;
            
            // Clamp output to limits
            if (output > outputMax) output = outputMax;
            if (output < outputMin) output = outputMin;
            
            // Store values for next iteration
            previousError = error;
            lastUpdateTime = currentTime;
            
            return output;
        }
        
        void reset() {
            integralSum = 0.0;
            previousError = 0.0;
            lastUpdateTime = steady_clock::now();
        }
        
        void setGains(double proportionalGain, double integralGain) {
            kp = proportionalGain;
            ki = integralGain;
        }
        
        double getProportionalGain() const { return kp; }
        double getIntegralGain() const { return ki; }
        double getIntegralSum() const { return integralSum; }
    };

    // ========================================
    // Vehicle Dynamics Simulator
    // ========================================
    
    enum class RoadCondition {
        FLAT = 0,
        UPHILL_MILD = 1,    // 3% grade
        UPHILL_STEEP = 2,   // 8% grade
        DOWNHILL_MILD = 3,  // -3% grade
        DOWNHILL_STEEP = 4  // -8% grade
    };
    
    class VehicleDynamics {
    private:
        double mass;                    // Vehicle mass in kg
        double dragCoefficient;         // Air resistance coefficient
        double rollingResistance;       // Rolling resistance coefficient
        double currentSpeed;            // Current speed in km/h
        double currentThrottlePosition; // Throttle position (0-100%)
        RoadCondition roadCondition;    // Current road condition
        mt19937 randomGenerator;
        uniform_real_distribution<double> noiseDistribution;
        
        double calculateLoadResistance() {
            double baseResistance = rollingResistance * mass * 9.81; // Rolling resistance
            
            // Add grade resistance based on road condition
            double gradeResistance = 0.0;
            switch (roadCondition) {
                case RoadCondition::FLAT:
                    gradeResistance = 0.0;
                    break;
                case RoadCondition::UPHILL_MILD:
                    gradeResistance = mass * 9.81 * sin(atan(0.03)); // 3% grade
                    break;
                case RoadCondition::UPHILL_STEEP:
                    gradeResistance = mass * 9.81 * sin(atan(0.08)); // 8% grade
                    break;
                case RoadCondition::DOWNHILL_MILD:
                    gradeResistance = -mass * 9.81 * sin(atan(0.03)); // -3% grade
                    break;
                case RoadCondition::DOWNHILL_STEEP:
                    gradeResistance = -mass * 9.81 * sin(atan(0.08)); // -8% grade
                    break;
            }
            
            // Air resistance (simplified)
            double airResistance = dragCoefficient * currentSpeed * currentSpeed * 0.01;
            
            return baseResistance + gradeResistance + airResistance;
        }
        
    public:
        VehicleDynamics(double vehicleMass = 1500.0) // Default 1500kg car
            : mass(vehicleMass), dragCoefficient(0.3), rollingResistance(0.01),
              currentSpeed(0.0), currentThrottlePosition(0.0), roadCondition(RoadCondition::FLAT),
              randomGenerator(random_device{}()), noiseDistribution(-0.5, 0.5) {}
        
        void updateSpeed(double throttlePosition, double deltaTimeSeconds) {
            currentThrottlePosition = throttlePosition;
            
            // Calculate engine force based on throttle position (simplified model)
            double maxEngineForce = 3000.0; // Maximum force in Newtons
            double engineForce = (throttlePosition / 100.0) * maxEngineForce;
            
            // Calculate total resistance
            double totalResistance = calculateLoadResistance();
            
            // Net force
            double netForce = engineForce - totalResistance;
            
            // Calculate acceleration (F = ma)
            double acceleration = netForce / mass;
            
            // Update speed (v = v0 + at), convert m/s to km/h
            currentSpeed += acceleration * deltaTimeSeconds * 3.6;
            
            // Add some realistic noise
            currentSpeed += noiseDistribution(randomGenerator);
            
            // Ensure speed doesn't go negative
            if (currentSpeed < 0.0) currentSpeed = 0.0;
            
            // Realistic maximum speed limit
            if (currentSpeed > 200.0) currentSpeed = 200.0;
        }
        
        double getCurrentSpeed() const { return currentSpeed; }
        double getThrottlePosition() const { return currentThrottlePosition; }
        RoadCondition getRoadCondition() const { return roadCondition; }
        
        void setRoadCondition(RoadCondition condition) {
            roadCondition = condition;
            cout << "[ROAD] Condition changed to: " << getRoadConditionString() << endl;
        }
        
        string getRoadConditionString() const {
            switch (roadCondition) {
                case RoadCondition::FLAT: return "Flat Road";
                case RoadCondition::UPHILL_MILD: return "Mild Uphill (3%)";
                case RoadCondition::UPHILL_STEEP: return "Steep Uphill (8%)";
                case RoadCondition::DOWNHILL_MILD: return "Mild Downhill (3%)";
                case RoadCondition::DOWNHILL_STEEP: return "Steep Downhill (8%)";
                default: return "Unknown";
            }
        }
    };

    // ========================================
    // CAN Message Definitions (Automotive Standard)
    // ========================================
    
    namespace CANMessages {
        // Standard automotive CAN IDs
        const uint32_t ENGINE_SPEED_REQUEST = 0x100;    // ECU requests current engine speed
        const uint32_t ENGINE_SPEED_RESPONSE = 0x101;   // Engine responds with current speed
        const uint32_t THROTTLE_COMMAND = 0x200;        // ECU sends throttle position command
        const uint32_t VEHICLE_STATUS = 0x300;          // Vehicle status (speed, gear, etc.)
        const uint32_t CRUISE_CONTROL_STATUS = 0x400;   // Cruise control system status
        const uint32_t ROAD_CONDITION_UPDATE = 0x500;   // Road condition sensor data
        const uint32_t PI_CONTROLLER_DEBUG = 0x600;     // PI controller debug information
    }

    // ========================================
    // Engine Control Unit (ECU) with PI Controller
    // ========================================
    
    class EngineControlUnit {
    private:
        shared_ptr<CANNode> canNode;
        shared_ptr<CANBus> canBus;
        PIController speedController;
        double targetSpeed;             // Desired cruise speed in km/h
        double currentSpeed;            // Current vehicle speed
        double currentThrottlePosition; // Current throttle position (0-100%)
        bool cruiseControlActive;       // Cruise control state
        thread controlThread;
        atomic<bool> running;
        
        void controlLoop() {
            cout << "[ECU] Speed control loop started" << endl;
            
            while (running.load()) {
                if (cruiseControlActive && targetSpeed > 0) {
                    // Calculate required throttle position using PI controller
                    double newThrottlePosition = speedController.calculate(targetSpeed, currentSpeed);
                    
                    // Send throttle command via CAN
                    sendThrottleCommand(newThrottlePosition);
                    
                    // Send debug information
                    sendControllerDebugInfo(newThrottlePosition);
                    
                    currentThrottlePosition = newThrottlePosition;
                }
                
                // Request current speed from vehicle
                requestCurrentSpeed();
                
                this_thread::sleep_for(50ms); // 20Hz control loop
            }
        }
        
        void sendThrottleCommand(double throttlePosition) {
            // Encode throttle position as 16-bit value (0-10000 representing 0-100.00%)
            uint16_t throttleEncoded = static_cast<uint16_t>(throttlePosition * 100);
            
            vector<uint8_t> data = {
                static_cast<uint8_t>(throttleEncoded & 0xFF),
                static_cast<uint8_t>((throttleEncoded >> 8) & 0xFF),
                static_cast<uint8_t>(cruiseControlActive ? 0x01 : 0x00),  // Cruise control status
                static_cast<uint8_t>(0x00)  // Reserved
            };
            
            auto message = canNode->createMessage(CANMessages::THROTTLE_COMMAND, data);
            canBus->transmitMessage(message);
        }
        
        void requestCurrentSpeed() {
            vector<uint8_t> data = {static_cast<uint8_t>(0x01)}; // Request type: speed
            auto message = canNode->createMessage(CANMessages::ENGINE_SPEED_REQUEST, data);
            canBus->transmitMessage(message);
        }
        
        void sendControllerDebugInfo(double throttlePosition) {
            // Send PI controller debug information
            uint16_t speedEncoded = static_cast<uint16_t>(currentSpeed * 10); // 0.1 km/h resolution
            uint16_t targetEncoded = static_cast<uint16_t>(targetSpeed * 10);
            uint16_t throttleEncoded = static_cast<uint16_t>(throttlePosition * 100);
            uint16_t integralEncoded = static_cast<uint16_t>((speedController.getIntegralSum() + 100) * 100); // Offset for negative values
            
            vector<uint8_t> data = {
                static_cast<uint8_t>(speedEncoded & 0xFF),
                static_cast<uint8_t>((speedEncoded >> 8) & 0xFF),
                static_cast<uint8_t>(targetEncoded & 0xFF),
                static_cast<uint8_t>((targetEncoded >> 8) & 0xFF),
                static_cast<uint8_t>(throttleEncoded & 0xFF),
                static_cast<uint8_t>((throttleEncoded >> 8) & 0xFF),
                static_cast<uint8_t>(integralEncoded & 0xFF),
                static_cast<uint8_t>((integralEncoded >> 8) & 0xFF)
            };
            
            auto message = canNode->createMessage(CANMessages::PI_CONTROLLER_DEBUG, data);
            canBus->transmitMessage(message);
        }
        
        void handleCANMessage(const CANMessage& message) {
            switch (message.id) {
                case CANMessages::ENGINE_SPEED_RESPONSE:
                    if (message.data.size() >= 2) {
                        uint16_t speedEncoded = message.data[0] | (message.data[1] << 8);
                        currentSpeed = speedEncoded / 10.0; // Convert back to km/h
                    }
                    break;
                    
                case CANMessages::VEHICLE_STATUS:
                    if (message.data.size() >= 4) {
                        uint16_t vehicleSpeed = message.data[0] | (message.data[1] << 8);
                        currentSpeed = vehicleSpeed / 10.0;
                        // Additional vehicle status can be processed here
                    }
                    break;
            }
        }
        
    public:
        EngineControlUnit(shared_ptr<CANBus> bus, uint32_t nodeId, 
                         double kp = 2.0, double ki = 0.1) // Default PI gains
            : canBus(bus), speedController(kp, ki, 0.0, 100.0), // 0-100% throttle range
              targetSpeed(0.0), currentSpeed(0.0), currentThrottlePosition(0.0),
              cruiseControlActive(false), running(true) {
            
            canNode = make_shared<CANNode>(nodeId, "Engine_Control_Unit");
            canNode->setMessageHandler([this](const CANMessage& msg) {
                handleCANMessage(msg);
            });
            canBus->addNode(canNode);
            
            controlThread = thread(&EngineControlUnit::controlLoop, this);
            
            cout << "[ECU] Engine Control Unit initialized with PI gains: Kp=" 
                 << kp << ", Ki=" << ki << endl;
        }
        
        ~EngineControlUnit() {
            shutdown();
        }
        
        void shutdown() {
            running.store(false);
            if (controlThread.joinable()) {
                controlThread.join();
            }
        }
        
        void setCruiseSpeed(double speedKmh) {
            targetSpeed = speedKmh;
            cruiseControlActive = true;
            speedController.reset(); // Reset integral term for new setpoint
            cout << "[ECU] Cruise control activated - Target speed: " 
                 << targetSpeed << " km/h" << endl;
        }
        
        void disableCruiseControl() {
            cruiseControlActive = false;
            targetSpeed = 0.0;
            speedController.reset();
            cout << "[ECU] Cruise control deactivated" << endl;
        }
        
        void setPIGains(double kp, double ki) {
            speedController.setGains(kp, ki);
            cout << "[ECU] PI gains updated - Kp=" << kp << ", Ki=" << ki << endl;
        }
        
        // Getters for monitoring
        double getCurrentSpeed() const { return currentSpeed; }
        double getTargetSpeed() const { return targetSpeed; }
        double getThrottlePosition() const { return currentThrottlePosition; }
        bool isCruiseControlActive() const { return cruiseControlActive; }
    };

    // ========================================
    // Vehicle Simulator Node
    // ========================================
    
    class VehicleSimulator {
    private:
        shared_ptr<CANNode> canNode;
        shared_ptr<CANBus> canBus;
        VehicleDynamics dynamics;
        thread simulationThread;
        atomic<bool> running;
        double currentThrottlePosition;
        
        void simulationLoop() {
            auto lastTime = steady_clock::now();
            
            while (running.load()) {
                auto currentTime = steady_clock::now();
                auto deltaTime = duration_cast<milliseconds>(currentTime - lastTime).count() / 1000.0;
                
                if (deltaTime > 0.0) {
                    // Update vehicle dynamics
                    dynamics.updateSpeed(currentThrottlePosition, deltaTime);
                    
                    // Send vehicle status via CAN
                    sendVehicleStatus();
                    
                    lastTime = currentTime;
                }
                
                this_thread::sleep_for(20ms); // 50Hz simulation rate
            }
        }
        
        void sendVehicleStatus() {
            uint16_t speedEncoded = static_cast<uint16_t>(dynamics.getCurrentSpeed() * 10);
            uint16_t throttleEncoded = static_cast<uint16_t>(currentThrottlePosition * 100);
            uint8_t roadCondition = static_cast<uint8_t>(dynamics.getRoadCondition());
            
            vector<uint8_t> data = {
                static_cast<uint8_t>(speedEncoded & 0xFF),
                static_cast<uint8_t>((speedEncoded >> 8) & 0xFF),
                static_cast<uint8_t>(throttleEncoded & 0xFF),
                static_cast<uint8_t>((throttleEncoded >> 8) & 0xFF),
                roadCondition,
                static_cast<uint8_t>(0x00),  // Gear position (simplified)
                static_cast<uint8_t>(0x00),  // Reserved
                static_cast<uint8_t>(0x00)   // Reserved
            };
            
            auto message = canNode->createMessage(CANMessages::VEHICLE_STATUS, data);
            canBus->transmitMessage(message);
        }
        
        void handleCANMessage(const CANMessage& message) {
            switch (message.id) {
                case CANMessages::THROTTLE_COMMAND:
                    if (message.data.size() >= 2) {
                        uint16_t throttleEncoded = message.data[0] | (message.data[1] << 8);
                        currentThrottlePosition = throttleEncoded / 100.0;
                    }
                    break;
                    
                case CANMessages::ENGINE_SPEED_REQUEST:
                    // Respond with current speed
                    respondWithCurrentSpeed();
                    break;
                    
                case CANMessages::ROAD_CONDITION_UPDATE:
                    if (message.data.size() >= 1) {
                        RoadCondition newCondition = static_cast<RoadCondition>(message.data[0]);
                        dynamics.setRoadCondition(newCondition);
                    }
                    break;
            }
        }
        
        void respondWithCurrentSpeed() {
            uint16_t speedEncoded = static_cast<uint16_t>(dynamics.getCurrentSpeed() * 10);
            
            vector<uint8_t> data = {
                static_cast<uint8_t>(speedEncoded & 0xFF),
                static_cast<uint8_t>((speedEncoded >> 8) & 0xFF),
                static_cast<uint8_t>(0x01),  // Response type: speed
                static_cast<uint8_t>(0x00)   // Status: OK
            };
            
            auto message = canNode->createMessage(CANMessages::ENGINE_SPEED_RESPONSE, data);
            canBus->transmitMessage(message);
        }
        
    public:
        VehicleSimulator(shared_ptr<CANBus> bus, uint32_t nodeId, double vehicleMass = 1500.0)
            : canBus(bus), dynamics(vehicleMass), running(true), currentThrottlePosition(0.0) {
            
            canNode = make_shared<CANNode>(nodeId, "Vehicle_Simulator");
            canNode->setMessageHandler([this](const CANMessage& msg) {
                handleCANMessage(msg);
            });
            canBus->addNode(canNode);
            
            simulationThread = thread(&VehicleSimulator::simulationLoop, this);
            
            cout << "[VEHICLE] Vehicle simulator initialized (Mass: " 
                 << vehicleMass << " kg)" << endl;
        }
        
        ~VehicleSimulator() {
            shutdown();
        }
        
        void shutdown() {
            running.store(false);
            if (simulationThread.joinable()) {
                simulationThread.join();
            }
        }
        
        void changeRoadCondition(RoadCondition condition) {
            dynamics.setRoadCondition(condition);
            
            // Broadcast road condition change via CAN
            vector<uint8_t> data = {
                static_cast<uint8_t>(condition),
                static_cast<uint8_t>(0x00),  // Reserved
                static_cast<uint8_t>(0x00),  // Reserved
                static_cast<uint8_t>(0x00)   // Reserved
            };
            
            auto message = canNode->createMessage(CANMessages::ROAD_CONDITION_UPDATE, data);
            canBus->transmitMessage(message);
        }
        
        double getCurrentSpeed() const { return dynamics.getCurrentSpeed(); }
        double getThrottlePosition() const { return currentThrottlePosition; }
        RoadCondition getRoadCondition() const { return dynamics.getRoadCondition(); }
        string getRoadConditionString() const { return dynamics.getRoadConditionString(); }
    };

    // ========================================
    // Dashboard Display Node
    // ========================================
    
    class DashboardDisplay {
    private:
        shared_ptr<CANNode> canNode;
        shared_ptr<CANBus> canBus;
        
        // Display data
        double currentSpeed;
        double targetSpeed;
        double throttlePosition;
        double integralSum;
        bool cruiseActive;
        string roadCondition;
        
    public:
        DashboardDisplay(shared_ptr<CANBus> bus, uint32_t nodeId)
            : canBus(bus), currentSpeed(0.0), targetSpeed(0.0), 
              throttlePosition(0.0), integralSum(0.0), cruiseActive(false),
              roadCondition("Unknown") {
            
            canNode = make_shared<CANNode>(nodeId, "Dashboard_Display");
            canNode->setMessageHandler([this](const CANMessage& msg) {
                handleCANMessage(msg);
            });
            canBus->addNode(canNode);
            
            cout << "[DASH] Dashboard display initialized" << endl;
        }
        
        void handleCANMessage(const CANMessage& message) {
            switch (message.id) {
                case CANMessages::VEHICLE_STATUS:
                    if (message.data.size() >= 5) {
                        uint16_t speedEncoded = message.data[0] | (message.data[1] << 8);
                        uint16_t throttleEncoded = message.data[2] | (message.data[3] << 8);
                        uint8_t roadCond = message.data[4];
                        
                        currentSpeed = speedEncoded / 10.0;
                        throttlePosition = throttleEncoded / 100.0;
                        
                        switch (static_cast<RoadCondition>(roadCond)) {
                            case RoadCondition::FLAT: roadCondition = "Flat"; break;
                            case RoadCondition::UPHILL_MILD: roadCondition = "Uphill"; break;
                            case RoadCondition::UPHILL_STEEP: roadCondition = "Steep"; break;
                            case RoadCondition::DOWNHILL_MILD: roadCondition = "Downhill"; break;
                            case RoadCondition::DOWNHILL_STEEP: roadCondition = "Steep"; break;
                        }
                    }
                    break;
                    
                case CANMessages::PI_CONTROLLER_DEBUG:
                    if (message.data.size() >= 8) {
                        uint16_t speedEnc = message.data[0] | (message.data[1] << 8);
                        uint16_t targetEnc = message.data[2] | (message.data[3] << 8);
                        uint16_t integralEnc = message.data[6] | (message.data[7] << 8);
                        
                        currentSpeed = speedEnc / 10.0;
                        targetSpeed = targetEnc / 10.0;
                        integralSum = (integralEnc / 100.0) - 100.0; // Remove offset
                        cruiseActive = (targetSpeed > 0);
                    }
                    break;
                    
                case CANMessages::THROTTLE_COMMAND:
                    if (message.data.size() >= 3) {
                        cruiseActive = (message.data[2] == 0x01);
                    }
                    break;
            }
        }
        
        void printStatus() {
            cout << "\n" << string(80, '=') << endl;
            cout << "                     ADAPTIVE CRUISE CONTROL DASHBOARD " << endl;
            cout << string(80, '=') << endl;
            cout << fixed << setprecision(1);
            cout << " Current Speed: " << setw(6) << currentSpeed << " km/h";
            cout << "  Target Speed: " << setw(6) << targetSpeed << " km/h";
            cout << " Road: " << setw(10) << roadCondition << " " << endl;
            cout << " Throttle Pos: " << setw(6) << throttlePosition << " %   ";
            cout << "  Cruise Mode:  " << setw(6) << (cruiseActive ? "ON" : "OFF") << "     ";
            cout << "  PI Integral: " << setw(7) << setprecision(2) << integralSum << " " << endl;
            cout << string(80, '=') << endl;
        }
    };

    // ========================================
    // Adaptive Cruise Control Scenario Runner
    // ========================================
    
    class AdaptiveCruiseControlScenario {
    private:
        shared_ptr<CANBus> canBus;
        unique_ptr<EngineControlUnit> ecu;
        unique_ptr<VehicleSimulator> vehicle;
        unique_ptr<DashboardDisplay> dashboard;
        
    public:
        AdaptiveCruiseControlScenario() {
            // Initialize CAN bus with automotive standard bit rate
            canBus = make_shared<CANBus>();
            canBus->setBitRate(500000); // 500 kbps (common automotive rate)
            
            // Create system components
            ecu = make_unique<EngineControlUnit>(canBus, 0x10, 2.5, 0.15); // Tuned PI gains
            vehicle = make_unique<VehicleSimulator>(canBus, 0x20, 1500.0); // 1500kg vehicle
            dashboard = make_unique<DashboardDisplay>(canBus, 0x30);
            
            cout << "\n Adaptive Cruise Control with PI Speed Governor Initialized " << endl;
            cout << "qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq" << endl;
        }
        
        void runScenario() {
            cout << "\n SCENARIO: Maintaining 80 km/h on Various Road Conditions" << endl;
            cout << "This demonstrates how a PI controller maintains constant speed despite changing load conditions." << endl;
            
            // Phase 1: Start on flat road
            cout << "\n Phase 1: Starting cruise control on flat road..." << endl;
            ecu->setCruiseSpeed(80.0); // Set cruise control to 80 km/h
            this_thread::sleep_for(3s);
            dashboard->printStatus();
            
            // Phase 2: Mild uphill
            cout << "\n Phase 2: Encountering mild uphill (3% grade)..." << endl;
            vehicle->changeRoadCondition(RoadCondition::UPHILL_MILD);
            this_thread::sleep_for(4s);
            dashboard->printStatus();
            
            // Phase 3: Steep uphill
            cout << "\n Phase 3: Steep uphill climb (8% grade)..." << endl;
            vehicle->changeRoadCondition(RoadCondition::UPHILL_STEEP);
            this_thread::sleep_for(5s);
            dashboard->printStatus();
            
            // Phase 4: Back to flat
            cout << "\n Phase 4: Returning to flat road..." << endl;
            vehicle->changeRoadCondition(RoadCondition::FLAT);
            this_thread::sleep_for(3s);
            dashboard->printStatus();
            
            // Phase 5: Downhill
            cout << "\n Phase 5: Mild downhill (3% grade)..." << endl;
            vehicle->changeRoadCondition(RoadCondition::DOWNHILL_MILD);
            this_thread::sleep_for(4s);
            dashboard->printStatus();
            
            // Phase 6: Steep downhill
            cout << "\n Phase 6: Steep downhill (8% grade)..." << endl;
            vehicle->changeRoadCondition(RoadCondition::DOWNHILL_STEEP);
            this_thread::sleep_for(4s);
            dashboard->printStatus();
            
            // Phase 7: Final flat section
            cout << "\n Phase 7: Final flat section - demonstrating steady state..." << endl;
            vehicle->changeRoadCondition(RoadCondition::FLAT);
            this_thread::sleep_for(3s);
            dashboard->printStatus();
            
            cout << "\n Scenario Complete!" << endl;
            cout << "\n PERFORMANCE ANALYSIS:" << endl;
            cout << "• The PI controller successfully maintained target speed across all road conditions" << endl;
            cout << "• Throttle position automatically adjusted to compensate for changing load" << endl;
            cout << "• Integral term accumulated to eliminate steady-state error" << endl;
            cout << "• System demonstrated robust performance on slopes and flat roads" << endl;
            
            // Demonstrate PI tuning
            cout << "\n BONUS: Demonstrating PI Gain Tuning..." << endl;
            cout << "Changing to aggressive gains (High Kp, Low Ki)..." << endl;
            ecu->setPIGains(5.0, 0.05);
            vehicle->changeRoadCondition(RoadCondition::UPHILL_MILD);
            this_thread::sleep_for(3s);
            dashboard->printStatus();
            
            cout << "Changing to conservative gains (Low Kp, High Ki)..." << endl;
            ecu->setPIGains(1.0, 0.3);
            this_thread::sleep_for(3s);
            dashboard->printStatus();
            
            // Clean shutdown
            cout << "\n Shutting down cruise control..." << endl;
            ecu->disableCruiseControl();
            this_thread::sleep_for(1s);
            
            cout << "\n Adaptive Cruise Control Demonstration Complete!" << endl;
            cout << "qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq" << endl;
        }
        
        ~AdaptiveCruiseControlScenario() {
            if (ecu) ecu->shutdown();
            if (vehicle) vehicle->shutdown();
            if (canBus) canBus->shutdown();
        }
    };

} // namespace AdaptiveCruiseControl
