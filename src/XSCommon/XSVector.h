#pragma once

#include <string>

#ifdef _MSC_VER
	#pragma warning( push )
	#pragma warning( disable : 4201 ) // nonstandard extension used : nameless struct/union
#endif // _MSC_VER

namespace XS {

	//TODO: unit test precision at different ranges (and double conversion/promotion?)
	static bool flcmp( const real32_t &f1, const real32_t &f2, const real32_t epsilon = 0.00001f ) {
		const real32_t delta = std::abs( f2 - f1 );

		return delta < epsilon;
	}
	static bool dblcmp( const real64_t &f1, const real64_t &f2, const real64_t epsilon = 0.000000000000001 ) {
		const real64_t delta = std::abs( f2 - f1 );

		return delta < epsilon;
	}

	struct vector2 {
		union {
			struct {
				real32_t x;
				real32_t y;
			};
			struct {
				real32_t w;
				real32_t h;
			};
			real32_t raw[2];
		};

		// xtors
		vector2( real32_t x = 0.0f, real32_t y = 0.0f )
		: x( x ), y( y )
		{
		}

		// string representation of the vector
		std::string tostring(
			void
		) const;

		// access operators
		inline real32_t& operator[]( const size_t idx ) {
			return raw[idx];
		}

		// clear
		inline void clear( void ) {
			x = y = 0.0f;
		}

		// compare
		static inline bool compare( const vector2 &lhs, const vector2 &rhs ) {
			return (flcmp( lhs.x, rhs.x ) && flcmp( lhs.y, rhs.y ));
		}
		inline bool compare( const vector2 &rhs ) const {
			return (flcmp( x, rhs.x ) && flcmp( y, rhs.y ));
		}
		inline bool operator==( const vector2 &rhs ) {
			return compare( rhs );
		}

		// addition
		inline vector2 operator+( const vector2 &rhs ) const {
			return vector2( x + rhs.x, y + rhs.y );
		}
        inline vector2& operator+=( const vector2& rhs ) {
			x += rhs.x;
			y += rhs.y;
			return *this;
		}

		// subtraction
		inline vector2 operator-( const vector2 &rhs ) const {
			return vector2( x - rhs.x, y - rhs.y );
		}
        inline vector2& operator-=( const vector2& rhs ) {
			x -= rhs.x;
			y -= rhs.y;
			return *this;
		}

		// increment
		inline void increment( void ) {
			x += 1.0f;
			y += 1.0f;
		}

		// decrement
		inline void decrement( void ) {
			x -= 1.0f;
			y -= 1.0f;
		}

		// inverse
		inline void inverse( void ) {
			x = -x;
			y = -y;
		}
		static inline vector2 inverse( const vector2 &in ) {
			return vector2( -in.x, -in.y );
		}

		// scalar multiplication
		inline vector2 operator*( const real32_t scalar ) const {
			return vector2( x * scalar, y * scalar );
		}
		inline vector2& operator*=( const real32_t scalar ) {
			x *= scalar;
			y *= scalar;
			return *this;
		}

		// vector multiplication
		inline vector2 operator*( const vector2 &rhs ) const {
			return vector2( x * rhs.x, y * rhs.y );
		}
		inline vector2& operator*=( const vector2 &rhs ) {
			x *= rhs.x;
			y *= rhs.y;
			return *this;
		}

		// scalar division
		inline vector2 operator/( const real32_t divisor ) const {
			return vector2( x / divisor, y / divisor );
		}
		inline vector2& operator/=( const real32_t divisor ) {
			x /= divisor;
			y /= divisor;
			return *this;
		}

		// vector division
		inline vector2 operator/( const vector2 &rhs ) const {
			return vector2( x / rhs.x, y / rhs.y );
		}
		inline vector2& operator/=( const vector2 &rhs ) {
			x /= rhs.x;
			y /= rhs.y;
			return *this;
		}

		// ???
		static inline vector2 ma( const vector2 &a, real32_t scale, const vector2 &b ) {
			return a + b*scale;
		}

