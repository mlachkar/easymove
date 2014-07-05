#ifndef SPEAKER_H
#define SPEAKER_H

#include <festival/festival.h>
#include <QThread>

class speaker : public QThread
{
    Q_OBJECT

protected:
    void run();

public:
    void say(char*);
};

#endif // SPEAKER_H
