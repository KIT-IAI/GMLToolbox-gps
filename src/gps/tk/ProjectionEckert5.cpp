#include "StdAfx.h"

#include "Projection.h"

#include "ProjectionUtils.h"

#include "ProjectionEckert5.h"

#define _USE_MATH_DEFINES
#include <math.h>

#define XF 0.44101277172455148219
#define RXF 2.26750802723822639137
#define YF 0.88202554344910296438
#define RYF 1.13375401361911319568

CProjectionEckert5::CProjectionEckert5 ()
{
}

CProjectionEckert5::~CProjectionEckert5 ()
{
}

void CProjectionEckert5::Initialize (CCfgMapProjection & proj)
{
    a = proj.m_fAxis;

    lon0 = DEG2RAD (proj.m_fOriginLongitude);

    fe = UnitsToMeters (proj.m_lUnits, proj.m_fFalseEasting);
    fn = UnitsToMeters (proj.m_lUnits, proj.m_fFalseNorthing);
}

void CProjectionEckert5::Forward ()
{
    double lat = m_fLatitude;
    double lon = m_fLongitude;

    double dlam = lon - lon0;

    m_fEasting = (a * XF * (1.0 + cos (lat)) * dlam) + fe;
    m_fNorthing = (a * YF * lat) + fn;
}

void CProjectionEckert5::Inverse ()
{
    double dx = (m_fEasting - fe) / a;
    double dy = (m_fNorthing - fn) / a;

    double lat = RYF * dy;
    double lon = RXF * dx / (1.0 + cos (lat));

    m_fLatitude = lat;
    m_fLongitude = lon;
}

