#pragma once
#include <memory>
#include <vector>
#include <iostream>
#include <string>

// Forward declaration for circular reference demonstration
class Shape;

class Rectangle
{
private:
	int length;
	int breadth;
	std::string name;
	
	// Smart pointer to demonstrate composition
	std::unique_ptr<std::vector<int>> coordinates;
	
	// Weak pointer to parent shape (prevents circular reference)
	std::weak_ptr<Shape> parentShape;
	
	// Shared pointer to demonstrate shared ownership of resources
	std::shared_ptr<std::string> description;

public:
	// Constructor with smart pointer initialization
	Rectangle(int l = 1, int b = 1, const std::string& rectName = "Rectangle")
		: length(l), breadth(b), name(rectName)
	{
		// Initialize unique_ptr with a vector of coordinates
		coordinates = std::make_unique<std::vector<int>>(std::initializer_list<int>{0, 0, l, b});
		
		// Initialize shared_ptr with description
		description = std::make_shared<std::string>("A rectangle with area calculation");
		
		std::cout << "Rectangle '" << name << "' created with dimensions " << l << "x" << b << std::endl;
	}
	
	// Copy constructor (deep copy with smart pointers)
	Rectangle(const Rectangle& other)
		: length(other.length), breadth(other.breadth), name(other.name + "_copy")
	{
		// Deep copy of unique_ptr content
		if (other.coordinates) {
			coordinates = std::make_unique<std::vector<int>>(*other.coordinates);
		}
		
		// Shared ownership of description (shallow copy is intentional for shared_ptr)
		description = other.description;
		
		// Don't copy weak_ptr (would need to be set by parent)
		
		std::cout << "Rectangle '" << name << "' copied" << std::endl;
	}
	
	// Move constructor
	Rectangle(Rectangle&& other) noexcept
		: length(other.length), breadth(other.breadth), name(std::move(other.name)),
		  coordinates(std::move(other.coordinates)), parentShape(std::move(other.parentShape)),
		  description(std::move(other.description))
	{
		std::cout << "Rectangle '" << name << "' moved" << std::endl;
	}
	
	// Destructor
	~Rectangle()
	{
		std::cout << "Rectangle '" << name << "' destroyed" << std::endl;
	}
	
	// Basic area calculation
	int area() const
	{
		return length * breadth;
	}
	
	// Get rectangle info with smart pointer demonstration
	void printInfo() const
	{
		std::cout << "Rectangle '" << name << "':" << std::endl;
		std::cout << "  Dimensions: " << length << "x" << breadth << std::endl;
		std::cout << "  Area: " << area() << std::endl;
		
		if (coordinates) {
			std::cout << "  Coordinates: ";
			for (const auto& coord : *coordinates) {
				std::cout << coord << " ";
			}
			std::cout << std::endl;
		}
		
		if (description) {
			std::cout << "  Description: " << *description << std::endl;
			std::cout << "  Description reference count: " << description.use_count() << std::endl;
		}
		
		if (auto parent = parentShape.lock()) {
			std::cout << "  Has valid parent shape" << std::endl;
		} else {
			std::cout << "  No parent shape or parent destroyed" << std::endl;
		}
	}
	
	// Method to demonstrate unique_ptr usage
	void updateCoordinates(int x1, int y1, int x2, int y2)
	{
		if (coordinates) {
			(*coordinates)[0] = x1;
			(*coordinates)[1] = y1;
			(*coordinates)[2] = x2;
			(*coordinates)[3] = y2;
		}
	}
	
	// Method to share description with another rectangle
	void shareDescriptionWith(Rectangle& other)
	{
		other.description = this->description;
		std::cout << "Description shared. Reference count: " << description.use_count() << std::endl;
	}
	
	// Method to set parent shape (weak_ptr usage)
	void setParentShape(std::shared_ptr<Shape> parent)
	{
		parentShape = parent;
	}
	
	// Factory method returning unique_ptr
	static std::unique_ptr<Rectangle> createSquare(int side, const std::string& name = "Square")
	{
		return std::make_unique<Rectangle>(side, side, name);
	}
	
	// Factory method returning shared_ptr
	static std::shared_ptr<Rectangle> createSharedRectangle(int l, int b, const std::string& name = "SharedRect")
	{
		return std::make_shared<Rectangle>(l, b, name);
	}
	
	// Method to get coordinates (demonstrates returning reference to unique_ptr content)
	const std::vector<int>& getCoordinates() const
	{
		if (!coordinates) {
			throw std::runtime_error("Coordinates not initialized");
		}
		return *coordinates;
	}
	
	// Method to release ownership of coordinates
	std::unique_ptr<std::vector<int>> releaseCoordinates()
	{
		std::cout << "Releasing coordinates ownership from " << name << std::endl;
		return std::move(coordinates);
	}
	
	// Getters
	const std::string& getName() const { return name; }
	int getLength() const { return length; }
	int getBreadth() const { return breadth; }
};

// Shape class for weak_ptr demonstration
class Shape
{
private:
	std::vector<std::shared_ptr<Rectangle>> rectangles;
	std::string shapeName;

public:
	Shape(const std::string& name) : shapeName(name)
	{
		std::cout << "Shape '" << shapeName << "' created" << std::endl;
	}
	
	~Shape()
	{
		std::cout << "Shape '" << shapeName << "' destroyed" << std::endl;
	}
	
	void addRectangle(std::shared_ptr<Rectangle> rect)
	{
		rectangles.push_back(rect);
		rect->setParentShape(std::shared_ptr<Shape>(this, [](Shape*){})); // Custom deleter to prevent deletion
		std::cout << "Rectangle added to shape '" << shapeName << "'" << std::endl;
	}
	
	void printAllRectangles() const
	{
		std::cout << "Shape '" << shapeName << "' contains:" << std::endl;
		for (const auto& rect : rectangles) {
			rect->printInfo();
			std::cout << "---" << std::endl;
		}
	}
	
	size_t getRectangleCount() const { return rectangles.size(); }
};

