#include "Canvas.h"
#include <algorithm>
#include <iostream>

Canvas::Canvas(int width, int height) : width(width), height(height)
{
    // 1. Create a CPU image buffer (Format: RGBA 8-bit per channel)
    canvas = GenImageColor(width, height, BLACK);

    // 2. Load an empty texture to the GPU that we will update every frame
    targetTexture = LoadTextureFromImage(canvas);
}

Canvas::~Canvas()
{
    std::cout << "Destroying Canvas" << std::endl;
    UnloadTexture(targetTexture);
    UnloadImage(canvas);
}

void Canvas::Clear()
{
    ImageClearBackground(&canvas, clearColor);
}

void Canvas::SetClearColor(Color c)
{
    clearColor = c;
}

void Canvas::PutPixel(int x, int y, Color c)
{
    if (x >= 0 && x < width && y >= 0 && y < height)
    {
        ((Color *)canvas.data)[y * width + x] = c;
    }
}

Color Canvas::GetPixel(int x, int y)
{
    if (x < 0 || x > width - 1 || y < 0 || y > height - 1)
    {
        return BLACK;
    }

    return ((Color *)canvas.data)[y * width + x];
}

/// @brief Update the existing GPU texture with new CPU pixel data
void Canvas::Update()
{
    UpdateTexture(targetTexture, canvas.data);
}

/// @brief Draw software buffer to the screen
void Canvas::Blit(int x, int y)
{
    DrawTexture(targetTexture, x, y, WHITE);
}

// ======================== Primitives ========================================
void Canvas::DrawBresenhamLine(int xP, int yP, int xQ, int yQ, Color c)
{
    if (xP < 0 || xP > width - 1 || xQ < 0 || xQ > width - 1)
        return;
    if (yP < 0 || yP > height - 1 || yQ < 0 || yQ > height - 1)
        return;

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
            PutPixel(x, y, c);
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
            PutPixel(x, y, c);
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

/// @brief NOT WORKING
void Canvas::DrawWuIndexedLine(int X0, int Y0, int X1, int Y1, WuColor color)
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
    PutPixel(X0, Y0, c);

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
            PutPixel(X0, Y0, c);
        }
        return;
    }

    if (DeltaX == 0)
    {
        /* Vertical line */
        do
        {
            Y0++;
            PutPixel(X0, Y0, c);
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
            PutPixel(X0, Y0, c);
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

            CColor &wi = color.GetColor(Weighting);
            colorWeighting.r = wi.r;
            colorWeighting.g = wi.g;
            colorWeighting.b = wi.b;
            colorWeighting.a = wi.a;
            PutPixel(X0, Y0, colorWeighting);

            Weighting = Weighting ^ WeightingComplementMask;
            PutPixel(X0 + XDir, Y0, colorWeighting);
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

            CColor &wi = color.GetColor(Weighting);
            colorWeighting.r = wi.r;
            colorWeighting.g = wi.g;
            colorWeighting.b = wi.b;
            colorWeighting.a = wi.a;
            PutPixel(X0, Y0, colorWeighting);

            Weighting = Weighting ^ WeightingComplementMask;
            PutPixel(X0, Y0 + 1, colorWeighting);
        }
    }

    /*
     * Draw the final pixel, which is always exactly intersected by the line
     * and needs no weighting
     */
    PutPixel(X1, Y1, c);
}

void Canvas::DrawWuBlendedLine(int X0, int Y0, int X1, int Y1, WuColor color)
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
    PutPixel(X0, Y0, c);

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

    //  Special-case horizontal, vertical, and diagonal lines, which
    //  require no weighting because they go right through the center of
    //  every pixel
    DeltaY = Y1 - Y0;
    if (DeltaY == 0)
    {
        /* Horizontal line */
        while (DeltaX-- != 0)
        {
            X0 += XDir;
            PutPixel(X0, Y0, c);
        }
        return;
    }

    if (DeltaX == 0)
    {
        /* Vertical line */
        do
        {
            Y0++;
            PutPixel(X0, Y0, c);
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
            PutPixel(X0, Y0, c);
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

    Color cc{};
    Color cp{};

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
            cp = GetPixel(X0 + XDir, Y0);

            I = (float)Weighting / (float)color.numIntensityLevels; // I = intensity

            cc.r = (int)(cp.r - ((cp.r - c.r) * I));
            cc.g = (int)(cp.g - ((cp.g - c.g) * I));
            cc.b = (int)(cp.b - ((cp.b - c.b) * I));
            cc.a = 255;

            PutPixel(X0 + XDir, Y0, cc);

            // get background color
            cp = GetPixel(X0, Y0);

            I = 1.0f - I;

            cc.r = (int)(cp.r - ((cp.r - c.r) * I));
            cc.g = (int)(cp.g - ((cp.g - c.g) * I));
            cc.b = (int)(cp.b - ((cp.b - c.b) * I));

            PutPixel(X0, Y0, cc);
        }

        /* Draw the final pixel, which is always exactly intersected by the line
        and doesn't need weighting */
        PutPixel(X1, Y1, c);

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
        cp = GetPixel(X0, Y0 + 1);

        I = (float)Weighting / (float)color.numIntensityLevels; // I = intensity

        cc.r = (int)(cp.r - ((cp.r - c.r) * I));
        cc.g = (int)(cp.g - ((cp.g - c.g) * I));
        cc.b = (int)(cp.b - ((cp.b - c.b) * I));
        cc.a = 255;

        PutPixel(X0, Y0 + 1, cc);

        // get background color
        cp = GetPixel(X0, Y0);

        I = 1.0f - I;

        cc.r = (int)(cp.r - ((cp.r - c.r) * I));
        cc.g = (int)(cp.g - ((cp.g - c.g) * I));
        cc.b = (int)(cp.b - ((cp.b - c.b) * I));

        PutPixel(X0, Y0, cc);
    }

    // Draw the final pixel, which is always exactly intersected by the line
    // and thus needs no weighting
    PutPixel(X1, Y1, c);
}
