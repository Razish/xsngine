#pragma once

#include <string>

#include "XSCommon/XSVector.h"

namespace XS {

	// column-major 3D matrix
	struct matrix4 {
		real32_t	data[16];

		// xtors
		matrix4() {
			identity();
		}

		// string representation of the matrix4 object
		std::string tostring(
			void
		) const;

		inline real32_t& operator[]( const size_t idx ) {
			return data[idx];
		}
		inline const real32_t& operator[]( const size_t idx ) const {
			return data[idx];
		}
		inline real32_t &at( size_t column, size_t row ) {
			return data[(column * 4) + row];
		}

		// clear the matrix
		inline void clear( void ) {
			memset( data, 0, sizeof(data) );
		}

		// initialise with an identity matrix
		inline void identity( void ) {
			clear();
			data[0] = data[5] = data[10] = data[15] = 1.0f;
		}

		inline void transpose( void ) {
			matrix4 m = *this;

			for ( size_t x = 0u; x < 4u; x++ ) {
				for ( size_t y = 0u; y < 4u; y++ ) {
					m.at( y, x ) = at( x, y );
				}
			}

			*this = m;
		}

		inline matrix4 inverse( void ) const {
			const real32_t a0 = (data[ 0] * data[ 5]) - (data[ 1] * data[ 4]);
			const real32_t a1 = (data[ 0] * data[ 6]) - (data[ 2] * data[ 4]);
			const real32_t a2 = (data[ 0] * data[ 7]) - (data[ 3] * data[ 4]);
			const real32_t a3 = (data[ 1] * data[ 6]) - (data[ 2] * data[ 5]);
			const real32_t a4 = (data[ 1] * data[ 7]) - (data[ 3] * data[ 5]);
			const real32_t a5 = (data[ 2] * data[ 7]) - (data[ 3] * data[ 6]);
			const real32_t b0 = (data[ 8] * data[13]) - (data[ 9] * data[12]);
			const real32_t b1 = (data[ 8] * data[14]) - (data[10] * data[12]);
			const real32_t b2 = (data[ 8] * data[15]) - (data[11] * data[12]);
			const real32_t b3 = (data[ 9] * data[14]) - (data[10] * data[13]);
			const real32_t b4 = (data[ 9] * data[15]) - (data[11] * data[13]);
			const real32_t b5 = (data[10] * data[15]) - (data[11] * data[14]);

			const real32_t determinant = (a0 * b5) - (a1 * b4) + (a2 * b3) + (a3 * b2) - (a4 * b1) + (a5 * b0);
			const real32_t epsilon = 0.00001f;
			if ( std::abs( determinant ) <= epsilon ) {
				return matrix4(); // identity
			}

			matrix4 inv;
			inv[ 0] = + data[ 5] * b5 - data[ 6] * b4 + data[ 7] * b3;
			inv[ 4] = - data[ 4] * b5 + data[ 6] * b2 - data[ 7] * b1;
			inv[ 8] = + data[ 4] * b4 - data[ 5] * b2 + data[ 7] * b0;
			inv[12] = - data[ 4] * b3 + data[ 5] * b1 - data[ 6] * b0;
			inv[ 1] = - data[ 1] * b5 + data[ 2] * b4 - data[ 3] * b3;
			inv[ 5] = + data[ 0] * b5 - data[ 2] * b2 + data[ 3] * b1;
			inv[ 9] = - data[ 0] * b4 + data[ 1] * b2 - data[ 3] * b0;
			inv[13] = + data[ 0] * b3 - data[ 1] * b1 + data[ 2] * b0;
			inv[ 2] = + data[13] * a5 - data[14] * a4 + data[15] * a3;
			inv[ 6] = - data[12] * a5 + data[14] * a2 - data[15] * a1;
			inv[10] = + data[12] * a4 - data[13] * a2 + data[15] * a0;
			inv[14] = - data[12] * a3 + data[13] * a1 - data[14] * a0;
			inv[ 3] = - data[ 9] * a5 + data[10] * a4 - data[11] * a3;
			inv[ 7] = + data[ 8] * a5 - data[10] * a2 + data[11] * a1;
			inv[11] = - data[ 8] * a4 + data[ 9] * a2 - data[11] * a0;
			inv[15] = + data[ 8] * a3 - data[ 9] * a1 + data[10] * a0;

			const real32_t invDeterminant = 1.0f / determinant;
			for ( auto &f : inv.data ) {
				f *= invDeterminant;
			}

			return inv;
		}

