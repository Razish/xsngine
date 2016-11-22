#include "Common/Common.h"
#include "Common/Console.h"
#include "Client/ClientGame.h"
#include "Client/Perlin.h"

namespace ClientGame {

	// constructor
	Perlin::Perlin( real64_t inPersistence, real64_t inFrequency, real64_t inAmplitude, int32_t inOctaves,
		int32_t inRandomSeed )
	:	persistence( privatePersistence ),
		frequency( privateFrequency ),
		amplitude( privateAmplitude ),
		octaves( privateOctaves ),
		randomSeed( privateRandomSeed )
	{
		Set( inPersistence, inFrequency, inAmplitude, inOctaves, inRandomSeed );
	}

	void Perlin::Set( real64_t inPersistence, real64_t inFrequency, real64_t inAmplitude, int32_t inOctaves,
		int32_t inRandomSeed )
	{
		privatePersistence = inPersistence;
		privateFrequency = inFrequency;
		privateAmplitude = inAmplitude;
		privateOctaves = inOctaves;
		privateRandomSeed = 2 + (inRandomSeed * inRandomSeed);
	}

	real64_t Perlin::GetHeight( real64_t x, real64_t y ) const {
		return amplitude * Total( x, y );
	}

	real64_t Perlin::Total( real64_t i, real64_t j ) const {
		// properties of one octave (changing each loop)
		real64_t t = 0.0;
		real64_t amp = 1;
		real64_t freq = frequency;

		for ( int32_t k = 0u; k < octaves; k++ ) {
			real64_t v = GetValue(
				j * freq + randomSeed,
				i * freq + randomSeed
			);
			t += v * amp;
			amp *= persistence;
			freq *= 2;
		}

		return t;
	}

	real64_t Perlin::GetValue( real64_t x, real64_t y ) const {
		int32_t iX = static_cast<int32_t>( x );
		int32_t iY = static_cast<int32_t>( y );
		real64_t fracX = x - iX;
		real64_t fracY = y - iY;

		// noise values
		real64_t n01 = Noise( iX - 1, iY - 1 );
		real64_t n02 = Noise( iX + 1, iY - 1 );
		real64_t n03 = Noise( iX - 1, iY + 1 );
		real64_t n04 = Noise( iX + 1, iY + 1 );
		real64_t n05 = Noise( iX - 1, iY + 0 );
		real64_t n06 = Noise( iX + 1, iY + 0 );
		real64_t n07 = Noise( iX + 0, iY - 1 );
		real64_t n08 = Noise( iX + 0, iY + 1 );
		real64_t n09 = Noise( iX + 0, iY + 0 );

		real64_t n12 = Noise( iX + 2, iY - 1 );
		real64_t n14 = Noise( iX + 2, iY + 1 );
		real64_t n16 = Noise( iX + 2, iY + 0 );

		real64_t n23 = Noise( iX - 1, iY + 2 );
		real64_t n24 = Noise( iX + 1, iY + 2 );
		real64_t n28 = Noise( iX + 0, iY + 2 );
		real64_t n34 = Noise( iX + 2, iY + 2 );

		// find the noise values of the four corners
		real64_t x0y0 = (0.0625 * (n01 + n02 + n03 + n04)) + (0.125 * (n05 + n06 + n07 + n08)) + (0.25 * n09);
		real64_t x1y0 = (0.0625 * (n07 + n12 + n08 + n14)) + (0.125 * (n09 + n16 + n02 + n04)) + (0.25 * n06);
		real64_t x0y1 = (0.0625 * (n05 + n06 + n23 + n24)) + (0.125 * (n03 + n04 + n09 + n28)) + (0.25 * n08);
		real64_t x1y1 = (0.0625 * (n09 + n16 + n28 + n34)) + (0.125 * (n08 + n14 + n06 + n24)) + (0.25 * n04);

		// interpolate between those values according to the x and y fractions
		real64_t v1 = Interpolate( x0y0, x1y0, fracX ); // interpolate in x direction (y+0)
		real64_t v2 = Interpolate( x0y1, x1y1, fracX ); // interpolate in x direction (y+1)
		real64_t fin = Interpolate( v1, v2, fracY );  // interpolate in y direction

		return fin;
	}

	real64_t Perlin::Interpolate( real64_t x, real64_t y, real64_t a ) const {
		real64_t negA = 1.0 - a;
		real64_t negASqr = negA * negA;
		real64_t factor1 = (3.0 * negASqr) - (2.0 * negASqr * negA);
		real64_t aSqr = a * a;
		real64_t factor2 = (3.0 * aSqr) - (2.0 * aSqr * a);

		// add the weighted factors
		return (x * factor1) + (y * factor2);
	}

	double Perlin::Noise( int32_t x, int32_t y ) const {
		int32_t n = x + y * 57;
		n = (n << 13) ^ n;
		int32_t t = (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff;
		real64_t dT = static_cast<real64_t>( t );
		return 1.0 - (dT * 0.931322574615478515625e-9);
	}

} // namespace ClientGame
