#include "Common/Common.h"
#include "Common/Matrix.h"
#include "Common/String.h"

std::string matrix3::tostring( void ) const {
	return String::Format(
		"%.2f, %.2f, %.2f\n%.2f, %.2f, %.2f\n%.2f, %.2f, %.2f",
		data[0], data[1], data[2],
		data[3], data[4], data[5],
		data[6], data[7], data[8]
	);
}

real32_t &matrix3::at( size_t row, size_t column ) {
	return data[(row * 3) + column];
}
real32_t matrix3::at( size_t row, size_t column ) const {
	return data[(row * 3) + column];
}

void matrix3::clear( void ) {
	memset( data, 0, sizeof(data) );
}

void matrix3::identity( void ) {
	clear();
	at( 0, 0 ) = 1.0f;
	at( 1, 1 ) = 1.0f;
	at( 2, 2 ) = 1.0f;
}

void matrix3::transpose( void ) {
	matrix3 m = *this;

	for ( size_t x = 0u; x < 3u; x++ ) {
		for ( size_t y = 0u; y < 3u; y++ ) {
			m.at( y, x ) = at( x, y );
		}
	}

	*this = m;
}

matrix3 matrix3::invert( void ) const {
	const matrix3 tmp {
		data[4] * data[8] - data[5] * data[7],
		data[2] * data[7] - data[1] * data[8],
		data[1] * data[5] - data[2] * data[4],
		data[5] * data[6] - data[3] * data[8],
		data[0] * data[8] - data[2] * data[6],
		data[2] * data[3] - data[0] * data[5],
		data[3] * data[7] - data[4] * data[6],
		data[1] * data[6] - data[0] * data[7],
		data[0] * data[4] - data[1] * data[3]
	};

	const real32_t determinant = data[0] * tmp.data[0] + data[1] * tmp.data[3] + data[2] * tmp.data[6];
	if ( std::abs( determinant ) <= 0.00000f ) {
		return matrix3();
	}

	const real32_t invDeterminant = 1.0f / determinant;

	matrix3 m = *this;
	m.data[0] = invDeterminant * tmp.data[0];
	m.data[1] = invDeterminant * tmp.data[1];
	m.data[2] = invDeterminant * tmp.data[2];
	m.data[3] = invDeterminant * tmp.data[3];
	m.data[4] = invDeterminant * tmp.data[4];
	m.data[5] = invDeterminant * tmp.data[5];
	m.data[6] = invDeterminant * tmp.data[6];
	m.data[7] = invDeterminant * tmp.data[7];
	m.data[8] = invDeterminant * tmp.data[8];

	return m;
}

void matrix3::invert( void ) {
	const matrix3 tmp {
		data[4] * data[8] - data[5] * data[7],
		data[2] * data[7] - data[1] * data[8],
		data[1] * data[5] - data[2] * data[4],
		data[5] * data[6] - data[3] * data[8],
		data[0] * data[8] - data[2] * data[6],
		data[2] * data[3] - data[0] * data[5],
		data[3] * data[7] - data[4] * data[6],
		data[1] * data[6] - data[0] * data[7],
		data[0] * data[4] - data[1] * data[3]
	};

	const real32_t determinant = data[0] * tmp.data[0] + data[1] * tmp.data[3] + data[2] * tmp.data[6];
	if ( std::abs( determinant ) <= 0.00000f ) {
		identity();
		return;
	}

	const real32_t invDeterminant = 1.0f / determinant;
	data[0] = invDeterminant * tmp.data[0];
	data[1] = invDeterminant * tmp.data[1];
	data[2] = invDeterminant * tmp.data[2];
	data[3] = invDeterminant * tmp.data[3];
	data[4] = invDeterminant * tmp.data[4];
	data[5] = invDeterminant * tmp.data[5];
	data[6] = invDeterminant * tmp.data[6];
	data[7] = invDeterminant * tmp.data[7];
	data[8] = invDeterminant * tmp.data[8];
}

// matrix multiplication
matrix3 matrix3::operator*( const matrix3 &rhs ) const {
	matrix3 m;
	SDL_assert( !"TODO: matrix3::operator*" );
	return m;
}

matrix3& matrix3::operator*=( const matrix3 &rhs ) {
	*this = *this * rhs;
	return *this;
}

std::string matrix4::tostring( void ) const {
	return String::Format(
		"%.2f, %.2f, %.2f, %.2f\n%.2f, %.2f, %.2f, %.2f\n%.2f, %.2f, %.2f, %.2f",
		data[ 0], data[ 1], data[ 2], data[ 3],
		data[ 4], data[ 5], data[ 6], data[ 7],
		data[ 8], data[ 9], data[10], data[11],
		data[12], data[13], data[14], data[15]
	);
}

real32_t &matrix4::at( size_t row, size_t column ) {
	return data[(row * 4) + column];
}
real32_t matrix4::at( size_t row, size_t column ) const {
	return data[(row * 4) + column];
}

