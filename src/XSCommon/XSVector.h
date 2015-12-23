#pragma once

#include <string>

namespace XS {

	//TODO: unit test precision at different ranges (and double conversion/promotion?)
	static bool flcmp( const real32_t &f1, const real32_t &f2, const real32_t epsilon = 0.00001f )
		XS_WARN_UNUSED_RESULT;
	static bool flcmp( const real32_t &f1, const real32_t &f2, const real32_t epsilon ) {
		const real32_t delta = std::abs( f2 - f1 );

		return delta < epsilon;
	}
	static bool dblcmp( const real64_t &f1, const real64_t &f2, const real64_t epsilon = 0.000000000000001 )
		XS_WARN_UNUSED_RESULT;
	static bool dblcmp( const real64_t &f1, const real64_t &f2, const real64_t epsilon ) {
		const real64_t delta = std::abs( f2 - f1 );

		return delta < epsilon;
	}

	struct vector2 {
		real32_t data[2];

		// string representation of the vector
		std::string tostring(
			void
		) const XS_WARN_UNUSED_RESULT;

		// access operators
		inline const real32_t& operator[]( const size_t idx ) const {
			return data[idx];
		}
		inline real32_t& operator[]( const size_t idx ) {
			return data[idx];
		}

		// clear
		inline void clear( void ) {
			data[0] = 0.0f;
			data[1] = 0.0f;
		}

		// compare
		static inline bool compare( const vector2 &lhs, const vector2 &rhs ) XS_WARN_UNUSED_RESULT {
			return (flcmp( lhs[0], rhs[0] ) && flcmp( lhs[1], rhs[1] ));
		}

		// ???
		inline bool compare( const vector2 &rhs ) const XS_WARN_UNUSED_RESULT {
			return (flcmp( data[0], rhs[0] ) && flcmp( data[1], rhs[1] ));
		}

		// ???
		inline bool operator==( const vector2 &rhs ) {
			return compare( rhs );
		}

		// addition
		inline vector2 operator+( const vector2 &rhs ) const {
			vector2 result = { data[0] + rhs[0], data[1] + rhs[1] };
			return result;
		}

		// ???
        inline vector2& operator+=( const vector2& rhs ) {
			data[0] += rhs[0];
			data[1] += rhs[1];
			return *this;
		}

		// subtraction
		inline vector2 operator-( const vector2 &rhs ) const {
			vector2 result = { data[0] - rhs[0], data[1] - rhs[1] };
			return result;
		}

		// ???
        inline vector2& operator-=( const vector2& rhs ) {
			data[0] -= rhs[0];
			data[1] -= rhs[1];
			return *this;
		}

		// increment
		inline void increment( void ) {
			data[0] += 1.0f;
			data[1] += 1.0f;
		}

		// decrement
		inline void decrement( void ) {
			data[0] -= 1.0f;
			data[1] -= 1.0f;
		}

		// inverse
		inline void inverse( void ) {
			data[0] = -data[0];
			data[1] = -data[1];
		}

		// ???
		static inline vector2 inverse( const vector2 &in ) XS_WARN_UNUSED_RESULT {
			vector2 result = { -in[0], -in[1] };
			return result;
		}

		// scalar multiplication
		inline vector2 operator*( const real32_t scalar ) const XS_WARN_UNUSED_RESULT {
			vector2 result = { data[0] * scalar, data[1] * scalar };
			return result;
		}

		// ???
		inline vector2& operator*=( const real32_t scalar ) {
			data[0] *= scalar;
			data[1] *= scalar;
			return *this;
		}

		// vector multiplication
		inline vector2 operator*( const vector2 &rhs ) const {
			vector2 result = { data[0] * rhs[0], data[1] * rhs[1] };
			return result;
		}

		// ???
		inline vector2& operator*=( const vector2 &rhs ) {
			data[0] *= rhs[0];
			data[1] *= rhs[1];
			return *this;
		}

