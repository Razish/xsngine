#pragma once

#include <string>

#include "XSCommon/XSVector.h"

namespace XS {

	// typical C layout for multidimensional array is row-major, i.e. you read it row by row
	// we will store matrices in row-major and offer transposition for opengl

	struct matrix3 {
		real32_t	data[9];

		// string representation of the matrix3 object
		std::string tostring(
			void
		) const;

		// access (r/w) an element
		inline real32_t &at( size_t row, size_t column ) {
			return data[(row * 3) + column];
		}
		// access (r/w) an element
		inline real32_t at( size_t row, size_t column ) const {
			return data[(row * 3) + column];
		}

		// clear the matrix
		inline void clear( void ) {
			memset( data, 0, sizeof(data) );
		}

		// initialise with an identity matrix
		inline void identity( void ) {
			clear();
			at( 0, 0 ) = 1.0f;
			at( 1, 1 ) = 1.0f;
			at( 2, 2 ) = 1.0f;
		}

		// transpose between row-major and column-major
		inline void transpose( void ) {
			matrix3 m = *this;

			for ( size_t x = 0u; x < 3u; x++ ) {
				for ( size_t y = 0u; y < 3u; y++ ) {
					m.at( y, x ) = at( x, y );
				}
			}

			*this = m;
		}

		inline matrix3 invert( void ) {
			real32_t determinant = 0.0f;
			real32_t invDeterminant = 0.0f;
			real32_t tmp[9];

			tmp[0] = data[4] * data[8] - data[5] * data[7];
			tmp[1] = data[2] * data[7] - data[1] * data[8];
			tmp[2] = data[1] * data[5] - data[2] * data[4];
			tmp[3] = data[5] * data[6] - data[3] * data[8];
			tmp[4] = data[0] * data[8] - data[2] * data[6];
			tmp[5] = data[2] * data[3] - data[0] * data[5];
			tmp[6] = data[3] * data[7] - data[4] * data[6];
			tmp[7] = data[1] * data[6] - data[0] * data[7];
			tmp[8] = data[0] * data[4] - data[1] * data[3];

			determinant = data[0] * tmp[0] + data[1] * tmp[3] + data[2] * tmp[6];
			if ( std::abs( determinant ) <= 0.00000f ) {
				return matrix3();
			}

			invDeterminant = 1.0f / determinant;
			data[0] = invDeterminant * tmp[0];
			data[1] = invDeterminant * tmp[1];
			data[2] = invDeterminant * tmp[2];
			data[3] = invDeterminant * tmp[3];
			data[4] = invDeterminant * tmp[4];
			data[5] = invDeterminant * tmp[5];
			data[6] = invDeterminant * tmp[6];
			data[7] = invDeterminant * tmp[7];
			data[8] = invDeterminant * tmp[8];

			return *this;
		}

		// matrix multiplication
		inline matrix3 operator*( const matrix3 &rhs ) const {
			matrix3 m;
			//TODO: matrix3::operator*
			return m;
		}

		// matrix multiplication
		inline matrix3& operator*=( const matrix3 &rhs ) {
			*this = *this * rhs;
			return *this;
		}
	};

	struct matrix4 {
		real32_t	data[16];

		// string representation of the matrix4 object
		std::string tostring(
			void
		) const;

		// access (r/w) an element
		inline real32_t &at( size_t row, size_t column ) {
			return data[(row * 4) + column];
		}
		// access (r/w) an element
		inline real32_t at( size_t row, size_t column ) const {
			return data[(row * 4) + column];
		}

		// clear the matrix
		inline void clear( void ) {
			memset( data, 0, sizeof(data) );
		}

		// initialise with an identity matrix
		inline void identity( void ) {
			clear();
			at( 0, 0 ) = 1.0f;
			at( 1, 1 ) = 1.0f;
			at( 2, 2 ) = 1.0f;
			at( 3, 3 ) = 1.0f;
		}

		// transpose between row-major and column-major
		inline void transpose( void ) {
			matrix4 m = *this;

			for ( size_t x = 0u; x < 4u; x++ ) {
				for ( size_t y = 0u; y < 4u; y++ ) {
					m.at( y, x ) = at( x, y );
				}
			}

			*this = m;
		}

