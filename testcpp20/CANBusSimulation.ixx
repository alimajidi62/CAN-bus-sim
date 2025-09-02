// CANBusSimulation.ixx - Complete CAN Bus Simulation Module
// This module provides a comprehensive software-based CAN bus simulation
// for learning and testing CAN protocols

module;

#include <iostream>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <bitset>
#include <iomanip>
#include <sstream>
#include <atomic>
#include <condition_variable>
#include <map>
#include <algorithm>

export module CANBusSimulation;

using namespace std;
using namespace std::chrono;

export namespace CANSim {

    // ========================================
    // CAN Frame Structure (ISO 11898 Standard)
    // ========================================
    
    // CAN Frame Types
    enum class CANFrameType {
        DATA_FRAME = 0,     // Normal data transmission
        REMOTE_FRAME = 1,   // Request for data
        ERROR_FRAME = 2,    // Error indication
        OVERLOAD_FRAME = 3  // Delay next transmission
    };

    // CAN Frame Format
    enum class CANFormat {
        STANDARD = 0,       // 11-bit identifier (CAN 2.0A)
        EXTENDED = 1        // 29-bit identifier (CAN 2.0B)
    };

    // CAN Error Types
    enum class CANErrorType {
        NO_ERROR = 0,
        STUFF_ERROR = 1,    // Bit stuffing violation
        FORM_ERROR = 2,     // Frame format error
        ACK_ERROR = 3,      // Acknowledgment error
        BIT1_ERROR = 4,     // Recessive bit error
        BIT0_ERROR = 5,     // Dominant bit error
        CRC_ERROR = 6       // CRC check failed
    };

    // ========================================
    // CAN Message Structure
    // ========================================
    
    struct CANMessage {
        uint32_t id;                    // CAN identifier (11 or 29 bits)
        CANFormat format;               // Standard or Extended format
        CANFrameType frameType;         // Frame type
        uint8_t dlc;                   // Data Length Code (0-8 bytes)
        vector<uint8_t> data;          // Message data (0-8 bytes)
        bool rtr;                      // Remote Transmission Request
        steady_clock::time_point timestamp; // When message was created
        uint32_t nodeId;               // Source node ID
        
        // Constructor for data frame
        CANMessage(uint32_t canId, const vector<uint8_t>& msgData, 
                  CANFormat fmt = CANFormat::STANDARD, uint32_t source = 0)
            : id(canId), format(fmt), frameType(CANFrameType::DATA_FRAME),
              dlc(static_cast<uint8_t>(msgData.size())), data(msgData), 
              rtr(false), timestamp(steady_clock::now()), nodeId(source) {
            
            // Validate data length
            if (dlc > 8) {
                dlc = 8;
                data.resize(8);
            }
            
            // Validate identifier based on format
            if (format == CANFormat::STANDARD && id > 0x7FF) {
                throw invalid_argument("Standard CAN ID must be <= 0x7FF (11 bits)");
            }
            if (format == CANFormat::EXTENDED && id > 0x1FFFFFFF) {
                throw invalid_argument("Extended CAN ID must be <= 0x1FFFFFFF (29 bits)");
            }
        }
        
        // Constructor for remote frame
        CANMessage(uint32_t canId, uint8_t dataLength, 
                  CANFormat fmt = CANFormat::STANDARD, uint32_t source = 0)
            : id(canId), format(fmt), frameType(CANFrameType::REMOTE_FRAME),
              dlc(dataLength), rtr(true), timestamp(steady_clock::now()), nodeId(source) {
            
            if (dlc > 8) dlc = 8;
            data.clear(); // Remote frames don't carry data
        }
        