		// linear interpolation
		static inline vector2 lerp( const vector2 &a, real32_t scale, const vector2 &b ) {
			return a + (b-a)*scale;
		}
	};

	struct pvector2 {
		union {
			struct {
				real64_t x;
				real64_t y;
			};
			struct {
				real64_t w;
				real64_t h;
			};
			real64_t raw[2];
		};

		// xtors
		pvector2( real64_t x = 0.0, real64_t y = 0.0 )
		: x( x ), y( y )
		{
		}

		// string representation of the vector
		std::string tostring(
			void
		) const;

		// access operators
		inline real64_t& operator[]( const size_t idx ) {
			return raw[idx];
		}

		// clear
		inline void clear( void ) {
			x = y = 0.0;
		}

		// compare
		static inline bool compare( const pvector2 &lhs, const pvector2 &rhs ) {
			return (dblcmp( lhs.x, rhs.x ) && dblcmp( lhs.y, rhs.y ));
		}
		inline bool compare( const pvector2 &rhs ) const {
			return (dblcmp( x, rhs.x ) && dblcmp( y, rhs.y ));
		}
		inline bool operator==( const pvector2 &rhs ) {
			return compare( rhs );
		}

		// addition
		inline pvector2 operator+( const pvector2 &rhs ) const {
			return pvector2( x + rhs.x, y + rhs.y );
		}
        inline pvector2& operator+=( const pvector2& rhs ) {
			x += rhs.x;
			y += rhs.y;
			return *this;
		}

		// subtraction
		inline pvector2 operator-( const pvector2 &rhs ) const {
			return pvector2( x - rhs.x, y - rhs.y );
		}
        inline pvector2& operator-=( const pvector2& rhs ) {
			x -= rhs.x;
			y -= rhs.y;
			return *this;
		}

		// increment
		inline void increment( void ) {
			x += 1.0;
			y += 1.0;
		}

		// decrement
		inline void decrement( void ) {
			x -= 1.0;
			y -= 1.0;
		}

		// inverse
		inline void inverse( void ) {
			x = -x;
			y = -y;
		}
		static inline pvector2 inverse( const pvector2 &in ) {
			return pvector2( -in.x, -in.y );
		}

		// scalar multiplication
		inline pvector2 operator*( const real64_t scalar ) const {
			return pvector2( x * scalar, y * scalar );
		}
		inline pvector2& operator*=( const real64_t scalar ) {
			x *= scalar;
			y *= scalar;
			return *this;
		}

		// vector multiplication
		inline pvector2 operator*( const pvector2 &rhs ) const {
			return pvector2( x * rhs.x, y * rhs.y );
		}
		inline pvector2& operator*=( const pvector2 &rhs ) {
			x *= rhs.x;
			y *= rhs.y;
			return *this;
		}

		// scalar division
		inline pvector2 operator/( const real64_t divisor ) const {
			return pvector2( x / divisor, y / divisor );
		}
		inline pvector2& operator/=( const real64_t divisor ) {
			x /= divisor;
			y /= divisor;
			return *this;
		}

		// vector division
		inline pvector2 operator/( const pvector2 &rhs ) const {
			return pvector2( x / rhs.x, y / rhs.y );
		}
		inline pvector2& operator/=( const pvector2 &rhs ) {
			x /= rhs.x;
			y /= rhs.y;
			return *this;
		}

		// ???
		static inline pvector2 ma( const pvector2 &a, real64_t scale, const pvector2 &b ) {
			return a + b*scale;
		}

		// linear interpolation
		static inline pvector2 lerp( const pvector2 &a, real64_t scale, const pvector2 &b ) {
			return a + (b-a)*scale;
		}
	};

	struct vector3 {
		union {
			struct {
				real32_t x;
				real32_t y;
				real32_t z;
			};
			struct {
				real32_t r;
				real32_t g;
				real32_t b;
			};
			struct {
				real32_t pitch;
				real32_t yaw;
				real32_t roll;
			};
			real32_t raw[3];
		};

