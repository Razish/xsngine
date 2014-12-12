#pragma once

#include <string>

#ifdef _MSC_VER
	#pragma warning( push )
	#pragma warning( disable : 4201 ) // nonstandard extension used : nameless struct/union
#endif // _MSC_VER

namespace XS {

	struct vector2 {
		union {
			struct { float x, y; };
			struct { float w, h; };
			float _raw[2];
		};

		// xtors
		vector2()
		: x( 0.0f ), y( 0.0f )
		{
		}

		vector2( float x, float y )
		: x( x ), y( y )
		{
		}

		std::string tostring( void ) const;

		// access operators
		inline float& operator[]( const int idx ) {
			return _raw[idx];
		}

		// clear
		inline void clear( void ) {
			x = y = 0.0f;
		}

		// compare
		static inline bool compare( const vector2 &lhs, const vector2 &rhs ) {
			return ( lhs.x == rhs.x && lhs.y == rhs.y );
		}
		inline bool compare( const vector2 &rhs ) const {
			return ( x == rhs.x && y == rhs.y );
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
		inline vector2 operator*( const float scalar ) const {
			return vector2( x * scalar, y * scalar );
		}
		inline vector2& operator*=( const float scalar ) {
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
		static inline vector2 ma( const vector2 &a, float scale, const vector2 &b ) {
			return a + b*scale;
		}

		// linear interpolation
		static inline vector2 lerp( const vector2 &a, float scale, const vector2 &b ) {
			return a + (b-a)*scale;
		}
	};

	struct vector3 {
		union {
			struct { float x, y, z; };
			struct { float r, g, b; }; // red, green, blue?
			float _raw[3];
		};

		// xtors
		vector3()
		: x( 0.0f ), y( 0.0f ), z( 0.0f )
		{
		}

		vector3( float x, float y, float z )
		: x( x ), y( y ), z( z )
		{
		}

		std::string tostring( void ) const;

		// access operators
		inline float& operator[]( const int idx ) {
			return _raw[idx];
		}

		// clear
		inline void clear( void ) {
			x = y = z = 0.0f;
		}

		// compare
		static inline bool compare( const vector3 &lhs, const vector3 &rhs ) {
			return ( lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z );
		}
		inline bool compare( const vector3 &rhs ) const {
			return ( x == rhs.x && y == rhs.y && z == rhs.z );
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
		inline vector3 operator*( const float scalar ) const {
			return vector3( x * scalar, y * scalar, z * scalar );
		}
		inline vector3& operator*=( const float scalar ) {
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
		static inline vector3 ma( const vector3 &a, float scale, const vector3 &b ) {
			return a + b*scale;
		}

		// linear interpolation
		static inline vector3 lerp( const vector3 &a, float scale, const vector3 &b ) {
			return a + (b-a)*scale;
		}

		// length
		inline float length( void ) const {
			return sqrtf( x*x + y*y + z*z );
		}
		static inline float length( const vector3 &vec ) {
			return vec.length();
		}
		inline float lengthSquared( void ) const {
			return x*x + y*y + z*z;
		}
		static inline float lengthSquared( const vector3 &vec ) {
			return vec.lengthSquared();
		}

		// distance
		static inline float distance( const vector3 &a, const vector3 &b ) {
			return length( a - b );
		}
		static inline float distanceSquared( const vector3 &a, const vector3 &b ) {
			return lengthSquared( a - b );
		}

		// normalise
		// does not check that length != 0
		// does not return length
		inline void normaliseFast( void ) {
			*this *= 1.0f / length();
		}
		inline float normalise( void ) {
			float length = this->length();

			if ( length ) {
				float iLength = 1.0f / length;
				*this *= iLength;
			}

			return length;
		}
		static inline vector3 normalise( const vector3 &vec ) {
			vector3 result = vec;

			float length = result.length();
			if ( length ) {
				float iLength = 1.0f / length;
				result *= iLength;
			}

			return result;
		}

		// dot
		inline float dot( const vector3 &vec ) const {
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
			struct { float x, y, z, w; };
			struct { float r, g, b, a; }; // red, green, blue?
			float _raw[4];
		};

		// xtors
		vector4()
		: x( 0.0f ), y( 0.0f ), z( 0.0f ), w( 0.0f )
		{
		}

		vector4( float x, float y, float z, float w )
		: x( x ), y( y ), z( z ), w( w )
		{
		}

		std::string tostring( void ) const;

		// access operators
		inline float& operator[]( const int idx ) {
			return _raw[idx];
		}

		// clear
		inline void clear( void ) {
			x = y = z = w = 0.0f;
		}

		// compare
		static inline bool compare( const vector4 &lhs, const vector4 &rhs ) {
			return ( lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w );
		}
		inline bool compare( const vector4 &rhs ) const {
			return ( x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w );
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
		inline vector4 operator*( const float scalar ) const {
			return vector4( x * scalar, y * scalar, z * scalar, w * scalar );
		}
		inline vector4& operator*=( const float scalar ) {
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
		static inline vector4 ma( const vector4 &a, float scale, const vector4 &b ) {
			return a + b*scale;
		}

		// linear interpolation
		static inline vector4 lerp( const vector4 &a, float scale, const vector4 &b ) {
			return a + (b-a)*scale;
		}
	};

} // namespace XS

#ifdef _MSC_VER
	#pragma warning( pop )
#endif // _MSC_VER
