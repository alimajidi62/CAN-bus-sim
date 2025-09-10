// CANBusDemo.ixx - Demonstration and Learning Examples for CAN Bus
// This module contains practical examples and educational content

module;

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <memory>
#include <string>

export module CANBusDemo;

import CANBusSimulation;

using namespace std;
using namespace std::chrono;
using namespace CANSim;

export namespace CANDemo {

    // ========================================
    // CAN Bus Educational Demonstrations
    // ========================================

    class CANBusLearningSystem {
    public:
        static void demonstrateBasicConcepts() {
            cout << "\n" << string(60, '=') << endl;
            cout << "    CAN BUS LEARNING SYSTEM - BASIC CONCEPTS" << endl;
            cout << string(60, '=') << endl;

            cout << "\n1. WHAT IS CAN BUS?" << endl;
            cout << "   - Controller Area Network (CAN) is a robust vehicle bus standard" << endl;
            cout << "   - Designed for microcontrollers to communicate without a host computer" << endl;
            cout << "   - Used in automotive, industrial automation, and embedded systems" << endl;

            cout << "\n2. KEY FEATURES:" << endl;
            cout << "   - Multi-master: Any node can initiate communication" << endl;
            cout << "   - Message-based: Communication via messages, not node addresses" << endl;
            cout << "   - Priority-based: Lower ID = Higher Priority" << endl;
            cout << "   - Error detection: Built-in error detection and recovery" << endl;
            cout << "   - Real-time: Deterministic timing for critical applications" << endl;

            cout << "\n3. CAN FRAME STRUCTURE:" << endl;
            cout << "   ┌─────────┬─────┬─────┬─────┬──────────┬─────┬─────┬─────┐" << endl;
            cout << "   │   SOF   │ ID  │ RTR │ Ctrl│   Data   │ CRC │ ACK │ EOF │" << endl;
            cout << "   │ 1 bit   │11bit│1bit │2bit│  0-8byte │15bit│2bit │7bit │" << endl;
            cout << "   └─────────┴─────┴─────┴─────┴──────────┴─────┴─────┴─────┘" << endl;

            cout << "\n4. ARBITRATION (Priority Resolution):" << endl;
            cout << "   - When multiple nodes transmit simultaneously" << endl;
            cout << "   - Bit-by-bit comparison during arbitration phase" << endl;
            cout << "   - Dominant bit (0) wins over recessive bit (1)" << endl;
            cout << "   - Lower ID wins = Higher Priority" << endl;
            cout << "   - Example: ID 0x100 beats ID 0x200" << endl;

            cout << "\n5. COMMON APPLICATIONS:" << endl;
            cout << "   - Automotive ECUs (Engine, ABS, Airbag, etc.)" << endl;
            cout << "   - Industrial automation (PLCs, sensors, actuators)" << endl;
            cout << "   - Medical devices" << endl;
            cout << "   - Aerospace systems" << endl;
        }

        static void demonstrateFrameTypes() {
            cout << "\n" << string(60, '=') << endl;
            cout << "    CAN FRAME TYPES DEMONSTRATION" << endl;
            cout << string(60, '=') << endl;

            // Create demo messages
            cout << "\n1. DATA FRAME (Normal data transmission):" << endl;
            vector<uint8_t> sensorData = {0x12, 0x34, 0x56, 0x78};
            CANMessage dataFrame(0x123, sensorData, CANFormat::STANDARD, 1);
            cout << "   " << dataFrame.toString() << endl;
            cout << "   - Carries actual data from sender to receiver(s)" << endl;
            cout << "   - Most common frame type in CAN networks" << endl;

            cout << "\n2. REMOTE FRAME (Request for data):" << endl;
            CANMessage remoteFrame(0x124, 4, CANFormat::STANDARD, 2);
            cout << "   " << remoteFrame.toString() << endl;
            cout << "   - Requests data from another node" << endl;
            cout << "   - RTR=1, no data field, only specifies DLC" << endl;

            cout << "\n3. STANDARD vs EXTENDED FORMAT:" << endl;
            CANMessage stdFrame(0x7FF, {0xAA, 0xBB}, CANFormat::STANDARD, 3);
            CANMessage extFrame(0x1FFFFFFF, {0xCC, 0xDD}, CANFormat::EXTENDED, 4);
            cout << "   Standard (11-bit ID): " << stdFrame.toString() << endl;
            cout << "   Extended (29-bit ID): " << extFrame.toString() << endl;
            cout << "   - Standard: IDs 0x000 to 0x7FF (2048 IDs)" << endl;
            cout << "   - Extended: IDs 0x00000000 to 0x1FFFFFFF (536M+ IDs)" << endl;
        }

