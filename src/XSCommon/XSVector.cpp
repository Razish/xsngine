#include "XSCommon/XSCommon.h"
#include "XSCommon/XSVector.h"
#include "XSCommon/XSString.h"

//TODO: unit test precision at different ranges (and double conversion/promotion?)
bool flcmp( const real32_t &f1, const real32_t &f2, const real32_t epsilon ) {
	const real32_t delta = std::abs( f2 - f1 );

	return delta < epsilon;
}

bool dblcmp( const real64_t &f1, const real64_t &f2, const real64_t epsilon ) {
	const real64_t delta = std::abs( f2 - f1 );

	return delta < epsilon;
}

template<typename T>
T ma( const T &a, real32_t scale, const T &b ) {
	return a + b*scale;
}

template<typename T>
T lerp( const T &a, real32_t scale, const T &b ) {
	return a + (b-a)*scale;
}

std::string vector2::tostring( void ) const {
	return String::Format( "%.2f, %.2f", data[0], data[1] );
}

const real32_t& vector2::operator[]( const size_t idx ) const {
	return data[idx];
}

real32_t& vector2::operator[]( const size_t idx ) {
	return data[idx];
}

void vector2::clear( void ) {
	data[0] = 0.0f;
	data[1] = 0.0f;
}

// static
bool vector2::compare( const vector2 &lhs, const vector2 &rhs ) {
	return (flcmp( lhs[0], rhs[0] ) && flcmp( lhs[1], rhs[1] ));
}

bool vector2::compare( const vector2 &rhs ) const {
	return (flcmp( data[0], rhs[0] ) && flcmp( data[1], rhs[1] ));
}

bool vector2::operator==( const vector2 &rhs ) {
	return compare( rhs );
}

vector2 vector2::operator+( const vector2 &rhs ) const {
	return vector2{ data[0] + rhs[0], data[1] + rhs[1] };
}

vector2& vector2::operator+=( const vector2& rhs ) {
	data[0] += rhs[0];
	data[1] += rhs[1];
	return *this;
}

vector2 vector2::operator-( const vector2 &rhs ) const {
	return vector2{ data[0] - rhs[0], data[1] - rhs[1] };
}

vector2& vector2::operator-=( const vector2& rhs ) {
	data[0] -= rhs[0];
	data[1] -= rhs[1];
	return *this;
}

void vector2::increment( void ) {
	data[0] += 1.0f;
	data[1] += 1.0f;
}

void vector2::decrement( void ) {
	data[0] -= 1.0f;
	data[1] -= 1.0f;
}

void vector2::inverse( void ) {
	data[0] = -data[0];
	data[1] = -data[1];
}

// static
vector2 vector2::inverse( const vector2 &in ) {
	return vector2{ -in[0], -in[1] };
}

vector2 vector2::operator*( const real32_t scalar ) const {
	return vector2{ data[0] * scalar, data[1] * scalar };
}

vector2& vector2::operator*=( const real32_t scalar ) {
	data[0] *= scalar;
	data[1] *= scalar;
	return *this;
}

vector2 vector2::operator*( const vector2 &rhs ) const {
	return vector2{ data[0] * rhs[0], data[1] * rhs[1] };
}

vector2& vector2::operator*=( const vector2 &rhs ) {
	data[0] *= rhs[0];
	data[1] *= rhs[1];
	return *this;
}

vector2 vector2::operator/( const real32_t divisor ) const {
	return vector2{ data[0] / divisor, data[1] / divisor };
}

vector2& vector2::operator/=( const real32_t divisor ) {
	data[0] /= divisor;
	data[1] /= divisor;
	return *this;
}

vector2 vector2::operator/( const vector2 &rhs ) const {
	return vector2{ data[0] / rhs[0], data[1] / rhs[1] };
}

vector2& vector2::operator/=( const vector2 &rhs ) {
	data[0] /= rhs[0];
	data[1] /= rhs[1];
	return *this;
}

