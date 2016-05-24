#pragma once

#include <string>

namespace XS {

	bool flcmp(
		const real32_t &f1,
		const real32_t &f2,
		const real32_t epsilon = 0.00001f
	) XS_WARN_UNUSED_RESULT;

	bool dblcmp(
		const real64_t &f1,
		const real64_t &f2,
		const real64_t epsilon = 0.000000000000001
	) XS_WARN_UNUSED_RESULT;

	// ???
	template<typename T>
	T XS_WARN_UNUSED_RESULT ma(
		const T &a,
		real32_t scale,
		const T &b
	);

	// linear interpolation
	template<typename T>
	T XS_WARN_UNUSED_RESULT lerp(
		const T &a,
		real32_t scale,
		const T &b
	);

	struct vector2 {

		real32_t data[2];

		// string representation of the vector
		std::string tostring(
			void
		) const XS_WARN_UNUSED_RESULT;

		// access operator (r/o)
		const real32_t& operator[](
			const size_t idx
		) const;

		// access operator (r/w)
		real32_t& operator[](
			const size_t idx
		);

		// clear
		void clear(
			void
		);

		// compare
		static bool compare(
			const vector2 &lhs,
			const vector2 &rhs
		) XS_WARN_UNUSED_RESULT;

		// compare
		bool compare(
			const vector2 &rhs
		) const XS_WARN_UNUSED_RESULT;

		// compare
		bool operator==(
			const vector2 &rhs
		);

		// addition
		vector2 operator+(
			const vector2 &rhs
		) const;

		// addition
		vector2& operator+=(
			const vector2& rhs
		);

		// subtraction
		vector2 operator-(
			const vector2 &rhs
		) const;

		// subtraction
		vector2& operator-=(
			const vector2& rhs
		);

		// increment
		void increment(
			void
		);

		// decrement
		void decrement(
			void
		);

		// inverse
		void inverse(
			void
		);

		// inverse
		static vector2 inverse(
			const vector2 &in
		) XS_WARN_UNUSED_RESULT;

		// scalar multiplication
		vector2 operator*(
			const real32_t scalar
		) const XS_WARN_UNUSED_RESULT;

		// scalar multiplication
		vector2& operator*=(
			const real32_t scalar
		);

		// vector multiplication
		vector2 operator*(
			const vector2 &rhs
		) const;

		// vector multiplication
		vector2& operator*=(
			const vector2 &rhs
		);

		// scalar division
		vector2 operator/(
			const real32_t divisor
		) const;

		// scalar division
		vector2& operator/=(
			const real32_t divisor
		);

		// vector division
		vector2 operator/(
			const vector2 &rhs
		) const;

		// vector division
		vector2& operator/=(
			const vector2 &rhs
		);

		// point within bounds
		static bool PointWithinBounds(
			const vector2 &mins,
			const vector2 &point,
			const vector2 &size
		);

	};

	struct pvector2 {

		real64_t data[2];

		// string representation of the vector
		std::string tostring(
			void
		) const XS_WARN_UNUSED_RESULT;

		// access operator (r/o)
		const real64_t& operator[](
			const size_t idx
		) const;

		// access operator (r/w)
		real64_t& operator[](
			const size_t idx
		);

		// clear
		void clear(
			void
		);

		// compare
		static bool compare(
			const pvector2 &lhs,
			const pvector2 &rhs
		) XS_WARN_UNUSED_RESULT;

		// compare
		bool compare(
			const pvector2 &rhs
		) const XS_WARN_UNUSED_RESULT;

		// compare
		bool operator==(
			const pvector2 &rhs
		);

		// addition
		pvector2 operator+(
			const pvector2 &rhs
		) const;

		// addition
		pvector2& operator+=(
			const pvector2& rhs
		);

		// subtraction
		pvector2 operator-(
			const pvector2 &rhs
		) const;

		// subtraction
		pvector2& operator-=(
			const pvector2& rhs
		);

		// increment
		void increment(
			void
		);

		// decrement
		void decrement(
			void
		);

		// inverse
		void inverse(
			void
		);

		// inverse
		static pvector2 inverse(
			const pvector2 &in
		) XS_WARN_UNUSED_RESULT;

		// scalar multiplication
		pvector2 operator*(
			const real64_t scalar
		) const;

		// scalar multiplication
		pvector2& operator*=(
			const real64_t scalar
		);

		// vector multiplication
		pvector2 operator*(
			const pvector2 &rhs
		) const;

		// vector multiplication
		pvector2& operator*=(
			const pvector2 &rhs
		);

		// scalar division
		pvector2 operator/(
			const real64_t divisor
		) const;

		// scalar division
		pvector2& operator/=(
			const real64_t divisor
		);