        static void demonstrateArbitration() {
            cout << "\n" << string(60, '=') << endl;
            cout << "    CAN ARBITRATION DEMONSTRATION" << endl;
            cout << string(60, '=') << endl;

            cout << "\nArbitration Example - Multiple nodes transmit simultaneously:" << endl;
            
            // Create competing messages
            vector<CANMessage> competingMessages = {
                CANMessage(0x200, {0x01}, CANFormat::STANDARD, 1),  // Lower priority
                CANMessage(0x100, {0x02}, CANFormat::STANDARD, 2),  // Higher priority  
                CANMessage(0x150, {0x03}, CANFormat::STANDARD, 3),  // Medium priority
                CANMessage(0x080, {0x04}, CANFormat::STANDARD, 4)   // Highest priority
            };

            cout << "\nCompeting Messages:" << endl;
            for (const auto& msg : competingMessages) {
                cout << "   " << msg.toString() << endl;
            }

            // Demonstrate arbitration
            CANMessage winner = CANArbitration::arbitrate(competingMessages);
            cout << "\nWinner (Highest Priority): " << winner.toString() << endl;
            cout << "Reason: ID 0x" << hex << uppercase << winner.id 
                 << " is the lowest (highest priority)" << endl;

            cout << "\nArbitration Process:" << endl;
            cout << "   Step 1: All nodes start transmitting simultaneously" << endl;
            cout << "   Step 2: Bit-by-bit comparison during ID field" << endl;
            cout << "   Step 3: Node with highest priority (lowest ID) wins" << endl;
            cout << "   Step 4: Other nodes automatically retry later" << endl;
        }
    };

    // ========================================
    // Practical CAN Bus Scenarios
    // ========================================

    class AutomotiveCANDemo {
    public:
        static void runEngineManagementDemo() {
            cout << "\n" << string(60, '=') << endl;
            cout << "    AUTOMOTIVE CAN DEMO - ENGINE MANAGEMENT" << endl;
            cout << string(60, '=') << endl;

            // Create CAN bus with automotive bit rate (500 kbps is common)
            auto canBus = make_shared<CANBus>();
            canBus->setBitRate(500000); // 500 kbps

            cout << "\nSimulating Automotive CAN Network:" << endl;
            cout << "- Engine Control Unit (ECU)" << endl;
            cout << "- Transmission Control Unit (TCU)" << endl;
            cout << "- Dashboard Display" << endl;
            cout << "- Anti-lock Braking System (ABS)" << endl;

            // Create ECU nodes
            auto engine = make_shared<CANNode>(0x10, "Engine_ECU");
            auto transmission = make_shared<CANNode>(0x20, "Transmission_ECU");
            auto dashboard = make_shared<CANNode>(0x30, "Dashboard");
            auto abs = make_shared<CANNode>(0x40, "ABS_ECU");

            // Set up message handlers
            dashboard->setMessageHandler([](const CANMessage& msg) {
                if (msg.id == 0x200 && msg.data.size() >= 4) { // Engine data
                    uint16_t rpm = msg.data[0] | (msg.data[1] << 8);
                    uint16_t temp = msg.data[2] | (msg.data[3] << 8);
                    cout << "[DASH] Engine: " << rpm << " RPM, " << temp << "°C" << endl;
                }
                else if (msg.id == 0x300 && msg.data.size() >= 2) { // Speed data
                    uint16_t speed = msg.data[0] | (msg.data[1] << 8);
                    cout << "[DASH] Vehicle Speed: " << speed << " km/h" << endl;
                }
            });

            abs->setMessageHandler([&](const CANMessage& msg) {
                if (msg.id == 0x300 && msg.data.size() >= 2) { // Speed data
                    uint16_t speed = msg.data[0] | (msg.data[1] << 8);
                    if (speed > 100) { // Speed warning
                        vector<uint8_t> warning = {0xFF, 0x01}; // ABS warning
                        auto warnMsg = abs->createMessage(0x400, warning);
                        canBus->transmitMessage(warnMsg);
                        cout << "[ABS] Speed warning transmitted!" << endl;
                    }
                }
            });

            // Add nodes to bus
            canBus->addNode(engine);
            canBus->addNode(transmission);
            canBus->addNode(dashboard);
            canBus->addNode(abs);

            cout << "\nStarting simulation..." << endl;

            // Simulate engine data transmission
            for (int i = 0; i < 5; ++i) {
                // Engine sends RPM and temperature
                uint16_t rpm = 1000 + i * 500;
                uint16_t temp = 80 + i * 5;
                vector<uint8_t> engineData = {
                    static_cast<uint8_t>(rpm & 0xFF),
                    static_cast<uint8_t>((rpm >> 8) & 0xFF),
                    static_cast<uint8_t>(temp & 0xFF),
                    static_cast<uint8_t>((temp >> 8) & 0xFF)
                };
                auto engineMsg = engine->createMessage(0x200, engineData);
                canBus->transmitMessage(engineMsg);

                // Transmission sends speed data
                uint16_t speed = 20 + i * 25;
                vector<uint8_t> speedData = {
                    static_cast<uint8_t>(speed & 0xFF),
                    static_cast<uint8_t>((speed >> 8) & 0xFF)
                };
                auto speedMsg = transmission->createMessage(0x300, speedData);
                canBus->transmitMessage(speedMsg);

                this_thread::sleep_for(500ms);
            }

            this_thread::sleep_for(1s);
            canBus->printStatus();
            canBus->shutdown();
        }
    };

