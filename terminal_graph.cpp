#include "terminal_graph.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <sstream>

TerminalGraph::TerminalGraph(int w, int h) 
    : width(w), height(h), max_points(w - 10), min_value(0), max_value(100) {
    // Reserve space for efficiency
    data_points.reserve(max_points);
}

void TerminalGraph::addDataPoint(double value) {
    data_points.push_back(value);
    
    // Remove old points if we exceed maximum
    if (data_points.size() > max_points) {
        data_points.erase(data_points.begin());
    }
    
    updateMinMax();
}

void TerminalGraph::updateMinMax() {
    if (data_points.empty()) {
        min_value = 0;
        max_value = 100;
        return;
    }
    
    auto minmax = std::minmax_element(data_points.begin(), data_points.end());
    min_value = *minmax.first;
    max_value = *minmax.second;
    
    // Add some padding to make the graph more readable
    double range = max_value - min_value;
    if (range < 0.001) { // Handle case where all values are the same
        range = std::max(1.0, std::abs(max_value) * 0.1);
        min_value -= range / 2;
        max_value += range / 2;
    } else {
        double padding = range * 0.1;
        min_value -= padding;
        max_value += padding;
    }
}

char TerminalGraph::getBarChar(double value, double row_min, double row_max) const {
    if (value < row_min || value > row_max) {
        return ' ';
    }
    
    // Different characters for different intensities
    double intensity = (value - row_min) / (row_max - row_min);
    
    if (intensity > 0.875) return '█';
    if (intensity > 0.75)  return '▇';
    if (intensity > 0.625) return '▆';
    if (intensity > 0.5)   return '▅';
    if (intensity > 0.375) return '▄';
    if (intensity > 0.25)  return '▃';
    if (intensity > 0.125) return '▂';
    if (intensity > 0)     return '▁';
    
    return ' ';
}

std::string TerminalGraph::formatValue(double value) const {
    std::ostringstream oss;
    if (std::abs(value) >= 1000) {
        oss << std::fixed << std::setprecision(0) << value;
    } else if (std::abs(value) >= 1) {
        oss << std::fixed << std::setprecision(1) << value;
    } else {
        oss << std::fixed << std::setprecision(3) << value;
    }
    return oss.str();
}

void TerminalGraph::render() const {
    // Clear the area we're going to draw
    for (int i = 0; i < height + 4; ++i) {
        std::cout << "\033[K\n"; // Clear line and move to next
    }
    std::cout << "\033[" << (height + 4) << "A"; // Move cursor back up
    
    // Title
    std::cout << "\033[1m" << "Real-time UDP Data Graph" << "\033[0m" << std::endl;
    std::cout << "Points: " << data_points.size() << "/" << max_points;
    if (!data_points.empty()) {
        std::cout << " | Range: " << formatValue(min_value) << " to " << formatValue(max_value);
        std::cout << " | Latest: " << formatValue(data_points.back());
    }
    std::cout << std::endl << std::endl;
    
    if (data_points.empty()) {
        std::cout << "Waiting for data..." << std::endl;
        return;
    }
    
    // Calculate the graph area dimensions
    int graph_width = width - 10; // Leave space for Y-axis labels
    int graph_height = height - 2; // Leave space for X-axis
    
    // Draw the graph from top to bottom
    for (int row = 0; row < graph_height; ++row) {
        // Calculate the value range for this row
        double row_max = max_value - (double(row) / graph_height) * (max_value - min_value);
        double row_min = max_value - (double(row + 1) / graph_height) * (max_value - min_value);
        
        // Y-axis label
        std::cout << std::setw(8) << std::right << formatValue((row_max + row_min) / 2) << " |";
        
        // Draw the graph points
        for (int col = 0; col < graph_width; ++col) {
            if (col < data_points.size()) {
                int data_index = data_points.size() - graph_width + col;
                if (data_index >= 0) {
                    double value = data_points[data_index];
                    char bar_char = getBarChar(value, row_min, row_max);
                    
                    // Color coding based on value
                    if (bar_char != ' ') {
                        if (value > (max_value + min_value) / 2) {
                            std::cout << "\033[32m"; // Green for high values
                        } else {
                            std::cout << "\033[36m"; // Cyan for low values
                        }
                        std::cout << bar_char << "\033[0m";
                    } else {
                        std::cout << ' ';
                    }
                } else {
                    std::cout << ' ';
                }
            } else {
                std::cout << ' ';
            }
        }
        std::cout << std::endl;
    }
    
    // X-axis
    std::cout << "         +";
    for (int i = 0; i < graph_width; ++i) {
        std::cout << "-";
    }
    std::cout << std::endl;
    
    // X-axis labels (time indicators)
    std::cout << "          ";
    for (int i = 0; i < graph_width; i += 10) {
        if (i == 0) {
            std::cout << "oldest";
        } else if (i >= graph_width - 6) {
            std::cout << "latest";
            break;
        } else {
            std::cout << "|";
        }
        
        // Add spacing
        int spacing = (i == 0) ? 10 - 6 : 9;
        for (int j = 0; j < spacing && i + j + 1 < graph_width; ++j) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
}

void TerminalGraph::clear() {
    data_points.clear();
    min_value = 0;
    max_value = 100;
}
