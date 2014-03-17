/* 
 * File:   YIN.cpp
 * Author: Bas
 * 
 * Created on 20 januari 2014, 12:30
 */

#include "YIN.h"
#include "../utils/utils.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <float.h>

using namespace std;

void YIN::defaultValues(){
    dimensions = 3;

    maxdelay = 100;
    length = 60;
    avgThreshold = 0.7f;
    dipThreshold = 0.7f;

    minDips = 2;
    sync = false;

    buffers = NULL;
    dimvalues = NULL;
    values = NULL;
    avgs = NULL;
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
    if (buffers) {
        for (int i = 0; i < dimensions; i++) {
            delete[] buffers[i];
            delete[] dimvalues[i];
        }
        delete[] buffers;
        delete[] dimvalues;
    }

    if (values)
        delete[] values;
    if (avgs)
        delete[] avgs;

}

void YIN::init() {
    cleanUP();
    buffersize = maxdelay + length + 1;

    buffers = new float*[dimensions];
    dimvalues = new float*[dimensions];

    for (int i = 0; i < dimensions; i++) {
        buffers[i] = new float[buffersize];
        for (int j = 0; j < buffersize; j++)
            buffers[i][j] = 0.0f;

        dimvalues[i] = new float[maxdelay];
        for (int j = 0; j < maxdelay; j++)
            dimvalues[i][j] = 0.0f;
    }

    values = new float[maxdelay];
    avgs = new float[maxdelay];
    for (int j = 0; j < maxdelay; j++) {
        values[j] = 0.0f;
        avgs[j] = 0.0f;
    }
}

float YIN::r(int t, int delta, float* buffer) {
    float sum = 0.0f;
    int i;
    for (i = 0; i < length; i++)
        sum += (*(buffer + i + t)) * (*(buffer + i + t + delta));
    return sum / length;
}

void YIN::process(vector<float> v) {
    if (v.size() != dimensions)
        return;

    // shift and add to buffer
    for (int i = 0; i < dimensions; i++) {
        for (int j = buffersize - 1; j > 0; j--)
            buffers[i][j] = buffers[i][j - 1];
        buffers[i][0] = v[i];
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
        cmndf(dimvalues[i], maxdelay);

    values[0] = 0.0f;
    for (int j = 1; j < maxdelay; j++) {
        float sum = 0;
        for (int i = 0; i < dimensions; i++)
            sum += (float) pow(dimvalues[i][j], 2);
        values[j] = (float) sqrt(sum);
    }

    int minimumIndex = -1;
    float minimumValue = 1.;

    float dips[2] = {-1., -1.};
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

float* YIN::getYIN() {
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

float* YIN::getAvgs() {
    return avgs;
}

int YIN::getMinDips() {
    return minDips;
}

void YIN::setMinDips(int md) {
    minDips = md;
}