// static
bool vector2::PointWithinBounds( const vector2 &mins, const vector2 &point, const vector2 &size ) {
	if ( point[0] < mins[0] ) {
		return false;
	}
	else if ( point[1] < mins[1] ) {
		return false;
	}
	else if ( point[0] > mins[0] + size[0] ) {
		return false;
	}
	else if ( point[1] > mins[1] + size[1] ) {
		return false;
	}
	return true;
}

std::string pvector2::tostring( void ) const {
	SDL_assert( !"TODO: pvector2::tostring" );
	return "";
}

const real64_t& pvector2::operator[]( const size_t idx ) const {
	return data[idx];
}

real64_t& pvector2::operator[]( const size_t idx ) {
	return data[idx];
}

void pvector2::clear( void ) {
	data[0] = 0.0;
	data[1] = 0.0;
}

// static
bool pvector2::compare( const pvector2 &lhs, const pvector2 &rhs ) {
	return (dblcmp( lhs[0], rhs[0] ) && dblcmp( lhs[1], rhs[1] ));
}

bool pvector2::compare( const pvector2 &rhs ) const {
	return (dblcmp( data[0], rhs[0] ) && dblcmp( data[1], rhs[1] ));
}

bool pvector2::operator==( const pvector2 &rhs ) {
	return compare( rhs );
}

pvector2 pvector2::operator+( const pvector2 &rhs ) const {
	return pvector2{ data[0] + rhs[0], data[1] + rhs[1] };
}

pvector2& pvector2::operator+=( const pvector2& rhs ) {
	data[0] += rhs[0];
	data[1] += rhs[1];
	return *this;
}

pvector2 pvector2::operator-( const pvector2 &rhs ) const {
	return pvector2{ data[0] - rhs[0], data[1] - rhs[1] };
}

pvector2& pvector2::operator-=( const pvector2& rhs ) {
	data[0] -= rhs[0];
	data[1] -= rhs[1];
	return *this;
}

void pvector2::increment( void ) {
	data[0] += 1.0;
	data[1] += 1.0;
}

void pvector2::decrement( void ) {
	data[0] -= 1.0;
	data[1] -= 1.0;
}

void pvector2::inverse( void ) {
	data[0] = -data[0];
	data[1] = -data[1];
}

// static
pvector2 pvector2::inverse( const pvector2 &in ) {
	pvector2 result = { -in[0], -in[1] };
	return result;
}

pvector2 pvector2::operator*( const real64_t scalar ) const {
	pvector2 result = { data[0] * scalar, data[1] * scalar };
	return result;
}

pvector2& pvector2::operator*=( const real64_t scalar ) {
	data[0] *= scalar;
	data[1] *= scalar;
	return *this;
}

pvector2 pvector2::operator*( const pvector2 &rhs ) const {
	return pvector2{ data[0] * rhs[0], data[1] * rhs[1] };
}

pvector2& pvector2::operator*=( const pvector2 &rhs ) {
	data[0] *= rhs[0];
	data[1] *= rhs[1];
	return *this;
}

pvector2 pvector2::operator/( const real64_t divisor ) const {
	return pvector2{ data[0] / divisor, data[1] / divisor };
}

pvector2& pvector2::operator/=( const real64_t divisor ) {
	data[0] /= divisor;
	data[1] /= divisor;
	return *this;
}

pvector2 pvector2::operator/( const pvector2 &rhs ) const {
	return pvector2{ data[0] / rhs[0], data[1] / rhs[1] };
}

pvector2& pvector2::operator/=( const pvector2 &rhs ) {
	data[0] /= rhs[0];
	data[1] /= rhs[1];
	return *this;
}

// static
bool pvector2::PointWithinBounds( const pvector2 &mins, const pvector2 &point, const pvector2 &size ) {
	if ( point[0] < mins[0] ) {
		return false;
	}
	else if ( point[1] < mins[1] ) {
		return false;
	}
	else if ( point[0] > mins[0] + size[0] ) {
		return false;
	}
	else if ( point[1] > mins[1] + size[1] ) {
		return false;
	}
	return true;
}

std::string vector3::tostring( void ) const {
	return String::Format( "%.2f, %.2f, %.2f", data[0], data[1], data[2] );
}

const real32_t& vector3::operator[]( const size_t idx ) const {
	return data[idx];
}

