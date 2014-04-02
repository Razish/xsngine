#pragma once

#ifdef _MSC_VER
	#pragma warning( push )
	#pragma warning( disable : 4201 ) // nonstandard extension used : nameless struct/union
#endif // _MSC_VER

namespace XS {

	class vector2 {
	private:

	public:
		union {
			struct { number x, y; };
			struct { number w, h; };
			number _raw[2];
		};

		// xtors
		vector2() {
			this->x = this->y = 0.0f;
		}

		vector2( number x, number y ) {
			this->x = x;
			this->y = y;
		}

		// access operators
		inline number& operator[]( const int idx ) {
			return _raw[idx];
		}

		// clear
		inline void clear( void ) {
			this->x = this->y = 0.0f;
		}

		// compare
		static inline bool compare( const vector2 &lhs, const vector2 &rhs ) {
			return ( lhs.x == rhs.x && lhs.y == rhs.y );
		}

		// addition
		inline vector2 operator+( const vector2 &rhs ) const {
			return vector2( this->x + rhs.x, this->y + rhs.y );
		}
        inline vector2& operator+=( const vector2& rhs ) {
			this->x += rhs.x;
			this->y += rhs.y;
			return *this;
		}

		// subtraction
		inline vector2 operator-( const vector2 &rhs ) const {
			return vector2( this->x - rhs.x, this->y - rhs.y );
		}
        inline vector2& operator-=( const vector2& rhs ) {
			this->x -= rhs.x;
			this->y -= rhs.y;
			return *this;
		}

		// increment
		inline void increment( void ) {
			this->x += 1.0f;
			this->y += 1.0f;
		}

		// decrement
		inline void decrement( void ) {
			this->x -= 1.0f;
			this->y -= 1.0f;
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
		inline vector2 operator*( const number scalar ) const {
			return vector2( this->x * scalar, this->y * scalar );
		}
		inline vector2& operator*=( const number scalar ) {
			this->x *= scalar;
			this->y *= scalar;
			return *this;
		}

		// vector multiplication
		inline vector2 operator*( const vector2 &rhs ) const {
			return vector2( this->x * rhs.x, this->y * rhs.y );
		}
		inline vector2& operator*=( const vector2 &rhs ) {
			this->x *= rhs.x;
			this->y *= rhs.y;
			return *this;
		}

		// ???
		static inline vector2 ma( const vector2 &a, number scale, const vector2 &b ) {
			vector2 result;
			result.x = a.x + b.x*scale;
			result.y = a.y + b.y*scale;
			return result;
		}

		// linear interpolation
		static inline vector2 lerp( const vector2 &a, number scale, const vector2 &b ) {
			vector2 result;
			result.x = a.x + (b.x-a.x)*scale;
			result.y = a.y + (b.y-a.y)*scale;
			return result;
		}
	};

	class vector3 {
	private:

	public:
		union {
			struct { number x, y, z; };
			struct { number r, g, b; }; // red, green, blue?
			number _raw[3];
		};

		// xtors
		vector3() {
			this->x = this->y = this->z = 0.0f;
		}

		vector3( number x, number y, number z ) {
			this->x = x;
			this->y = y;
			this->z = z;
		}

		// access operators
		inline number& operator[]( const int idx ) {
			return _raw[idx];
		}

		// clear
		inline void clear( void ) {
			this->x = this->y = this->z = 0.0f;
		}

		// compare
		static inline bool compare( const vector3 &lhs, const vector3 &rhs ) {
			return ( lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z );
		}

		// addition
		inline vector3 operator+( const vector3 &rhs ) const {
			return vector3( this->x + rhs.x, this->y + rhs.y, this->z + rhs.z );
		}
        inline vector3& operator+=( const vector3& rhs ) {
			this->x += rhs.x;
			this->y += rhs.y;
			this->z += rhs.z;
			return *this;
		}

		// subtraction
		inline vector3 operator-( const vector3 &rhs ) const {
			return vector3( this->x - rhs.x, this->y - rhs.y, this->z - rhs.z );
		}
        inline vector3& operator-=( const vector3& rhs ) {
			this->x -= rhs.x;
			this->y -= rhs.y;
			this->z -= rhs.z;
			return *this;
		}

		// increment
		inline void increment( void ) {
			this->x += 1.0f;
			this->y += 1.0f;
			this->z += 1.0f;
		}

