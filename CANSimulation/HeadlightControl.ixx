// HeadlightControl.ixx - Simple Headlight Control System via CAN Bus
// This module demonstrates a basic automotive lighting control system

module;

#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <string>
#include <atomic>
#include <iomanip>

export module HeadlightControl;

import CANBusSimulation;

using namespace std;
using namespace std::chrono;
using namespace CANSim;

export namespace HeadlightControl {

    // CAN Message IDs for headlight system
    constexpr uint32_t HEADLIGHT_COMMAND_ID = 0x200;
    constexpr uint32_t HEADLIGHT_STATUS_ID = 0x201;
    constexpr uint32_t USER_INPUT_ID = 0x202;

    // Headlight states
    enum class HeadlightState {
        OFF = 0,
        ON = 1,
        AUTO = 2
    };

    // User Input Controller (simulates dashboard switch)
    class UserInputController {
    private:
        shared_ptr<CANSim::CANBus> canBus;
        uint8_t nodeId;
        atomic<bool> running{ true };
        thread inputThread;

    public:
        UserInputController(shared_ptr<CANSim::CANBus> bus, uint8_t id)
            : canBus(bus), nodeId(id) {
            
            cout << "[USER_INPUT] Controller initialized (Node ID: 0x" 
                 << hex << (int)nodeId << dec << ")" << endl;
            
            // Start input monitoring thread
            inputThread = thread(&UserInputController::inputLoop, this);
        }

        ~UserInputController() {
            running = false;
            if (inputThread.joinable()) {
                inputThread.join();
            }
        }

        void inputLoop() {
            cout << "\n[USER_INPUT] Commands available:" << endl;
            cout << "  1 = Turn headlights ON" << endl;
            cout << "  0 = Turn headlights OFF" << endl;
            cout << "  2 = Auto mode" << endl;
            cout << "  q = Quit" << endl;
            cout << "\nEnter command: ";

            string input;
            while (running && getline(cin, input)) {
                if (input == "q" || input == "Q") {
                    running = false;
                    break;
                }

                HeadlightState command = HeadlightState::OFF;
                if (input == "1") {
                    command = HeadlightState::ON;
                    cout << "[USER_INPUT] Sending command: HEADLIGHTS ON" << endl;
                } else if (input == "0") {
                    command = HeadlightState::OFF;
                    cout << "[USER_INPUT] Sending command: HEADLIGHTS OFF" << endl;
                } else if (input == "2") {
                    command = HeadlightState::AUTO;
                    cout << "[USER_INPUT] Sending command: AUTO MODE" << endl;
                } else {
                    cout << "[USER_INPUT] Invalid command. Use 0, 1, 2, or q" << endl;
                    cout << "Enter command: ";
                    continue;
                }

                sendHeadlightCommand(command);
                cout << "Enter command: ";
            }
        }

        void sendHeadlightCommand(HeadlightState state) {
            CANSim::CANMessage msg;
            msg.id = USER_INPUT_ID;
            msg.length = 1;
            msg.data[0] = static_cast<uint8_t>(state);
            
            canBus->sendMessage(msg);
            cout << "[USER_INPUT] CAN message sent: ID=0x" << hex << msg.id 
                 << ", Data=0x" << (int)msg.data[0] << dec << endl;
        }

        void stop() {
            running = false;
        }
    };

    // ECU (Engine Control Unit) - processes user commands and controls headlights
    class HeadlightECU {
    private:
        shared_ptr<CANSim::CANBus> canBus;
        uint8_t nodeId;
        atomic<bool> running{ true };
        thread messageThread;
        HeadlightState currentMode{ HeadlightState::OFF };

    public:
        HeadlightECU(shared_ptr<CANSim::CANBus> bus, uint8_t id)
            : canBus(bus), nodeId(id) {
            
            cout << "[ECU] Headlight ECU initialized (Node ID: 0x" 
                 << hex << (int)nodeId << dec << ")" << endl;
            
            // Register for CAN messages
            canBus->registerNode(nodeId, [this](const CANSim::CANMessage& msg) {
                this->onCANMessage(msg);
            });
            
            messageThread = thread(&HeadlightECU::messageLoop, this);
        }

        ~HeadlightECU() {
            running = false;
            if (messageThread.joinable()) {
                messageThread.join();
            }
        }

        void onCANMessage(const CANSim::CANMessage& msg) {
            if (msg.id == USER_INPUT_ID) {
                HeadlightState requestedState = static_cast<HeadlightState>(msg.data[0]);
                cout << "[ECU] Received user command: ";
                
                switch (requestedState) {
                    case HeadlightState::OFF:
                        cout << "TURN OFF" << endl;
                        break;
                    case HeadlightState::ON:
                        cout << "TURN ON" << endl;
                        break;
                    case HeadlightState::AUTO:
                        cout << "AUTO MODE" << endl;
                        break;
                }
                
                currentMode = requestedState;
                sendHeadlightControlCommand(requestedState);
            }
        }

        void sendHeadlightControlCommand(HeadlightState state) {
            CANSim::CANMessage msg;
            msg.id = HEADLIGHT_COMMAND_ID;
            msg.length = 2;
            msg.data[0] = static_cast<uint8_t>(state);
            msg.data[1] = 0xFF; // Command validity flag
            
            canBus->sendMessage(msg);
            cout << "[ECU] Sending headlight command to controller: State=" 
                 << (int)state << endl;
        }