real32_t& vector3::operator[]( const size_t idx ) {
	return data[idx];
}

void vector3::clear( void ) {
	data[0] = 0.0f;
	data[1] = 0.0f;
	data[2] = 0.0f;
}

// static
bool vector3::compare( const vector3 &lhs, const vector3 &rhs ) {
	return (flcmp( lhs[0], rhs[0] ) && flcmp( lhs[1], rhs[1] ) && flcmp( lhs[2], rhs[2] ));
}

bool vector3::compare( const vector3 &rhs ) const {
	return (flcmp( data[0], rhs[0] ) && flcmp( data[1], rhs[1] ) && flcmp( data[2], rhs[2] ));
}

bool vector3::operator==( const vector3 &rhs ) {
	return compare( rhs );
}

vector3 vector3::operator+( const vector3 &rhs ) const {
	return vector3{ data[0] + rhs[0], data[1] + rhs[1], data[2] + rhs[2] };
}

vector3& vector3::operator+=( const vector3& rhs ) {
	data[0] += rhs[0];
	data[1] += rhs[1];
	data[2] += rhs[2];
	return *this;
}

vector3 vector3::operator-( const vector3 &rhs ) const {
	return vector3{ data[0] - rhs[0], data[1] - rhs[1], data[2] - rhs[2] };
}

vector3& vector3::operator-=( const vector3& rhs ) {
	data[0] -= rhs[0];
	data[1] -= rhs[1];
	data[2] -= rhs[2];
	return *this;
}

void vector3::increment( void ) {
	data[0] += 1.0f;
	data[1] += 1.0f;
	data[2] += 1.0f;
}

void vector3::decrement( void ) {
	data[0] -= 1.0f;
	data[1] -= 1.0f;
	data[2] -= 1.0f;
}

void vector3::inverse( void ) {
	data[0] = -data[0];
	data[1] = -data[1];
	data[2] = -data[2];
}

// static
vector3 vector3::inverse( const vector3 &in ) {
	return vector3{ -in[0], -in[1], -in[2] };
}

vector3 vector3::operator*( const real32_t scalar ) const {
	return vector3{ data[0] * scalar, data[1] * scalar, data[1] * scalar };
}

vector3& vector3::operator*=( const real32_t scalar ) {
	data[0] *= scalar;
	data[1] *= scalar;
	data[2] *= scalar;
	return *this;
}

vector3 vector3::operator*( const vector3 &rhs ) const {
	return vector3{ data[0] * rhs[0], data[1] * rhs[1], data[2] * rhs[2] };
}

vector3& vector3::operator*=( const vector3 &rhs ) {
	data[0] *= rhs[0];
	data[1] *= rhs[1];
	data[2] *= rhs[2];
	return *this;
}

vector3 vector3::operator/( const real32_t divisor ) const {
	return vector3{ data[0] / divisor, data[1] / divisor, data[2] / divisor };
}

vector3& vector3::operator/=( const real32_t divisor ) {
	data[0] /= divisor;
	data[1] /= divisor;
	data[2] /= divisor;
	return *this;
}

vector3 vector3::operator/( const vector3 &rhs ) const {
	return vector3{ data[0] / rhs[0], data[1] / rhs[1], data[2] / rhs[2] };
}

vector3& vector3::operator/=( const vector3 &rhs ) {
	data[0] /= rhs[0];
	data[1] /= rhs[1];
	data[2] /= rhs[2];
	return *this;
}

real32_t vector3::length( void ) const {
	return sqrtf( data[0]*data[0] + data[1]*data[1] + data[2]*data[2] );
}

// static
real32_t vector3::length( const vector3 &vec ) {
	return sqrtf( vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2] );
}

real32_t vector3::lengthSquared( void ) const {
	return data[0]*data[0] + data[1]*data[1] + data[2]*data[2];
}

// static
real32_t vector3::lengthSquared( const vector3 &vec ) {
	return vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2];
}

// static
real32_t vector3::distance( const vector3 &a, const vector3 &b ) {
	return length( a - b );
}

// static
real32_t vector3::distanceSquared( const vector3 &a, const vector3 &b ) {
	return lengthSquared( a - b );
}

