#pragma once

#include <cmath>

namespace  
{
	#define PI                        3.141592654    
	#define HALFPI                    1.570796327    
	#define TWOPI                     6.283185307    
	#define RADIANS_PER_DEGREE        1.74532952e-2    
	#define DEGREES_PER_RADIAN        57.29577951   
	#define ROOT_2                    1.414213562    
	#define SIN_45                    0.707106781
	
	static double const pi = 3.141592654;
	static double const halfpi = pi / 2.0;
	static double const pi_over_four = pi / 4.0;
	static double const twopi = 2.0 * pi;
	static double const radians_per_degree = pi / 180.0;
	static double const degrees_per_radian = 180.0 / pi;
	static double const root_2 = sqrt(2.0);
	static double const sin_45 = root_2 / 2.0;
	static double const sin_30 = 0.5;
	static double const cos_30 = sqrt(0.75);
	static double const BIG_FLOAT = 1.0e20;
	
	double my_sqrt(double a, int n)
	{
		double x = a;
	
		if (x <= 0.0)
			return x;
	
		for (int i = 0; i < n; i++)
		{
			//cout << "The guess is " << x << endl;
			x = 0.5 * (a / x + x);
		}
	
		return x;
	}
	
	typedef union
	{
		double fp;
		struct
		{
			unsigned long lo;
			unsigned long hi;
		}n;
	} hack_structure;
	
	// Model for fast square root with optimal first guess
	double my_fastsqrt(double a)
	{
		short expo;
		double factor = root_2 / 2;
		const double A = 0.417319242;
		const double B = 0.590178532;
		hack_structure x;
		double root;
		// check for negative or zero    
		if (a <= 0)
			return 0;
		x.fp = a;
	
		// grab the exponent    
		expo = (short)(x.n.hi >> 20);
		expo -= 0x3fe;
	
		// normalize the number    
		x.n.hi &= 0x000fffff;
		x.n.hi += 0x3fe00000;
	
		// get square root of normalized number    
		// generate first guess    
		root = A + B * x.fp;
	
		// iterate three times (probably overkill)
		root = 0.5 * (x.fp / root + root);
		root = 0.5 * (x.fp / root + root);
		root = 0.5 * (x.fp / root + root);
		// now rebuild the result    
		x.fp = root;
		// force the exponent to be even    
		if (expo & 1)
		{
			x.fp *= factor;
			++expo;
		}
	
		// halve the exponent    
		if (expo < 0)
			expo = expo / 2;
		else
			expo = (expo + 1) / 2;
	
		// put it back    
		expo += 0x3fe;
		x.n.hi &= 0x000fffff;
		x.n.hi += ((unsigned long)expo << 20);
	
		return x.fp;
	}
	
	unsigned short lisqrt(unsigned long a)
	{
		unsigned long rem = 0;
		unsigned long root = 0;
		for (int i = 0; i < 16; i++)
		{
			root <<= 1;
			rem = ((rem << 2) + (a >> 30));
			a <<= 2;
			root++;
			if (root <= rem)
			{
				rem -= root;
				root++;
			}
			else
				root--;
		}
	
		return (unsigned short)(root >> 1);
	}
	
	/* Rational approximation for atan(x)
	* This polynomial uses a minimaxed 5th-order
	* ratio of polynomials.  Input angles are restricted
	* to values no greater than 15 degrees.  Accuracy is
	* better than 4e-8 over the full range of inputs.
	*/
	double arctan(double x)
	{
		// constants for segmentation  
		static const double b = pi / 6;
		static const double k = tan(b);
		static const double b0 = pi / 12;
		static const double k0 = tan(b0);
	
		// constants for rational polynomial  
		static const double A = 0.999999020228907;
		static const double B = 0.257977658811405;
		static const double C = 0.59120450521312;
		double ang;  double x2;
		int comp = FALSE;
		int hi_seg = FALSE;
	
		// make argument positive  
		int sign = (x < 0);
		x = abs(x);
	
		// limit argument to 0..1  
		if (x > 1)
		{
			comp = TRUE;
			x = 1 / x;
		}
	
		// determine segmentation  
		if (x > k0)
		{
			hi_seg = TRUE;
			x = (x - k) / (1 + k * x);
		}
	
		/* argument is now < tan(15 degrees)
		* approximate the function   */
		x2 = x * x;
		ang = x * (A + B * x2) / (1 + C * x2);
	
		// now restore offset if needed  
		if (hi_seg)
			ang += b;
	
		// restore complement if needed  
		if (comp)
			ang = halfpi - ang;
	
		// restore sign if needed  
		if (sign)
			return -ang;
		else
			return ang;
	}
	
	// Four-quadrant arctan 
	double arctan2(double y, double x)
	{
		double retval;
		if (x == 0)
			retval = halfpi;
		else
			retval = atan(abs(y / x));
	
		if (x < 0)
			retval = pi - retval;
	
		if (y < 0)
			retval = -retval;
	
		return retval;
	
	}

	/* Assumes that float is in the IEEE 754 single precision floating point format
	* and that int is 32 bits. */
	float sqrtbithack2(float z) {
		int val_int = *(int*)&z; /* Same bits, but as an int */

		/*
		 * To justify the following code, prove that
		 *
		 * ((((val_int / 2^m) - b) / 2) + b) * 2^m = ((val_int - 2^m) / 2) + ((b + 1) / 2) * 2^m)
		 *
		 * where
		 *
		 * b = exponent bias
		 * m = number of mantissa bits
		 *
		 * .
		 */

		val_int -= 1 << 23;		/* Subtract 2^m. */
		val_int >>= 1;			/* Divide by 2. */
		val_int += 1 << 29;		/* Add ((b + 1) / 2) * 2^m. */

		return *(float*)&val_int; /* Interpret again as float */
	}

	float sqrtbithack(float x)
	{
		//Dependant on IEEE representation and only works for 32 bits
		unsigned int i = *(unsigned int*)&x;
		// adjust bias
		i += 127 << 23;
		// approximation of square root
		i >>= 1;
		return *(float*)&i;
	}

	//Fast inverse square root implementation from Quake III Arena
	float Q_rsqrt(float number)
	{
		long i;
		float x2, y;
		const float threehalfs = 1.5F;

		x2 = number * 0.5F;
		y = number;
		i = *(long*)&y;                       // evil floating point bit level hacking
		i = 0x5f3759df - (i >> 1);               // what the fuck? 
		y = *(float*)&i;
		y = y * (threehalfs - (x2 * y * y));   // 1st iteration
	//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

		return y;
	}

	double fastPow(double a, double b) 
	{
		union 
		{
			double d;
			int x[2];
		} u = { a };

		u.x[1] = (int)(b * (u.x[1] - 1072632447) + 1072632447);
		u.x[0] = 0;

		return u.d;
	}

}
