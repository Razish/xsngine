#pragma once

#include <string>

#include "XSCommon/XSVector.h"

namespace XS {

#define COLUMN_MAJOR

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

		// access (r/w) an element
		inline real32_t &at( size_t column, size_t row, bool doTranspose = false ) {
			if ( doTranspose ) {
				size_t tmp = column;
				column = row;
				row = tmp;
			}
#ifdef COLUMN_MAJOR
			return data[(column * 4) + row];
#else
			return data[(row * 4) + column];
#endif
		}
		// access (r/w) an element
		inline real32_t at( size_t column, size_t row, bool doTranspose = false ) const {
			if ( doTranspose ) {
				size_t tmp = column;
				column = row;
				row = tmp;
			}
#ifdef COLUMN_MAJOR
			return data[(column * 4) + row];
#else
			return data[(row * 4) + column];
#endif
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

		// transpose between column-major and row-major
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
			inv.at( 3, 1 ) = + at( 0, 0 ) * b3 - at( 0, 1 ) * b1 + data[ 2] * b0;
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
			m.at( 0, 0 ) = (at( 0, 0 ) * rhs.at( 0, 0 )) + (at( 0, 1 ) * rhs.at( 1, 0 )) + (at( 0, 2 ) * rhs.at( 2, 0 )) + (at( 0, 3 ) * rhs.at( 3, 0 ));
			m.at( 0, 1 ) = (at( 0, 0 ) * rhs.at( 0, 1 )) + (at( 0, 1 ) * rhs.at( 1, 1 )) + (at( 0, 2 ) * rhs.at( 2, 1 )) + (at( 0, 3 ) * rhs.at( 3, 1 ));
			m.at( 0, 2 ) = (at( 0, 0 ) * rhs.at( 0, 2 )) + (at( 0, 1 ) * rhs.at( 1, 2 )) + (at( 0, 2 ) * rhs.at( 2, 2 )) + (at( 0, 3 ) * rhs.at( 3, 2 ));
			m.at( 0, 3 ) = (at( 0, 0 ) * rhs.at( 0, 3 )) + (at( 0, 1 ) * rhs.at( 1, 3 )) + (at( 0, 2 ) * rhs.at( 2, 3 )) + (at( 0, 3 ) * rhs.at( 3, 3 ));
			m.at( 1, 0 ) = (at( 1, 0 ) * rhs.at( 0, 0 )) + (at( 1, 1 ) * rhs.at( 1, 0 )) + (at( 1, 2 ) * rhs.at( 2, 0 )) + (at( 1, 3 ) * rhs.at( 3, 0 ));
			m.at( 1, 1 ) = (at( 1, 0 ) * rhs.at( 0, 1 )) + (at( 1, 1 ) * rhs.at( 1, 1 )) + (at( 1, 2 ) * rhs.at( 2, 1 )) + (at( 1, 3 ) * rhs.at( 3, 1 ));
			m.at( 1, 2 ) = (at( 1, 0 ) * rhs.at( 0, 2 )) + (at( 1, 1 ) * rhs.at( 1, 2 )) + (at( 1, 2 ) * rhs.at( 2, 2 )) + (at( 1, 3 ) * rhs.at( 3, 2 ));
			m.at( 1, 3 ) = (at( 1, 0 ) * rhs.at( 0, 3 )) + (at( 1, 1 ) * rhs.at( 1, 3 )) + (at( 1, 2 ) * rhs.at( 2, 3 )) + (at( 1, 3 ) * rhs.at( 3, 3 ));
			m.at( 2, 0 ) = (at( 2, 0 ) * rhs.at( 0, 0 )) + (at( 2, 1 ) * rhs.at( 1, 0 )) + (at( 2, 2 ) * rhs.at( 2, 0 )) + (at( 2, 3 ) * rhs.at( 3, 0 ));
			m.at( 2, 1 ) = (at( 2, 0 ) * rhs.at( 0, 1 )) + (at( 2, 1 ) * rhs.at( 1, 1 )) + (at( 2, 2 ) * rhs.at( 2, 1 )) + (at( 2, 3 ) * rhs.at( 3, 1 ));
			m.at( 2, 2 ) = (at( 2, 0 ) * rhs.at( 0, 2 )) + (at( 2, 1 ) * rhs.at( 1, 2 )) + (at( 2, 2 ) * rhs.at( 2, 2 )) + (at( 2, 3 ) * rhs.at( 3, 2 ));
			m.at( 2, 3 ) = (at( 2, 0 ) * rhs.at( 0, 3 )) + (at( 2, 1 ) * rhs.at( 1, 3 )) + (at( 2, 2 ) * rhs.at( 2, 3 )) + (at( 2, 3 ) * rhs.at( 3, 3 ));
			m.at( 3, 0 ) = (at( 3, 0 ) * rhs.at( 0, 0 )) + (at( 3, 1 ) * rhs.at( 1, 0 )) + (at( 3, 2 ) * rhs.at( 2, 0 )) + (at( 3, 3 ) * rhs.at( 3, 0 ));
			m.at( 3, 1 ) = (at( 3, 0 ) * rhs.at( 0, 1 )) + (at( 3, 1 ) * rhs.at( 1, 1 )) + (at( 3, 2 ) * rhs.at( 2, 1 )) + (at( 3, 3 ) * rhs.at( 3, 1 ));
			m.at( 3, 2 ) = (at( 3, 0 ) * rhs.at( 0, 2 )) + (at( 3, 1 ) * rhs.at( 1, 2 )) + (at( 3, 2 ) * rhs.at( 2, 2 )) + (at( 3, 3 ) * rhs.at( 3, 2 ));
			m.at( 3, 3 ) = (at( 3, 0 ) * rhs.at( 0, 3 )) + (at( 3, 1 ) * rhs.at( 1, 3 )) + (at( 3, 2 ) * rhs.at( 2, 3 )) + (at( 3, 3 ) * rhs.at( 3, 3 ));
			return m;
		}

