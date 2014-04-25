/* 
 * File:   YIN.cpp
 * Author: Bas
 * 
 * Created on 20 januari 2014, 12:30
 */

#include "YIN.h"
#include "utils.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <float.h>
#include <vector>

using namespace std;

void YIN::defaultValues(){
    dimensions = 3;

    maxdelay = 200;
    length = 60;
    avgThreshold = 0.2f;
    dipThreshold = 0.1f;

    minDips = 2;
    sync = false;

}

YIN::YIN(int dims) {
    defaultValues();
    dimensions = dims;
    init();
}

YIN::YIN() {
    defaultValues();
    init();
}

YIN::YIN(const YIN& orig) {
    defaultValues();
    dimensions = orig.dimensions;
    maxdelay = orig.maxdelay;
    length = orig.length;
    avgThreshold = orig.avgThreshold;
    dipThreshold = orig.dipThreshold;
    init();
}

YIN::~YIN() {
    cleanUP();
}

void YIN::cleanUP() {
    for(int i = 0; i < buffers.size(); i++)
        buffers[i].clear();
    buffers.clear();
    for(int i = 0; i < dimvalues.size(); i++)
        dimvalues[i].clear();
    dimvalues.clear();
    values.clear();
    avgs.clear();
}

void YIN::init() {
    cleanUP();
    buffersize = maxdelay + length + 1;

    buffers.clear();
    dimvalues.clear();
    for(int i = 0; i < dimensions; i++){
        buffers.push_back(deque<float>(buffersize, 0.));
        dimvalues.push_back(vector<float>(maxdelay, 0.));
    }
    
    values = vector<float>(maxdelay, 0.);
    avgs = vector<float>(maxdelay, 0.);
}

float YIN::r(int t, int delta, deque<float> &buffer) {
    float sum = 0.0f;
    int i;
    for (i = 0; i < length; i++)
        sum += buffer[i + t] * buffer[i + t + delta];
    return sum / length;
}

void YIN::process(const vector<float> &v) {
    if (v.size() != dimensions)
        return;

    // shift and add to buffer
    for (int i = 0; i < dimensions; i++) {
        buffers[i].push_front(v[i]);
        if(buffers[i].size() > buffersize)
            buffers[i].pop_back();
    }

    // compute values
    for (int i = 0; i < dimensions; i++) {
        for (int j = 0; j < maxdelay; j++) {
            dimvalues[i][j] = r(0, 0, buffers[i]) + r(j, 0, buffers[i])
                    - 2 * r(0, j, buffers[i]);
        }
    }

    // cmndf
    for (int i = 0; i < dimensions; i++)
        dimvalues[i] = cmndf(dimvalues[i], maxdelay);

    values[0] = 0.0f;
    for (int j = 1; j < maxdelay; j++) {
        float sum = 0;
        for (int i = 0; i < dimensions; i++)
            sum += (float) pow(dimvalues[i][j], 2);
        values[j] = (float) sqrt(sum);
    }

    int minimumIndex = -1;
    float minimumValue = 1.;

    float dips[minDips];
    for(int i = 0; i < minDips; i++)
        dips[i] = -1.f;
    int detectedDips = 0;

    for (int i = 0; i < maxdelay && detectedDips < minDips; i++) {
        avg = average(values, 0, min(maxdelay - 1, (i + 1) * 3));
        avgs[i] = avg;

        float val = values[i];
        if (dips[detectedDips] < 0.) {
            if (val > avg) {
                if (minimumValue < dipThreshold) {
                    dips[detectedDips] = (float) minimumIndex;
                }
            } else if (val < dipThreshold || i == 0) {
                if (val < minimumValue) {
                    minimumValue = val;
                    minimumIndex = i;
                }
            }
        } else {
            if (val > avg) {
                detectedDips++;
                minimumValue = FLT_MAX;
            }
        }
    }

    sync = detectedDips >= minDips && average(values, 0, maxdelay - 1) > avgThreshold;
    if (sync)
        dip = dips[1];
    else
        dip = 0;

}

vector<float> YIN::getYIN() {
    return values;
}

int YIN::getLength() {
    return dip;
}

int YIN::getDimensions() {
    return dimensions;
}

bool YIN::isSync() {
    //    return (avg > avgThreshold) && ((values[dip]) < (dipThreshold * avg)) && (dip != -1);
    return sync;
}

void YIN::setAverageThreshold(float avgt) {
    avgThreshold = avgt;
}

void YIN::setDipThreshold(float dt) {
    dipThreshold = dt;
}

void YIN::clear() {
    init();
}

float YIN::getAvg() {
    return avg;
}

void YIN::setMaxLength(int l) {
    length = l;
    init();
}

void YIN::setMaxDelay(int l) {
    maxdelay = (int) l;
    init();

}

int YIN::getDip() {
    return dip;
}

int YIN::getMaxDelay() {
    return maxdelay;
}

float YIN::getAverageThreshold() {
    return avgThreshold;
}

float YIN::getDipThreshold() {
    return dipThreshold;
}

vector<float> YIN::getAvgs() {
    return avgs;
}

int YIN::getMinDips() {
    return minDips;
}

void YIN::setMinDips(int md) {
    minDips = md;
}