void matrix4::clear( void ) {
	memset( data, 0, sizeof(data) );
}

void matrix4::identity( void ) {
	clear();
	at( 0, 0 ) = 1.0f;
	at( 1, 1 ) = 1.0f;
	at( 2, 2 ) = 1.0f;
	at( 3, 3 ) = 1.0f;
}

void matrix4::transpose( void ) {
	matrix4 m = *this;

	for ( size_t x = 0u; x < 4u; x++ ) {
		for ( size_t y = 0u; y < 4u; y++ ) {
			m.at( y, x ) = at( x, y );
		}
	}

	*this = m;
}

matrix4 matrix4::invertAffine( void ) {
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

	const vector3 v {
		data[12],
		data[13],
		data[14],
	};

	data[12] = -(r.data[0] * v[0] + r.data[3] * v[1] + r.data[6] * v[2]);
	data[13] = -(r.data[1] * v[0] + r.data[4] * v[1] + r.data[7] * v[2]);
	data[14] = -(r.data[2] * v[0] + r.data[5] * v[1] + r.data[8] * v[2]);

	return *this;
}

real32_t matrix4::getCofactor( real32_t m0, real32_t m1, real32_t m2, real32_t m3, real32_t m4, real32_t m5,
	real32_t m6, real32_t m7, real32_t m8 ) const
{
	return m0 * (m4 * m8 - m5 * m7)
		- m1 * (m3 * m8 - m5 * m6)
		+ m2 * (m3 * m7 - m4 * m6);
}

matrix4 matrix4::invertGeneral( void ) const {
	const real32_t cofactor0 = getCofactor( data[5], data[6], data[7], data[9], data[10], data[11], data[13],
		data[14], data[15] );
	const real32_t cofactor1 = getCofactor( data[4], data[6], data[7], data[8], data[10], data[11], data[12],
		data[14], data[15] );
	const real32_t cofactor2 = getCofactor( data[4], data[5], data[7], data[8], data[ 9], data[11], data[12],
		data[13], data[15] );
	const real32_t cofactor3 = getCofactor( data[4], data[5], data[6], data[8], data[ 9], data[10], data[12],
		data[13], data[14] );

	const real32_t determinant = data[0] * cofactor0 - data[1] * cofactor1 + data[2] * cofactor2 - data[3] * cofactor3;
	if ( std::abs( determinant ) < 0.000001f ) {
		matrix4 m;
		m.identity();
		return m;
	}

	const real32_t invDeterminant = 1.0f / determinant;
	const real32_t cofactor4  = getCofactor( data[1], data[2], data[3], data[9], data[10], data[11], data[13],
		data[14], data[15] );
	const real32_t cofactor5  = getCofactor( data[0], data[2], data[3], data[8], data[10], data[11], data[12],
		data[14], data[15] );
	const real32_t cofactor6  = getCofactor( data[0], data[1], data[3], data[8], data[ 9], data[11], data[12], data[13], data[15] );
	const real32_t cofactor7  = getCofactor( data[0], data[1], data[2], data[8], data[ 9], data[10], data[12], data[13], data[14] );

	const real32_t cofactor8  = getCofactor( data[1], data[2], data[3], data[5], data[6], data[7], data[13], data[14], data[15] );
	const real32_t cofactor9  = getCofactor( data[0], data[2], data[3], data[4], data[6], data[7], data[12], data[14], data[15] );
	const real32_t cofactor10 = getCofactor( data[0], data[1], data[3], data[4], data[5], data[7], data[12], data[13], data[15] );
	const real32_t cofactor11 = getCofactor( data[0], data[1], data[2], data[4], data[5], data[6], data[12], data[13], data[14] );

	const real32_t cofactor12 = getCofactor( data[1], data[2], data[3], data[5], data[6], data[7], data[9], data[10], data[11] );
	const real32_t cofactor13 = getCofactor( data[0], data[2], data[3], data[4], data[6], data[7], data[8], data[10], data[11] );
	const real32_t cofactor14 = getCofactor( data[0], data[1], data[3], data[4], data[5], data[7], data[8], data[ 9], data[11] );
	const real32_t cofactor15 = getCofactor( data[0], data[1], data[2], data[4], data[5], data[6], data[8], data[ 9], data[10] );

	return matrix4 {
		 invDeterminant * cofactor0 ,  invDeterminant * cofactor4 ,
		 invDeterminant * cofactor8 ,  invDeterminant * cofactor12,

		-invDeterminant * cofactor1 ,  invDeterminant * cofactor5 ,
		-invDeterminant * cofactor9 ,  invDeterminant * cofactor13,

		 invDeterminant * cofactor2 , -invDeterminant * cofactor6 ,
		 invDeterminant * cofactor10, -invDeterminant * cofactor14,

		-invDeterminant * cofactor3 ,  invDeterminant * cofactor7 ,
		-invDeterminant * cofactor11,  invDeterminant * cofactor15,
	};
}

