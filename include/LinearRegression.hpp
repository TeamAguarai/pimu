#include <iostream>

namespace pimu
{
    
class LinearRegression {
private:
    int num_points_ = 0;      
    float x_sum_ = 0.0f;        
    float y_sum_ = 0.0f;        
    float x_squared_sum_ = 0.0f; 
    float x_times_y_sum_ = 0.0f;  
    float slope = 0.0f;       
    float intercept = 0.0f;   
    bool computed = false;      

public:
    LinearRegression() {
        zero();
    }

    /* Adds a new data point (x, y) and updates the accumulated sums */
    void addDataPoint(float x, float y) {
        num_points_++;
        x_sum_ += x;
        y_sum_ += y;
        x_squared_sum_ += (x * x);
        x_times_y_sum_ += (x * y);
        computed = false;  // New data requires re-computation of m and b
    }

    /* Sets all variables to zero. Resets class */
    void zero() {
        num_points_ = 0;      
        x_sum_ = 0.0;        
        y_sum_ = 0.0;        
        x_squared_sum_ = 0.0; 
        x_times_y_sum_ = 0.0;  
        slope = 0.0;       
        intercept = 0.0;   
        computed = false;      
    }

    /* Computes the regression coefficients m (slope) and b (intercept) */
    void computeCoefficients() {
        if (num_points_ < 2) {
            std::cerr << "Error: At least 2 points are needed to compute linear regression." << std::endl;
            return;
        }

        float denominator = (num_points_ * x_squared_sum_ - (x_sum_ * x_sum_));
        if (denominator == 0) {
            std::cerr << "Error: Division by zero when computing regression." << std::endl;
            return;
        }

        slope = (num_points_ * x_times_y_sum_ - x_sum_ * y_sum_) / denominator;
        intercept = (y_sum_ - slope * x_sum_) / num_points_;
        computed = true;
    }

    /* Sets the slope (m) and intercept (b) */
    void setCoefficients(float slope, float intercept) {
        slope = slope;
        intercept = intercept;
        computed = true;
    }

    /* Returns the slope (m) */
    float getSlope() const {
        return computed ? slope : 0;
    }

    /* Returns the intercept (b) */
    float getIntercept() const {
        return computed ? intercept : 0;
    }

    /* Predicts y for a given x using the regression equation */
    float predict(float x) const {
        return computed ? (slope * x + intercept) : 0;
    }

    /* Prints the equation of the regression line */
    void printEquation() const {
        if (computed) {
            std::cout << "Regression equation: y = " << slope << "x + " << intercept << std::endl;
        } else {
            std::cerr << "Error: Coefficients not computed. Call computeCoefficients() first." << std::endl;
        }
    }          
};  

} // namespace pimu