		inline matrix4 invertAffine( void ) {
			matrix3 r = {};
			r.identity();
			r.data[0] = data[ 0];
			r.data[1] = data[ 1];
			r.data[2] = data[ 2];

			r.data[3] = data[ 4];
			r.data[4] = data[ 5];
			r.data[5] = data[ 6];

			r.data[6] = data[ 8];
			r.data[7] = data[ 9];
			r.data[8] = data[10];
			r.invert();

			data[ 0] = r.data[0];	data[ 1] = r.data[1];	data[ 2] = r.data[2];
			data[ 4] = r.data[3];	data[ 5] = r.data[4];	data[ 6] = r.data[5];
			data[ 8] = r.data[6];	data[ 9] = r.data[7];	data[10] = r.data[8];

			real32_t x = data[12];
			real32_t y = data[13];
			real32_t z = data[14];

			data[12] = -(r.data[0] * x + r.data[3] * y + r.data[6] * z);
			data[13] = -(r.data[1] * x + r.data[4] * y + r.data[7] * z);
			data[14] = -(r.data[2] * x + r.data[5] * y + r.data[8] * z);

			return *this;
		}

		inline real32_t getCofactor( real32_t m0, real32_t m1, real32_t m2, real32_t m3, real32_t m4, real32_t m5,
			real32_t m6, real32_t m7, real32_t m8 )
		{
			return m0 * (m4 * m8 - m5 * m7)
				- m1 * (m3 * m8 - m5 * m6)
				+ m2 * (m3 * m7 - m4 * m6);
		}

		inline matrix4 invertGeneral( void ) {
			real32_t cofactor0 = getCofactor( data[5], data[6], data[7], data[9], data[10], data[11], data[13], data[14], data[15] );
			real32_t cofactor1 = getCofactor( data[4], data[6], data[7], data[8], data[10], data[11], data[12], data[14], data[15] );
			real32_t cofactor2 = getCofactor( data[4], data[5], data[7], data[8], data[ 9], data[11], data[12], data[13], data[15] );
			real32_t cofactor3 = getCofactor( data[4], data[5], data[6], data[8], data[ 9], data[10], data[12], data[13], data[14] );

			real32_t determinant = data[0] * cofactor0 - data[1] * cofactor1 + data[2] * cofactor2 - data[3] * cofactor3;
			if ( std::abs( determinant ) < 0.000001f ) {
				matrix4 m;
				m.identity();
				return m;
			}

			real32_t cofactor4  = getCofactor( data[1], data[2], data[3], data[9], data[10], data[11], data[13], data[14], data[15] );
			real32_t cofactor5  = getCofactor( data[0], data[2], data[3], data[8], data[10], data[11], data[12], data[14], data[15] );
			real32_t cofactor6  = getCofactor( data[0], data[1], data[3], data[8], data[ 9], data[11], data[12], data[13], data[15] );
			real32_t cofactor7  = getCofactor( data[0], data[1], data[2], data[8], data[ 9], data[10], data[12], data[13], data[14] );

			real32_t cofactor8  = getCofactor( data[1], data[2], data[3], data[5], data[6], data[7], data[13], data[14], data[15] );
			real32_t cofactor9  = getCofactor( data[0], data[2], data[3], data[4], data[6], data[7], data[12], data[14], data[15] );
			real32_t cofactor10 = getCofactor( data[0], data[1], data[3], data[4], data[5], data[7], data[12], data[13], data[15] );
			real32_t cofactor11 = getCofactor( data[0], data[1], data[2], data[4], data[5], data[6], data[12], data[13], data[14] );

			real32_t cofactor12 = getCofactor( data[1], data[2], data[3], data[5], data[6], data[7], data[9], data[10], data[11] );
			real32_t cofactor13 = getCofactor( data[0], data[2], data[3], data[4], data[6], data[7], data[8], data[10], data[11] );
			real32_t cofactor14 = getCofactor( data[0], data[1], data[3], data[4], data[5], data[7], data[8], data[ 9], data[11] );
			real32_t cofactor15 = getCofactor( data[0], data[1], data[2], data[4], data[5], data[6], data[8], data[ 9], data[10] );

			real32_t invDeterminant = 1.0f / determinant;

			data[ 0] =  invDeterminant * cofactor0;
			data[ 1] = -invDeterminant * cofactor4;
			data[ 2] =  invDeterminant * cofactor8;
			data[ 3] = -invDeterminant * cofactor12;

			data[ 4] = -invDeterminant * cofactor1;
			data[ 5] =  invDeterminant * cofactor5;
			data[ 6] = -invDeterminant * cofactor9;
			data[ 7] =  invDeterminant * cofactor13;

			data[ 8] =  invDeterminant * cofactor2;
			data[ 9] = -invDeterminant * cofactor6;
			data[10] =  invDeterminant * cofactor10;
			data[11] = -invDeterminant * cofactor14;

			data[12] = -invDeterminant * cofactor3;
			data[13] =  invDeterminant * cofactor7;
			data[14] = -invDeterminant * cofactor11;
			data[15] =  invDeterminant * cofactor15;

			return *this;
		}