		// scalar division
		inline vector2 operator/( const real32_t divisor ) const {
			vector2 result = { data[0] / divisor, data[1] / divisor };
			return result;
		}

		// ???
		inline vector2& operator/=( const real32_t divisor ) {
			data[0] /= divisor;
			data[1] /= divisor;
			return *this;
		}

		// vector division
		inline vector2 operator/( const vector2 &rhs ) const {
			vector2 result = { data[0] / rhs[0], data[1] / rhs[1] };
			return result;
		}

		// ???
		inline vector2& operator/=( const vector2 &rhs ) {
			data[0] /= rhs[0];
			data[1] /= rhs[1];
			return *this;
		}

		// ???
		static inline vector2 ma( const vector2 &a, real32_t scale, const vector2 &b ) XS_WARN_UNUSED_RESULT {
			return a + b*scale;
		}

		// linear interpolation
		static inline vector2 lerp( const vector2 &a, real32_t scale, const vector2 &b ) XS_WARN_UNUSED_RESULT {
			return a + (b-a)*scale;
		}
	};

	struct pvector2 {
		real64_t data[2];

		// string representation of the vector
		std::string tostring(
			void
		) const XS_WARN_UNUSED_RESULT;

		// access operators
		inline const real64_t& operator[]( const size_t idx ) const {
			return data[idx];
		}
		inline real64_t& operator[]( const size_t idx ) {
			return data[idx];
		}

		// clear
		inline void clear( void ) {
			data[0] = 0.0;
			data[1] = 0.0;
		}

		// compare
		static inline bool compare( const pvector2 &lhs, const pvector2 &rhs ) XS_WARN_UNUSED_RESULT {
			return (dblcmp( lhs[0], rhs[0] ) && dblcmp( lhs[1], rhs[1] ));
		}

		// ???
		inline bool compare( const pvector2 &rhs ) const XS_WARN_UNUSED_RESULT {
			return (dblcmp( data[0], rhs[0] ) && dblcmp( data[1], rhs[1] ));
		}

		// ???
		inline bool operator==( const pvector2 &rhs ) {
			return compare( rhs );
		}

		// addition
		inline pvector2 operator+( const pvector2 &rhs ) const {
			pvector2 result = { data[0] + rhs[0], data[1] + rhs[1] };
			return result;
		}

		// ???
        inline pvector2& operator+=( const pvector2& rhs ) {
			data[0] += rhs[0];
			data[1] += rhs[1];
			return *this;
		}

		// subtraction
		inline pvector2 operator-( const pvector2 &rhs ) const {
			pvector2 result = { data[0] - rhs[0], data[1] - rhs[1] };
			return result;
		}

		// ???
        inline pvector2& operator-=( const pvector2& rhs ) {
			data[0] -= rhs[0];
			data[1] -= rhs[1];
			return *this;
		}

		// increment
		inline void increment( void ) {
			data[0] += 1.0;
			data[1] += 1.0;
		}

		// decrement
		inline void decrement( void ) {
			data[0] -= 1.0;
			data[1] -= 1.0;
		}

		// inverse
		inline void inverse( void ) {
			data[0] = -data[0];
			data[1] = -data[1];
		}

		// ???
		static inline pvector2 inverse( const pvector2 &in ) XS_WARN_UNUSED_RESULT {
			pvector2 result = { -in[0], -in[1] };
			return result;
		}

		// scalar multiplication
		inline pvector2 operator*( const real64_t scalar ) const {
			pvector2 result = { data[0] * scalar, data[1] * scalar };
			return result;
		}

		// ???
		inline pvector2& operator*=( const real64_t scalar ) {
			data[0] *= scalar;
			data[1] *= scalar;
			return *this;
		}

		// vector multiplication
		inline pvector2 operator*( const pvector2 &rhs ) const {
			pvector2 result = { data[0] * rhs[0], data[1] * rhs[1] };
			return result;
		}

		// ???
		inline pvector2& operator*=( const pvector2 &rhs ) {
			data[0] *= rhs[0];
			data[1] *= rhs[1];
			return *this;
		}