		// xtors
		vector3( real32_t x = 0.0f, real32_t y = 0.0f, real32_t z = 0.0f )
		: x( x ), y( y ), z( z )
		{
		}

		// string representation of the vector
		std::string tostring(
			void
		) const;

		// access operators
		inline real32_t& operator[]( const size_t idx ) {
			return raw[idx];
		}

		// clear
		inline void clear( void ) {
			x = y = z = 0.0f;
		}

		// compare
		static inline bool compare( const vector3 &lhs, const vector3 &rhs ) {
			return (flcmp( lhs.x, rhs.x ) && flcmp( lhs.y, rhs.y ) && flcmp( lhs.z, rhs.z ));
		}
		inline bool compare( const vector3 &rhs ) const {
			return (flcmp( x, rhs.x ) && flcmp( y, rhs.y ) && flcmp( z, rhs.z ));
		}
		inline bool operator==( const vector3 &rhs ) {
			return compare( rhs );
		}

		// addition
		inline vector3 operator+( const vector3 &rhs ) const {
			return vector3( x + rhs.x, y + rhs.y, z + rhs.z );
		}
        inline vector3& operator+=( const vector3& rhs ) {
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			return *this;
		}

		// subtraction
		inline vector3 operator-( const vector3 &rhs ) const {
			return vector3( x - rhs.x, y - rhs.y, z - rhs.z );
		}
        inline vector3& operator-=( const vector3& rhs ) {
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			return *this;
		}

		// increment
		inline void increment( void ) {
			x += 1.0f;
			y += 1.0f;
			z += 1.0f;
		}

		// decrement
		inline void decrement( void ) {
			x -= 1.0f;
			y -= 1.0f;
			z -= 1.0f;
		}

		// inverse
		inline void inverse( void ) {
			x = -x;
			y = -y;
			z = -z;
		}
		static inline vector3 inverse( const vector3 &in ) {
			return vector3( -in.x, -in.y, -in.z );
		}

		// scalar multiplication
		inline vector3 operator*( const real32_t scalar ) const {
			return vector3( x * scalar, y * scalar, z * scalar );
		}
		inline vector3& operator*=( const real32_t scalar ) {
			x *= scalar;
			y *= scalar;
			z *= scalar;
			return *this;
		}

		// vector multiplication
		inline vector3 operator*( const vector3 &rhs ) const {
			return vector3( x * rhs.x, y * rhs.y, z * rhs.z );
		}
		inline vector3& operator*=( const vector3 &rhs ) {
			x *= rhs.x;
			y *= rhs.y;
			z *= rhs.z;
			return *this;
		}

		// scalar division
		inline vector3 operator/( const real32_t divisor ) const {
			return vector3( x / divisor, y / divisor, z / divisor );
		}
		inline vector3& operator/=( const real32_t divisor ) {
			x /= divisor;
			y /= divisor;
			z /= divisor;
			return *this;
		}

		// vector division
		inline vector3 operator/( const vector3 &rhs ) const {
			return vector3( x / rhs.x, y / rhs.y, z / rhs.z );
		}
		inline vector3& operator/=( const vector3 &rhs ) {
			x /= rhs.x;
			y /= rhs.y;
			z /= rhs.z;
			return *this;
		}

		// ???
		static inline vector3 ma( const vector3 &a, real32_t scale, const vector3 &b ) {
			return a + b*scale;
		}

		// linear interpolation
		static inline vector3 lerp( const vector3 &a, real32_t scale, const vector3 &b ) {
			return a + (b-a)*scale;
		}

		// length
		inline real32_t length( void ) const {
			return sqrtf( x*x + y*y + z*z );
		}
		static inline real32_t length( const vector3 &vec ) {
			return vec.length();
		}
		inline real32_t lengthSquared( void ) const {
			return x*x + y*y + z*z;
		}
		static inline real32_t lengthSquared( const vector3 &vec ) {
			return vec.lengthSquared();
		}