		// vector division
		pvector2 operator/(
			const pvector2 &rhs
		) const;

		// vector division
		pvector2& operator/=(
			const pvector2 &rhs
		);

		// point within bounds
		static bool PointWithinBounds(
			const pvector2 &mins,
			const pvector2 &point,
			const pvector2 &size
		);

	};

	struct vector3 {

		real32_t data[3];

		// string representation of the vector
		std::string tostring(
			void
		) const XS_WARN_UNUSED_RESULT;

		// access operator (r/o)
		const real32_t& operator[](
			const size_t idx
		) const;

		// access opeartor (r/w)
		real32_t& operator[](
			const size_t idx
		);

		// clear
		void clear(
			void
		);

		// compare
		static bool compare(
			const vector3 &lhs,
			const vector3 &rhs
		) XS_WARN_UNUSED_RESULT;

		// compare
		bool compare(
			const vector3 &rhs
		) const XS_WARN_UNUSED_RESULT;

		// compare
		bool operator==(
			const vector3 &rhs
		);

		// addition
		vector3 operator+(
			const vector3 &rhs
		) const;

		// addition
		vector3& operator+=(
			const vector3& rhs
		);

		// subtraction
		vector3 operator-(
			const vector3 &rhs
		) const;

		// subtraction
		vector3& operator-=(
			const vector3& rhs
		);

		// increment
		void increment(
			void
		);

		// decrement
		void decrement(
			void
		);

		// inverse
		void inverse(
			void
		);

		// inverse
		static vector3 inverse(
			const vector3 &in
		) XS_WARN_UNUSED_RESULT;

		// scalar multiplication
		vector3 operator*(
			const real32_t scalar
		) const;

		// scalar multiplication
		vector3& operator*=(
			const real32_t scalar
		);

		// vector multiplication
		vector3 operator*(
			const vector3 &rhs
		) const;

		// vector multiplication
		vector3& operator*=(
			const vector3 &rhs
		);

		// scalar division
		vector3 operator/(
			const real32_t divisor
		) const;

		// scalar division
		vector3& operator/=(
			const real32_t divisor
		);

		// vector division
		vector3 operator/(
			const vector3 &rhs
		) const;

		// vector division
		vector3& operator/=(
			const vector3 &rhs
		);

		// length
		real32_t length(
			void
		) const XS_WARN_UNUSED_RESULT;

		// length
		static real32_t length(
			const vector3 &vec
		) XS_WARN_UNUSED_RESULT;

		// length squared
		real32_t lengthSquared(
			void
		) const XS_WARN_UNUSED_RESULT;

		// length squared
		static real32_t lengthSquared(
			const vector3 &vec
		) XS_WARN_UNUSED_RESULT;

		// distance
		static real32_t distance(
			const vector3 &a,
			const vector3 &b
		) XS_WARN_UNUSED_RESULT;

		// distance squared
		static real32_t distanceSquared(
			const vector3 &a,
			const vector3 &b
		) XS_WARN_UNUSED_RESULT;

		// normalise
		// does not check that length != 0
		// does not return length
		void normaliseFast(
			void
		);

		// normalise
		real32_t normalise(
			void
		);

		// normalise
		static vector3 normalise(
			const vector3 &vec
		) XS_WARN_UNUSED_RESULT;

		// dot product
		static real32_t dot(
			const vector3 &a,
			const vector3 &b
		) XS_WARN_UNUSED_RESULT;

		// dot product
		real32_t dot(
			const vector3 &vec
		) const XS_WARN_UNUSED_RESULT;

		// cross product
		static vector3 cross(
			const vector3 &a,
			const vector3 &b
		) XS_WARN_UNUSED_RESULT;

		// cross product
		vector3 cross(
			const vector3 &v
		) const XS_WARN_UNUSED_RESULT;

		// point within bounds
		static bool PointWithinBounds(
			const vector3 &mins,
			const vector3 &point,
			const vector3 &size
		);

		//TODO: snap?

	};

	struct pvector3 {

		real64_t data[3];

		// string representation of the vector
		std::string tostring(
			void
		) const XS_WARN_UNUSED_RESULT;

		// access operators (r/o)
		const real64_t& operator[](
			const size_t idx
		) const;

		// access operator (r/w)
		real64_t& operator[](
			const size_t idx
		);

		// clear
		void clear(
			void
		);

		// compare
		static bool compare(
			const pvector3 &lhs,
			const pvector3 &rhs
		) XS_WARN_UNUSED_RESULT;

		// compare
		bool compare(
			const pvector3 &rhs
		) const XS_WARN_UNUSED_RESULT;

		// compare
		bool operator==(
			const pvector3 &rhs
		);

		// addition
		pvector3 operator+(
			const pvector3 &rhs
		) const;