		// scalar division
		inline pvector2 operator/( const real64_t divisor ) const {
			pvector2 result = { data[0] / divisor, data[1] / divisor };
			return result;
		}

		// ???
		inline pvector2& operator/=( const real64_t divisor ) {
			data[0] /= divisor;
			data[1] /= divisor;
			return *this;
		}

		// vector division
		inline pvector2 operator/( const pvector2 &rhs ) const {
			pvector2 result = { data[0] / rhs[0], data[1] / rhs[1] };
			return result;
		}

		// ???
		inline pvector2& operator/=( const pvector2 &rhs ) {
			data[0] /= rhs[0];
			data[1] /= rhs[1];
			return *this;
		}

		// ???
		static inline pvector2 ma( const pvector2 &a, real64_t scale, const pvector2 &b ) XS_WARN_UNUSED_RESULT {
			return a + b*scale;
		}

		// linear interpolation
		static inline pvector2 lerp( const pvector2 &a, real64_t scale, const pvector2 &b ) XS_WARN_UNUSED_RESULT {
			return a + (b-a)*scale;
		}
	};

	struct vector3 {
		real32_t data[3];

		// string representation of the vector
		std::string tostring(
			void
		) const XS_WARN_UNUSED_RESULT;

		// access operators
		inline const real32_t& operator[]( const size_t idx ) const {
			return data[idx];
		}
		inline real32_t& operator[]( const size_t idx ) {
			return data[idx];
		}

		// clear
		inline void clear( void ) {
			data[0] = 0.0f;
			data[1] = 0.0f;
			data[2] = 0.0f;
		}

		// compare
		static inline bool compare( const vector3 &lhs, const vector3 &rhs ) XS_WARN_UNUSED_RESULT {
			return (flcmp( lhs[0], rhs[0] ) && flcmp( lhs[1], rhs[1] ) && flcmp( lhs[2], rhs[2] ));
		}

		// ???
		inline bool compare( const vector3 &rhs ) const XS_WARN_UNUSED_RESULT {
			return (flcmp( data[0], rhs[0] ) && flcmp( data[1], rhs[1] ) && flcmp( data[2], rhs[2] ));
		}

		// ???
		inline bool operator==( const vector3 &rhs ) {
			return compare( rhs );
		}

		// addition
		inline vector3 operator+( const vector3 &rhs ) const {
			vector3 result = { data[0] + rhs[0], data[1] + rhs[1], data[2] + rhs[2] };
			return result;
		}

		// ???
        inline vector3& operator+=( const vector3& rhs ) {
			data[0] += rhs[0];
			data[1] += rhs[1];
			data[2] += rhs[2];
			return *this;
		}

		// subtraction
		inline vector3 operator-( const vector3 &rhs ) const {
			vector3 result = { data[0] - rhs[0], data[1] - rhs[1], data[2] - rhs[2] };
			return result;
		}

		// ???
        inline vector3& operator-=( const vector3& rhs ) {
			data[0] -= rhs[0];
			data[1] -= rhs[1];
			data[2] -= rhs[2];
			return *this;
		}

		// increment
		inline void increment( void ) {
			data[0] += 1.0f;
			data[1] += 1.0f;
			data[2] += 1.0f;
		}

		// decrement
		inline void decrement( void ) {
			data[0] -= 1.0f;
			data[1] -= 1.0f;
			data[2] -= 1.0f;
		}

		// inverse
		inline void inverse( void ) {
			data[0] = -data[0];
			data[1] = -data[1];
			data[2] = -data[2];
		}

		// ???
		static inline vector3 inverse( const vector3 &in ) XS_WARN_UNUSED_RESULT {
			vector3 result = { -in[0], -in[1], -in[2] };
			return result;
		}

		// scalar multiplication
		inline vector3 operator*( const real32_t scalar ) const {
			vector3 result = { data[0] * scalar, data[1] * scalar, data[1] * scalar };
			return result;
		}