		// distance
		static inline real32_t distance( const vector3 &a, const vector3 &b ) {
			return length( a - b );
		}
		static inline real32_t distanceSquared( const vector3 &a, const vector3 &b ) {
			return lengthSquared( a - b );
		}

		// normalise
		// does not check that length != 0
		// does not return length
		inline void normaliseFast( void ) {
			*this *= 1.0f / length();
		}
		inline real32_t normalise( void ) {
			real32_t length = this->length();

			if ( length ) {
				real32_t iLength = 1.0f / length;
				*this *= iLength;
			}

			return length;
		}
		static inline vector3 normalise( const vector3 &vec ) {
			vector3 result = vec;

			real32_t length = result.length();
			if ( length ) {
				// written this way to aid compiler optimsation, it can substitute for intrinsics easier like this
				real32_t recip = 1.0f / length;
				result *= recip;
			}

			return result;
		}

		// dot
		static inline real32_t dot( const vector3 &a, const vector3 &b ) {
			return a.x*b.x + a.y*b.y + a.z*b.z;
		}
		inline real32_t dot( const vector3 &vec ) const {
			return x*vec.x + y*vec.y + z*vec.z;
		}

		// cross
		static inline vector3 cross( const vector3 &a, const vector3 &b ) {
			vector3 result;

			result.x = (a.y*b.z) - (a.z*b.y);
			result.y = (a.z*b.x) - (a.x*b.z);
			result.z = (a.x*b.y) - (a.y*b.x);

			return result;
		}
		inline vector3 cross( const vector3 &v ) const {
			return cross( *this, v );
		}

		//TODO: snap?
	};

	struct pvector3 {
		union {
			struct {
				real64_t x;
				real64_t y;
				real64_t z;
			};
			struct {
				real64_t r;
				real64_t g;
				real64_t b;
			};
			struct {
				real64_t pitch;
				real64_t yaw;
				real64_t roll;
			};
			real64_t raw[3];
		};

		// xtors
		pvector3( real64_t x = 0.0, real64_t y = 0.0, real64_t z = 0.0 )
		: x( x ), y( y ), z( z )
		{
		}

		// string representation of the vector
		std::string tostring(
			void
		) const;

		// access operators
		inline real64_t& operator[]( const size_t idx ) {
			return raw[idx];
		}

		// clear
		inline void clear( void ) {
			x = y = z = 0.0;
		}

		// compare
		static inline bool compare( const pvector3 &lhs, const pvector3 &rhs ) {
			return (dblcmp( lhs.x, rhs.x ) && flcmp( lhs.y, rhs.y ) && flcmp( lhs.z, rhs.z ));
		}
		inline bool compare( const pvector3 &rhs ) const {
			return (dblcmp( x, rhs.x ) && flcmp( y, rhs.y ) && flcmp( z, rhs.z ));
		}
		inline bool operator==( const pvector3 &rhs ) {
			return compare( rhs );
		}

		// addition
		inline pvector3 operator+( const pvector3 &rhs ) const {
			return pvector3( x + rhs.x, y + rhs.y, z + rhs.z );
		}
        inline pvector3& operator+=( const pvector3 &rhs ) {
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			return *this;
		}

		// subtraction
		inline pvector3 operator-( const pvector3 &rhs ) const {
			return pvector3( x - rhs.x, y - rhs.y, z - rhs.z );
		}
        inline pvector3& operator-=( const pvector3& rhs ) {
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			return *this;
		}

		// increment
		inline void increment( void ) {
			x += 1.0;
			y += 1.0;
			z += 1.0;
		}

		// decrement
		inline void decrement( void ) {
			x -= 1.0;
			y -= 1.0;
			z -= 1.0;
		}

		// inverse
		inline void inverse( void ) {
			x = -x;
			y = -y;
			z = -z;
		}
		static inline pvector3 inverse( const pvector3 &in ) {
			return pvector3( -in.x, -in.y, -in.z );
		}

