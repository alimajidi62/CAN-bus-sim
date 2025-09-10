// CANSimulation.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
import CANBusSimulation;
import CANBusDemo;
import AdaptiveCruiseControl;
using namespace std;
int main()
{

	cout << "\033[1;32m ****** CAN Bus Simulation Tutorial ****** \033[0m \n";
	cout << "\nWelcome to the comprehensive CAN Bus learning system!" << endl;
	cout << "This simulation will teach you everything about CAN bus communication." << endl;
	cout << "\nChoose a demonstration:" << endl;
	cout << "1. Basic CAN Demo (Quick)" << endl;
	cout << "2. CAN Arbitration Demo" << endl;
	cout << "3. Automotive CAN Demo" << endl;
	cout << "4. Industrial CAN Demo" << endl;
	cout << "5. Full Interactive Tutorial" << endl;
	cout << "6. Adaptive Cruise Control with PI Speed Governor" << endl;
	cout << "7. Headlight Control System Demo (NEW!)" << endl;
	cout << "\nRunning Basic CAN Demo..." << endl;

	// Run the basic CAN demo by default
	//CANDemo::runBasicCANDemo();
	//CANDemo::IndustrialCANDemo::runFactoryAutomationDemo();

	cout << "\n\033[1;33m ****** NEW: Simple Headlight Control Demo ****** \033[0m \n";
	cout << "Running simple automotive headlight control scenario..." << endl;

	// Run the new Headlight Control demo
	CANDemo::runHeadlightDemo();

	cout << "\n\033[1;33m ****** Adaptive Cruise Control (Currently Disabled) ****** \033[0m \n";
	cout << "The cruise control demo is currently commented out due to speed control issues." << endl;

	// Run the new Adaptive Cruise Control scenario
	//AdaptiveCruiseControl::AdaptiveCruiseControlScenario cruiseControlDemo;
	//cruiseControlDemo.runScenario();

	cout << "\n\nFor more detailed learning, uncomment the other demo functions in Main.cpp:" << endl;
	cout << "// CANDemo::runArbitrationDemo();" << endl;
	cout << "// CANDemo::runAutomotiveDemo();" << endl;
	cout << "// CANDemo::runIndustrialDemo();" << endl;
	cout << "// CANDemo::runFullTutorial();" << endl;
}