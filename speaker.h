#ifndef SPEAKER_H
#define SPEAKER_H

#include <festival/festival.h>
#include <QThread>

class speaker : public QThread
{
    Q_OBJECT
    void run() Q_DECL_OVERRIDE;

public:
    speaker(const char* string);

private:
    const char* _string;
};

#endif // SPEAKER_H