		// scalar multiplication
		inline pvector3 operator*( const real64_t scalar ) const {
			return pvector3( x * scalar, y * scalar, z * scalar );
		}
		inline pvector3& operator*=( const real64_t scalar ) {
			x *= scalar;
			y *= scalar;
			z *= scalar;
			return *this;
		}

		// vector multiplication
		inline pvector3 operator*( const pvector3 &rhs ) const {
			return pvector3( x * rhs.x, y * rhs.y, z * rhs.z );
		}
		inline pvector3& operator*=( const pvector3 &rhs ) {
			x *= rhs.x;
			y *= rhs.y;
			z *= rhs.z;
			return *this;
		}

		// scalar division
		inline pvector3 operator/( const real64_t divisor ) const {
			return pvector3( x / divisor, y / divisor, z / divisor );
		}
		inline pvector3& operator/=( const real64_t divisor ) {
			x /= divisor;
			y /= divisor;
			z /= divisor;
			return *this;
		}

		// vector division
		inline pvector3 operator/( const pvector3 &rhs ) const {
			return pvector3( x / rhs.x, y / rhs.y, z / rhs.z );
		}
		inline pvector3& operator/=( const pvector3 &rhs ) {
			x /= rhs.x;
			y /= rhs.y;
			z /= rhs.z;
			return *this;
		}

		// ???
		static inline pvector3 ma( const pvector3 &a, real64_t scale, const pvector3 &b ) {
			return a + b*scale;
		}

		// linear interpolation
		static inline pvector3 lerp( const pvector3 &a, real64_t scale, const pvector3 &b ) {
			return a + (b-a)*scale;
		}

		// length
		inline real64_t length( void ) const {
			return sqrt( x*x + y*y + z*z );
		}
		static inline real64_t length( const pvector3 &vec ) {
			return vec.length();
		}
		inline real64_t lengthSquared( void ) const {
			return x*x + y*y + z*z;
		}
		static inline real64_t lengthSquared( const pvector3 &vec ) {
			return vec.lengthSquared();
		}

		// distance
		static inline real64_t distance( const pvector3 &a, const pvector3 &b ) {
			return length( a - b );
		}
		static inline real64_t distanceSquared( const pvector3 &a, const pvector3 &b ) {
			return lengthSquared( a - b );
		}

		// normalise
		// does not check that length != 0
		// does not return length
		inline void normaliseFast( void ) {
			*this *= 1.0 / length();
		}
		inline real64_t normalise( void ) {
			real64_t length = this->length();

			if ( length ) {
				real64_t iLength = 1.0 / length;
				*this *= iLength;
			}

			return length;
		}
		static inline pvector3 normalise( const pvector3 &vec ) {
			pvector3 result = vec;

			real64_t length = result.length();
			if ( length ) {
				real64_t iLength = 1.0f / length;
				result *= iLength;
			}

			return result;
		}

		// dot
		inline real64_t dot( const pvector3 &vec ) const {
			return x*vec.x + y*vec.y + z*vec.z;
		}

		// cross
		static inline pvector3 cross( const pvector3 &a, const pvector3 &b ) {
			pvector3 result;

			result.x = (a.y*b.z) - (a.z*b.y);
			result.y = (a.z*b.x) - (a.x*b.z);
			result.z = (a.x*b.y) - (a.y*b.x);

			return result;
		}
		inline pvector3 cross( const pvector3 &v ) const {
			return cross( *this, v );
		}

		//TODO: snap?
	};

	struct vector4 {
		union {
			struct {
				real32_t x;
				real32_t y;
				real32_t z;
				real32_t w;
			};
			struct {
				real32_t r;
				real32_t g;
				real32_t b;
				real32_t a;
			};
			real32_t raw[4];
		};

		// xtors
		vector4( real32_t x = 0.0f, real32_t y = 0.0f, real32_t z = 0.0f, real32_t w = 0.0f )
		: x( x ), y( y ), z( z ), w( w )
		{
		}

		// string representation of the vector
		std::string tostring(
			void
		) const;