        // Convert message to string for debugging
        string toString() const {
            stringstream ss;
            ss << "[CAN] ID:0x" << hex << uppercase << setfill('0');
            
            if (format == CANFormat::STANDARD) {
                ss << setw(3) << id;
            } else {
                ss << setw(8) << id;
            }
            
            ss << " " << (format == CANFormat::STANDARD ? "STD" : "EXT");
            ss << " " << (frameType == CANFrameType::DATA_FRAME ? "DATA" : "RTR");
            ss << " DLC:" << dec << (int)dlc;
            
            if (frameType == CANFrameType::DATA_FRAME && !data.empty()) {
                ss << " DATA:[";
                for (size_t i = 0; i < data.size(); ++i) {
                    if (i > 0) ss << " ";
                    ss << hex << uppercase << setfill('0') << setw(2) << (int)data[i];
                }
                ss << "]";
            }
            
            ss << " Node:" << dec << nodeId;
            return ss.str();
        }
    };

    // ========================================
    // CAN Bus Arbitration and Priority
    // ========================================
    
    class CANArbitration {
    public:
        // CAN uses CSMA/CA (Carrier Sense Multiple Access with Collision Avoidance)
        // Lower ID = Higher Priority (dominant wins over recessive)
        static bool hasHigherPriority(const CANMessage& msg1, const CANMessage& msg2) {
            // Extended format has lower priority than standard format
            if (msg1.format != msg2.format) {
                return msg1.format == CANFormat::STANDARD;
            }
            
            // Lower ID has higher priority
            if (msg1.id != msg2.id) {
                return msg1.id < msg2.id;
            }
            
            // RTR bit: data frame has higher priority than remote frame
            if (msg1.rtr != msg2.rtr) {
                return !msg1.rtr; // data frame (rtr=false) wins
            }
            
            return false; // Equal priority
        }
        
        // Simulate bit-by-bit arbitration
        static CANMessage arbitrate(const vector<CANMessage>& messages) {
            if (messages.empty()) {
                throw invalid_argument("No messages to arbitrate");
            }
            
            if (messages.size() == 1) {
                return messages[0];
            }
            
            // Find message with highest priority
            auto winner = messages[0];
            for (size_t i = 1; i < messages.size(); ++i) {
                if (hasHigherPriority(messages[i], winner)) {
                    winner = messages[i];
                }
            }
            
            return winner;
        }
    };

    // ========================================
    // CAN Node (ECU Simulation)
    // ========================================
    
    class CANNode {
    private:
        uint32_t nodeId;
        string nodeName;
        atomic<bool> isActive;
        function<void(const CANMessage&)> messageHandler;
        
    public:
        CANNode(uint32_t id, const string& name) 
            : nodeId(id), nodeName(name), isActive(true) {}
        
        uint32_t getId() const { return nodeId; }
        string getName() const { return nodeName; }
        bool getActive() const { return isActive.load(); }
        
        void setActive(bool active) { isActive.store(active); }
        
        void setMessageHandler(function<void(const CANMessage&)> handler) {
            messageHandler = handler;
        }
        
        void processMessage(const CANMessage& message) {
            if (isActive && messageHandler) {
                messageHandler(message);
            }
        }
        
        CANMessage createMessage(uint32_t canId, const vector<uint8_t>& data, 
                               CANFormat format = CANFormat::STANDARD) {
            return CANMessage(canId, data, format, nodeId);
        }
        
        CANMessage createRemoteFrame(uint32_t canId, uint8_t dlc, 
                                   CANFormat format = CANFormat::STANDARD) {
            return CANMessage(canId, dlc, format, nodeId);
        }
    };

    // ========================================
    // CAN Bus (Virtual Bus Simulation)
    // ========================================
    
    class CANBus {
    private:
        vector<shared_ptr<CANNode>> nodes;
        queue<CANMessage> transmissionQueue;
        mutex busMutex;
        condition_variable busCondition;
        atomic<bool> busActive;
        thread busThread;
        
        // Bus statistics
        atomic<uint64_t> totalMessages;
        atomic<uint64_t> totalErrors;
        atomic<uint32_t> busLoad; // Percentage
        
        // Bus timing parameters (simplified)
        chrono::microseconds bitTime{1000}; // 1ms per bit (1 kbps for demo)
        chrono::microseconds frameTime{20000}; // ~20ms per frame
        
