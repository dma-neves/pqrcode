#ifndef VECTOR2D_HPP
#define VECTOR2D_HPP

#include <math.h>

class Vector2D
{
public:
    double x,y;

    Vector2D(double x, double y) : x(x), y(y)
    {}

    Vector2D()
    {
        x = 0;
        y = 0;
    }
    
    double crossProd(Vector2D v)
    {
        return x * v.y - y * v.x;
    }

    double norm()
    {
        return sqrt(x * x + y * y);
    }

    Vector2D normalize()
    {
        double n = norm();
        return Vector2D(x/n, y/n);
    }


    /*
        Returns the smallest angle between this vector and v, including the direction of rotation, represented by
        the sign of the returned value. + being counter-clock-wise and - being clock-wise. Therefore, this
        represents the angle that rotates v1 to v2
    */
    double angleTo(Vector2D v)
    {
        if((x == 0 && v.x == 0) || (y == 0 && v.y == 0) || x/v.x == y/v.y)
        {
            if (x * v.x < 0)
                return M_PI;
            else
                return 0;
        }

        return asin( normalize().crossProd( v.normalize() ) );
    }

    Vector2D operator *(double scale)
    {
        return Vector2D(x*scale, y*scale);
    }

    Vector2D operator +(Vector2D v)
    {
        return Vector2D(x+v.x, y+v.y);
    }

    Vector2D operator -(Vector2D v)
    {
        return Vector2D(x-v.x, y-v.y);
    }

    Vector2D rotate(double angle)
    {
        return Vector2D(
            x * cos(angle) - y * sin(angle),
            x * sin(angle) + y * cos(angle)
        );
    }

    Vector2D rotateAroundPoint(double angle, Vector2D center)
    {
        double s = sin(angle);
        double c = cos(angle);

        // translate point back to origin:
        double x = this->x - center.x;
        double y = this->y - center.y;

        // rotate point
        double xnew = x * c - y * s;
        double ynew = x * s + y * c;

        // translate point back:
        x = xnew + center.x;
        y = ynew + center.y;

        return Vector2D(x, y);
    }

    Vector2D shear(double x_shear, double y_shear)
    {
        return Vector2D(
            x + x_shear * y,
            y_shear * x + y
        );
    }
};

#endif