		// access operators
		inline real32_t& operator[]( const size_t idx ) {
			return raw[idx];
		}

		// clear
		inline void clear( void ) {
			x = y = z = w = 0.0f;
		}

		// compare
		static inline bool compare( const vector4 &lhs, const vector4 &rhs ) {
			return (flcmp( lhs.x, rhs.x ) && flcmp( lhs.y, rhs.y ) && flcmp( lhs.z, rhs.z ) && flcmp( lhs.w, rhs.w ));
		}
		inline bool compare( const vector4 &rhs ) const {
			return (flcmp( x, rhs.x ) && flcmp( y, rhs.y ) && flcmp( z, rhs.z ) && flcmp( w, rhs.w ));
		}
		inline bool operator==( const vector4 &rhs ) const {
			return compare( rhs );
		}

		// addition
		inline vector4 operator+( const vector4 &rhs ) const {
			return vector4( this->x + rhs.x, this->y + rhs.y, this->z + rhs.z, this->w + rhs.w );
		}
        inline vector4& operator+=( const vector4& rhs ) {
			this->x += rhs.x;
			this->y += rhs.y;
			this->z += rhs.z;
			this->w += rhs.w;
			return *this;
		}

		// subtraction
		inline vector4 operator-( const vector4 &rhs ) const {
			return vector4( this->x - rhs.x, this->y - rhs.y, this->z - rhs.z, this->w - rhs.w );
		}
        inline vector4& operator-=( const vector4& rhs ) {
			this->x -= rhs.x;
			this->y -= rhs.y;
			this->z -= rhs.z;
			this->w -= rhs.w;
			return *this;
		}

		// increment
		inline void increment( void ) {
			this->x += 1.0f;
			this->y += 1.0f;
			this->z += 1.0f;
			this->w += 1.0f;
		}

		// decrement
		inline void decrement( void ) {
			this->x -= 1.0f;
			this->y -= 1.0f;
			this->z -= 1.0f;
			this->w -= 1.0f;
		}

		// inverse
		inline void inverse( void ) {
			x = -x;
			y = -y;
			z = -z;
			w = -w;
		}
		static inline vector4 inverse( const vector4 &in ) {
			return vector4( -in.x, -in.y, -in.z, -in.w );
		}

		// scalar multiplication
		inline vector4 operator*( const real32_t scalar ) const {
			return vector4( x * scalar, y * scalar, z * scalar, w * scalar );
		}
		inline vector4& operator*=( const real32_t scalar ) {
			x *= scalar;
			y *= scalar;
			z *= scalar;
			w *= scalar;
			return *this;
		}

		// vector multiplication
		inline vector4 operator*( const vector4 &rhs ) const {
			return vector4( x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w );
		}
		inline vector4& operator*=( const vector4 &rhs ) {
			x *= rhs.x;
			y *= rhs.y;
			z *= rhs.z;
			w *= rhs.w;
			return *this;
		}

		// scalar division
		inline vector4 operator/( const real32_t divisor ) const {
			return vector4( x / divisor, y / divisor, z / divisor, w / divisor );
		}
		inline vector4& operator/=( const real32_t divisor ) {
			x /= divisor;
			y /= divisor;
			z /= divisor;
			w /= divisor;
			return *this;
		}

		// vector division
		inline vector4 operator/( const vector4 &rhs ) const {
			return vector4( x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w );
		}
		inline vector4& operator/=( const vector4 &rhs ) {
			x /= rhs.x;
			y /= rhs.y;
			z /= rhs.z;
			w /= rhs.w;
			return *this;
		}

		// ???
		static inline vector4 ma( const vector4 &a, real32_t scale, const vector4 &b ) {
			return a + b*scale;
		}

		// linear interpolation
		static inline vector4 lerp( const vector4 &a, real32_t scale, const vector4 &b ) {
			return a + (b-a)*scale;
		}
	};

} // namespace XS

#ifdef _MSC_VER
	#pragma warning( pop )
#endif // _MSC_VER
