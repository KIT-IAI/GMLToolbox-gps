#include "StdAfx.h"

#include "Projection.h"

#include "ProjectionUtils.h"

#include "ProjectionRobinson.h"

#define V(C,z) (C.c0 + z * (C.c1 + z * (C.c2 + z * C.c3)))
#define D(C,z) (C.c1 + z * (C.c2 + C.c2 + z * 3. * C.c3))

struct COEFS
{
    double c0;
    double c1;
    double c2;
    double c3;
};

static COEFS X [] =
{
    1.0000, -5.67239e-12, -7.15511e-05, 3.11028e-06,
    0.9986, -0.000482241, -2.4897e-05, -1.33094e-06,
    0.9954, -0.000831031, -4.4861e-05, -9.86588e-07,
    0.9900, -0.00135363, -5.96598e-05, 3.67749e-06,
    0.9822, -0.00167442, -4.4975e-06, -5.72394e-06,
    0.9730, -0.00214869, -9.03565e-05, 1.88767e-08,
    0.9600, -0.00305084, -9.00732e-05, 1.64869e-06,
    0.9427, -0.00382792, -6.53428e-05, -2.61493e-06,
    0.9216, -0.00467747, -0.000104566, 4.8122e-06,
    0.8962, -0.00536222, -3.23834e-05, -5.43445e-06,
    0.8679, -0.00609364, -0.000113900, 3.32521e-06,
    0.8350, -0.00698325, -6.40219e-05, 9.34582e-07,
    0.7986, -0.00755337, -5.00038e-05, 9.35532e-07,
    0.7597, -0.00798325, -3.59716e-05, -2.27604e-06,
    0.7186, -0.00851366, -7.01120e-05, -8.63072e-06,
    0.6732, -0.00986209, -0.000199572, 1.91978e-05,
    0.6213, -0.01041800, 8.83948e-05, 6.24031e-06,
    0.5722, -0.00906601, 0.000181999, 6.24033e-06,
    0.5322, 0.00000000, 0.000000000, 0.00000e-00,
};

static COEFS Y [] =
{
    0.0000, 0.01240000, 3.72529e-10, 1.15484e-09,
    0.0620, 0.01240010, 1.76951e-08, -5.92321e-09,
    0.1240, 0.01239980, -7.09668e-08, 2.25753e-08,
    0.1860, 0.01240080, 2.66917e-07, -8.44523e-08,
    0.2480, 0.01239710, -9.99682e-07, 3.15569e-07,
    0.3100, 0.01241080, 3.73349e-06, -1.1779e-06,
    0.3720, 0.01235980, -1.39350e-05, 4.39588e-06,
    0.4340, 0.01255010, 5.20034e-05, -1.00051e-05,
    0.4968, 0.01231980, -9.80735e-05, 9.22397e-06,
    0.5571, 0.01203080, 4.02857e-05, -5.2901e-06,
    0.6176, 0.01203690, -3.90662e-05, 7.36117e-07,
    0.6769, 0.01170150, -2.80246e-05, -8.54283e-07,
    0.7346, 0.01135720, -4.08389e-05, -5.18524e-07,
    0.7903, 0.01090990, -4.86169e-05, -1.0718e-06,
    0.8435, 0.01034330, -6.46934e-05, 5.36384e-09,
    0.8936, 0.00969679, -6.46129e-05, -8.54894e-06,
    0.9394, 0.00840949, -0.000192847, -4.21023e-06,
    0.9761, 0.00616525, -0.000256001, -4.21021e-06,
    1.0000, 0.00000000, 0.000000000, 0,
};

#define C1 11.45915590261646417544
#define RC1 0.08726646259971647884
#define FXC 0.8487
#define FYC 1.3523

#define NODES 18L

#define EPS 1.0e-10

#define _USE_MATH_DEFINES
#include <math.h>

CProjectionRobinson::CProjectionRobinson ()
{
}

CProjectionRobinson::~CProjectionRobinson ()
{
}

void CProjectionRobinson::Initialize (CCfgMapProjection & proj)
{
    a = proj.m_fAxis;

    lon0 = DEG2RAD (proj.m_fOriginLongitude);

    fe = UnitsToMeters (proj.m_lUnits, proj.m_fFalseEasting);
    fn = UnitsToMeters (proj.m_lUnits, proj.m_fFalseNorthing);
}

void CProjectionRobinson::Forward ()
{
    double lat = m_fLatitude;
    double lon = m_fLongitude;

    double lam = lon - lon0;

    double dphi = fabs (lat);

    int i = int (dphi * C1);

    dphi = RAD2DEG (dphi - RC1 * i);

    double dblX = V (X [ i ], dphi);
    double dblY = V (Y [ i ], dphi);

    m_fEasting = (a * dblX * FXC * lam) + fe;
    m_fNorthing = (a * dblY * FYC) + fn;
}

void CProjectionRobinson::Inverse ()
{
    double dx = (m_fEasting - fe) / a;
    double dy = (m_fNorthing - fn) / a;

    double lat = fabs (dy / FYC);
    double lon = dx / FXC;

    int i = 0L;

    double t = 0.0;
    double t1 = 0.0;

    struct COEFS T;

    if (lat >= 1.0)
    {
        lat = dy < 0 ? -M_PI_2 : M_PI_2;
        lon = lon / X [ 18L ].c0;
    }
    else
    {
        for (i = int (lat * NODES) ; ;)
        {
            if (Y [ i ].c0 > lat)
            {
                --i;
            }
            else if (Y [ i + 1 ].c0 <= lat)
            {
                ++i;
            }
            else break;
        }

        T = Y [ i ];

        t = 5.0 * (lat - T.c0) / (Y [ i + 1 ].c0 - T.c0);

        T.c0 -= lat;

        for (; ;)
        {
            t -= t1 = V(T,t) / D(T, t);

            if (fabs (t1) < EPS)
                break;
        }

        lat = DEG2RAD (5.0 * i + t);
        if (dy < 0.) lat = -lat;
        lon /= V(X[i], t);
    }

    m_fLatitude = lat;
    m_fLongitude = lon;
}
