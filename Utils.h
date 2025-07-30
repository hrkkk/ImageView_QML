#ifndef UTILS_H
#define UTILS_H

#include <QObject>

enum class Result {
    SUCCESS,
    NOT_READY,
    TIMEOUT,
    UNKNOWN_ERROR,
    ERROR_DECODE_FAILED,
    ERROR_READ_FILE,
    ERROR_OPEN_FILE
};

enum class File_Type {
    TYPE_JPEG,
    TYPE_PNG,
    TYPE_RAW,
    TYPE_EXIF
};

enum class Orientation {

};

class Utils
{
public:
    Utils();
};

#endif // UTILS_H
