#include <cmath>

#include "Painting.h"

Painting::Painting()
{
}

Painting::~Painting()
{
}

void Painting::Initialize(int width, int height)
{
    this->width = width;
    this->height = height;

    zb.initialize(width, height);
}

void Painting::Update()
{
    // Reset z buffer for next frame
    zb.reset();
}

// ======================== Primitives ========================================
void Painting::DrawBresenhamLine(Canvas &canvas, int xP, int yP, int xQ, int yQ, PaintColoring::CColor color)
{
    if (xP < 0 || xP > width - 1 || xQ < 0 || xQ > width - 1)
        return;
    if (yP < 0 || yP > height - 1 || yQ < 0 || yQ > height - 1)
        return;

    ca.r = color.r;
    ca.g = color.g;
    ca.b = color.b;
    ca.a = color.a;

    x = xP;
    y = yP;
    D = 0;
    HX = xQ - xP;
    HY = yQ - yP;
    xInc = 1;
    yInc = 1;
    if (HX < 0)
    {
        xInc = -1;
        HX = -HX;
    }
    if (HY < 0)
    {
        yInc = -1;
        HY = -HY;
    }
    if (HY <= HX)
    {
        C = 2 * HX;
        M = 2 * HY;
        for (;;)
        {
            canvas.PutPixel(x, y, ca);
            if (x == xQ)
                break;
            x += xInc;
            D += M;
            if (D > HX)
            {
                y += yInc;
                D -= C;
            }
        }
    }
    else
    {
        C = 2 * HY;
        M = 2 * HX;
        for (;;)
        {
            canvas.PutPixel(x, y, ca);
            if (y == yQ)
                break;
            y += yInc;
            D += M;
            if (D > HY)
            {
                x += xInc;
                D -= C;
            }
        }
    }
}

void Painting::DrawZBresenhamLine(Canvas &canvas, int xP, int yP, int xQ, int yQ, float zP, float zQ, PaintColoring::CColor color)
{
    if (xP < 0 || xP > width - 1 || xQ < 0 || xQ > width - 1)
        return;
    if (yP < 0 || yP > height - 1 || yQ < 0 || yQ > height - 1)
        return;

    ca.r = color.r;
    ca.g = color.g;
    ca.b = color.b;
    ca.a = color.a;

    zrP = 1.0f / zP;
    zrQ = 1.0f / zQ;
    dzr = zrQ - zrP;
    dx = (xQ - xP);
    dy = (yQ - yP);
    z = zrP;
    dzdx = dzr / dx;
    dzdy = dzr / dy;

    x = xP;
    y = yP;
    D = 0;
    HX = xQ - xP;
    HY = yQ - yP;
    xInc = 1;
    yInc = 1;
    if (HX < 0)
    {
        xInc = -1;
        dzdx = -dzdx;
        HX = -HX;
    }
    if (HY < 0)
    {
        yInc = -1;
        dzdy = -dzdy;
        HY = -HY;
    }
    if (HY <= HX)
    {
        C = 2 * HX;
        M = 2 * HY;
        for (;;)
        {
            zl = zb.getIndex(x, y);
            if (zl < 0)
                break; // pixel off screen
            zstatus = zb.setZ(zl, z, false);
            if (zstatus == 1)
            {
                canvas.PutPixel(x, y, ca); // pixel closer
            }
            if (x == xQ)
                break;
            x += xInc;
            z += dzdx;
            D += M;
            if (D > HX)
            {
                y += yInc;
                D -= C;
            }
        }
    }
    else
    {
        C = 2 * HY;
        M = 2 * HX;
        for (;;)
        {
            zl = zb.getIndex(x, y);
            if (zl < 0)
                break; // pixel off screen
            zstatus = zb.setZ(zl, z, false);
            if (zstatus == 1)
            {
                canvas.PutPixel(x, y, ca); // pixel closer
            }
            if (y == yQ)
                break;
            y += yInc;
            z += dzdy;
            D += M;
            if (D > HY)
            {
                x += xInc;
                D -= C;
            }
        }
    }
}

