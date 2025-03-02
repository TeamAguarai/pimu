#ifdef USING_VSCODE_AS_EDITOR
    #include "LinearRegression.h"
#endif

#include <iostream>

namespace pimu
{


LinearRegression::LinearRegression() {
    zero();
}

/* Adds a new data point (x, y) and updates the accumulated sums */
void LinearRegression::addDataPoint(float x, float y) {
    numPoints++;
    xSum += x;
    ySum += y;
    xSquaredSum += (x * x);
    xTimesYSum += (x * y);
    computed = false;  // New data requires re-computation of m and b
}

/* Sets all variables to zero. Resets class */
void LinearRegression::zero() {
    numPoints = 0;      
    xSum = 0.0;        
    ySum = 0.0;        
    xSquaredSum = 0.0; 
    xTimesYSum = 0.0;  
    slope = 0.0;       
    intercept = 0.0;   
    computed = false;      
}

/* Computes the regression coefficients m (slope) and b (intercept) */
void LinearRegression::computeCoefficients() {
    if (numPoints < 2) {
        std::cerr << "Error: At least 2 points are needed to compute linear regression." << std::endl;
        return;
    }

    float denominator = (numPoints * xSquaredSum - (xSum * xSum));
    if (denominator == 0) {
        std::cerr << "Error: Division by zero when computing regression." << std::endl;
        return;
    }

    slope = (numPoints * xTimesYSum - xSum * ySum) / denominator;
    intercept = (ySum - slope * xSum) / numPoints;
    computed = true;
}

/* Sets the slope (m) and intercept (b) */
void LinearRegression::setCoefficients(float slope, float intercept) {
    slope = slope;
    intercept = intercept;
    computed = true;
}

/* Returns the slope (m) */
float LinearRegression::getSlope() const {
    return computed ? slope : 0;
}

/* Returns the intercept (b) */
float LinearRegression::getIntercept() const {
    return computed ? intercept : 0;
}

/* Predicts y for a given x using the regression equation */
float LinearRegression::predict(float x) const {
    return computed ? (slope * x + intercept) : 0;
}

/* Prints the equation of the regression line */
void LinearRegression::printEquation() const {
    if (computed) {
        std::cout << "Regression equation: y = " << slope << "x + " << intercept << std::endl;
    } else {
        std::cerr << "Error: Coefficients not computed. Call computeCoefficients() first." << std::endl;
    }
}
    

} // namespace pimu