		// matrix multiplication
		inline matrix4& operator*=( const matrix4 &rhs ) {
			matrix4 m;
			m.at( 0, 0 ) = (at( 0, 0 ) * rhs.at( 0, 0 )) + (at( 0, 1 ) * rhs.at( 1, 0 )) + (at( 0, 2 ) * rhs.at( 2, 0 )) + (at( 0, 3 ) * rhs.at( 3, 0 ));
			m.at( 0, 1 ) = (at( 0, 0 ) * rhs.at( 0, 1 )) + (at( 0, 1 ) * rhs.at( 1, 1 )) + (at( 0, 2 ) * rhs.at( 2, 1 )) + (at( 0, 3 ) * rhs.at( 3, 1 ));
			m.at( 0, 2 ) = (at( 0, 0 ) * rhs.at( 0, 2 )) + (at( 0, 1 ) * rhs.at( 1, 2 )) + (at( 0, 2 ) * rhs.at( 2, 2 )) + (at( 0, 3 ) * rhs.at( 3, 2 ));
			m.at( 0, 3 ) = (at( 0, 0 ) * rhs.at( 0, 3 )) + (at( 0, 1 ) * rhs.at( 1, 3 )) + (at( 0, 2 ) * rhs.at( 2, 3 )) + (at( 0, 3 ) * rhs.at( 3, 3 ));
			m.at( 1, 0 ) = (at( 1, 0 ) * rhs.at( 0, 0 )) + (at( 1, 1 ) * rhs.at( 1, 0 )) + (at( 1, 2 ) * rhs.at( 2, 0 )) + (at( 1, 3 ) * rhs.at( 3, 0 ));
			m.at( 1, 1 ) = (at( 1, 0 ) * rhs.at( 0, 1 )) + (at( 1, 1 ) * rhs.at( 1, 1 )) + (at( 1, 2 ) * rhs.at( 2, 1 )) + (at( 1, 3 ) * rhs.at( 3, 1 ));
			m.at( 1, 2 ) = (at( 1, 0 ) * rhs.at( 0, 2 )) + (at( 1, 1 ) * rhs.at( 1, 2 )) + (at( 1, 2 ) * rhs.at( 2, 2 )) + (at( 1, 3 ) * rhs.at( 3, 2 ));
			m.at( 1, 3 ) = (at( 1, 0 ) * rhs.at( 0, 3 )) + (at( 1, 1 ) * rhs.at( 1, 3 )) + (at( 1, 2 ) * rhs.at( 2, 3 )) + (at( 1, 3 ) * rhs.at( 3, 3 ));
			m.at( 2, 0 ) = (at( 2, 0 ) * rhs.at( 0, 0 )) + (at( 2, 1 ) * rhs.at( 1, 0 )) + (at( 2, 2 ) * rhs.at( 2, 0 )) + (at( 2, 3 ) * rhs.at( 3, 0 ));
			m.at( 2, 1 ) = (at( 2, 0 ) * rhs.at( 0, 1 )) + (at( 2, 1 ) * rhs.at( 1, 1 )) + (at( 2, 2 ) * rhs.at( 2, 1 )) + (at( 2, 3 ) * rhs.at( 3, 1 ));
			m.at( 2, 2 ) = (at( 2, 0 ) * rhs.at( 0, 2 )) + (at( 2, 1 ) * rhs.at( 1, 2 )) + (at( 2, 2 ) * rhs.at( 2, 2 )) + (at( 2, 3 ) * rhs.at( 3, 2 ));
			m.at( 2, 3 ) = (at( 2, 0 ) * rhs.at( 0, 3 )) + (at( 2, 1 ) * rhs.at( 1, 3 )) + (at( 2, 2 ) * rhs.at( 2, 3 )) + (at( 2, 3 ) * rhs.at( 3, 3 ));
			m.at( 3, 0 ) = (at( 3, 0 ) * rhs.at( 0, 0 )) + (at( 3, 1 ) * rhs.at( 1, 0 )) + (at( 3, 2 ) * rhs.at( 2, 0 )) + (at( 3, 3 ) * rhs.at( 3, 0 ));
			m.at( 3, 1 ) = (at( 3, 0 ) * rhs.at( 0, 1 )) + (at( 3, 1 ) * rhs.at( 1, 1 )) + (at( 3, 2 ) * rhs.at( 2, 1 )) + (at( 3, 3 ) * rhs.at( 3, 1 ));
			m.at( 3, 2 ) = (at( 3, 0 ) * rhs.at( 0, 2 )) + (at( 3, 1 ) * rhs.at( 1, 2 )) + (at( 3, 2 ) * rhs.at( 2, 2 )) + (at( 3, 3 ) * rhs.at( 3, 2 ));
			m.at( 3, 3 ) = (at( 3, 0 ) * rhs.at( 0, 3 )) + (at( 3, 1 ) * rhs.at( 1, 3 )) + (at( 3, 2 ) * rhs.at( 2, 3 )) + (at( 3, 3 ) * rhs.at( 3, 3 ));
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