/// @brief NOT WORKING
void Painting::DrawWuIndexedLine(Canvas &canvas, int X0, int Y0, int X1, int Y1, PaintColoring::WuColor color)
{
    if (X0 < 0 || X0 > width - 1 || X1 < 0 || X1 > width - 1)
        return;
    if (Y0 < 0 || Y0 > height - 1 || Y1 < 0 || Y1 > height - 1)
        return;

    /* Make sure the line runs top to bottom */
    if (Y0 > Y1)
    {
        Temp = Y0;
        Y0 = Y1;
        Y1 = Temp;
        Temp = X0;
        X0 = X1;
        X1 = Temp;
    }

    // Draw the initial pixel, which is always exactly intersected by the
    // line and needs no weighting
    Color c = CLITERAL(Color){(unsigned char)color.color.r, (unsigned char)color.color.g, (unsigned char)color.color.b, (unsigned char)color.color.a};
    canvas.PutPixel(X0, Y0, c);

    DeltaX = X1 - X0;

    if (DeltaX >= 0)
    {
        XDir = 1;
    }
    else
    {
        XDir = -1;
        DeltaX = -DeltaX; /* make DeltaX positive */
    }

    // Special-case horizontal, vertical, and diagonal lines, which require
    // no weighting because they go right through the center of every pixel
    DeltaY = Y1 - Y0;
    if (DeltaY == 0)
    {
        /* Horizontal line */
        while (DeltaX-- != 0)
        {
            X0 += XDir;
            canvas.PutPixel(X0, Y0, c);
        }
        return;
    }

    if (DeltaX == 0)
    {
        /* Vertical line */
        do
        {
            Y0++;
            canvas.PutPixel(X0, Y0, c);
        } while (--DeltaY != 0);
        return;
    }

    if (DeltaX == DeltaY)
    {
        /* Diagonal line */
        do
        {
            X0 += XDir;
            Y0++;
            canvas.PutPixel(X0, Y0, c);
        } while (--DeltaY != 0);
        return;
    }

    // Line is not horizontal, diagonal, or vertical
    ErrorAcc = 0; // initialize the line error accumulator to 0
    // # of bits by which to shift ErrorAcc to get intensity level
    IntensityShift = 16 - color.intensityBits;

    // Mask used to flip all bits in an intensity weighting, producing the
    // result (1 - intensity weighting)
    WeightingComplementMask = color.numIntensityLevels - 1;

    /* Is this an X-major or Y-major line? */
    if (DeltaY > DeltaX)
    {
        /*
         * Y-major line; calculate 16-bit fixed-point fractional part of a
         * pixel that X advances each time Y advances 1 pixel, truncating
         * the result so that we won't overrun the endpoint along the X axis
         */
        ErrorAdj = (int)(((long)DeltaX << 16) / (long)DeltaY);
        /* Draw all pixels other than the first and last */
        while (--DeltaY != 0)
        {
            ErrorAccTemp = ErrorAcc;
            /* remember currrent accumulated error */
            ErrorAcc += ErrorAdj; /* calculate error for next pixel */
            if (ErrorAcc > 65535)
            {
                /* The error accumulator turned over, so advance the X coord */
                ErrorAcc = ErrorAcc & 0x0000ffff;
                X0 += XDir;
            }
            Y0++; /* Y-major, so always advance Y */
            /*
             * The IntensityBits most significant bits of ErrorAcc give us
             * the intensity weighting for this pixel, and the complement of
             * the weighting for the paired pixel
             */
            Weighting = ErrorAcc >> IntensityShift;

            PaintColoring::CColor &wi = color.GetColor(Weighting);
            colorWeighting.r = wi.r;
            colorWeighting.g = wi.g;
            colorWeighting.b = wi.b;
            colorWeighting.a = wi.a;
            canvas.PutPixel(X0, Y0, colorWeighting);

            Weighting = Weighting ^ WeightingComplementMask;

            canvas.PutPixel(X0 + XDir, Y0, colorWeighting);
        }
    }
    else
    {
        /*
         * It's an X-major line; calculate 16-bit fixed-point fractional
         * part of a pixel that Y advances each time X advances 1 pixel,
         * truncating the result to avoid overrunning the endpoint along the
         * X axis
         */
        ErrorAdj = (int)(((long)DeltaY << 16) / (long)DeltaX);
        /* Draw all pixels other than the first and last */
        while (--DeltaX != 0)
        {
            ErrorAccTemp = ErrorAcc; /* remember currrent accumulated error */
            ErrorAcc += ErrorAdj;    /* calculate error for next pixel */
            if (ErrorAcc > 65535)
            {
                /* The error accumulator turned over, so advance the Y coord */
                ErrorAcc = ErrorAcc & 0x0000ffff;
                Y0++;
                // System.out.println("turn over");
            }
            X0 += XDir; /* X-major, so always advance X */
            /*
             * The IntensityBits most significant bits of ErrorAcc give us
             * the intensity weighting for this pixel, and the complement of
             * the weighting for the paired pixel
             */
            Weighting = ErrorAcc >> IntensityShift;

            PaintColoring::CColor &wi = color.GetColor(Weighting);
            colorWeighting.r = wi.r;
            colorWeighting.g = wi.g;
            colorWeighting.b = wi.b;
            colorWeighting.a = wi.a;
            canvas.PutPixel(X0, Y0, colorWeighting);

            Weighting = Weighting ^ WeightingComplementMask;
            canvas.PutPixel(X0, Y0 + 1, colorWeighting);
        }
    }

    /*
     * Draw the final pixel, which is always exactly intersected by the line
     * and needs no weighting
     */
    canvas.PutPixel(X1, Y1, c);
}