    class IndustrialCANDemo {
    public:
        static void runFactoryAutomationDemo() {
            cout << "\n" << string(60, '=') << endl;
            cout << "    INDUSTRIAL CAN DEMO - FACTORY AUTOMATION" << endl;
            cout << string(60, '=') << endl;

            auto canBus = make_shared<CANBus>();
            canBus->setBitRate(1000000); // 1 Mbps for industrial applications

            cout << "\nSimulating Industrial CAN Network:" << endl;
            cout << "- Temperature Sensors" << endl;
            cout << "- Pressure Sensors" << endl;
            cout << "- Motor Controllers" << endl;
            cout << "- Central PLC" << endl;

            // Create sensor nodes
            auto tempSensor1 = make_unique<SensorNode>(canBus, 0x01, 0x100, 2000ms);
            auto tempSensor2 = make_unique<SensorNode>(canBus, 0x02, 0x101, 2500ms);
            auto pressureSensor = make_unique<SensorNode>(canBus, 0x03, 0x110, 1500ms);

            // Create controller node (PLC)
            auto plc = make_unique<ControllerNode>(canBus, 0x50);

            cout << "\nStarting industrial automation simulation..." << endl;
            cout << "- Sensors will send periodic data" << endl;
            cout << "- PLC will monitor and control based on received data" << endl;
            cout << "- Simulation will run for 10 seconds" << endl;

            // Let simulation run
            this_thread::sleep_for(100s);

            cout << "\nStopping simulation..." << endl;
            tempSensor1->stop();
            tempSensor2->stop();
            pressureSensor->stop();

            this_thread::sleep_for(500ms);
            canBus->printStatus();
            canBus->shutdown();
        }
    };

    // ========================================
    // Interactive CAN Bus Learning
    // ========================================

    class InteractiveCANLearning {
    public:
        static void runInteractiveTutorial() {
            cout << "\n" << string(60, '=') << endl;
            cout << "    INTERACTIVE CAN BUS TUTORIAL" << endl;
            cout << string(60, '=') << endl;

            cout << "\nThis tutorial will guide you through CAN bus concepts step by step." << endl;
            cout << "Press Enter to continue through each section..." << endl;
            cin.get();

            // Step 1: Basic Concepts
            CANBusLearningSystem::demonstrateBasicConcepts();
            cout << "\nPress Enter to continue to Frame Types..." << endl;
            cin.get();

            // Step 2: Frame Types
            CANBusLearningSystem::demonstrateFrameTypes();
            cout << "\nPress Enter to continue to Arbitration..." << endl;
            cin.get();

            // Step 3: Arbitration
            CANBusLearningSystem::demonstrateArbitration();
            cout << "\nPress Enter to continue to Automotive Demo..." << endl;
            cin.get();

            // Step 4: Automotive Demo
            AutomotiveCANDemo::runEngineManagementDemo();
            cout << "\nPress Enter to continue to Industrial Demo..." << endl;
            cin.get();

            // Step 5: Industrial Demo
            IndustrialCANDemo::runFactoryAutomationDemo();

            cout << "\n" << string(60, '=') << endl;
            cout << "    TUTORIAL COMPLETE!" << endl;
            cout << string(60, '=') << endl;
            cout << "\nCongratulations! You have completed the CAN bus tutorial." << endl;
            cout << "Key takeaways:" << endl;
            cout << "1. CAN is a robust, real-time communication protocol" << endl;
            cout << "2. Priority-based arbitration ensures deterministic behavior" << endl;
            cout << "3. Message-based communication enables flexible network topologies" << endl;
            cout << "4. Widely used in automotive and industrial applications" << endl;
        }
    };

