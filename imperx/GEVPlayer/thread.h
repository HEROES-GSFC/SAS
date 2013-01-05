#pragma once

#include <QtCore/QThread>

class Thread : public QThread
{
public:
    virtual void stop() = 0;
};