        void busProcessingLoop() {
            while (busActive.load()) {
                unique_lock<mutex> lock(busMutex);
                
                // Wait for messages or timeout
                if (busCondition.wait_for(lock, frameTime, 
                    [this] { return !transmissionQueue.empty() || !busActive.load(); })) {
                    
                    if (!busActive.load()) break;
                    
                    if (!transmissionQueue.empty()) {
                        // Get all messages waiting for transmission
                        vector<CANMessage> pendingMessages;
                        while (!transmissionQueue.empty()) {
                            pendingMessages.push_back(transmissionQueue.front());
                            transmissionQueue.pop();
                        }
                        
                        lock.unlock();
                        
                        // Simulate arbitration if multiple messages
                        CANMessage winner = CANArbitration::arbitrate(pendingMessages);
                        
                        // Simulate transmission time
                        this_thread::sleep_for(frameTime);
                        
                        // Deliver message to all nodes (broadcast)
                        broadcastMessage(winner);
                        
                        totalMessages.fetch_add(1);
                        
                        // If there were other messages, put them back in queue
                        if (pendingMessages.size() > 1) {
                            lock_guard<mutex> lockGuard(busMutex);
                            for (const auto& msg : pendingMessages) {
                                if (msg.id != winner.id || msg.nodeId != winner.nodeId) {
                                    transmissionQueue.push(msg);
                                }
                            }
                        }
                    }
                }
            }
        }
        
        void broadcastMessage(const CANMessage& message) {
            cout << "\n[BUS] Broadcasting: " << message.toString() << endl;
            
            for (auto& node : nodes) {
                if (node->getActive() && node->getId() != message.nodeId) {
                    node->processMessage(message);
                }
            }
        }
        
    public:
        CANBus() : busActive(true), totalMessages(0), totalErrors(0), busLoad(0) {
            busThread = thread(&CANBus::busProcessingLoop, this);
        }
        
        ~CANBus() {
            shutdown();
        }
        
        void shutdown() {
            busActive.store(false);
            busCondition.notify_all();
            if (busThread.joinable()) {
                busThread.join();
            }
        }
        
        void addNode(shared_ptr<CANNode> node) {
            nodes.push_back(node);
            cout << "[BUS] Node added: " << node->getName() 
                 << " (ID: " << node->getId() << ")" << endl;
        }
        
        void removeNode(uint32_t nodeId) {
            nodes.erase(
                remove_if(nodes.begin(), nodes.end(),
                    [nodeId](const shared_ptr<CANNode>& node) {
                        return node->getId() == nodeId;
                    }),
                nodes.end()
            );
            cout << "[BUS] Node removed: ID " << nodeId << endl;
        }
        
        bool transmitMessage(const CANMessage& message) {
            if (!busActive.load()) {
                return false;
            }
            
            {
                lock_guard<mutex> lock(busMutex);
                transmissionQueue.push(message);
            }
            busCondition.notify_one();
            return true;
        }
        
        void setBitRate(uint32_t bitsPerSecond) {
            if (bitsPerSecond > 0) {
                bitTime = chrono::microseconds(1000000 / bitsPerSecond);
                frameTime = bitTime * 64; // Approximate frame time
                cout << "[BUS] Bit rate set to " << bitsPerSecond << " bps" << endl;
            }
        }
        
        // Get bus statistics
        uint64_t getTotalMessages() const { return totalMessages.load(); }
        uint64_t getTotalErrors() const { return totalErrors.load(); }
        uint32_t getBusLoad() const { return busLoad.load(); }
        size_t getNodeCount() const { return nodes.size(); }
        
        void printStatus() const {
            cout << "\n=== CAN Bus Status ===" << endl;
            cout << "Active Nodes: " << nodes.size() << endl;
            cout << "Total Messages: " << totalMessages.load() << endl;
            cout << "Total Errors: " << totalErrors.load() << endl;
            cout << "Bus Load: " << busLoad.load() << "%" << endl;
            cout << "Bus Active: " << (busActive.load() ? "YES" : "NO") << endl;
            cout << "======================" << endl;
        }
    };