        void messageLoop() {
            while (running) {
                // ECU could perform additional logic here
                // For now, just a heartbeat
                this_thread::sleep_for(1s);
            }
        }

        void stop() {
            running = false;
        }
    };

    // Headlight Controller - actually controls the physical headlights
    class HeadlightController {
    private:
        shared_ptr<CANSim::CANBus> canBus;
        uint8_t nodeId;
        atomic<bool> running{ true };
        thread controlThread;
        HeadlightState currentState{ HeadlightState::OFF };
        bool lightsPhysicallyOn{ false };

    public:
        HeadlightController(shared_ptr<CANSim::CANBus> bus, uint8_t id)
            : canBus(bus), nodeId(id) {
            
            cout << "[HEADLIGHT_CTRL] Controller initialized (Node ID: 0x" 
                 << hex << (int)nodeId << dec << ")" << endl;
            
            // Register for CAN messages
            canBus->registerNode(nodeId, [this](const CANSim::CANMessage& msg) {
                this->onCANMessage(msg);
            });
            
            controlThread = thread(&HeadlightController::controlLoop, this);
        }

        ~HeadlightController() {
            running = false;
            if (controlThread.joinable()) {
                controlThread.join();
            }
        }

        void onCANMessage(const CANSim::CANMessage& msg) {
            if (msg.id == HEADLIGHT_COMMAND_ID) {
                HeadlightState commandedState = static_cast<HeadlightState>(msg.data[0]);
                cout << "[HEADLIGHT_CTRL] Received command from ECU: ";
                
                switch (commandedState) {
                    case HeadlightState::OFF:
                        cout << "TURN OFF" << endl;
                        setHeadlights(false);
                        break;
                    case HeadlightState::ON:
                        cout << "TURN ON" << endl;
                        setHeadlights(true);
                        break;
                    case HeadlightState::AUTO:
                        cout << "AUTO MODE (simulating ambient light sensor)" << endl;
                        // Simulate auto mode - for demo, let's turn them on
                        setHeadlights(true);
                        break;
                }
                
                currentState = commandedState;
                sendStatusUpdate();
            }
        }

        void setHeadlights(bool on) {
            lightsPhysicallyOn = on;
            if (on) {
                cout << "\033[1;33m[HEADLIGHT_CTRL] *** HEADLIGHTS ON ***\033[0m" << endl;
            } else {
                cout << "[HEADLIGHT_CTRL] --- HEADLIGHTS OFF ---" << endl;
            }
        }

        void sendStatusUpdate() {
            CANSim::CANMessage msg;
            msg.id = HEADLIGHT_STATUS_ID;
            msg.length = 3;
            msg.data[0] = static_cast<uint8_t>(currentState);
            msg.data[1] = lightsPhysicallyOn ? 1 : 0;
            msg.data[2] = 0xAA; // Status validity flag
            
            canBus->sendMessage(msg);
            cout << "[HEADLIGHT_CTRL] Status update sent: Mode=" << (int)currentState 
                 << ", Physical=" << (lightsPhysicallyOn ? "ON" : "OFF") << endl;
        }

        void controlLoop() {
            while (running) {
                // Send periodic status updates
                sendStatusUpdate();
                this_thread::sleep_for(3s);
            }
        }

        void stop() {
            running = false;
        }

        bool isOn() const { return lightsPhysicallyOn; }
    };

    // Main scenario class
    class HeadlightControlScenario {
    private:
        shared_ptr<CANSim::CANBus> canBus;
        unique_ptr<UserInputController> userInput;
        unique_ptr<HeadlightECU> ecu;
        unique_ptr<HeadlightController> headlightController;

    public:
        HeadlightControlScenario() {
            cout << "\n=== Initializing Headlight Control System ===" << endl;
            
            // Create CAN bus
            canBus = make_shared<CANSim::CANBus>();
            canBus->setBitRate(500000); // 500 kbps
            
            // Create system components
            userInput = make_unique<UserInputController>(canBus, 0x10);
            ecu = make_unique<HeadlightECU>(canBus, 0x20);
            headlightController = make_unique<HeadlightController>(canBus, 0x30);
            
            cout << "\n=== System Ready ===" << endl;
            cout << "CAN Bus initialized at 500 kbps" << endl;
            cout << "User Input Controller: Node 0x10" << endl;
            cout << "Headlight ECU: Node 0x20" << endl;
            cout << "Headlight Controller: Node 0x30" << endl;
        }

        void runScenario() {
            cout << "\n*** HEADLIGHT CONTROL DEMO ***" << endl;
            cout << "This demo shows how headlights are controlled via CAN bus:" << endl;
            cout << "1. User input (dashboard switch) sends command" << endl;
            cout << "2. ECU processes the command" << endl;
            cout << "3. ECU sends control signal to headlight controller" << endl;
            cout << "4. Headlight controller turns lights on/off" << endl;
            cout << "5. Status is sent back via CAN bus" << endl;
            cout << "\nType commands and watch the CAN bus communication!" << endl;
            cout << "==========================================" << endl;

            // Let the system run - user input thread handles interaction
            cout << "\nPress Enter when you want to stop the demo..." << endl;
            cin.ignore();
            
            // Stop all components
            userInput->stop();
            ecu->stop();
            headlightController->stop();
            
            cout << "\n=== Demo Complete ===" << endl;
        }
    };
}