void vector3::normaliseFast( void ) {
	*this *= 1.0f / length();
}

real32_t vector3::normalise( void ) {
	real32_t length = this->length();

	//TODO: validate this form improves gcc's ability to optimise
	if ( length ) {
		real32_t iLength = 1.0f / length;
		*this *= iLength;
	}

	return length;
}

// static
vector3 vector3::normalise( const vector3 &vec ) {
	vector3 result = vec;

	real32_t length = result.length();
	if ( length ) {
		// written this way to aid compiler optimsation, it can substitute for intrinsics easier like this
		real32_t recip = 1.0f / length;
		result *= recip;
	}

	return result;
}

// static
real32_t vector3::dot( const vector3 &a, const vector3 &b ) {
	return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

real32_t vector3::dot( const vector3 &vec ) const {
	return data[0]*vec[0] + data[1]*vec[1] + data[2]*vec[2];
}

// static
vector3 vector3::cross( const vector3 &a, const vector3 &b ) {
	return vector3{
		(a[1]*b[2]) - (a[2]*b[1]),
		(a[2]*b[0]) - (a[0]*b[2]),
		(a[0]*b[1]) - (a[1]*b[0])
	};
}

vector3 vector3::cross( const vector3 &v ) const {
	return cross( *this, v );
}

// static
bool vector3::PointWithinBounds( const vector3 &mins, const vector3 &point, const vector3 &size ) {
	if ( point[0] < mins[0] ) {
		return false;
	}
	else if ( point[1] < mins[1] ) {
		return false;
	}
	else if ( point[2] < mins[2] ) {
		return false;
	}
	else if ( point[0] > mins[0] + size[0] ) {
		return false;
	}
	else if ( point[1] > mins[1] + size[1] ) {
		return false;
	}
	else if ( point[2] > mins[2] + size[2] ) {
		return false;
	}
	return true;
}

std::string pvector3::tostring( void ) const {
	return String::Format( "%.2f, %.2f, %.2f", data[0], data[1], data[2] );
}

const real64_t& pvector3::operator[]( const size_t idx ) const {
	return data[idx];
}

real64_t& pvector3::operator[]( const size_t idx ) {
	return data[idx];
}

void pvector3::clear( void ) {
	data[0] = 0.0;
	data[1] = 0.0;
	data[2] = 0.0;
}

// static
bool pvector3::compare( const pvector3 &lhs, const pvector3 &rhs ) {
	return (dblcmp( lhs[0], rhs[0] ) && flcmp( lhs[1], rhs[1] ) && flcmp( lhs[2], rhs[2] ));
}

bool pvector3::compare( const pvector3 &rhs ) const {
	return (dblcmp( data[0], rhs[0] ) && flcmp( data[1], rhs[1] ) && flcmp( data[2], rhs[2] ));
}

bool pvector3::operator==( const pvector3 &rhs ) {
	return compare( rhs );
}

pvector3 pvector3::operator+( const pvector3 &rhs ) const {
	return pvector3{ data[0] + rhs[0], data[1] + rhs[1], data[2] + rhs[2] };
}

pvector3& pvector3::operator+=( const pvector3 &rhs ) {
	data[0] += rhs[0];
	data[1] += rhs[1];
	data[2] += rhs[2];
	return *this;
}

pvector3 pvector3::operator-( const pvector3 &rhs ) const {
	return pvector3{ data[0] - rhs[0], data[1] - rhs[1], data[2] - rhs[2] };
}

pvector3& pvector3::operator-=( const pvector3& rhs ) {
	data[0] -= rhs[0];
	data[1] -= rhs[1];
	data[2] -= rhs[2];
	return *this;
}

void pvector3::increment( void ) {
	data[0] += 1.0;
	data[1] += 1.0;
	data[2] += 1.0;
}

void pvector3::decrement( void ) {
	data[0] -= 1.0;
	data[1] -= 1.0;
	data[2] -= 1.0;
}

void pvector3::inverse( void ) {
	data[0] = -data[0];
	data[1] = -data[1];
	data[2] = -data[2];
}

// static
pvector3 pvector3::inverse( const pvector3 &in ) {
	return pvector3{ -in[0], -in[1], -in[2] };
}

pvector3 pvector3::operator*( const real64_t scalar ) const {
	return pvector3{ data[0] * scalar, data[1] * scalar, data[2] * scalar };
}

pvector3& pvector3::operator*=( const real64_t scalar ) {
	data[0] *= scalar;
	data[1] *= scalar;
	data[2] *= scalar;
	return *this;
}

pvector3 pvector3::operator*( const pvector3 &rhs ) const {
	return pvector3{ data[0] * rhs[0], data[1] * rhs[1], data[2] * rhs[2] };
}

pvector3& pvector3::operator*=( const pvector3 &rhs ) {
	data[0] *= rhs[0];
	data[1] *= rhs[1];
	data[2] *= rhs[2];
	return *this;
}

pvector3 pvector3::operator/( const real64_t divisor ) const {
	return pvector3{ data[0] / divisor, data[1] / divisor, data[2] / divisor };
}

pvector3& pvector3::operator/=( const real64_t divisor ) {
	data[0] /= divisor;
	data[1] /= divisor;
	data[2] /= divisor;
	return *this;
}

pvector3 pvector3::operator/( const pvector3 &rhs ) const {
	return pvector3{ data[0] / rhs[0], data[1] / rhs[1], data[2] / rhs[2] };
}

pvector3& pvector3::operator/=( const pvector3 &rhs ) {
	data[0] /= rhs[0];
	data[1] /= rhs[1];
	data[2] /= rhs[2];
	return *this;
}

real64_t pvector3::length( void ) const {
	return sqrt( data[0]*data[0] + data[1]*data[1] + data[2]*data[2] );
}

// static
real64_t pvector3::length( const pvector3 &vec ) {
	return vec.length();
}

real64_t pvector3::lengthSquared( void ) const {
	return data[0]*data[0] + data[1]*data[1] + data[2]*data[2];
}

// static
real64_t pvector3::lengthSquared( const pvector3 &vec ) {
	return vec.lengthSquared();
}

// static
real64_t pvector3::distance( const pvector3 &a, const pvector3 &b ) {
	return length( a - b );
}

// static
real64_t pvector3::distanceSquared( const pvector3 &a, const pvector3 &b ) {
	return lengthSquared( a - b );
}

void pvector3::normaliseFast( void ) {
	*this *= 1.0 / length();
}

real64_t pvector3::normalise( void ) {
	real64_t length = this->length();

	if ( length ) {
		real64_t iLength = 1.0 / length;
		*this *= iLength;
	}

	return length;
}

// static
pvector3 pvector3::normalise( const pvector3 &vec ) {
	pvector3 result = vec;

	real64_t length = result.length();
	if ( length ) {
		real64_t iLength = 1.0f / length;
		result *= iLength;
	}

	return result;
}

real64_t pvector3::dot( const pvector3 &vec ) const {
	return data[0]*vec[0] + data[1]*vec[1] + data[1]*vec[2];
}

// static
pvector3 pvector3::cross( const pvector3 &a, const pvector3 &b ) {
	return pvector3{
		(a[1]*b[2]) - (a[2]*b[1]),
		(a[2]*b[0]) - (a[0]*b[2]),
		(a[0]*b[1]) - (a[1]*b[0])
	};
}

pvector3 pvector3::cross( const pvector3 &v ) const {
	return cross( *this, v );
}

// static
bool pvector3::PointWithinBounds( const pvector3 &mins, const pvector3 &point, const pvector3 &size ) {
	if ( point[0] < mins[0] ) {
		return false;
	}
	else if ( point[1] < mins[1] ) {
		return false;
	}
	else if ( point[2] < mins[2] ) {
		return false;
	}
	else if ( point[0] > mins[0] + size[0] ) {
		return false;
	}
	else if ( point[1] > mins[1] + size[1] ) {
		return false;
	}
	else if ( point[2] > mins[2] + size[2] ) {
		return false;
	}
	return true;
}

std::string vector4::tostring( void ) const {
	return String::Format( "%.2f, %.2f, %.2f, %.2f", data[0], data[1], data[2], data[3] );
}

const real32_t& vector4::operator[]( const size_t idx ) const {
	return data[idx];
}

real32_t& vector4::operator[]( const size_t idx ) {
	return data[idx];
}

void vector4::clear( void ) {
	data[0] = 0.0f;
	data[1] = 0.0f;
	data[2] = 0.0f;
	data[3] = 0.0f;
}

// static
bool vector4::compare( const vector4 &lhs, const vector4 &rhs ) {
	return (flcmp( lhs[0], rhs[0] ) && flcmp( lhs[1], rhs[1] ) && flcmp( lhs[2], rhs[2] ) && flcmp( lhs[3], rhs[3] ));
}

bool vector4::compare( const vector4 &rhs ) const {
	return (flcmp( data[0], rhs[0] ) && flcmp( data[1], rhs[1] ) && flcmp( data[2], rhs[2] ) && flcmp( data[3], rhs[3] ));
}

bool vector4::operator==( const vector4 &rhs ) const {
	return compare( rhs );
}

vector4 vector4::operator+( const vector4 &rhs ) const {
	return vector4{ data[0] + rhs[0], data[1] + rhs[1], data[2] + rhs[2], data[3] + rhs[3] };
}

vector4& vector4::operator+=( const vector4& rhs ) {
	data[0] += rhs[0];
	data[1] += rhs[1];
	data[2] += rhs[2];
	data[3] += rhs[3];
	return *this;
}

vector4 vector4::operator-( const vector4 &rhs ) const {
	return vector4{ data[0] - rhs[0], data[1] - rhs[1], data[2] - rhs[2], data[3] - rhs[3] };
}

vector4& vector4::operator-=( const vector4& rhs ) {
	data[0] -= rhs[0];
	data[1] -= rhs[1];
	data[2] -= rhs[2];
	data[3] -= rhs[3];
	return *this;
}

void vector4::increment( void ) {
	data[0] += 1.0f;
	data[1] += 1.0f;
	data[2] += 1.0f;
	data[3] += 1.0f;
}

void vector4::decrement( void ) {
	data[0] -= 1.0f;
	data[1] -= 1.0f;
	data[2] -= 1.0f;
	data[3] -= 1.0f;
}

void vector4::inverse( void ) {
	data[0] = -data[0];
	data[1] = -data[1];
	data[2] = -data[2];
	data[3] = -data[3];
}

// static
vector4 vector4::inverse( const vector4 &in ) {
	return vector4{ -in[0], -in[1], -in[2], -in[3] };
}

vector4 vector4::operator*( const real32_t scalar ) const {
	return vector4{ data[0] * scalar, data[1] * scalar, data[2] * scalar, data[3] * scalar };
}

vector4& vector4::operator*=( const real32_t scalar ) {
	data[0] *= scalar;
	data[1] *= scalar;
	data[2] *= scalar;
	data[3] *= scalar;
	return *this;
}

vector4 vector4::operator*( const vector4 &rhs ) const {
	return vector4{ data[0] * rhs[0], data[1] * rhs[1], data[2] * rhs[2], data[3] * rhs[3] };
}

vector4& vector4::operator*=( const vector4 &rhs ) {
	data[0] *= rhs[0];
	data[1] *= rhs[1];
	data[2] *= rhs[2];
	data[3] *= rhs[3];
	return *this;
}

vector4 vector4::operator/( const real32_t divisor ) const {
	return vector4{ data[0] / divisor, data[1] / divisor, data[2] / divisor, data[3] / divisor };
}

vector4& vector4::operator/=( const real32_t divisor ) {
	data[0] /= divisor;
	data[1] /= divisor;
	data[2] /= divisor;
	data[3] /= divisor;
	return *this;
}

vector4 vector4::operator/( const vector4 &rhs ) const {
	return vector4{ data[0] / rhs[0], data[1] / rhs[1], data[2] / rhs[2], data[3] / rhs[3] };
}

vector4& vector4::operator/=( const vector4 &rhs ) {
	data[0] /= rhs[0];
	data[1] /= rhs[1];
	data[2] /= rhs[2];
	data[3] /= rhs[3];
	return *this;
}
