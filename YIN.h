/* 
 * File:   YIN.h
 * Author: Bas
 *
 * Created on 20 januari 2014, 12:30
 */

#ifndef YIN_H
#define	YIN_H

#include <vector>
#include <deque>

using namespace std;

class YIN {
public:
    YIN(int dims);
    YIN();
    YIN(const YIN& orig);
    virtual ~YIN();

    void process(const vector<float>&);
    void clear();
    
    // setters
    void setAverageThreshold(float f);
    void setDipThreshold(float f);
    void setMaxLength(int l);
    void setMaxDelay(int d);
    void setMinDips(int md);
    
    // getters of attributes
    int getDip();
    int getMaxDelay();
    int getDimensions();
    float getAverageThreshold();
    float getDipThreshold();
    int getMinDips();
    
    // getters for values
    vector<float> getAvgs();
    float getAvg();
    int getLength();
    vector<float> getYIN();
    bool isSync();
    
private:
//    float* values;
//    float* avgs;
//    float** buffers;
//    float** dimvalues;
    vector<float> values;
    vector<float> avgs;
    vector<deque<float> > buffers;
    vector<vector<float> > dimvalues;

    int buffersize, maxdelay, length, dimensions, dip;
    float avgThreshold, dipThreshold, avg;

    int minDips;
    bool sync;
    
    void init();
    void defaultValues();
    void cleanUP();
    float r(int t, int delta, deque<float> &buffer);
};

#endif	/* YIN_H */