    // ========================================
    // CAN Application Layer Examples
    // ========================================
    
    // Simple sensor node that sends periodic data
    class SensorNode {
    private:
        shared_ptr<CANNode> canNode;
        shared_ptr<CANBus> canBus;
        thread sensorThread;
        atomic<bool> running;
        uint32_t sensorId;
        chrono::milliseconds updateInterval;
        
        void sensorLoop() {
            uint16_t sensorValue = 0;
            
            while (running.load()) {
                // Simulate sensor reading (e.g., temperature, pressure)
                sensorValue = (sensorValue + 10) % 1000; // Simple incrementing value
                
                // Create CAN message with sensor data
                vector<uint8_t> data = {
                    static_cast<uint8_t>(sensorValue & 0xFF),        // Low byte
                    static_cast<uint8_t>((sensorValue >> 8) & 0xFF), // High byte
                    0x01,  // Sensor status (OK)
                    0x00   // Reserved
                };
                
                auto message = canNode->createMessage(sensorId, data);
                canBus->transmitMessage(message);
                
                this_thread::sleep_for(updateInterval);
            }
        }
        
    public:
        SensorNode(shared_ptr<CANBus> bus, uint32_t nodeId, uint32_t canId, 
                  chrono::milliseconds interval = 1000ms)
            : canBus(bus), running(true), sensorId(canId), updateInterval(interval) {
            
            canNode = make_shared<CANNode>(nodeId, "Sensor_" + to_string(nodeId));
            canBus->addNode(canNode);
            
            sensorThread = thread(&SensorNode::sensorLoop, this);
        }
        
        ~SensorNode() {
            stop();
        }
        
        void stop() {
            running.store(false);
            if (sensorThread.joinable()) {
                sensorThread.join();
            }
        }
        
        shared_ptr<CANNode> getNode() const { return canNode; }
    };
    
    // Controller node that receives and processes messages
    class ControllerNode {
    private:
        shared_ptr<CANNode> canNode;
        shared_ptr<CANBus> canBus;
        map<uint32_t, vector<uint8_t>> lastReceivedData;
        mutex dataMutex;
        
        void messageHandler(const CANMessage& message) {
            lock_guard<mutex> lock(dataMutex);
            lastReceivedData[message.id] = message.data;
            
            cout << "[CTRL] Received: " << message.toString() << endl;
            
            // Example: Process temperature sensor data
            if (message.id == 0x100 && message.data.size() >= 2) {
                uint16_t temperature = message.data[0] | (message.data[1] << 8);
                cout << "[CTRL] Temperature: " << temperature << "°C" << endl;
                
                // Example control logic: send actuator command if temp > 500
                if (temperature > 500) {
                    vector<uint8_t> actuatorCmd = {0x01, 0xFF}; // Turn on cooling
                    auto cmdMessage = canNode->createMessage(0x200, actuatorCmd);
                    canBus->transmitMessage(cmdMessage);
                    cout << "[CTRL] Cooling activated!" << endl;
                }
            }
        }
        
    public:
        ControllerNode(shared_ptr<CANBus> bus, uint32_t nodeId)
            : canBus(bus) {
            
            canNode = make_shared<CANNode>(nodeId, "Controller_" + to_string(nodeId));
            canNode->setMessageHandler([this](const CANMessage& msg) {
                messageHandler(msg);
            });
            canBus->addNode(canNode);
        }
        
        vector<uint8_t> getLastData(uint32_t canId) {
            lock_guard<mutex> lock(dataMutex);
            auto it = lastReceivedData.find(canId);
            return (it != lastReceivedData.end()) ? it->second : vector<uint8_t>();
        }
        
        shared_ptr<CANNode> getNode() const { return canNode; }
    };

} // namespace CANSim