void Painting::DrawWuBlendedLine(Canvas &canvas, int X0, int Y0, int X1, int Y1, PaintColoring::WuColor color)
{
    if (X0 < 0 || X0 > width - 1 || X1 < 0 || X1 > width - 1)
        return;
    if (Y0 < 0 || Y0 > height - 1 || Y1 < 0 || Y1 > height - 1)
        return;

    I = 0.0f;

    /* Make sure the line runs top to bottom */
    if (Y0 > Y1)
    {
        Temp = Y0;
        Y0 = Y1;
        Y1 = Temp;
        Temp = X0;
        X0 = X1;
        X1 = Temp;
    }

    // Draw the initial pixel, which is always exactly intersected by the
    // line and needs no weighting
    Color c = CLITERAL(Color){(unsigned char)color.color.r, (unsigned char)color.color.g, (unsigned char)color.color.b, (unsigned char)color.color.a};
    canvas.PutPixel(X0, Y0, c);

    DeltaX = X1 - X0;

    if (DeltaX >= 0)
    {
        XDir = 1;
    }
    else
    {
        XDir = -1;
        DeltaX = -DeltaX; /* make DeltaX positive */
    }

    // Special-case horizontal, vertical, and diagonal lines, which
    // require no weighting because they go right through the center of
    // every pixel
    DeltaY = Y1 - Y0;
    if (DeltaY == 0)
    {
        /* Horizontal line */
        while (DeltaX-- != 0)
        {
            X0 += XDir;
            canvas.PutPixel(X0, Y0, c);
        }
        return;
    }

    if (DeltaX == 0)
    {
        /* Vertical line */
        do
        {
            Y0++;
            canvas.PutPixel(X0, Y0, c);
        } while (--DeltaY != 0);
        return;
    }

    if (DeltaX == DeltaY)
    {
        /* Diagonal line */
        do
        {
            X0 += XDir;
            Y0++;
            canvas.PutPixel(X0, Y0, c);
        } while (--DeltaY != 0);
        return;
    }

    /* Line is not horizontal, diagonal, or vertical */
    ErrorAcc = 0; /* initialize the line error accumulator to 0 */
    /* # of bits by which to shift ErrorAcc to get intensity level */
    IntensityShift = 16 - color.intensityBits;
    /* Mask used to flip all bits in an intensity weighting, producing the
        result (1 - intensity weighting) */
    WeightingComplementMask = color.numIntensityLevels - 1;

    /* Is this an X-major or Y-major line? */
    if (DeltaY > DeltaX)
    {
        /* Y-major line; calculate 16-bit fixed-point fractional part of a
            pixel that X advances each time Y advances 1 pixel, truncating the
            result so that we won't overrun the endpoint along the X axis */
        ErrorAdj = (int)(((long)DeltaX << 16) / (long)DeltaY);

        /* Draw all pixels other than the first and last */
        while (--DeltaY != 0)
        {
            ErrorAccTemp = ErrorAcc; /* remember currrent accumulated error */
            ErrorAcc += ErrorAdj;    /* calculate error for next pixel */
            if (ErrorAcc > 65535)
            {
                /* The error accumulator turned over, so advance the X coord */
                ErrorAcc = ErrorAcc & 0x0000ffff;
                X0 += XDir;
            }
            Y0++; /* Y-major, so always advance Y */
            /* The IntensityBits most significant bits of ErrorAcc give us the
                intensity weighting for this pixel, and the complement of the
                weighting for the paired pixel */
            Weighting = ErrorAcc >> IntensityShift;

            // get background color
            cp = canvas.GetPixel(X0 + XDir, Y0);

            I = (float)Weighting / (float)color.numIntensityLevels; // I = intensity

            cc.r = (int)(cp.r - ((cp.r - c.r) * I));
            cc.g = (int)(cp.g - ((cp.g - c.g) * I));
            cc.b = (int)(cp.b - ((cp.b - c.b) * I));
            cc.a = 255;

            canvas.PutPixel(X0 + XDir, Y0, cc);

            // get background color
            cp = canvas.GetPixel(X0, Y0);

            I = 1.0f - I;

            cc.r = (int)(cp.r - ((cp.r - c.r) * I));
            cc.g = (int)(cp.g - ((cp.g - c.g) * I));
            cc.b = (int)(cp.b - ((cp.b - c.b) * I));

            canvas.PutPixel(X0, Y0, cc);
        }

        /* Draw the final pixel, which is always exactly intersected by the line
        and doesn't need weighting */
        canvas.PutPixel(X1, Y1, c);

        return;
    }

    // It's an X-major line; calculate 16-bit fixed-point fractional part of a
    // pixel that Y advances each time X advances 1 pixel, truncating the
    // result to avoid overrunning the endpoint along the X axis
    ErrorAdj = (int)(((long)DeltaY << 16) / (long)DeltaX);

    while (--DeltaX != 0)
    {
        ErrorAccTemp = ErrorAcc; /* remember currrent accumulated error */
        ErrorAcc += ErrorAdj;    /* calculate error for next pixel */
        if (ErrorAcc > 65535)
        {
            /* The error accumulator turned over, so advance the Y coord */
            ErrorAcc = ErrorAcc & 0x0000ffff;
            Y0++;
            // System.out.println("turn over");
        }
        X0 += XDir; /* X-major, so always advance X */
        /* The IntensityBits most significant bits of ErrorAcc give us the
            intensity weighting for this pixel, and the complement of the
            weighting for the paired pixel */
        Weighting = ErrorAcc >> IntensityShift;

        // get background color
        cp = canvas.GetPixel(X0, Y0 + 1);

        I = (float)Weighting / (float)color.numIntensityLevels; // I = intensity

        cc.r = (int)(cp.r - ((cp.r - c.r) * I));
        cc.g = (int)(cp.g - ((cp.g - c.g) * I));
        cc.b = (int)(cp.b - ((cp.b - c.b) * I));
        cc.a = 255;

        canvas.PutPixel(X0, Y0 + 1, cc);

        // get background color
        cp = canvas.GetPixel(X0, Y0);

        I = 1.0f - I;

        cc.r = (int)(cp.r - ((cp.r - c.r) * I));
        cc.g = (int)(cp.g - ((cp.g - c.g) * I));
        cc.b = (int)(cp.b - ((cp.b - c.b) * I));

        canvas.PutPixel(X0, Y0, cc);
    }

    // Draw the final pixel, which is always exactly intersected by the line
    // and thus needs no weighting
    canvas.PutPixel(X1, Y1, c);
}