    // ========================================
    // Quick Demo Functions
    // ========================================

    void runBasicCANDemo() {
        cout << "\n=== BASIC CAN BUS DEMONSTRATION ===" << endl;
        
        // Create bus and nodes
        auto canBus = make_shared<CANBus>();
        auto sender = make_shared<CANNode>(1, "Sender");
        auto receiver = make_shared<CANNode>(2, "Receiver");
        
        // Set up receiver
        receiver->setMessageHandler([](const CANMessage& msg) {
            cout << "[RECEIVER] Got message: " << msg.toString() << endl;
        });
        
        canBus->addNode(sender);
        canBus->addNode(receiver);
        
        // Send some messages
        vector<uint8_t> data1 = {0x01, 0x02, 0x03, 0x04};
        auto msg1 = sender->createMessage(0x123, data1);
        canBus->transmitMessage(msg1);
        
        vector<uint8_t> data2 = {0xAA, 0xBB, 0xCC};
        auto msg2 = sender->createMessage(0x456, data2);
        canBus->transmitMessage(msg2);
        
        this_thread::sleep_for(2s);
        canBus->printStatus();
        canBus->shutdown();
    }

    void runArbitrationDemo() {
        cout << "\n=== CAN ARBITRATION DEMONSTRATION ===" << endl;
        CANBusLearningSystem::demonstrateArbitration();
    }

    // ========================================
    // HEADLIGHT CONTROL DEMO
    // ========================================
    
    class HeadlightControlDemo {
    private:
        static constexpr uint32_t HEADLIGHT_COMMAND_ID = 0x200;
        static constexpr uint32_t HEADLIGHT_STATUS_ID = 0x201;
        static constexpr uint32_t USER_INPUT_ID = 0x202;

        enum class HeadlightState {
            OFF = 0,
            ON = 1,
            AUTO = 2
        };

    public:
        static void runHeadlightDemo() {
            cout << "\n" << string(60, '=') << endl;
            cout << "    HEADLIGHT CONTROL SYSTEM DEMO" << endl;
            cout << string(60, '=') << endl;
            
            cout << "\nThis demo simulates automotive headlight control via CAN bus:" << endl;
            cout << "1. User dashboard switch sends command" << endl;
            cout << "2. ECU processes the command" << endl;
            cout << "3. ECU sends control signal to headlight controller" << endl;
            cout << "4. Headlight controller turns lights on/off" << endl;
            cout << "5. Status feedback via CAN bus" << endl;

            // Create CAN bus
            auto canBus = make_shared<CANBus>();
            canBus->setBitRate(500000);

            cout << "\n*** Creating headlight control system..." << endl;
            cout << "* CAN Bus initialized (500 kbps)" << endl;
            cout << "* User Input Node (0x10)" << endl;
            cout << "* ECU Node (0x20)" << endl;
            cout << "* Headlight Controller Node (0x30)" << endl;

            // Simulate user commands
            simulateUserCommands(canBus);
        }

