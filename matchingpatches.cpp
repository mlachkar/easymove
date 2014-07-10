#include "matchingpatches.h"

matchingPatches::matchingPatches() {
    elbow = center = bow = Patch();
}

matchingPatches* matchingPatches::createIfMatching(std::vector<Patch> p)
{
    matchingPatches* result = new matchingPatches();
    std::sort(p.begin(), p.end(), Patch::compareByX);
    unsigned int nBlue = 0;
    unsigned int nRed = 0;

    std::vector<Patch>::const_iterator itr;
    for(itr = p.begin(); itr != p.end(); ++itr)
    {
        if((*itr).getColor() == BLUE)
        {
            nBlue++;
        }
        else
        {
            nRed++;
        }
    }
    if(nRed == 2 && nBlue == 1) {
        result->center = p[1];
        result->toRight = p[0].getColor() == RED;
        if(result->toRight) {
            result->elbow = p[0];
            result->bow = p[2];
        } else {
            result->elbow = p[2];
            result->bow = p[0];
        }
        return result;
    } else {
        return NULL;
    }
}

matchingPatches* matchingPatches::getAverage(std::vector<matchingPatches*> p)
{
    matchingPatches* average = new matchingPatches();
    int elbow_radius = 0;
    int bow_radius = 0;
    int center_radius = 0;
    unsigned char elbow_color = 0;
    unsigned char center_color = 0;
    unsigned char bow_color = 0;
    int elbow_x = 0;
    int elbow_y = 0;
    float center_x = 0;
    float center_y = 0;
    float bow_x = 0;
    float bow_y = 0;

    std::vector<matchingPatches*>::const_iterator itr;
    for(itr = p.begin(); itr != p.end(); ++itr){

       elbow_color = (*(p.begin()))->elbow.getColor();
       center_color = (*(p.begin()))->center.getColor();
       bow_color = (*(p.begin()))->bow.getColor();
       elbow_x +=  (*itr)->elbow.x();
       elbow_y +=  (*itr)->elbow.y();
       center_x += (*itr)->center.x();
       center_y += (*itr)->center.y();
       bow_x += (*itr)->bow.x() ;
       bow_y += (*itr)->bow.y();
       bow_radius = (*itr)->bow.getRadius();
       elbow_radius = (*itr)->elbow.getRadius();
       center_radius = (*itr)->center.getRadius();


    }
    average->elbow.setColor(elbow_color);
    average->elbow.setX(elbow_x / p.size());
    average->elbow.setY(elbow_y / p.size());
    average->elbow.setRadius(elbow_radius / p.size());
    average->center.setColor(center_color);
    average->center.setX(center_x / p.size());
    average->center.setY(center_y / p.size());
    average->center.setRadius(center_radius/ p.size());
    average->bow.setColor(bow_color);
    average->bow.setX(bow_x / p.size());
    average->bow.setY(bow_y / p.size());
    average->bow.setRadius(bow_radius/ p.size());

    return average;

}

Patch matchingPatches::getElbow() {
    return elbow;
}

Patch matchingPatches::getBow() {
    return bow;
}

Patch matchingPatches::getCenter() {
    return center;
}
