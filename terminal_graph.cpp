#include "terminal_graph.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <chrono>

TerminalGraph::TerminalGraph(int w, int h, int minutes) 
    : width(w), height(h), min_value(0), max_value(100), 
      time_window_minutes(minutes), avg_interval_seconds(1.0), last_data_time(0) {
    calculateMaxPoints();
    // Reserve space for efficiency
    data_points.reserve(max_points);
    timestamps.reserve(max_points);
}

void TerminalGraph::addDataPoint(double value) {
    long long current_time = getCurrentTimeMs();
    
    data_points.push_back(value);
    timestamps.push_back(current_time);
    
    // Update interval calculation
    updateInterval();
    
    // Remove old points based on time window or max points
    if (time_window_minutes > 0) {
        // Remove points older than time window
        long long cutoff_time = current_time - (time_window_minutes * 60 * 1000);
        while (!timestamps.empty() && timestamps[0] < cutoff_time) {
            data_points.erase(data_points.begin());
            timestamps.erase(timestamps.begin());
        }
    } else {
        // Remove old points if we exceed maximum
        if (data_points.size() > max_points) {
            data_points.erase(data_points.begin());
            timestamps.erase(timestamps.begin());
        }
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
    // Calculate the graph area dimensions first
    int graph_width = width - 12; // Leave more space for Y-axis labels
    int graph_height = height - 7; // Leave space for header, footer and margins
    
    // Ensure minimum graph size
    if (graph_width < 20) graph_width = 20;
    if (graph_height < 5) graph_height = 5;
    
    // Clear the area we're going to draw
    int total_lines = graph_height + 6; // header(3) + graph + footer(2) + margin
    for (int i = 0; i < total_lines; ++i) {
        std::cout << "\033[K\n"; // Clear line and move to next
    }
    std::cout << "\033[" << total_lines << "A"; // Move cursor back up
    
    // Title - keep it short to fit in terminal width
    std::cout << "\033[1m" << "UDP Graph";
    if (time_window_minutes > 0) {
        std::cout << " (" << time_window_minutes << "m)";
    }
    std::cout << "\033[0m" << std::endl;
    
    // Status line - truncated to fit terminal width
    std::cout << "Pts:" << data_points.size() << "/" << max_points;
    if (!data_points.empty()) {
        std::cout << " Range:" << formatValue(min_value) << "-" << formatValue(max_value);
        std::cout << " Last:" << formatValue(data_points.back());
        if (avg_interval_seconds > 0) {
            std::cout << " Int:" << std::fixed << std::setprecision(1) << avg_interval_seconds << "s";
        }
    }
    std::cout << std::endl << std::endl;
    
    if (data_points.empty()) {
        std::cout << "Waiting for data..." << std::endl;
        return;
    }
    
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
    std::cout << "      +";
    for (int i = 0; i < graph_width; ++i) {
        std::cout << "-";
    }
    std::cout << std::endl;
    
    // X-axis labels (time indicators) - shortened to fit
    std::cout << "       ";
    if (graph_width >= 15) {
        std::cout << "old";
        int middle_pos = graph_width / 2 - 1;
        for (int i = 3; i < middle_pos; ++i) {
            std::cout << " ";
        }
        std::cout << "|";
        for (int i = middle_pos + 1; i < graph_width - 3; ++i) {
            std::cout << " ";
        }
        if (graph_width >= graph_width - 3) {
            std::cout << "new";
        }
    }
    std::cout << std::endl;
}

void TerminalGraph::clear() {
    data_points.clear();
    timestamps.clear();
    min_value = 0;
    max_value = 100;
    avg_interval_seconds = 1.0;
    last_data_time = 0;
}

void TerminalGraph::calculateMaxPoints() {
    if (time_window_minutes > 0) {
        // For time-based mode, allow many more points
        // They'll be filtered by time window in addDataPoint
        max_points = time_window_minutes * 60 * 10; // Allow up to 10 points per second
    } else {
        // Auto-detect based on terminal width (original behavior)
        max_points = width - 12;
    }
    
    // Ensure reasonable bounds
    if (max_points < 20) max_points = 20;
    if (max_points > 10000) max_points = 10000; // Reasonable memory limit
}

long long TerminalGraph::getCurrentTimeMs() const {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

void TerminalGraph::updateInterval() {
    if (timestamps.size() < 2) {
        return;
    }
    
    // Calculate average interval from last 10 data points
    size_t start_idx = timestamps.size() > 10 ? timestamps.size() - 10 : 0;
    double total_intervals = 0;
    int count = 0;
    
    for (size_t i = start_idx + 1; i < timestamps.size(); ++i) {
        double interval = (timestamps[i] - timestamps[i-1]) / 1000.0; // Convert to seconds
        if (interval > 0.01 && interval < 300) { // Reasonable bounds: 10ms to 5 minutes
            total_intervals += interval;
            count++;
        }
    }
    
    if (count > 0) {
        avg_interval_seconds = total_intervals / count;
    }
}

void TerminalGraph::updateTerminalSize(int w, int h) {
    width = w;
    height = h;
    calculateMaxPoints();
    
    // Resize data_points if needed (only for non-time-based mode)
    if (time_window_minutes == 0 && data_points.size() > max_points) {
        // Remove old points to fit new size
        size_t to_remove = data_points.size() - max_points;
        data_points.erase(data_points.begin(), data_points.begin() + to_remove);
        timestamps.erase(timestamps.begin(), timestamps.begin() + to_remove);
    }
    data_points.reserve(max_points);
    timestamps.reserve(max_points);
}