    private:
        static void simulateUserCommands(shared_ptr<CANBus> canBus) {
            cout << "\n*** SIMULATING USER COMMANDS:" << endl;
            cout << "============================================" << endl;

            // Simulate different scenarios
            vector<pair<HeadlightState, string>> scenarios = {
                {HeadlightState::ON, "Driver turns headlights ON"},
                {HeadlightState::OFF, "Driver turns headlights OFF"},
                {HeadlightState::AUTO, "Driver sets to AUTO mode"},
                {HeadlightState::ON, "Driver manually turns ON again"}
            };

            bool currentLightState = false;

            for (size_t i = 0; i < scenarios.size(); i++) {
                auto [command, description] = scenarios[i];
                
                cout << "\n--- Scenario " << (i + 1) << ": " << description << " ---" << endl;
                
                // 1. User input simulation
                sendUserCommand(canBus, command);
                this_thread::sleep_for(200ms);
                
                // 2. ECU processing simulation
                processECUCommand(canBus, command);
                this_thread::sleep_for(200ms);
                
                // 3. Headlight controller response
                currentLightState = controlHeadlights(canBus, command, currentLightState);
                this_thread::sleep_for(200ms);
                
                // 4. Status feedback
                sendStatusFeedback(canBus, command, currentLightState);
                
                cout << "* Command processed successfully" << endl;
                this_thread::sleep_for(1s);
            }

            cout << "\n*** DEMO COMPLETE!" << endl;
            cout << "Key learning points:" << endl;
            cout << "• CAN messages carry specific data (command types, status)" << endl;
            cout << "• Multiple nodes communicate via shared bus" << endl;
            cout << "• Real-time feedback ensures system reliability" << endl;
            cout << "• Message IDs determine priority and routing" << endl;
        }

        static void sendUserCommand(shared_ptr<CANBus> canBus, HeadlightState command) {
            vector<uint8_t> data = {static_cast<uint8_t>(command)};
            CANMessage msg(USER_INPUT_ID, data);
            
            canBus->transmitMessage(msg);
            
            string commandStr;
            switch (command) {
                case HeadlightState::OFF: commandStr = "OFF"; break;
                case HeadlightState::ON: commandStr = "ON"; break;
                case HeadlightState::AUTO: commandStr = "AUTO"; break;
            }
            
            cout << "[DASHBOARD] *** User input: " << commandStr << " (CAN ID: 0x" 
                 << hex << msg.id << dec << ")" << endl;
        }

        static void processECUCommand(shared_ptr<CANBus> canBus, HeadlightState command) {
            cout << "[ECU] *** Processing command..." << endl;
            
            // ECU processes the user input and sends control command
            vector<uint8_t> data = {static_cast<uint8_t>(command), 0xFF}; // Command validation flag
            CANMessage msg(HEADLIGHT_COMMAND_ID, data);
            
            canBus->transmitMessage(msg);
            cout << "[ECU] >>> Sending control command to headlight controller (CAN ID: 0x" 
                 << hex << msg.id << dec << ")" << endl;
        }

        static bool controlHeadlights(shared_ptr<CANBus> canBus, HeadlightState command, bool currentState) {
            bool newState = currentState;
            
            switch (command) {
                case HeadlightState::OFF:
                    newState = false;
                    cout << "[HEADLIGHT_CTRL] *** Turning headlights OFF" << endl;
                    break;
                case HeadlightState::ON:
                    newState = true;
                    cout << "[HEADLIGHT_CTRL] *** Turning headlights ON" << endl;
                    break;
                case HeadlightState::AUTO:
                    // Simulate auto mode (let's say it's dark, so turn on)
                    newState = true;
                    cout << "[HEADLIGHT_CTRL] *** AUTO mode: Dark detected, turning ON" << endl;
                    break;
            }
            
            // Show visual status
            if (newState) {
                cout << "                    *** HEADLIGHTS ON ***" << endl;
            } else {
                cout << "                    --- HEADLIGHTS OFF ---" << endl;
            }
            
            return newState;
        }

        static void sendStatusFeedback(shared_ptr<CANBus> canBus, HeadlightState mode, bool lightState) {
            vector<uint8_t> data = {
                static_cast<uint8_t>(mode),
                static_cast<uint8_t>(lightState ? 1 : 0),
                0xAA  // Status validation flag
            };
            CANMessage msg(HEADLIGHT_STATUS_ID, data);
            
            canBus->transmitMessage(msg);
            cout << "[HEADLIGHT_CTRL] <<< Status update sent (CAN ID: 0x" 
                 << hex << msg.id << dec << ")" << endl;
        }
    };

    void runAutomotiveDemo() {
        AutomotiveCANDemo::runEngineManagementDemo();
    }

    void runIndustrialDemo() {
        IndustrialCANDemo::runFactoryAutomationDemo();
    }

    void runHeadlightDemo() {
        HeadlightControlDemo::runHeadlightDemo();
    }

    void runFullTutorial() {
        InteractiveCANLearning::runInteractiveTutorial();
    }

} // namespace CANDemo
