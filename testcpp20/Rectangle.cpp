#include "Rectangle.h"
#include <iostream>
#include <memory>
#include <vector>

// Demonstration functions for complex smart pointer usage

// Function demonstrating unique_ptr ownership transfer
void demonstrateUniquePtr()
{
    std::cout << "\n=== Demonstrating unique_ptr ===" << std::endl;
    
    // Create unique_ptr using factory method
    auto rect1 = Rectangle::createSquare(5, "UniqueSquare");
    rect1->printInfo();
    
    // Transfer ownership
    auto rect2 = std::move(rect1);
    std::cout << "After move: rect1 is " << (rect1 ? "valid" : "null") << std::endl;
    std::cout << "rect1.get(): " << rect1.get() << std::endl;
    std::cout << "rect1 is valid: " << (rect1?"valid":"null") << std::endl;
    if (rect2) {
        rect2->updateCoordinates(1, 1, 6, 6);
        rect2->printInfo();
    }
    
    // Demonstrate releasing coordinates
    auto coordinates = rect2->releaseCoordinates();
    if (coordinates) {
        std::cout << "Released coordinates: ";
        for (const auto& coord : *coordinates) {
            std::cout << coord << " ";
        }
        std::cout << std::endl;
    }
}

// Function demonstrating shared_ptr reference counting
void demonstrateSharedPtr()
{
    std::cout << "\n=== Demonstrating shared_ptr ===" << std::endl;
    
    // Create shared rectangles
    auto rect1 = Rectangle::createSharedRectangle(10, 20, "SharedRect1");
    std::cout << "rect1 use_count: " << rect1.use_count() << std::endl;
    
    {
        auto rect2 = rect1; // Copy shared_ptr
        std::cout << "After copying to rect2, use_count: " << rect1.use_count() << std::endl;
        
        auto rect3 = Rectangle::createSharedRectangle(5, 5, "SharedRect2");
        
        // Demonstrate sharing description
        rect1->shareDescriptionWith(*rect3);
        
        rect2->printInfo();
        rect3->printInfo();
        
        std::cout << "rect2 and rect3 going out of scope..." << std::endl;
        std::cout << "After copying to rect2, use_count: " << rect1.use_count() << std::endl;
        auto rect4= Rectangle::createSharedRectangle(7, 14, "SharedRect3");
        rect4=std::move(rect2);
        std::cout << "After move assignment:" << std::endl;
        std::cout << "rect2 is " << (rect2 ? "valid" : "null") << std::endl;
        std::cout << "rect4 use_count: " << rect4.use_count() << std::endl;
        std::cout << "rect3 use_count: " << rect3.use_count() << std::endl;
    }
    
    std::cout << "After scope, rect1 use_count: " << rect1.use_count() << std::endl;
    rect1->printInfo();
}

// Function demonstrating weak_ptr to prevent circular references
void demonstrateWeakPtr()
{
    std::cout << "\n=== Demonstrating weak_ptr (Circular Reference Prevention) ===" << std::endl;
    
    {
        auto shape = std::make_shared<Shape>("ComplexShape");
        
        auto rect1 = Rectangle::createSharedRectangle(8, 6, "WeakRect1");
        auto rect2 = Rectangle::createSharedRectangle(4, 3, "WeakRect2");
        
        std::cout << "Before adding to shape - rect1 use_count: " << rect1.use_count() << std::endl;
        
        shape->addRectangle(rect1);
        shape->addRectangle(rect2);
        
        std::cout << "After adding to shape - rect1 use_count: " << rect1.use_count() << std::endl;
        
        shape->printAllRectangles();
        
        std::cout << "Shape going out of scope..." << std::endl;
    }
    
    std::cout << "Shape destroyed, but rectangles might still exist" << std::endl;
}

// Function demonstrating custom deleters and array management
void demonstrateCustomDeleters()
{
    std::cout << "\n=== Demonstrating Custom Deleters ===" << std::endl;
    
    // Create array of rectangles with custom deleter
    std::shared_ptr<Rectangle[]> rectArray(
        new Rectangle[3]{
            Rectangle(2, 3, "ArrayRect1"),
            Rectangle(4, 5, "ArrayRect2"), 
            Rectangle(6, 7, "ArrayRect3")
        },
        [](Rectangle* ptr) {
            std::cout << "Custom deleter called for Rectangle array" << std::endl;
            delete[] ptr;
        }
    );
    
    // Access array elements
    for (int i = 0; i < 3; ++i) {
        std::cout << "Rectangle " << i << ": ";
        rectArray[i].printInfo();
    }
    
    std::cout << "Array use_count: " << rectArray.use_count() << std::endl;
}

// Function demonstrating RAII and exception safety
void demonstrateRAIIAndExceptionSafety()
{
    std::cout << "\n=== Demonstrating RAII and Exception Safety ===" << std::endl;
    
    try {
        auto rect = Rectangle::createSharedRectangle(3, 4, "ExceptionTestRect");
        rect->printInfo();
        
        // Simulate getting coordinates (might throw)
        const auto& coords = rect->getCoordinates();
        std::cout << "Coordinates retrieved safely" << std::endl;
        
        // This would throw an exception if coordinates were released
        // auto nullRect = Rectangle::createSharedRectangle(1, 1, "TestRect");
        // nullRect->releaseCoordinates();
        // const auto& coords2 = nullRect->getCoordinates(); // Would throw
        
    } catch (const std::exception& e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
        std::cout << "Resources automatically cleaned up due to RAII" << std::endl;
    }
}
void showUniqueVsShared()
{
    std::cout << "\n=== unique_ptr demonstration ===" << std::endl;
    std::unique_ptr<int> up1 = std::make_unique<int>(42);
    // std::unique_ptr<int> up2 = up1; // ERROR: cannot copy unique_ptr
    std::unique_ptr<int> up2 = std::move(up1); // OK: move ownership
    std::cout << "After move, up1 is " << (up1 ? "valid" : "null") << std::endl;
    std::cout << "up2 value: " << *up2 << std::endl;

    std::cout << "\n=== shared_ptr demonstration ===" << std::endl;
    std::shared_ptr<int> sp1 = std::make_shared<int>(99);
    std::shared_ptr<int> sp2 = sp1; // OK: copy, both own the object
    std::cout << "sp1 use_count: " << sp1.use_count() << std::endl;
    std::cout << "sp2 use_count: " << sp2.use_count() << std::endl;
    std::shared_ptr<int> sp3 = std::move(sp1); // move: sp1 becomes null
    std::cout << "After move, sp1 is " << (sp1 ? "valid" : "null") << std::endl;
    std::cout << "sp2 use_count: " << sp2.use_count() << std::endl;
    std::cout << "sp3 use_count: " << sp3.use_count() << std::endl;
    std::cout << "sp2 value: " << *sp2 << std::endl;
    std::cout << "sp3 value: " << *sp3 << std::endl;
}

// Main demonstration function
void demonstrateAdvancedSmartPointers()
{
    std::cout << "=== ADVANCED SMART POINTER DEMONSTRATIONS ===" << std::endl;
    
    demonstrateUniquePtr();
    demonstrateSharedPtr();
    demonstrateWeakPtr();
    demonstrateCustomDeleters();
    demonstrateRAIIAndExceptionSafety();
    
    std::cout << "\n=== END OF DEMONSTRATIONS ===" << std::endl;
}
