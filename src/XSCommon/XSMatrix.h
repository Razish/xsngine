#pragma once

#include <string>

#include "XSCommon/XSVector.h"

namespace XS {

	// typical C layout for multidimensional array is row-major, i.e. you read it row by row
	// we will store matrices in row-major and offer transposition for libraries that require it

	struct matrix3 {

		real32_t	data[9];

		// string representation of the matrix3 object
		std::string tostring(
			void
		) const XS_WARN_UNUSED_RESULT;

		// access (r/w) an element
		real32_t &at(
			size_t row,
			size_t column
		) XS_WARN_UNUSED_RESULT;

		// access (r/o) an element
		real32_t at(
			size_t row,
			size_t column
		) const XS_WARN_UNUSED_RESULT;

		// clear the matrix
		void clear(
			void
		);

		// initialise with an identity matrix
		void identity(
			void
		);

		// transpose between row-major and column-major
		void transpose(
			void
		);

		// ???
		matrix3 invert(
			void
		) const XS_WARN_UNUSED_RESULT;

		// ???
		void invert(
			void
		);

		// matrix multiplication
		matrix3 operator*(
			const matrix3 &rhs
		) const;

		// matrix multiplication
		matrix3& operator*=(
			const matrix3 &rhs
		);

	};

	struct matrix4 {

		real32_t	data[16];

		// string representation of the matrix4 object
		std::string tostring(
			void
		) const XS_WARN_UNUSED_RESULT;

		// access (r/w) an element
		real32_t &at(
			size_t row,
			size_t column
		);

		// access (r/o) an element
		real32_t at(
			size_t row,
			size_t column
		) const;

		// clear the matrix
		void clear(
			void
		);

		// initialise with an identity matrix
		void identity(
			void
		);

		// transpose between row-major and column-major
		void transpose(
			void
		);

		// ???
		matrix4 invertAffine(
			void
		);

		// ???
		real32_t getCofactor(
			real32_t m0,
			real32_t m1,
			real32_t m2,
			real32_t m3,
			real32_t m4,
			real32_t m5,
			real32_t m6,
			real32_t m7,
			real32_t m8
		) const XS_WARN_UNUSED_RESULT;

		// ???
		// will not operate in place upon failure, but return an identity matrix
		// therefore you should not ignore this return value.
		matrix4 invertGeneral(
			void
		) const XS_WARN_UNUSED_RESULT;

		// ???
		void invertGeneral(
			void
		);

		matrix4 invert(
			void
		) const XS_WARN_UNUSED_RESULT;

		matrix4 inverse(
			void
		) const XS_WARN_UNUSED_RESULT;

		static matrix4 lookAt(
			const vector3 &eye,
			const vector3 &center,
			const vector3 &up
		) XS_WARN_UNUSED_RESULT;

		// matrix multiplication
		matrix4 operator*(
			const matrix4 &rhs
		) const;

		// matrix multiplication
		matrix4& operator*=(
			const matrix4 &rhs
		);

	};

	matrix4 ortho(
		real32_t left,
		real32_t right,
		real32_t top,
		real32_t bottom,
		real32_t zNear,
		real32_t zFar
	) XS_WARN_UNUSED_RESULT;

	matrix4 perspectiveFov(
		real32_t fovY,
		real32_t aspectRatio,
		real32_t zNear,
		real32_t zFar
	) XS_WARN_UNUSED_RESULT;

} // namespace XS