		inline matrix4 invert( void ) {
			matrix4 m = *this;
			if ( flcmp( data[ 3], 0.0f )
				&& flcmp( data[ 7], 0.0f )
				&& flcmp( data[11], 0.0f )
				&& flcmp( data[15], 0.0f ) )
			{
				m.invertAffine();
			}
			else
			{
				m.invertGeneral();
			}

			return m;
		}

		inline matrix4 inverse( void ) const {
			//FIXME: affine/general inversion?
			const real32_t a0 = (at( 0, 0 ) * at( 1, 1 )) - (at( 0, 1 ) * at( 1, 0 ));
			const real32_t a1 = (at( 0, 0 ) * at( 1, 2 )) - (at( 0, 2 ) * at( 1, 0 ));
			const real32_t a2 = (at( 0, 0 ) * at( 1, 3 )) - (at( 0, 3 ) * at( 1, 0 ));
			const real32_t a3 = (at( 0, 1 ) * at( 1, 2 )) - (at( 0, 2 ) * at( 1, 1 ));
			const real32_t a4 = (at( 0, 1 ) * at( 1, 3 )) - (at( 0, 3 ) * at( 1, 1 ));
			const real32_t a5 = (at( 0, 2 ) * at( 1, 3 )) - (at( 0, 3 ) * at( 1, 2 ));
			const real32_t b0 = (at( 2, 0 ) * at( 3, 1 )) - (at( 2, 1 ) * at( 3, 0 ));
			const real32_t b1 = (at( 2, 0 ) * at( 3, 2 )) - (at( 2, 2 ) * at( 3, 0 ));
			const real32_t b2 = (at( 2, 0 ) * at( 3, 3 )) - (at( 2, 3 ) * at( 3, 0 ));
			const real32_t b3 = (at( 2, 1 ) * at( 3, 2 )) - (at( 2, 2 ) * at( 3, 1 ));
			const real32_t b4 = (at( 2, 1 ) * at( 3, 3 )) - (at( 2, 3 ) * at( 3, 1 ));
			const real32_t b5 = (at( 2, 2 ) * at( 3, 3 )) - (at( 2, 3 ) * at( 3, 2 ));

			const real32_t determinant = (a0 * b5) - (a1 * b4) + (a2 * b3) + (a3 * b2) - (a4 * b1) + (a5 * b0);
			const real32_t epsilon = 0.00001f;
			if ( std::abs( determinant ) <= epsilon ) {
				return matrix4(); // identity
			}

			matrix4 inv;
			inv.at( 0, 0 ) = + at( 1, 1 ) * b5 - at( 1, 2 ) * b4 + at( 1, 3 ) * b3;
			inv.at( 1, 0 ) = - at( 1, 0 ) * b5 + at( 1, 2 ) * b2 - at( 1, 3 ) * b1;
			inv.at( 2, 0 ) = + at( 1, 0 ) * b4 - at( 1, 1 ) * b2 + at( 1, 3 ) * b0;
			inv.at( 3, 0 ) = - at( 1, 0 ) * b3 + at( 1, 1 ) * b1 - at( 1, 2 ) * b0;
			inv.at( 0, 1 ) = - at( 0, 1 ) * b5 + at( 0, 2 ) * b4 - at( 0, 3 ) * b3;
			inv.at( 1, 1 ) = + at( 0, 0 ) * b5 - at( 0, 2 ) * b2 + at( 0, 3 ) * b1;
			inv.at( 2, 1 ) = - at( 0, 0 ) * b4 + at( 0, 1 ) * b2 - at( 0, 3 ) * b0;
			inv.at( 3, 1 ) = + at( 0, 0 ) * b3 - at( 0, 1 ) * b1 + at( 0, 3 ) * b0;
			inv.at( 0, 2 ) = + at( 3, 1 ) * a5 - at( 3, 2 ) * a4 + at( 3, 3 ) * a3;
			inv.at( 1, 2 ) = - at( 3, 0 ) * a5 + at( 3, 2 ) * a2 - at( 3, 3 ) * a1;
			inv.at( 2, 2 ) = + at( 3, 0 ) * a4 - at( 3, 1 ) * a2 + at( 3, 3 ) * a0;
			inv.at( 3, 2 ) = - at( 3, 0 ) * a3 + at( 3, 1 ) * a1 - at( 3, 2 ) * a0;
			inv.at( 0, 3 ) = - at( 2, 1 ) * a5 + at( 2, 2 ) * a4 - at( 2, 3 ) * a3;
			inv.at( 1, 3 ) = + at( 2, 0 ) * a5 - at( 2, 2 ) * a2 + at( 2, 3 ) * a1;
			inv.at( 2, 3 ) = - at( 2, 0 ) * a4 + at( 2, 1 ) * a2 - at( 2, 3 ) * a0;
			inv.at( 3, 3 ) = + at( 2, 0 ) * a3 - at( 2, 1 ) * a1 + at( 2, 2 ) * a0;

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

			m.at( 0, 0 ) = (data[ 0] * rhs.data[ 0]) + (data[ 4] * rhs.data[ 1]) + (data[ 8] * rhs.data[ 2]) + (data[12] * rhs.data[ 3]);
			m.at( 0, 1 ) = (data[ 1] * rhs.data[ 0]) + (data[ 5] * rhs.data[ 1]) + (data[ 9] * rhs.data[ 2]) + (data[13] * rhs.data[ 3]);
			m.at( 0, 2 ) = (data[ 2] * rhs.data[ 0]) + (data[ 6] * rhs.data[ 1]) + (data[10] * rhs.data[ 2]) + (data[14] * rhs.data[ 3]);
			m.at( 0, 3 ) = (data[ 3] * rhs.data[ 0]) + (data[ 7] * rhs.data[ 1]) + (data[11] * rhs.data[ 2]) + (data[15] * rhs.data[ 3]);

			m.at( 1, 0 ) = (data[ 0] * rhs.data[ 4]) + (data[ 4] * rhs.data[ 5]) + (data[ 8] * rhs.data[ 6]) + (data[12] * rhs.data[ 7]);
			m.at( 1, 1 ) = (data[ 1] * rhs.data[ 4]) + (data[ 5] * rhs.data[ 5]) + (data[ 9] * rhs.data[ 6]) + (data[13] * rhs.data[ 7]);
			m.at( 1, 2 ) = (data[ 2] * rhs.data[ 4]) + (data[ 6] * rhs.data[ 5]) + (data[10] * rhs.data[ 6]) + (data[14] * rhs.data[ 7]);
			m.at( 1, 3 ) = (data[ 3] * rhs.data[ 4]) + (data[ 7] * rhs.data[ 5]) + (data[11] * rhs.data[ 6]) + (data[15] * rhs.data[ 7]);

			m.at( 2, 0 ) = (data[ 0] * rhs.data[ 8]) + (data[ 4] * rhs.data[ 9]) + (data[ 8] * rhs.data[10]) + (data[12] * rhs.data[11]);
			m.at( 2, 1 ) = (data[ 1] * rhs.data[ 8]) + (data[ 5] * rhs.data[ 9]) + (data[ 9] * rhs.data[10]) + (data[13] * rhs.data[11]);
			m.at( 2, 2 ) = (data[ 2] * rhs.data[ 8]) + (data[ 6] * rhs.data[ 9]) + (data[10] * rhs.data[10]) + (data[14] * rhs.data[11]);
			m.at( 2, 3 ) = (data[ 3] * rhs.data[ 8]) + (data[ 7] * rhs.data[ 9]) + (data[11] * rhs.data[10]) + (data[15] * rhs.data[11]);

			m.at( 3, 0 ) = (data[ 0] * rhs.data[12]) + (data[ 4] * rhs.data[13]) + (data[ 8] * rhs.data[14]) + (data[12] * rhs.data[15]);
			m.at( 3, 1 ) = (data[ 1] * rhs.data[12]) + (data[ 5] * rhs.data[13]) + (data[ 9] * rhs.data[14]) + (data[13] * rhs.data[15]);
			m.at( 3, 2 ) = (data[ 2] * rhs.data[12]) + (data[ 6] * rhs.data[13]) + (data[10] * rhs.data[14]) + (data[14] * rhs.data[15]);
			m.at( 3, 3 ) = (data[ 3] * rhs.data[12]) + (data[ 7] * rhs.data[13]) + (data[11] * rhs.data[14]) + (data[15] * rhs.data[15]);

			return m;
		}