		// ???
		inline vector3& operator*=( const real32_t scalar ) {
			data[0] *= scalar;
			data[1] *= scalar;
			data[2] *= scalar;
			return *this;
		}

		// vector multiplication
		inline vector3 operator*( const vector3 &rhs ) const {
			vector3 result = { data[0] * rhs[0], data[1] * rhs[1], data[2] * rhs[2] };
			return result;
		}

		// ???
		inline vector3& operator*=( const vector3 &rhs ) {
			data[0] *= rhs[0];
			data[1] *= rhs[1];
			data[2] *= rhs[2];
			return *this;
		}

		// scalar division
		inline vector3 operator/( const real32_t divisor ) const {
			vector3 result = { data[0] / divisor, data[1] / divisor, data[2] / divisor };
			return result;
		}

		// ???
		inline vector3& operator/=( const real32_t divisor ) {
			data[0] /= divisor;
			data[1] /= divisor;
			data[2] /= divisor;
			return *this;
		}

		// vector division
		inline vector3 operator/( const vector3 &rhs ) const {
			vector3 result = { data[0] / rhs[0], data[1] / rhs[1], data[2] / rhs[2] };
			return result;
		}

		// ???
		inline vector3& operator/=( const vector3 &rhs ) {
			data[0] /= rhs[0];
			data[1] /= rhs[1];
			data[2] /= rhs[2];
			return *this;
		}

		// ???
		static inline vector3 ma( const vector3 &a, real32_t scale, const vector3 &b ) XS_WARN_UNUSED_RESULT {
			return a + b*scale;
		}

		// linear interpolation
		static inline vector3 lerp( const vector3 &a, real32_t scale, const vector3 &b ) XS_WARN_UNUSED_RESULT {
			return a + (b-a)*scale;
		}

		// length
		inline real32_t length( void ) const XS_WARN_UNUSED_RESULT {
			return sqrtf( data[0]*data[0] + data[1]*data[1] + data[2]*data[2] );
		}
		static inline real32_t length( const vector3 &vec ) XS_WARN_UNUSED_RESULT {
			return sqrtf( vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2] );
		}

