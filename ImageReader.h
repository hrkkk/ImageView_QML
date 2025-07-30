#ifndef IMAGEREADER_H
#define IMAGEREADER_H

#include <QObject>

#include "Utils.h"

struct ImageData {
    uint8_t* pixels;
    uint width;
    uint height;
    uint channels;
    Orientation orientation;

    ImageData(int w, int h, int ch) : width(w), height(h), channels(ch) {
        pixels = new uint8_t[w * h * ch];
    }

    ~ImageData() {
        delete[] pixels;
    }
};

class ImageReader
{
public:
    ImageReader();

    virtual Result Decode() = 0;
};

#endif // IMAGEREADER_H
