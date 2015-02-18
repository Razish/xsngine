#pragma once

#include <string>

#ifdef _MSC_VER
	#pragma warning( push )
	#pragma warning( disable : 4201 ) // nonstandard extension used : nameless struct/union
#endif // _MSC_VER

namespace XS {

	//TODO: unit test precision at different ranges (and double conversion/promotion?)
	static bool flcmp( const real32_t &f1, const real32_t &f2 ) {
		static const real32_t epsilon = 0.00001f;
		const real32_t delta = std::abs( f2 - f1 );

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

		// ???
		static inline vector2 ma( const vector2 &a, real32_t scale, const vector2 &b ) {
			return a + b*scale;
		}

		// linear interpolation
		static inline vector2 lerp( const vector2 &a, real32_t scale, const vector2 &b ) {
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
				real32_t iLength = 1.0f / length;
				result *= iLength;
			}

			return result;
		}

		// dot
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