		// length squared
		inline real32_t lengthSquared( void ) const XS_WARN_UNUSED_RESULT {
			return data[0]*data[0] + data[1]*data[1] + data[2]*data[2];
		}
		static inline real32_t lengthSquared( const vector3 &vec ) XS_WARN_UNUSED_RESULT {
			return vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2];
		}

		// distance
		static inline real32_t distance( const vector3 &a, const vector3 &b ) XS_WARN_UNUSED_RESULT {
			return length( a - b );
		}

		// ???
		static inline real32_t distanceSquared( const vector3 &a, const vector3 &b ) XS_WARN_UNUSED_RESULT {
			return lengthSquared( a - b );
		}

		// normalise
		// does not check that length != 0
		// does not return length
		inline void normaliseFast( void ) {
			*this *= 1.0f / length();
		}

		// ???
		inline real32_t normalise( void ) {
			real32_t length = this->length();

			//TODO: validate this form improves gcc's ability to optimise
			//	alternatively
			if ( length ) {
				real32_t iLength = 1.0f / length;
				*this *= iLength;
			}

			return length;
		}

		// ???
		static inline vector3 normalise( const vector3 &vec ) XS_WARN_UNUSED_RESULT {
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
		static inline real32_t dot( const vector3 &a, const vector3 &b ) XS_WARN_UNUSED_RESULT {
			return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
		}

		// ???
		inline real32_t dot( const vector3 &vec ) const XS_WARN_UNUSED_RESULT {
			return data[0]*vec[0] + data[1]*vec[1] + data[2]*vec[2];
		}

		// cross
		static inline vector3 cross( const vector3 &a, const vector3 &b ) XS_WARN_UNUSED_RESULT {
			vector3 result = {
				(a[1]*b[2]) - (a[2]*b[1]),
				(a[2]*b[0]) - (a[0]*b[2]),
				(a[0]*b[1]) - (a[1]*b[0])
			};

			return result;
		}

		// ???
		inline vector3 cross( const vector3 &v ) const XS_WARN_UNUSED_RESULT {
			return cross( *this, v );
		}

		//TODO: snap?
	};

	struct pvector3 {
		real64_t data[3];

		// string representation of the vector
		std::string tostring(
			void
		) const XS_WARN_UNUSED_RESULT;

		// access operators
		inline const real64_t& operator[]( const size_t idx ) const {
			return data[idx];
		}
		inline real64_t& operator[]( const size_t idx ) {
			return data[idx];
		}

		// clear
		inline void clear( void ) {
			data[0] = 0.0;
			data[1] = 0.0;
			data[2] = 0.0;
		}

		// compare
		static inline bool compare( const pvector3 &lhs, const pvector3 &rhs ) XS_WARN_UNUSED_RESULT {
			return (dblcmp( lhs[0], rhs[0] ) && flcmp( lhs[1], rhs[1] ) && flcmp( lhs[2], rhs[2] ));
		}

		// ???
		inline bool compare( const pvector3 &rhs ) const XS_WARN_UNUSED_RESULT {
			return (dblcmp( data[0], rhs[0] ) && flcmp( data[1], rhs[1] ) && flcmp( data[2], rhs[2] ));
		}

		// ???
		inline bool operator==( const pvector3 &rhs ) {
			return compare( rhs );
		}

		// addition
		inline pvector3 operator+( const pvector3 &rhs ) const {
			pvector3 result = { data[0] + rhs[0], data[1] + rhs[1], data[2] + rhs[2] };
			return result;
		}

		// ???
        inline pvector3& operator+=( const pvector3 &rhs ) {
			data[0] += rhs[0];
			data[1] += rhs[1];
			data[2] += rhs[2];
			return *this;
		}

		// subtraction
		inline pvector3 operator-( const pvector3 &rhs ) const {
			pvector3 result = { data[0] - rhs[0], data[1] - rhs[1], data[2] - rhs[2] };
			return result;
		}

		// ???
        inline pvector3& operator-=( const pvector3& rhs ) {
			data[0] -= rhs[0];
			data[1] -= rhs[1];
			data[2] -= rhs[2];
			return *this;
		}

		// increment
		inline void increment( void ) {
			data[0] += 1.0;
			data[1] += 1.0;
			data[2] += 1.0;
		}

		// decrement
		inline void decrement( void ) {
			data[0] -= 1.0;
			data[1] -= 1.0;
			data[2] -= 1.0;
		}

		// inverse
		inline void inverse( void ) {
			data[0] = -data[0];
			data[1] = -data[1];
			data[2] = -data[2];
		}

		// ???
		static inline pvector3 inverse( const pvector3 &in ) XS_WARN_UNUSED_RESULT {
			pvector3 result = { -in[0], -in[1], -in[2] };
			return result;
		}

		// scalar multiplication
		inline pvector3 operator*( const real64_t scalar ) const {
			pvector3 result = { data[0] * scalar, data[1] * scalar, data[2] * scalar };
			return result;
		}

		// ???
		inline pvector3& operator*=( const real64_t scalar ) {
			data[0] *= scalar;
			data[1] *= scalar;
			data[2] *= scalar;
			return *this;
		}

		// vector multiplication
		inline pvector3 operator*( const pvector3 &rhs ) const {
			pvector3 result = { data[0] * rhs[0], data[1] * rhs[1], data[2] * rhs[2] };
			return result;
		}

		// ???
		inline pvector3& operator*=( const pvector3 &rhs ) {
			data[0] *= rhs[0];
			data[1] *= rhs[1];
			data[2] *= rhs[2];
			return *this;
		}

		// scalar division
		inline pvector3 operator/( const real64_t divisor ) const {
			pvector3 result = { data[0] / divisor, data[1] / divisor, data[2] / divisor };
			return result;
		}
		inline pvector3& operator/=( const real64_t divisor ) {
			data[0] /= divisor;
			data[1] /= divisor;
			data[2] /= divisor;
			return *this;
		}

		// vector division
		inline pvector3 operator/( const pvector3 &rhs ) const {
			pvector3 result = { data[0] / rhs[0], data[1] / rhs[1], data[2] / rhs[2] };
			return result;
		}
		inline pvector3& operator/=( const pvector3 &rhs ) {
			data[0] /= rhs[0];
			data[1] /= rhs[1];
			data[2] /= rhs[2];
			return *this;
		}

		// ???
		static inline pvector3 ma( const pvector3 &a, real64_t scale, const pvector3 &b ) XS_WARN_UNUSED_RESULT {
			return a + b*scale;
		}

		// linear interpolation
		static inline pvector3 lerp( const pvector3 &a, real64_t scale, const pvector3 &b ) XS_WARN_UNUSED_RESULT {
			return a + (b-a)*scale;
		}

		// length
		inline real64_t length( void ) const XS_WARN_UNUSED_RESULT {
			return sqrt( data[0]*data[0] + data[1]*data[1] + data[2]*data[2] );
		}
		static inline real64_t length( const pvector3 &vec ) XS_WARN_UNUSED_RESULT {
			return vec.length();
		}

		// length squared
		inline real64_t lengthSquared( void ) const XS_WARN_UNUSED_RESULT {
			return data[0]*data[0] + data[1]*data[1] + data[2]*data[2];
		}
		static inline real64_t lengthSquared( const pvector3 &vec ) XS_WARN_UNUSED_RESULT {
			return vec.lengthSquared();
		}

		// distance
		static inline real64_t distance( const pvector3 &a, const pvector3 &b ) XS_WARN_UNUSED_RESULT {
			return length( a - b );
		}
		static inline real64_t distanceSquared( const pvector3 &a, const pvector3 &b ) XS_WARN_UNUSED_RESULT {
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
		inline real64_t dot( const pvector3 &vec ) const XS_WARN_UNUSED_RESULT {
			return data[0]*vec[0] + data[1]*vec[1] + data[1]*vec[2];
		}

		// cross
		static inline pvector3 cross( const pvector3 &a, const pvector3 &b ) XS_WARN_UNUSED_RESULT {
			pvector3 result = {
				(a[1]*b[2]) - (a[2]*b[1]),
				(a[2]*b[0]) - (a[0]*b[2]),
				(a[0]*b[1]) - (a[1]*b[0])
			};
			return result;
		}
		inline pvector3 cross( const pvector3 &v ) const XS_WARN_UNUSED_RESULT {
			return cross( *this, v );
		}

		//TODO: snap?
	};

	struct vector4 {
		real32_t data[4];

		// string representation of the vector
		std::string tostring(
			void
		) const XS_WARN_UNUSED_RESULT;

		// access operators
		inline const real32_t& operator[]( const size_t idx ) const {
			return data[idx];
		}
		inline real32_t& operator[]( const size_t idx ) {
			return data[idx];
		}

		// clear
		inline void clear( void ) {
			data[0] = 0.0f;
			data[1] = 0.0f;
			data[2] = 0.0f;
			data[3] = 0.0f;
		}

		// compare
		static inline bool compare( const vector4 &lhs, const vector4 &rhs ) XS_WARN_UNUSED_RESULT {
			return (flcmp( lhs[0], rhs[0] ) && flcmp( lhs[1], rhs[1] ) && flcmp( lhs[2], rhs[2] ) && flcmp( lhs[3], rhs[3] ));
		}
		inline bool compare( const vector4 &rhs ) const XS_WARN_UNUSED_RESULT {
			return (flcmp( data[0], rhs[0] ) && flcmp( data[1], rhs[1] ) && flcmp( data[2], rhs[2] ) && flcmp( data[3], rhs[3] ));
		}
		inline bool operator==( const vector4 &rhs ) const {
			return compare( rhs );
		}

		// addition
		inline vector4 operator+( const vector4 &rhs ) const {
			vector4 result = { data[0] + rhs[0], data[1] + rhs[1], data[2] + rhs[2], data[3] + rhs[3] };
			return result;
		}
        inline vector4& operator+=( const vector4& rhs ) {
			data[0] += rhs[0];
			data[1] += rhs[1];
			data[2] += rhs[2];
			data[3] += rhs[3];
			return *this;
		}

		// subtraction
		inline vector4 operator-( const vector4 &rhs ) const {
			vector4 result = { data[0] - rhs[0], data[1] - rhs[1], data[2] - rhs[2], data[3] - rhs[3] };
			return result;
		}
        inline vector4& operator-=( const vector4& rhs ) {
			data[0] -= rhs[0];
			data[1] -= rhs[1];
			data[2] -= rhs[2];
			data[3] -= rhs[3];
			return *this;
		}

		// increment
		inline void increment( void ) {
			data[0] += 1.0f;
			data[1] += 1.0f;
			data[2] += 1.0f;
			data[3] += 1.0f;
		}

		// decrement
		inline void decrement( void ) {
			data[0] -= 1.0f;
			data[1] -= 1.0f;
			data[2] -= 1.0f;
			data[3] -= 1.0f;
		}

		// inverse
		inline void inverse( void ) {
			data[0] = -data[0];
			data[1] = -data[1];
			data[2] = -data[2];
			data[3] = -data[3];
		}
		static inline vector4 inverse( const vector4 &in ) XS_WARN_UNUSED_RESULT {
			vector4 result = { -in[0], -in[1], -in[2], -in[3] };
			return result;
		}

		// scalar multiplication
		inline vector4 operator*( const real32_t scalar ) const {
			vector4 result = { data[0] * scalar, data[1] * scalar, data[2] * scalar, data[3] * scalar };
			return result;
		}
		inline vector4& operator*=( const real32_t scalar ) {
			data[0] *= scalar;
			data[1] *= scalar;
			data[2] *= scalar;
			data[3] *= scalar;
			return *this;
		}

		// vector multiplication
		inline vector4 operator*( const vector4 &rhs ) const {
			vector4 result = { data[0] * rhs[0], data[1] * rhs[1], data[2] * rhs[2], data[3] * rhs[3] };
			return result;
		}
		inline vector4& operator*=( const vector4 &rhs ) {
			data[0] *= rhs[0];
			data[1] *= rhs[1];
			data[2] *= rhs[2];
			data[3] *= rhs[3];
			return *this;
		}

		// scalar division
		inline vector4 operator/( const real32_t divisor ) const {
			vector4 result = { data[0] / divisor, data[1] / divisor, data[2] / divisor, data[3] / divisor };
			return result;
		}
		inline vector4& operator/=( const real32_t divisor ) {
			data[0] /= divisor;
			data[1] /= divisor;
			data[2] /= divisor;
			data[3] /= divisor;
			return *this;
		}

		// vector division
		inline vector4 operator/( const vector4 &rhs ) const {
			vector4 result = { data[0] / rhs[0], data[1] / rhs[1], data[2] / rhs[2], data[3] / rhs[3] };
			return result;
		}
		inline vector4& operator/=( const vector4 &rhs ) {
			data[0] /= rhs[0];
			data[1] /= rhs[1];
			data[2] /= rhs[2];
			data[3] /= rhs[3];
			return *this;
		}

		// ???
		static inline vector4 ma( const vector4 &a, real32_t scale, const vector4 &b ) XS_WARN_UNUSED_RESULT {
			return a + b*scale;
		}

		// linear interpolation
		static inline vector4 lerp( const vector4 &a, real32_t scale, const vector4 &b ) XS_WARN_UNUSED_RESULT {
			return a + (b-a)*scale;
		}
	};

} // namespace XS
