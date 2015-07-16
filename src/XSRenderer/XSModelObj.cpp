#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSTokenParser.h"
#include "XSCommon/XSVector.h"
#include "XSRenderer/XSModelObj.h"
#include "XSRenderer/XSInternalFormat.h"
#include "XSRenderer/XSMesh.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSShaderProgram.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSVertexAttributes.h"

namespace XS {

	namespace Renderer {

		void Obj::Process( Mesh *mesh ) {
			mesh->normals.resize( mesh->vertices.size(), vector3{} );
			size_t numIndices = mesh->indices.size();
			for ( size_t i = 0; i < numIndices; i += 3 ) {
				uint32_t ia = mesh->indices[i + 0];
				uint32_t ib = mesh->indices[i + 1];
				uint32_t ic = mesh->indices[i + 2];
				const size_t numVertices = mesh->vertices.size();
				if ( ia >= numVertices || ib >= numVertices || ic >= numVertices ) {
					continue;
				}
				vector3 normal = vector3::normalise( vector3::cross(
						vector3( mesh->vertices[ib] ) - vector3( mesh->vertices[ia] ),
						vector3( mesh->vertices[ic] ) - vector3( mesh->vertices[ia] ) ) );
				mesh->normals[ia] = mesh->normals[ib] = mesh->normals[ic] = normal;
			}

			// create texture
			static const uint8_t texture[128 * 128 * 4] = { 127 };
			mesh->texture = new Texture( 128, 128, InternalFormat::RGBA8, texture );

			// create shader program
			static const VertexAttribute attributes[] = {
				{ 0, "in_Position" },
				{ 1, "in_Normal" },
				{ 2, "in_TexCoord" },
			};
			mesh->shader = new ShaderProgram( "model", "model", attributes, ARRAY_LEN( attributes ) );

			// create material
			mesh->material = new Material();
			Material::SamplerBinding samplerBinding;
			samplerBinding.unit = 0;
			samplerBinding.texture = mesh->texture;
			mesh->material->samplerBindings.push_back( samplerBinding );
			mesh->material->shaderProgram = mesh->shader;

			mesh->Upload();
			meshes.push_back( mesh );
		}

		bool Obj::LoadMeshes( void ) {
			const File f( modelPath.c_str() );
			if ( !f.open ) {
				console.Print( PrintLevel::Normal, "%s failed to open file '%s'\n", XS_FUNCTION, modelPath.c_str() );
				return false;
			}
			char *buffer = new char[f.length];
			f.Read( reinterpret_cast<uint8_t *>( buffer ) );
			TokenParser parser( buffer );
			Mesh *mesh = new Mesh();
			console.Print( PrintLevel::Debug, "%s loading new mesh for '%s' at 0x%" PRIXPTR "\n",
				XS_FUNCTION,
				modelPath.c_str(),
				mesh
			);

			while ( true ) {
				const char *token = parser.ParseToken();
				if ( !token[0] ) {
					// end of file
					if ( mesh ) {
						Process( mesh );
					}
					mesh = nullptr;
					break;
				}
				else if ( !String::CompareCase( token, "#" ) ) {
					// # is used as a comment
					parser.SkipLine();
				}
				else if ( !String::CompareCase( token, "o" ) ) {
					// new object/mesh
					if ( mesh && mesh->vertices.size() > 0 ) {
						Process( mesh );
					}
					mesh = new Mesh();
					console.Print( PrintLevel::Debug, "%s loading new mesh for '%s' at 0x%" PRIXPTR "\n",
						XS_FUNCTION,
						modelPath.c_str(),
						mesh
					);
					parser.SkipLine();
				}
				else if ( !String::CompareCase( token, "s" ) ) {
					// check if smoothing is on/off
					// we don't care about this
					parser.SkipLine();
				}
				else if ( !String::CompareCase( token, "usemtl" ) ) {
					// no support for materials just yet
					parser.SkipLine();
				}
				else if ( !String::CompareCase( token, "mtllib" ) ) {
					// no support for materials just yet
					parser.SkipLine();
				}
				else if ( !String::CompareCase( token, "g" ) ) {
					// no support for groups
					parser.SkipLine();
				}
				else if ( !String::CompareCase( token, "v" ) ) {
					// new vertex
					if ( !mesh ) {
						// something went wrong, we can't parse vertices out if we don't know what mesh they're related
						//	to
						console.Print( PrintLevel::Normal,
							"%s tried to parse vertices for '%s' without specifying a mesh!\n",
							XS_FUNCTION,
							modelPath.c_str()
						);
						break;
					}

					vector3 vertex;
					for ( int i = 0; i < 3; i++ ) {
						parser.ParseReal32( &vertex[i] );
					}
					mesh->vertices.push_back( vertex );
					parser.SkipLine();
				}
				else if ( !String::CompareCase( token, "vn" ) ) {
					// new vertex normal
					if ( !mesh ) {
						// something went wrong, we can't parse normals out if we don't know what mesh they're related
						//	to
						console.Print( PrintLevel::Normal,
							"%s tried to parse normals for '%s' without specifying a mesh!\n",
							XS_FUNCTION,
							modelPath.c_str()
						);
						break;
					}

					vector3 normal;
					for ( int i = 0; i < 3; i++ ) {
						parser.ParseReal32( &normal[i] );
					}
					mesh->normals.push_back( normal );
					parser.SkipLine();
				}
				else if ( !String::CompareCase( token, "f" ) ) {
					if ( !mesh ) {
						// something went wrong, we can't parse faces out if we don't know what mesh they're related to
						console.Print( PrintLevel::Normal,
							"%s tried to parse face for '%s' without specifying a mesh!\n",
							XS_FUNCTION,
							modelPath.c_str()
						);
						break;
					}
					uint32_t a = 0, b = 0, c = 0;
					for ( int i = 0; i < 3; i++ ) {
						const char *str = nullptr;
						parser.ParseString( &str );
						if ( str ) {
							sscanf( str, "%i/%i/%i", &a, &b, &c );
							if ( a != 0 ) {
								a--;
							}
							mesh->indices.push_back( a );
							if ( b != 0 ) {
								b--;
							}
							mesh->indices.push_back( b );
							if ( c != 0 ) {
								c--;
							}
							mesh->indices.push_back( c );
						}
					}
					parser.SkipLine();
				}
				else {
					parser.SkipLine();
				}
			}

			delete[] buffer;

			return true;
		}

	} // namespace Renderer

} // namespace XS
