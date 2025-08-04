#ifndef TERMINAL_GRAPH_H
#define TERMINAL_GRAPH_H

#include <vector>
#include <string>

class TerminalGraph {
private:
    int width;
    int height;
    std::vector<double> data_points;
    size_t max_points;
    double min_value;
    double max_value;
    
    void updateMinMax();
    char getBarChar(double value, double row_min, double row_max) const;
    std::string formatValue(double value) const;
    
public:
    TerminalGraph(int w, int h);
    
    void addDataPoint(double value);
    void render() const;
    void clear();
    
    // Getters
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    size_t getDataPointCount() const { return data_points.size(); }
};

#endif // TERMINAL_GRAPH_H
