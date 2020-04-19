#pragma once

#include <cmath>

namespace  
{
	#define STRINGIFY(s) #s 
	#define MYASSERT( p ) { if(!p) MessageBoxA( NULL, STRINGIFY(p), "Err", MB_OK); }

	#define SAFE_DELETE(a) if( (a) != NULL ) delete (a); (a) = NULL;
	#define DXASSERT(a) MYASSERT(SUCCEEDED(a))

	#define BIG_EPSILON				0.00001f    
	#define SMALLL_EPSILON			0.000001f    
	#define PI						3.141592654    
	#define HALFPI					1.570796327    
	#define TWOPI					6.283185307    
	#define DEG_TO_RAD(x)			((1.74532952e-2)*x)    
	#define RAD_TO_DEG(x)			(57.29577951*x)   
	#define ROOT_2					1.414213562    
	#define SIN_45					0.707106781
	
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
	
	float clampf(float d, float min, float max) 
	{
		const float t = d < min ? min : d;

		return t > max ? max : t;
	}

	template <typename T>
	T Lerp(T min, T max, float blend)
	{
		return min + (max - min) * blend;
	}

	template <typename T>
	float InvLerp(T min, T max, T val)
	{
		return (val - min) / (max - min);
	}

	template <typename T>
	T Remap(T iMin, T iMax, T oMin, T oMax, T value)
	{
		float blend = InvLerp(iMin, iMax, value);

		return Lerp(oMin, oMax, blend);
	}

	Vector3 RLerp(Vector3 v1, Vector3 v2, float t)
	{
		v1.Normalize();
		v2.Normalize();

		float dot = v1.Dot(v2);
		
		if (dot < 0.0f) 
		{
			v2 = -v2;
			dot = -dot;
		}

		const float DOT_THRESHOLD = 0.9995f;

		if ( dot > DOT_THRESHOLD )
		{
			Vector3 result = Lerp(v1, v2, t);
			
			result.Normalize();

			return result;
		}
		 
		// Since dot is in range [0, DOT_THRESHOLD], acos is safe
		float theta_0 = acosf(dot);        // theta_0 = angle between input vectors
		float theta = theta_0 * t;          // theta = angle between v0 and result
		float sin_theta = sinf(theta);     // compute this value only once
		float sin_theta_0 = sinf(theta_0); // compute this value only once
 
		float s0 = cosf(theta) - dot * sin_theta / sin_theta_0;  // == sin(theta_0 - theta) / sin(theta_0)
		float s1 = sin_theta / sin_theta_0;

		Vector3 result = (s0 * v1) + (s1 * v2);

		result.Normalize();

		return result;
	}

	Vector3 Slerp(Vector3 v0, Vector3 v1, float t)
	{
		//; Inputs are : unit vectors v0 and v1, scalar t
		//	; v0 and v1 are linearly independent
		//
		//	Let θ0 = acos(v0 · v1)
		//	Let θ = tθ0
		//	Let v2 = Normalize(v1 - (v0 · v1)v0)
		//	return v0cosθ + v2sinθ

		v0.Normalize();
		v1.Normalize();

		float dot = v0.Dot(v1);

		float theta0 = acosf(dot);

		float theta = theta0 * t;

		Vector3 v2 = v1 - dot * v0;

		v2.Normalize();

		return v0 * cosf(theta) + v2 * sinf(theta);

	}

	Vector3 Slerp3(Vector3 v0, Vector3 v1, double t)
	{
		Quaternion q0 = Quaternion(v0);
		Quaternion q1 = Quaternion(v1);

		Quaternion q2 = Quaternion::Slerp(q0, q1, t);

		Vector3 result = Vector3(q2.x, q2.y, q2.z);
		result.Normalize();
		return result;
	}
		
	Vector3 Slerp2(Vector3 v0, Vector3 v1, double t)
	{
		v0.Normalize();
		v1.Normalize();

		// v0 and v1 should be unit length or else
		// something broken will happen.

		// Compute the cosine of the angle between the two vectors.
		double dot = v0.Dot(v1);

		const double DOT_THRESHOLD = 0.9995;

		if (dot > DOT_THRESHOLD) 
		{
			// If the inputs are too close for comfort, linearly interpolate
			// and normalize the result.

			Vector3 result = v0 + t * (v1 - v0);

			result.Normalize();

			return result;
		}

		clampf(dot, -1, 1);           // Robustness: Stay within domain of acos()

		double theta_0 = acos(dot);  // theta_0 = angle between input vectors

		double theta = theta_0 * t;    // theta = angle between v0 and result 

		Vector3 v2 = v1 - v0 * dot;

		v2.Normalize();              // { v0, v2 } is now an orthonormal basis

		return v0 * cos(theta) + v2 * sin(theta);
	}


	Vector3 Nlerp(Vector3 start, Vector3 end, float percent)
	{
		Vector3 result = Lerp(start, end, percent);

		result.Normalize();

		return result;
	}

	Vector3 PitchYawRollFromVector( Vector3 v )
	{
		Vector3 pyr;
		Vector3 up = Vector3(0.0f, 1.0f, 0.0f);

		v.Normalize(); 

		// Yaw is the bearing of the forward vector's shadow in the xz plane.
		float yaw = atan2f(v.x, v.z);

		// Pitch is the altitude of the forward vector off the xz plane, toward the down direction.
		float pitch = -asinf(v.y);

		//// Find the vector in the xz plane 90 degrees to the right of our bearing.
		//float planeRightX = sinf(yaw);
		//float planeRightZ = -cosf(yaw);

		// Roll is the rightward lean of our up vector, computed here using a dot product.
		float roll = 0.0f;// asinf(up.x * planeRightX + up.z * planeRightZ);

		//// If we're twisted upside-down, return a roll in the range +-(pi/2, pi)
		//if (up.z < 0)
		//{
		//	roll = roll > 0 ? PI - roll : -PI - roll;
		//}

		// Convert radians to degrees.
		pyr.x = RAD_TO_DEG(pitch);
		pyr.y = RAD_TO_DEG(yaw);
		pyr.z = RAD_TO_DEG( roll );

		return pyr;

	}


}
