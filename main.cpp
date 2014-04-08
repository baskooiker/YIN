/* 
 * File:   main.cpp
 * Author: Bas
 *
 * Created on 20 februari 2014, 12:20
 */

#include <cstdlib>
#include <vector>
#include "YIN.h"
#include <fstream>

//#include "../utils/utils.h"
#include "../utils/HistogramBuffer.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    YIN* yin = new YIN(1);
    yin->setAverageThreshold(.07);
    yin->setDipThreshold(.07);
    yin->setMaxDelay(200);
    yin->setMinDips(3);

    for (int bl = 1; bl < 15; bl += 1) {
        HistogramBuffer* hb = new HistogramBuffer(bl);

        int length = 50;
        for (length = 75; length < 150; length += 5) {

            vector<float> val;
            val.push_back(0.);
            for (int i = 0; i < 200; i++) {
                if (i % length == 0)
                    val[0] = 1.;
                yin->process(val);
                val[0] *= 0.975;
                hb->put(yin->getLength());
            }
            yin->process(val);

            printf("length = %d, dip = %d, dpt = %f, yin = %d\n", length, hb->get(), yin->getDipThreshold(), yin->getLength());
        }
        printf("\n");
    }
    return 0;
}