void Painting::DrawZWuBlendedLine(Canvas &canvas, int X0, int Y0, int X1, int Y1, float zP, float zQ, PaintColoring::WuColor color)
{
    if (X0 < 0 || X0 > width - 1 || X1 < 0 || X1 > width - 1)
        return;
    if (Y0 < 0 || Y0 > height - 1 || Y1 < 0 || Y1 > height - 1)
        return;

    I = 0.0f;

    /* Make sure the line runs top to bottom */
    if (Y0 > Y1)
    {
        Temp = Y0;
        Y0 = Y1;
        Y1 = Temp;
        Temp = X0;
        X0 = X1;
        X1 = Temp;
        Tempf = zP;
        zP = zQ;
        zQ = Tempf;
    }

    zrP = 1.0f / zP;
    zrQ = 1.0f / zQ;
    dzr = zrQ - zrP;
    dx = (X1 - X0);
    dy = (Y1 - Y0);
    z = zrP;
    dzdx = dzr / dx;
    dzdy = dzr / dy;

    // Draw the initial pixel, which is always exactly intersected by the
    // line and needs no weighting
    Color c = CLITERAL(Color){(unsigned char)color.color.r, (unsigned char)color.color.g, (unsigned char)color.color.b, (unsigned char)color.color.a};
    zl = zb.getIndex(X0, Y0);
    if (zl >= 0)
    {
        zstatus = zb.setZ(zl, z, false);
        if (zstatus == 1)
        {
            canvas.PutPixel(X0, Y0, c);
        }
    }

    DeltaX = X1 - X0;

    if (DeltaX >= 0)
    {
        XDir = 1;
    }
    else
    {
        XDir = -1;
        DeltaX = -DeltaX; /* make DeltaX positive */
        dzdx = -dzdx;
    }

    // Special-case horizontal, vertical, and diagonal lines, which
    // require no weighting because they go right through the center of
    // every pixel
    DeltaY = Y1 - Y0;
    if (DeltaY == 0)
    {
        /* Horizontal line */
        while (DeltaX-- != 0)
        {
            X0 += XDir;
            zl = zb.getIndex(X0, Y0);
            if (zl < 0)
                break; // pixel off screen
            zstatus = zb.setZ(zl, z, false);
            if (zstatus == 1)
            {
                canvas.PutPixel(X0, Y0, c); // pixel closer
            }
            z += dzdx;
        }
        return;
    }

    if (DeltaX == 0)
    {
        /* Vertical line */
        do
        {
            Y0++;
            zl = zb.getIndex(X0, Y0);
            if (zl < 0)
                break; // pixel off screen
            zstatus = zb.setZ(zl, z, false);
            if (zstatus == 1)
            {
                canvas.PutPixel(X0, Y0, c); // pixel closer
            }
            z += dzdy;
        } while (--DeltaY != 0);
        return;
    }

    if (DeltaX == DeltaY)
    {
        /* Diagonal line */
        do
        {
            X0 += XDir;
            Y0++;
            zl = zb.getIndex(X0, Y0);
            if (zl < 0)
                break; // pixel off screen
            zstatus = zb.setZ(zl, z, false);
            if (zstatus == 1)
            {
                canvas.PutPixel(X0, Y0, c); // pixel closer
            }
            z += dzdx;
        } while (--DeltaY != 0);
        return;
    }

    /* Line is not horizontal, diagonal, or vertical */
    ErrorAcc = 0; /* initialize the line error accumulator to 0 */
    /* # of bits by which to shift ErrorAcc to get intensity level */
    IntensityShift = 16 - color.intensityBits;
    /*
     * Mask used to flip all bits in an intensity weighting, producing the
     * result (1 - intensity weighting)
     */
    WeightingComplementMask = color.numIntensityLevels - 1;

    // ===================================================
    /* Is this an X-major or Y-major line? */
    if (DeltaY > DeltaX)
    {

        /*
         * Y-major line; calculate 16-bit fixed-point fractional part of a
         * pixel that X advances each time Y advances 1 pixel, truncating the
         * result so that we won't overrun the endpoint along the X axis
         */
        ErrorAdj = (int)(((long)DeltaX << 16) / (long)DeltaY);
        /* Draw all pixels other than the first and last */
        while (--DeltaY != 0)
        {
            ErrorAccTemp = ErrorAcc; /* remember currrent accumulated error */
            ErrorAcc += ErrorAdj;    /* calculate error for next pixel */
            if (ErrorAcc > 65535)
            {
                /* The error accumulator turned over, so advance the X coord */
                ErrorAcc = ErrorAcc & 0x0000ffff;
                X0 += XDir;
            }
            Y0++; /* Y-major, so always advance Y */
            /*
             * The IntensityBits most significant bits of ErrorAcc give us the
             * intensity weighting for this pixel, and the complement of the
             * weighting for the paired pixel
             */
            Weighting = ErrorAcc >> IntensityShift;

            // get background color
            cp = canvas.GetPixel(X0 + XDir, Y0);

            I = (float)Weighting / (float)color.numIntensityLevels; // I = intensity

            cc.r = (int)(cp.r - ((cp.r - c.r) * I));
            cc.g = (int)(cp.g - ((cp.g - c.g) * I));
            cc.b = (int)(cp.b - ((cp.b - c.b) * I));
            cc.a = 255;

            zl = zb.getIndex(X0 + XDir, Y0);
            if (zl >= 0)
            {
                zstatus = zb.setZ(zl, z, false);
                if (zstatus == 1)
                {
                    canvas.PutPixel(X0 + XDir, Y0, cc); // pixel closer
                }
            }

            // get background color
            cp = canvas.GetPixel(X0, Y0);

            I = 1.0f - I;

            cc.r = (int)(cp.r - ((cp.r - c.r) * I));
            cc.g = (int)(cp.g - ((cp.g - c.g) * I));
            cc.b = (int)(cp.b - ((cp.b - c.b) * I));
            cc.a = 255;

            zl = zb.getIndex(X0, Y0);
            if (zl >= 0)
            {
                zstatus = zb.setZ(zl, z, false);
                if (zstatus == 1)
                {
                    canvas.PutPixel(X0, Y0, cc); // pixel closer
                }
            }
            z += dzdy;
        }

        /*
         * Draw the final pixel, which is always exactly intersected by the line
         * and so needs no weighting
         */
        zl = zb.getIndex(X1, Y1);
        if (zl < 0)
            return; // pixel off screen
        zstatus = zb.setZ(zl, z, false);
        if (zstatus == 1)
        {
            canvas.PutPixel(X1, Y1, c); // pixel closer
        }
        return;
    }

    /*
     * It's an X-major line; calculate 16-bit fixed-point fractional part of a
     * pixel that Y advances each time X advances 1 pixel, truncating the
     * result to avoid overrunning the endpoint along the X axis
     */
    ErrorAdj = (int)(((long)DeltaY << 16) / (long)DeltaX);
    /* Draw all pixels other than the first and last */
    while (--DeltaX != 0)
    {
        ErrorAccTemp = ErrorAcc; /* remember currrent accumulated error */
        ErrorAcc += ErrorAdj;    /* calculate error for next pixel */
        if (ErrorAcc > 65535)
        {
            /* The error accumulator turned over, so advance the Y coord */
            ErrorAcc = ErrorAcc & 0x0000ffff;
            Y0++;
            // System.out.println("turn over");
        }
        X0 += XDir; /* X-major, so always advance X */
        /*
         * The IntensityBits most significant bits of ErrorAcc give us the
         * intensity weighting for this pixel, and the complement of the
         * weighting for the paired pixel
         */
        Weighting = ErrorAcc >> IntensityShift;

        // get background color
        cp = canvas.GetPixel(X0, Y0 + 1);

        I = (float)Weighting / (float)color.numIntensityLevels; // I = intensity

        cc.r = (int)(cp.r - ((cp.r - c.r) * I));
        cc.g = (int)(cp.g - ((cp.g - c.g) * I));
        cc.b = (int)(cp.b - ((cp.b - c.b) * I));
        cc.a = 255;

        zl = zb.getIndex(X0, Y0 + 1);
        if (zl >= 0)
        {
            zstatus = zb.setZ(zl, z, false);
            if (zstatus == 1)
            {
                canvas.PutPixel(X0, Y0 + 1, cc); // pixel closer
            }
        }

        // get background color
        cp = canvas.GetPixel(X0, Y0);

        I = 1.0f - I;

        cc.r = (int)(cp.r - ((cp.r - c.r) * I));
        cc.g = (int)(cp.g - ((cp.g - c.g) * I));
        cc.b = (int)(cp.b - ((cp.b - c.b) * I));
        cc.a = 255;

        zl = zb.getIndex(X0, Y0);
        if (zl >= 0)
        {
            zstatus = zb.setZ(zl, z, false);
            if (zstatus == 1)
            {
                canvas.PutPixel(X0, Y0, cc); // pixel closer
            }
        }
        z += dzdx;
    }
    // ===================================================

    /*
     * Draw the final pixel, which is always exactly intersected by the line
     * and so needs no weighting
     */
    zl = zb.getIndex(X1, Y1);
    if (zl < 0)
        return; // pixel off screen
    zstatus = zb.setZ(zl, z, false);
    if (zstatus == 1)
    {
        canvas.PutPixel(X1, Y1, c); // pixel closer
    }
}