		// decrement
		inline void decrement( void ) {
			this->x -= 1.0f;
			this->y -= 1.0f;
			this->z -= 1.0f;
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
		inline vector3 operator*( const number scalar ) const {
			return vector3( this->x * scalar, this->y * scalar, this->z * scalar );
		}
		inline vector3& operator*=( const number scalar ) {
			this->x *= scalar;
			this->y *= scalar;
			this->z *= scalar;
			return *this;
		}

		// vector multiplication
		inline vector3 operator*( const vector3 &rhs ) const {
			return vector3( this->x * rhs.x, this->y * rhs.y, this->z * rhs.z );
		}
		inline vector3& operator*=( const vector3 &rhs ) {
			this->x *= rhs.x;
			this->y *= rhs.y;
			this->z *= rhs.z;
			return *this;
		}

		// ???
		static inline vector3 ma( const vector3 &a, number scale, const vector3 &b ) {
			vector3 result;
			result.x = a.x + b.x*scale;
			result.y = a.y + b.y*scale;
			result.z = a.z + b.z*scale;
			return result;
		}

		// linear interpolation
		static inline vector3 lerp( const vector3 &a, number scale, const vector3 &b ) {
			vector3 result;
			result.x = a.x + (b.x-a.x)*scale;
			result.y = a.y + (b.y-a.y)*scale;
			result.z = a.z + (b.z-a.z)*scale;
			return result;
		}

		// length
		inline number length( void ) const {
			return sqrtf( this->x*this->x + this->y*this->y + this->z*this->z );
		}
		static inline number length( const vector3 &vec ) {
			return vec.length();
		}
		inline number lengthSquared( void ) const {
			return this->x*this->x + this->y*this->y + this->z*this->z;
		}
		static inline number lengthSquared( const vector3 &vec ) {
			return vec.lengthSquared();
		}

		// distance
		static inline number distance( const vector3 &a, const vector3 &b ) {
			return length( a - b );
		}
		static inline number distanceSquared( const vector3 &a, const vector3 &b ) {
			return lengthSquared( a - b );
		}

		// normalise
		inline void normaliseFast( void ) {
			// does not check that length != 0
			// does not return length
			*this *= length();
		}
		inline number normalise( void ) {
			number length = this->length();

			if ( length ) {
				number iLength = 1.0f/length;
				*this *= iLength;
			}

			return length;
		}
		static inline vector3 normalise( const vector3 &vec ) {
			vector3 result = vec;

			number length = result.length();
			if ( length ) {
				number iLength = 1.0f/length;
				result *= iLength;
			}

			return result;
		}

		// dot
		inline number dot( const vector3 &vec ) const {
			return this->x*vec.x + this->y*vec.y + this->z*vec.z;
		}

		// cross
		inline vector3 cross( const vector3 &a, const vector3 &b ) const {
			vector3 result;

			result.x = (a.y*b.z) - (a.z*b.y);
			result.y = (a.z*b.x) - (a.x*b.z);
			result.z = (a.x*b.y) - (a.y*b.x);

			return result;
		}

		//TODO: snap?
	};

	class vector4 {
	private:

	public:
		union {
			struct { number x, y, z, w; };
			struct { number r, g, b, a; }; // red, green, blue?
			number _raw[4];
		};

		// xtors
		vector4() {
			this->x = this->y = this->z = this->w = 0.0f;
		}

		vector4( number x, number y, number z, number w ) {
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		}

		// access operators
		inline number& operator[]( const int idx ) {
			return _raw[idx];
		}

		// clear
		inline void clear( void ) {
			this->x = this->y = this->z = this->w = 0.0f;
		}

		// compare
		static inline bool compare( const vector4 &lhs, const vector4 &rhs ) {
			return ( lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w );
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
		inline vector4 operator*( const number scalar ) const {
			return vector4( this->x * scalar, this->y * scalar, this->z * scalar, this->w * scalar );
		}
		inline vector4& operator*=( const number scalar ) {
			this->x *= scalar;
			this->y *= scalar;
			this->z *= scalar;
			this->w *= scalar;
			return *this;
		}

		// vector multiplication
		inline vector4 operator*( const vector4 &rhs ) const {
			return vector4( this->x * rhs.x, this->y * rhs.y, this->z * rhs.z, this->w * rhs.w );
		}
		inline vector4& operator*=( const vector4 &rhs ) {
			this->x *= rhs.x;
			this->y *= rhs.y;
			this->z *= rhs.z;
			this->w *= rhs.w;
			return *this;
		}

		// ???
		static inline vector4 ma( const vector4 &a, number scale, const vector4 &b ) {
			vector4 result;
			result.x = a.x + b.x*scale;
			result.y = a.y + b.y*scale;
			result.z = a.z + b.z*scale;
			result.w = a.w + b.w*scale;
			return result;
		}

		// linear interpolation
		static inline vector4 lerp( const vector4 &a, number scale, const vector4 &b ) {
			vector4 result;
			result.x = a.x + (b.x-a.x)*scale;
			result.y = a.y + (b.y-a.y)*scale;
			result.z = a.z + (b.z-a.z)*scale;
			result.w = a.w + (b.w-a.w)*scale;
			return result;
		}
	};

} // namespace XS

#ifdef _MSC_VER
	#pragma warning( pop )
#endif // _MSC_VER
