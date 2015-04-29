#pragma once

namespace XS {

	namespace ClientGame {

		class Perlin {
		private:
			real64_t privatePersistence;
			real64_t privateFrequency;
			real64_t privateAmplitude;
			int32_t privateOctaves;
			int32_t privateRandomSeed;

			real64_t Total(
				real64_t i,
				real64_t j
			) const;

			real64_t GetValue(
				real64_t x,
				real64_t y
			) const;

			real64_t Interpolate(
				real64_t x,
				real64_t y,
				real64_t a
			) const;

			real64_t Noise(
				int32_t x,
				int32_t y
			) const;

		public:
			const real64_t &persistence;
			const real64_t &frequency;
			const real64_t &amplitude;
			const int32_t &octaves;
			const int32_t &randomSeed;

			// don't allow default instantiation
			Perlin() = delete;
			Perlin( const Perlin& ) = delete;
			Perlin& operator=( const Perlin& ) = delete;

			// public constructor
			Perlin(
				real64_t persistence,
				real64_t frequency,
				real64_t amplitude,
				int32_t octaves,
				int32_t randomSeed
			);

			void Set(
				real64_t persistence,
				real64_t frequency,
				real64_t amplitude,
				int32_t octaves,
				int32_t randomSeed
			);

			real64_t GetHeight(
				real64_t x,
				real64_t y
			) const;

		};

	} // namespace ClientGame

} // namespace XS
