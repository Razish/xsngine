#pragma once

namespace XS {

	struct matrix4 {
		vector4 data[4];

		// xtors
		matrix4() { identity(); }
		std::string tostring( void ) const;

		// access operators
		inline vector4& operator[]( const int idx ) {
			return data[idx];
		}
		inline const vector4* operator[]( const int idx ) const {
			return &data[idx];
		}

		// clear
		inline void clear( void ) {
			memset( data, 0, sizeof(data) );
		}

		inline void identity( void ) {
			data[0][0] = 1.0f; data[1][0] = 0.0f; data[2][0] = 0.0f; data[3][0] = 0.0f;
			data[0][1] = 0.0f; data[1][1] = 1.0f; data[2][1] = 0.0f; data[3][1] = 0.0f;
			data[0][2] = 0.0f; data[1][2] = 0.0f; data[2][2] = 1.0f; data[3][2] = 0.0f;
			data[0][3] = 0.0f; data[1][3] = 0.0f; data[2][3] = 0.0f; data[3][3] = 1.0f;
		}

		//TODO: .inverse(), need .determinant()

		// matrix multiplication
		inline matrix4 operator*( const matrix4 &rhs ) const {
			return *this * rhs;
		}

		inline matrix4& operator*=( const matrix4 &rhs ) {
			data[0][0] = data[0][0]*rhs[0]->x + data[0][1]*rhs[1]->x + data[0][2]*rhs[2]->x;
			data[0][1] = data[0][0]*rhs[0]->y + data[0][1]*rhs[1]->y + data[0][2]*rhs[2]->y;
			data[0][2] = data[0][0]*rhs[0]->z + data[0][1]*rhs[1]->z + data[0][2]*rhs[2]->z;
			data[1][0] = data[1][0]*rhs[0]->x + data[1][1]*rhs[1]->x + data[1][2]*rhs[2]->x;
			data[1][1] = data[1][0]*rhs[0]->y + data[1][1]*rhs[1]->y + data[1][2]*rhs[2]->y;
			data[1][2] = data[1][0]*rhs[0]->z + data[1][1]*rhs[1]->z + data[1][2]*rhs[2]->z;
			data[2][0] = data[2][0]*rhs[0]->x + data[2][1]*rhs[1]->x + data[2][2]*rhs[2]->x;
			data[2][1] = data[2][0]*rhs[0]->y + data[2][1]*rhs[1]->y + data[2][2]*rhs[2]->y;
			data[2][2] = data[2][0]*rhs[0]->z + data[2][1]*rhs[1]->z + data[2][2]*rhs[2]->z;
			return *this;
		}
	};

} // namespace XS
