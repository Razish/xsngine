#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSTokenParser.h"
#include "XSCommon/XSVector.h"
#include "XSCommon/XSFileXMF.h"

namespace Common {

	uint32_t FileXMF::version = 3u;

	bool XMFMesh::ValidateNormals( void ) {
		if ( !normals.empty() ) {
			return false;
		}
		console.Print( PrintLevel::Developer, "Calculating surface normals using Newell's method\n" );

		std::vector<vector3> surfaceNormals;
		surfaceNormals.resize( indices.size() / 3 );
		normals.resize( vertices.size() );

		// first calculate surface normals
		for ( size_t i = 0; i < indices.size(); i += 3 ) {
			const auto &p1 = vertices[indices[i + 0]];
			const auto &p2 = vertices[indices[i + 1]];
			const auto &p3 = vertices[indices[i + 2]];

			surfaceNormals[i/3] = vector3::cross( p2 - p1, p3 - p1 );
		}

		// then for each vertex, sum all the neighbouring surface normals to obtain the vertex normal
		for ( size_t iV = 0u; iV < vertices.size(); iV++ ) {
			vector3 normal = {};
			for ( size_t iN = 0u; iN < indices.size(); iN += 3 ) {
				for ( size_t f = 0u; f < 3; f++ ) {
					if ( indices[iN + f] == iV ) {
						normal += surfaceNormals[iN / 3];
					}
				}
			}
			normal.normaliseFast();
			normals[iV] = normal;
		}
		return true;
	}

	FileXMF::FileXMF( const char *buffer, size_t bufferSize ) {
		SDL_assert( buffer );
		SDL_assert( bufferSize > 0u );

		TokenParser parser( buffer );

		// check the version identifier
		const char *token = parser.ParseToken();
		if ( !String::CompareCase( token, "version" ) ) {
			uint32_t ui = 0u;
			if ( parser.ParseUInt32( &ui ) ) {
				console.Print( PrintLevel::Normal,
					"%s invalid XMF file: could not parse version identifier\n",
					XS_FUNCTION
				);
				return;
			}
			else if ( ui != FileXMF::version ) {
				console.Print( PrintLevel::Normal,
					"%s invalid XMF file: wrong version identifier (%u != %u)\n",
					XS_FUNCTION,
					ui,
					FileXMF::version
				);
				return;
			}
		}
		else {
			console.Print( PrintLevel::Normal, "%s invalid XMF file: missing version identifier!\n",
				XS_FUNCTION
			);
			return;
		}

		// should be a valid XMF, try parsing it out
		XMFMesh *mesh = nullptr;
		while ( true ) {
			token = parser.ParseToken();
			if ( !token[0] ) {
				// end of file
				if ( mesh ) {
					mesh->ValidateNormals();
					meshes.push_back( mesh );
					mesh = nullptr;
				}
				break;
			}
			else if ( !String::CompareCase( token, "#" ) ) {
				// # is used as a comment
				parser.SkipLine();
			}
			else if ( !String::CompareCase( token, "o" ) ) {
				// new object/mesh
				if ( mesh ) {
					if ( mesh->vertices.size() > 0 ) {
						mesh->ValidateNormals();
					}
					meshes.push_back( mesh );
				}

				mesh = new XMFMesh();
				console.Print( PrintLevel::Developer, "%s loading new mesh at 0x%" PRIXPTR "\n",
					XS_FUNCTION,
					mesh
				);

				const char *str = nullptr;
				if ( parser.ParseString( &str ) ) {
					mesh->textureName = "invalid-name";
				}
				else if ( str ) {
					mesh->textureName = str;
				}

				parser.SkipLine();
			}
			else if ( !String::CompareCase( token, "v" ) ) {
				// new vertex
				if ( !mesh ) {
					// something went wrong, we can't parse vertices out if we don't know what mesh they're
					//	related to
					console.Print( PrintLevel::Normal,
						"%s tried to parse vertices without specifying a mesh!\n",
						XS_FUNCTION
					);
					break;
				}

				vector3 vertex;
				for ( int i = 0; i < 3; i++ ) {
					real32_t f;
					if ( !parser.ParseReal32( &f ) ) {
						vertex[i] = f;
					}
				}
				mesh->vertices.push_back( vertex );
				parser.SkipLine();
			}
			else if ( !String::CompareCase( token, "vn" ) ) {
				// new normal
				if ( !mesh ) {
					// something went wrong, we can't parse normals out if we don't know what mesh they're
					//	related to
					console.Print( PrintLevel::Normal,
						"%s tried to parse normals without specifying a mesh!\n",
						XS_FUNCTION
					);
					break;
				}

				vector3 normal;
				for ( int i = 0; i < 3; i++ ) {
					real32_t f;
					if ( !parser.ParseReal32( &f ) ) {
						normal[i] = f;
					}
				}
				mesh->normals.push_back( normal );
				parser.SkipLine();
			}
			else if ( !String::CompareCase( token, "uv" ) ) {
				// new texture coordinates
				if ( !mesh ) {
					// something went wrong, we can't parse texture coordinates out if we don't know what mesh
					//	they're related to
					console.Print( PrintLevel::Normal,
						"%s tried to parse UVs without specifying a mesh!\n",
						XS_FUNCTION
					);
					break;
				}

				vector2 uv{};
				for ( int i = 0; i < 2; i++ ) {
					real32_t f;
					if ( !parser.ParseReal32( &f ) ) {
						uv[i] = f;
					}
				}
				//FIXME: why flip the UVs?
				real32_t tmp = uv[0];
				uv[0] = uv[1];
				uv[1] = tmp;
				mesh->UVs.push_back( uv );
				parser.SkipLine();
			}
			else if ( !String::CompareCase( token, "f" ) ) {
				if ( !mesh ) {
					// something went wrong, we can't parse faces out if we don't know what mesh they're related
					//	to
					console.Print( PrintLevel::Normal,
						"%s tried to parse face without specifying a mesh!\n",
						XS_FUNCTION
					);
					break;
				}

				// vertex indices
				const char *str = nullptr;
				if ( parser.ParseString( &str ) ) {
					console.Print( PrintLevel::Normal,
						"%s missing face definition!\n",
						XS_FUNCTION
					);
				}
				else if ( str ) {
					uint32_t a = 0, b = 0, c = 0;
					sscanf( str, "%i/%i/%i", &a, &b, &c );
					mesh->indices.push_back( a );
					mesh->indices.push_back( b );
					mesh->indices.push_back( c );
				}

				parser.SkipLine();
			}
			else {
				parser.SkipLine();
			}
		}
	}

	FileXMF::~FileXMF() {
		for ( auto *mesh : meshes ) {
			delete mesh;
		}
	}

	size_t FileXMF::GetMeshCount( void ) const {
		return meshes.size();
	}

	XMFMesh *FileXMF::GetMeshFromIndex( size_t index ) {
		return meshes[index];
	}

} // namespace Renderer
