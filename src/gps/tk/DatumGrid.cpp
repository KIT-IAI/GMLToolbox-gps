#include "StdAfx.h"

//#include "DatumGridNadcon.h"
//#include "DatumGridNTv2.h"
#include "DatumSubGrid.h"

#include "DatumGrid.h"

#define _USE_MATH_DEFINES
#include <math.h>

inline double DEG2RAD(double x)
{
  return x * (M_PI / 180.0);
}

inline double RAD2DEG(double x)
{
  return x / (M_PI / 180.0);
}

CDatumGrid::CDatumGrid ()
{
    m_pGrids = NULL;

    Clear ();
}

CDatumGrid::~CDatumGrid ()
{
    Clear ();

    if (m_pGrids)
    {
        delete []m_pGrids;
        m_pGrids = NULL;
    }
}

void CDatumGrid::Clear ()
{
    long l = 0L;
    long lCount = 0L;

    CDatumSubGrid * pGrid = NULL;

    if (m_pGrids)
    {
        for (l = 0L ; l < m_lGrids ; l++)
        {
            pGrid = (CDatumSubGrid *) m_pGrids [ l ];

            if (pGrid)
            {
                delete pGrid;
                pGrid = NULL;
            }
        }
    }

    m_lGrids = 0L;
    m_lMode = 0L;
}

long CDatumGrid::Load(const std::wstring& lptszFileName)
{
  return 0;

  // Disabled. Bau.
#if 0
    long lResult = -1L;

// Determine type and load
    if (_tcsstr (lptszFileName, _T(".gsb")) || _tcsstr (lptszFileName, _T(".GSB")))
    {
        CDatumGridNTv2 grid;

        lResult = grid.Load (lptszFileName, this);

        if (lResult == 0L)
        {
            m_lMode = MODE_NTV2;
        }

        goto _EndLoad;
    }

    if (_tcsstr (lptszFileName, _T(".las")) || _tcsstr (lptszFileName, _T(".LAS")))
    {
        CDatumGridNadcon grid;

        lResult = grid.Load (lptszFileName, this);

        if (lResult == 0L)
        {
            m_lMode = MODE_NADCON;
        }

        goto _EndLoad;
    }

    if (_tcsstr (lptszFileName, _T(".los")) || _tcsstr (lptszFileName, _T(".LOS")))
    {
        CDatumGridNadcon grid;

        lResult = grid.Load (lptszFileName, this);

        if (lResult == 0L)
        {
            m_lMode = MODE_NADCON;
        }

        goto _EndLoad;
    }

_EndLoad:

    return lResult;
#endif
}

bool CDatumGrid::GetShift (double dblLongitude, double dblLatitude, double & dblShiftX, double & dblShiftY)
{
    long l = 0L;

    CDatumSubGrid * pGrid = NULL;

  if (m_lMode == 0L)
  {
    return false;
  }

  if (m_pGrids == NULL)
  {
    return false;
  }

    for (l = 0L ; l < m_lGrids ; l++)
    {
        pGrid = m_pGrids [ l ];

        if (pGrid)
        {
            if (m_lMode == MODE_NTV2)
            {
                if (pGrid->PointInGrid (dblLatitude, -dblLongitude))
                {
                    dblShiftY = pGrid->GetCorrectionLat (dblLatitude, -dblLongitude);
                    dblShiftX = pGrid->GetCorrectionLon (dblLatitude, -dblLongitude);

          return true;
                }
            }

            if (m_lMode == MODE_NADCON)
            {
                if (pGrid->PointInGrid (dblLatitude, dblLongitude))
                {
                    dblShiftY = pGrid->GetCorrectionLat (dblLatitude, dblLongitude);
                    dblShiftX = pGrid->GetCorrectionLon (dblLatitude, dblLongitude);

          return true;
                }
            }
        }
    }

  return false;
}

bool CDatumGrid::Forward (double & dblLongitude, double & dblLatitude)
{
    double dblLongitude0 = RAD2DEG (dblLongitude);
    double dblLatitude0 = RAD2DEG (dblLatitude);

    double dblShiftX = 0.0;
    double dblShiftY = 0.0;

  if (!GetShift(dblLongitude0, dblLatitude0, dblShiftX, dblShiftY))
  {
    return false;
  }

    dblLongitude -= DEG2RAD (dblShiftX / 3600.0);
    dblLatitude += DEG2RAD (dblShiftY / 3600.0);

  return true;
}

bool CDatumGrid::Inverse (double & dblLongitude, double & dblLatitude)
{
    double dblLongitude0 = RAD2DEG (dblLongitude);
    double dblLatitude0 = RAD2DEG (dblLatitude);

    double dblShiftX = 0.0;
    double dblShiftY = 0.0;

  if (!GetShift(dblLongitude0, dblLatitude0, dblShiftX, dblShiftY))
  {
    return false;
  }

    dblLongitude += DEG2RAD (dblShiftX / 3600.0);
    dblLatitude -= DEG2RAD (dblShiftY / 3600.0);

    return true;
}
