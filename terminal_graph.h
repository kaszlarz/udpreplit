#ifndef TERMINAL_GRAPH_H
#define TERMINAL_GRAPH_H

#include <vector>
#include <string>

class TerminalGraph {
private:
    int width;
    int height;
    std::vector<double> data_points;
    std::vector<long long> timestamps; // Store timestamps for dynamic interval
    size_t max_points;
    double min_value;
    double max_value;
    int time_window_minutes;
    double avg_interval_seconds;
    long long last_data_time;
    
    void updateMinMax();
    char getBarChar(double value, double row_min, double row_max) const;
    std::string formatValue(double value) const;
    void calculateMaxPoints();
    void updateInterval();
    long long getCurrentTimeMs() const;
    
public:
    // Constructor with terminal size detection and optional time window
    TerminalGraph(int w, int h, int minutes = 0);
    
    void addDataPoint(double value);
    void render() const;
    void clear();
    void updateTerminalSize(int w, int h);
    
    // Getters
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    size_t getDataPointCount() const { return data_points.size(); }
    int getTimeWindowMinutes() const { return time_window_minutes; }
    size_t getMaxPoints() const { return max_points; }
    double getAvgInterval() const { return avg_interval_seconds; }
};

#endif // TERMINAL_GRAPH_H