		// addition
		pvector3& operator+=(
			const pvector3 &rhs
		);

		// subtraction
		pvector3 operator-(
			const pvector3 &rhs
		) const;

		// subtraction
		pvector3& operator-=(
			const pvector3& rhs
		);

		// increment
		void increment(
			void
		);

		// decrement
		void decrement(
			void
		);

		// inverse
		void inverse(
			void
		);

		// inverse
		static pvector3 inverse(
			const pvector3 &in
		) XS_WARN_UNUSED_RESULT;

		// scalar multiplication
		pvector3 operator*(
			const real64_t scalar
		) const;

		// scalar multiplication
		pvector3& operator*=(
			const real64_t scalar
		);

		// vector multiplication
		pvector3 operator*(
			const pvector3 &rhs
		) const;

		// vector multiplication
		pvector3& operator*=(
			const pvector3 &rhs
		);

		// scalar division
		pvector3 operator/(
			const real64_t divisor
		) const;

		// scalar division
		pvector3& operator/=(
			const real64_t divisor
		);

		// vector division
		pvector3 operator/(
			const pvector3 &rhs
		) const;

		// vector division
		pvector3& operator/=(
			const pvector3 &rhs
		);

		// length
		real64_t length(
			void
		) const XS_WARN_UNUSED_RESULT;

		// length
		static real64_t length(
			const pvector3 &vec
		) XS_WARN_UNUSED_RESULT;

		// length squared
		real64_t lengthSquared(
			void
		) const XS_WARN_UNUSED_RESULT;

		// length squared
		static real64_t lengthSquared(
			const pvector3 &vec
		) XS_WARN_UNUSED_RESULT;

		// distance
		static real64_t distance(
			const pvector3 &a,
			const pvector3 &b
		) XS_WARN_UNUSED_RESULT;

		// distance squared
		static real64_t distanceSquared(
			const pvector3 &a,
			const pvector3 &b
		) XS_WARN_UNUSED_RESULT;

		// normalise
		// does not check that length != 0
		// does not return length
		void normaliseFast(
			void
		);

		// normalise
		real64_t normalise(
			void
		);

		// normalise
		static pvector3 normalise(
			const pvector3 &vec
		);

		// dot
		real64_t dot(
			const pvector3 &vec
		) const XS_WARN_UNUSED_RESULT;

		// cross
		static pvector3 cross(
			const pvector3 &a,
			const pvector3 &b
		) XS_WARN_UNUSED_RESULT;

		// cross
		pvector3 cross(
			const pvector3 &v
		) const XS_WARN_UNUSED_RESULT;

		// point within bounds
		static bool PointWithinBounds(
			const pvector3 &mins,
			const pvector3 &point,
			const pvector3 &size
		);

		//TODO: snap?

	};

	struct vector4 {

		real32_t data[4];

		// string representation of the vector
		std::string tostring(
			void
		) const XS_WARN_UNUSED_RESULT;

		// access operator (r/o)
		const real32_t& operator[](
			const size_t idx
		) const;

		// access operator (r/w)
		real32_t& operator[](
			const size_t idx
		);

		// clear
		void clear(
			void
		);

		// compare
		static bool compare(
			const vector4 &lhs,
			const vector4 &rhs
		) XS_WARN_UNUSED_RESULT;

		// compare
		bool compare(
			const vector4 &rhs
		) const XS_WARN_UNUSED_RESULT;

		// compare
		bool operator==(
			const vector4 &rhs
		) const;

		// addition
		vector4 operator+(
			const vector4 &rhs
		) const;

		// addition
		vector4& operator+=(
			const vector4& rhs
		);

		// subtraction
		vector4 operator-(
			const vector4 &rhs
		) const;

		// subtraction
		vector4& operator-=(
			const vector4& rhs
		);

		// increment
		void increment(
			void
		);

		// decrement
		void decrement(
			void
		);

		// inverse
		void inverse(
			void
		);

		// inverse
		static vector4 inverse(
			const vector4 &in
		) XS_WARN_UNUSED_RESULT;

		// scalar multiplication
		vector4 operator*(
			const real32_t scalar
		) const;

		// scalar multiplication
		vector4& operator*=(
			const real32_t scalar
		);

		// vector multiplication
		vector4 operator*(
			const vector4 &rhs
		) const;

		// vector multiplication
		vector4& operator*=(
			const vector4 &rhs
		);

		// scalar division
		vector4 operator/(
			const real32_t divisor
		) const;

		// scalar division
		vector4& operator/=(
			const real32_t divisor
		);

		// vector division
		vector4 operator/(
			const vector4 &rhs
		) const;

		// vector division
		vector4& operator/=(
			const vector4 &rhs
		);

	};

} // namespace XS