void matrix4::invertGeneral( void ) {
	const real32_t cofactor0 = getCofactor( data[5], data[6], data[7], data[9], data[10], data[11], data[13],
		data[14], data[15] );
	const real32_t cofactor1 = getCofactor( data[4], data[6], data[7], data[8], data[10], data[11], data[12],
		data[14], data[15] );
	const real32_t cofactor2 = getCofactor( data[4], data[5], data[7], data[8], data[ 9], data[11], data[12],
		data[13], data[15] );
	const real32_t cofactor3 = getCofactor( data[4], data[5], data[6], data[8], data[ 9], data[10], data[12],
		data[13], data[14] );

	const real32_t determinant = data[0] * cofactor0 - data[1] * cofactor1 + data[2] * cofactor2 - data[3] * cofactor3;
	if ( std::abs( determinant ) < 0.000001f ) {
		identity();
		return;
	}

	const real32_t invDeterminant = 1.0f / determinant;
	const real32_t cofactor4  = getCofactor( data[1], data[2], data[3], data[9], data[10], data[11], data[13],
		data[14], data[15] );
	const real32_t cofactor5  = getCofactor( data[0], data[2], data[3], data[8], data[10], data[11], data[12],
		data[14], data[15] );
	const real32_t cofactor6  = getCofactor( data[0], data[1], data[3], data[8], data[ 9], data[11], data[12], data[13], data[15] );
	const real32_t cofactor7  = getCofactor( data[0], data[1], data[2], data[8], data[ 9], data[10], data[12], data[13], data[14] );

	const real32_t cofactor8  = getCofactor( data[1], data[2], data[3], data[5], data[6], data[7], data[13], data[14], data[15] );
	const real32_t cofactor9  = getCofactor( data[0], data[2], data[3], data[4], data[6], data[7], data[12], data[14], data[15] );
	const real32_t cofactor10 = getCofactor( data[0], data[1], data[3], data[4], data[5], data[7], data[12], data[13], data[15] );
	const real32_t cofactor11 = getCofactor( data[0], data[1], data[2], data[4], data[5], data[6], data[12], data[13], data[14] );

	const real32_t cofactor12 = getCofactor( data[1], data[2], data[3], data[5], data[6], data[7], data[9], data[10], data[11] );
	const real32_t cofactor13 = getCofactor( data[0], data[2], data[3], data[4], data[6], data[7], data[8], data[10], data[11] );
	const real32_t cofactor14 = getCofactor( data[0], data[1], data[3], data[4], data[5], data[7], data[8], data[ 9], data[11] );
	const real32_t cofactor15 = getCofactor( data[0], data[1], data[2], data[4], data[5], data[6], data[8], data[ 9], data[10] );

	data[ 0] =  invDeterminant * cofactor0;
	data[ 1] =  invDeterminant * cofactor4;
	data[ 2] =  invDeterminant * cofactor8;
	data[ 3] =  invDeterminant * cofactor12;
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
}

matrix4 matrix4::invert( void ) const {
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

matrix4 matrix4::inverse( void ) const {
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

// static
matrix4 matrix4::lookAt( const vector3 &eye, const vector3 &center, const vector3 &up ) {
	const vector3 f = vector3::normalise( center - eye );
	const vector3 s = vector3::normalise( vector3::cross( f, up ) );
	const vector3 u = vector3::cross( s, f );

	matrix4 m;

	m.at( 0, 0 ) =  s[0];
	m.at( 1, 0 ) =  s[1];
	m.at( 2, 0 ) =  s[2];

	m.at( 0, 1 ) =  u[0];
	m.at( 1, 1 ) =  u[1];
	m.at( 2, 1 ) =  u[2];

	m.at( 0, 2 ) = -f[0];
	m.at( 1, 2 ) = -f[1];
	m.at( 2, 2 ) = -f[2];

	m.at( 3, 0 ) = -vector3::dot( s, eye );
	m.at( 3, 1 ) = -vector3::dot( u, eye );
	m.at( 3, 2 ) =  vector3::dot( f, eye );

	return m;
}

matrix4 matrix4::operator*( const matrix4 &rhs ) const {
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

matrix4& matrix4::operator*=( const matrix4 &rhs ) {
	*this = *this * rhs;
	return *this;
}

matrix4 ortho( real32_t left, real32_t right, real32_t top, real32_t bottom, real32_t zNear, real32_t zFar ) {
	matrix4 m;
	m.identity();

	m.at( 0, 0 ) = 2.0f / (right - left);
	m.at( 1, 1 ) = 2.0f / (top - bottom);
	m.at( 2, 2 ) = 2.0f / (zNear - zFar);
	m.at( 3, 0 ) = (right + left) / (left - right);
	m.at( 3, 1 ) = (top + bottom) / (bottom - top);
	m.at( 3, 2 ) = (zNear - zFar) / (zFar - zNear);

	return m;
}

matrix4 perspectiveFov( real32_t fovY, real32_t aspectRatio, real32_t zNear, real32_t zFar ) {
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