		// matrix multiplication
		inline matrix4& operator*=( const matrix4 &rhs ) {
			*this = *this * rhs;
			return *this;
		}
	};

	inline matrix4 ortho( real32_t left, real32_t right, real32_t top, real32_t bottom, real32_t zNear, real32_t zFar )
	{
		matrix4 m = {};
		m.identity();

		m.at( 0, 0 ) = 2.0f / (right - left);
		m.at( 1, 1 ) = 2.0f / (top - bottom);
		m.at( 2, 2 ) = 2.0f / (zNear - zFar);
		m.at( 3, 0 ) = (right + left) / (left - right);
		m.at( 3, 1 ) = (top + bottom) / (bottom - top);
		m.at( 3, 2 ) = (zNear - zFar) / (zFar - zNear);

		return m;
	}

	inline matrix4 perspectiveFov( real32_t fovY, real32_t aspectRatio, real32_t zNear, real32_t zFar ) {
#if 0
		// convert degrees to radians and divide by 2
		const real32_t f = 1.0f / tanf( fovY * static_cast<real32_t>( M_PI ) / 360.0f );

		matrix4 m = {};
		m.identity();
		m.at( 0, 0 ) = f / aspectRatio;
		m.at( 1, 1 ) = f;
		m.at( 2, 2 ) = (zFar + zNear) / (zFar - zNear);
		m.at( 2, 3 ) = 2.0f * zFar * zNear / (zFar - zNear);
		m.at( 3, 2 ) = -1.0f;
		m.at( 3, 3 ) = 0.0f;
		return m;
#else
		const real32_t tanHalfFOV = tanf( ((fovY / 2.0f) * (M_PI / 180.0f)) );

		matrix4 m;
		m.at( 0, 0 ) = 1.0f / (tanHalfFOV * aspectRatio);
		m.at( 0, 1 ) = 0.0f;
		m.at( 0, 2 ) = 0.0f;
		m.at( 0, 3 ) = 0.0f;

		m.at( 1, 0 ) = 0.0f;
		m.at( 1, 1 ) = 1.0f / tanHalfFOV;
		m.at( 1, 2 ) = 0.0f;
		m.at( 1, 3 ) = 0.0f;

		m.at( 2, 1 ) = 0.0f;
		m.at( 2, 2 ) = 0.0f;
		m.at( 2, 0 ) = (-zNear - zFar) / (zNear - zFar);
		m.at( 2, 1 ) = 2.0f * zFar * zNear / (zNear - zFar);

		m.at( 3, 0 ) = 0.0f;
		m.at( 3, 1 ) = 0.0f;
		m.at( 3, 2 ) = 1.0f;
		m.at( 3, 3 ) = 0.0f;

		return m;
#endif
	}

} // namespace XS