long Painting::DrawFlatTriangle(Canvas &canvas, GradientInterpolation &g,
                                EdgeInterpolation &TM, EdgeInterpolation &TB, EdgeInterpolation &MB,
                                bool blnMiddleIsLeft, PaintColoring::CColor &color)
{
    ////////////////////////////////////////////////
    // Top half of triangle
    ////////////////////////////////////////////////
    if (blnMiddleIsLeft)
    {
        Left = TM;
        Right = TB;
    }
    else
    {
        Left = TB;
        Right = TM; // works
    }

    // Middle indicates height
    int Height = TM.Height;
    while (Height-- > 0)
    {
        DrawZFlatScanLine(canvas, g, Left, Right, color);
        Left.Step();
        Right.Step();
    }

    ////////////////////////////////////////////////
    // Bottom half of triangle
    ////////////////////////////////////////////////
    if (blnMiddleIsLeft)
    {
        Left = MB;
        Right = TB;
    }
    else
    {
        Left = TB;
        Right = MB; // works
    }

    Height = MB.Height;
    while (Height-- > 0)
    {
        DrawZFlatScanLine(canvas, g, Left, Right, color);
        Left.Step();
        Right.Step();
    }

    return 0; // PixelsProcessed;
}

void Painting::DrawZFlatScanLine(Canvas &canvas, GradientInterpolation &g,
                                 EdgeInterpolation &l, EdgeInterpolation &r,
                                 PaintColoring::CColor &color)
{
    ca.r = color.r;
    ca.g = color.g;
    ca.b = color.b;
    ca.a = color.a;

    int XStart = (int)std::ceil(l.X);
    float XPrestep = XStart - l.X;
    int Width = (int)std::ceil(r.X) - XStart;

    // -----------------------------------------------
    // Setup Z-Depth gradients and counters
    // -----------------------------------------------
    int zstatus;
    // 1/z zstart + (adjust * dz/dx)
    float OneOverZ = l.OneOverZ + (XPrestep * g.dOneOverZdX);
    // Get the starting zbuffer index for the current scanline(l.Y) and X position
    int zl = zb.getIndex(XStart, l.Y);
    if (zl < 0)
        return; // Forget it. scanline or pixel is off the screen.

    int x = XStart; // A hack to show edges in black
    while (Width-- > 0)
    {
        // -----------------------------------------------
        // -- Draw pixel if closer
        // -----------------------------------------------
        zstatus = zb.setZ(zl, OneOverZ, false);
        if (zstatus == 1)
        {
            // pixel closer
            if (x == XStart)
                canvas.PutPixel(XStart, l.Y, BLACK);
            else
                canvas.PutPixel(XStart, l.Y, ca);
        }
        else
        {
            // Just hacking. It creates a neat effect.
            // setPixel(XStart, l.Y, RED);
        }

        // -----------------------------------------------
        // -- increment gradients and counters
        // -----------------------------------------------
        XStart++;                  // move to next pixel
        zl++;                      // move to next zbuffer cell
        OneOverZ += g.dOneOverZdX; // increment z gradient
    }
}