		static inline matrix4 lookAt( const vector3 &eye, const vector3 &center, const vector3 &up ) {
			const vector3 f = vector3::normalise( center - eye );
			const vector3 s = vector3::normalise( vector3::cross( f, up ) );
			const vector3 u = vector3::cross( s, f );

			matrix4 m;

			m.at( 0, 0 ) =  s.x;
			m.at( 1, 0 ) =  s.y;
			m.at( 2, 0 ) =  s.z;

			m.at( 0, 1 ) =  u.x;
			m.at( 1, 1 ) =  u.y;
			m.at( 2, 1 ) =  u.z;

			m.at( 0, 2 ) = -f.x;
			m.at( 1, 2 ) = -f.y;
			m.at( 2, 2 ) = -f.z;

			m.at( 3, 0 ) = -vector3::dot( s, eye );
			m.at( 3, 1 ) = -vector3::dot( u, eye );
			m.at( 3, 2 ) =  vector3::dot( f, eye );

			return m;
		}

		// matrix multiplication
		inline matrix4 operator*( const matrix4 &rhs ) const {
			matrix4 m;
			m[ 0] = (data[ 0] * rhs[ 0]) + (data[ 1] * rhs[ 4]) + (data[ 2] * rhs[ 8]) + (data[ 3] * rhs[12]);
			m[ 1] = (data[ 0] * rhs[ 1]) + (data[ 1] * rhs[ 5]) + (data[ 2] * rhs[ 9]) + (data[ 3] * rhs[13]);
			m[ 2] = (data[ 0] * rhs[ 2]) + (data[ 1] * rhs[ 6]) + (data[ 2] * rhs[10]) + (data[ 3] * rhs[14]);
			m[ 3] = (data[ 0] * rhs[ 3]) + (data[ 1] * rhs[ 7]) + (data[ 2] * rhs[11]) + (data[ 3] * rhs[15]);
			m[ 4] = (data[ 4] * rhs[ 0]) + (data[ 5] * rhs[ 4]) + (data[ 6] * rhs[ 8]) + (data[ 7] * rhs[12]);
			m[ 5] = (data[ 4] * rhs[ 1]) + (data[ 5] * rhs[ 5]) + (data[ 6] * rhs[ 9]) + (data[ 7] * rhs[13]);
			m[ 6] = (data[ 4] * rhs[ 2]) + (data[ 5] * rhs[ 6]) + (data[ 6] * rhs[10]) + (data[ 7] * rhs[14]);
			m[ 7] = (data[ 4] * rhs[ 3]) + (data[ 5] * rhs[ 7]) + (data[ 6] * rhs[11]) + (data[ 7] * rhs[15]);
			m[ 8] = (data[ 8] * rhs[ 0]) + (data[ 9] * rhs[ 4]) + (data[10] * rhs[ 8]) + (data[11] * rhs[12]);
			m[ 9] = (data[ 8] * rhs[ 1]) + (data[ 9] * rhs[ 5]) + (data[10] * rhs[ 9]) + (data[11] * rhs[13]);
			m[10] = (data[ 8] * rhs[ 2]) + (data[ 9] * rhs[ 6]) + (data[10] * rhs[10]) + (data[11] * rhs[14]);
			m[11] = (data[ 8] * rhs[ 3]) + (data[ 9] * rhs[ 7]) + (data[10] * rhs[11]) + (data[11] * rhs[15]);
			m[12] = (data[12] * rhs[ 0]) + (data[13] * rhs[ 4]) + (data[14] * rhs[ 8]) + (data[15] * rhs[12]);
			m[13] = (data[12] * rhs[ 1]) + (data[13] * rhs[ 5]) + (data[14] * rhs[ 9]) + (data[15] * rhs[13]);
			m[14] = (data[12] * rhs[ 2]) + (data[13] * rhs[ 6]) + (data[14] * rhs[10]) + (data[15] * rhs[14]);
			m[15] = (data[12] * rhs[ 3]) + (data[13] * rhs[ 7]) + (data[14] * rhs[11]) + (data[15] * rhs[15]);
			return m;
		}

