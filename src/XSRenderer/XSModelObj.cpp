#include <inttypes.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSTokenParser.h"
#include "XSCommon/XSVector.h"
#include "XSRenderer/XSModelObj.h"
#include "XSRenderer/XSMesh.h"

namespace XS {

	namespace Renderer {

		void Obj::Process( Mesh *mesh, std::vector<vector3> &tmpVertices, std::vector<vector2> &tmpUVs,
			std::vector<vector3> &tmpNormals )
		{
			#if 0
			if ( mesh->numVertices ) {
				mesh->vertices = new vector3[mesh->numVertices];
				for ( size_t i = 0; i < mesh->vertexIndices.size(); i++ ) {
					const uint16_t vertexIndex = mesh->vertexIndices[i];
					mesh->vertices[i] = tmpVertices[ vertexIndex - 1 ];
				}
			}

			if ( mesh->numUVs ) {
				mesh->uv = new vector2[mesh->numUVs];
				for ( size_t i = 0; i < mesh->uvIndices.size(); i++ ) {
					const uint16_t uvIndex = mesh->vertexIndices[i];
					mesh->uv[i] = tmpUVs[ uvIndex - 1 ];
				}
			}

			if ( mesh->numNormals ) {
				mesh->normals = new vector3[mesh->numNormals];
				for ( size_t i = 0; i < mesh->normalIndices.size(); i++ ) {
					const uint16_t normalIndex = mesh->normalIndices[i];
					mesh->normals[i] = tmpNormals[ normalIndex - 1 ];
				}
			}
			#endif

			meshes.push_back( mesh );
		}

		bool Obj::LoadMeshes( void ) {
			const File f( modelPath.c_str() );
			if ( !f.open ) {
				console.Print( "Obj::LoadMeshes( '%s' ) failed to open file\n", modelPath.c_str() );
				return false;
			}
			char *buffer = new char[f.length];
				f.Read( reinterpret_cast<byte *>( buffer ) );
				TokenParser *parser = new TokenParser( buffer );
				Mesh *mesh = nullptr;
				std::vector<vector3> tmpVertices;
				std::vector<vector2> tmpUVs;
				std::vector<vector3> tmpNormals;

				while ( true ) {
					const char *token = parser->ParseToken();
					if ( !token[0] ) {
						// end of file
						if ( mesh ) {
							Process( mesh, tmpVertices, tmpUVs, tmpNormals );
						}
						mesh = nullptr;
						break;
					}
					else if ( !String::CompareCase( token, "#" ) ) {
						// # is used as a comment
						parser->SkipLine();
					}
					else if ( !String::CompareCase( token, "o" ) ) {
						// new object/mesh
						if ( mesh ) {
							Process( mesh, tmpVertices, tmpUVs, tmpNormals );
							tmpVertices.clear();
							tmpUVs.clear();
							tmpNormals.clear();
						}
						mesh = new Mesh();
						console.Print( "Obj::LoadMeshes( '%s' ) loading new mesh at 0x%" PRIXPTR "\n",
							modelPath.c_str(), mesh );
						parser->SkipLine();
					}
					else if ( !String::CompareCase( token, "s" ) ) {
						// check if smoothing is on/off
						// we don't care about this
						parser->SkipLine();
					}
					else if ( !String::CompareCase( token, "v" ) ) {
						// new vertex
						if ( !mesh ) {
							// something went wrong, we can't parse vertices out if we don't know what mesh they're
							//	related to
							console.Print( "Obj::LoadMeshes( '%s' ) tried to parse vertices without specifying a mesh!"
								"\n", modelPath.c_str() );
							break;
						}

						vector3 vertex;
						for ( int i = 0; i < 3; i++ ) {
							parser->ParseFloat( &vertex[i] );
						}
						tmpVertices.push_back( vertex );
						mesh->numVertices++;
						parser->SkipLine();
					}
					else if ( !String::CompareCase( token, "vn" ) ) {
						// new vertex normal
						if ( !mesh ) {
							// something went wrong, we can't parse normals out if we don't know what mesh they're
							//	related to
							console.Print( "Obj::LoadMeshes( '%s' ) tried to parse normals without specifying a mesh!"
								"\n", modelPath.c_str() );
							break;
						}

						vector3 normal;
						for ( int i = 0; i < 3; i++ ) {
							parser->ParseFloat( &normal[i] );
						}
						tmpNormals.push_back( normal );
						mesh->numNormals++;
						parser->SkipLine();
					}
					else if ( !String::CompareCase( token, "f" ) ) {
						if ( !mesh ) {
							// something went wrong, we can't parse faces out if we don't know what mesh they're
							//	related to
							console.Print( "Obj::LoadMeshes( '%s' ) tried to parse face without specifying a mesh!\n",
								modelPath.c_str() );
							break;
						}
						for ( int i = 0; i < 3; i++ ) {
							const char *str = nullptr;
							parser->ParseString( &str );
							if ( str ) {
								uint16_t indices[3];
								sscanf( str, "%hd/%hd/%hd", &indices[0], &indices[1], &indices[2] );
								mesh->vertexIndices.push_back( indices[0] );
								mesh->uvIndices.push_back( indices[1] );
								mesh->normalIndices.push_back( indices[2] );
							}
						}
						parser->SkipLine();
					}
					else {
						parser->SkipLine();
					}
				}

				delete parser;
			delete[] buffer;

			return false;
		}

	} // namespace Renderer

} // namespace XS
