#pragma once

#include "Projection.h"

class CProjectionOrthographic : public CProjection
{
public:
    CProjectionOrthographic ();
    virtual ~CProjectionOrthographic ();

    void Initialize (CCfgMapProjection & proj);

    void Forward ();
    void Inverse ();

protected:

    double clat0;
    double slat0;
};