		// matrix multiplication
		inline matrix4& operator*=( const matrix4 &rhs ) {
			matrix4 m;
			m[ 0] = (data[ 0] * rhs[ 0]) + (data[ 1] * rhs[ 4]) + (data[ 2] * rhs[ 8]) + (data[ 3] * rhs[12]);
			m[ 1] = (data[ 0] * rhs[ 1]) + (data[ 1] * rhs[ 5]) + (data[ 2] * rhs[ 9]) + (data[ 3] * rhs[13]);
			m[ 2] = (data[ 0] * rhs[ 2]) + (data[ 1] * rhs[ 6]) + (data[ 2] * rhs[10]) + (data[ 3] * rhs[14]);
			m[ 3] = (data[ 0] * rhs[ 3]) + (data[ 1] * rhs[ 7]) + (data[ 2] * rhs[11]) + (data[ 3] * rhs[15]);
			m[ 4] = (data[ 4] * rhs[ 0]) + (data[ 5] * rhs[ 4]) + (data[ 6] * rhs[ 8]) + (data[ 7] * rhs[12]);
			m[ 5] = (data[ 4] * rhs[ 1]) + (data[ 5] * rhs[ 5]) + (data[ 6] * rhs[ 9]) + (data[ 7] * rhs[13]);
			m[ 6] = (data[ 4] * rhs[ 2]) + (data[ 5] * rhs[ 6]) + (data[ 6] * rhs[10]) + (data[ 7] * rhs[14]);
			m[ 7] = (data[ 4] * rhs[ 3]) + (data[ 5] * rhs[ 7]) + (data[ 6] * rhs[11]) + (data[ 7] * rhs[15]);
			m[ 8] = (data[ 8] * rhs[ 0]) + (data[ 9] * rhs[ 4]) + (data[10] * rhs[ 8]) + (data[11] * rhs[12]);
			m[ 9] = (data[ 8] * rhs[ 1]) + (data[ 9] * rhs[ 5]) + (data[10] * rhs[ 9]) + (data[11] * rhs[13]);
			m[10] = (data[ 8] * rhs[ 2]) + (data[ 9] * rhs[ 6]) + (data[10] * rhs[10]) + (data[11] * rhs[14]);
			m[11] = (data[ 8] * rhs[ 3]) + (data[ 9] * rhs[ 7]) + (data[10] * rhs[11]) + (data[11] * rhs[15]);
			m[12] = (data[12] * rhs[ 0]) + (data[13] * rhs[ 4]) + (data[14] * rhs[ 8]) + (data[15] * rhs[12]);
			m[13] = (data[12] * rhs[ 1]) + (data[13] * rhs[ 5]) + (data[14] * rhs[ 9]) + (data[15] * rhs[13]);
			m[14] = (data[12] * rhs[ 2]) + (data[13] * rhs[ 6]) + (data[14] * rhs[10]) + (data[15] * rhs[14]);
			m[15] = (data[12] * rhs[ 3]) + (data[13] * rhs[ 7]) + (data[14] * rhs[11]) + (data[15] * rhs[15]);
			*this = m;
			return *this;
		}
	};

	inline matrix4 ortho( real32_t left, real32_t right, real32_t top, real32_t bottom, real32_t zNear, real32_t zFar )
	{
		matrix4 m;

		m.at( 0, 0 ) = 2.0f / (right - left);
		m.at( 1, 1 ) = 2.0f / (top - bottom);
		m.at( 2, 2 ) = 2.0f / (zNear - zFar);
		m.at( 3, 0 ) = (right + left) / (left - right);
		m.at( 3, 1 ) = (top + bottom) / (bottom - top);
		m.at( 3, 2 ) = (zNear - zFar) / (zFar - zNear);

		return m;
	}

	inline matrix4 perspectiveFov( real32_t fovY, real32_t aspectRatio, real32_t zNear, real32_t zFar ) {
		// convert degrees to radians and divide by 2
		const real32_t f = 1.0f / tanf( fovY * static_cast<real32_t>( M_PI ) / 360.0f );

		matrix4 m;
		m.at( 0, 0 ) = f / aspectRatio;
		m.at( 1, 1 ) = f;
		m.at( 2, 2 ) = (zFar + zNear) / (zFar - zNear);
		m.at( 2, 3 ) = 2.0f * zFar * zNear / (zFar - zNear);
		m.at( 3, 2 ) = -1.0f;
		m.at( 3, 3 ) = 0.0f;
		return m;
	}

} // namespace XS
