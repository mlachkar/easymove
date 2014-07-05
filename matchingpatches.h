#ifndef MATCHINGPATCHES_H
#define MATCHINGPATCHES_H

#include <QImage>
#include <patch.h>

class matchingPatches
{
public:
    matchingPatches();
    static matchingPatches* createIfMatching(std::vector<Patch> p);
    static matchingPatches* getAverage(std::vector<matchingPatches*> p);
    Patch getElbow();

private:
    Patch elbow;
    Patch center;
    Patch bow;
    bool toRight;
};



#endif // MATCHINGPATCHES_H
