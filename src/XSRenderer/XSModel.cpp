#include <unordered_map>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSFile.h"
#include "XSRenderer/XSModel.h"
#include "XSRenderer/XSModelObj.h"
#include "XSRenderer/XSRenderer.h"

namespace XS {

	namespace Renderer {

		// list of registered models
		static std::unordered_map<std::string, Model*> models;

		// map file extensions to internal model formats
		static const struct {
			const char *name;
			Model::Type type;
		} extensionTable[] = {
			{ "obj",	Model::Type::OBJ },
		};
		static const size_t numExtensions = ARRAY_LEN( extensionTable );
		static Model::Type GetTypeForExtension( const char *string ) {
			for ( size_t extension = 0; extension < numExtensions; extension++ ) {
				if ( !String::Compare( string, extensionTable[extension].name ) ) {
					return extensionTable[extension].type;
				}
			}
			return Model::Type::UNKNOWN;
		}

		Model *Model::Register( const char *path ) {
			//TODO: check for duplicates? or only for mesh info etc?
			// for now, just add a new entry
			Model *model = models[path];

			if ( model ) {
				console.Print( "Model::Register( '%s' ) using existing model\n", path );
				model->refCount++;
				return model;
			}

			char extension[XS_MAX_FILENAME];
			if ( !File::GetExtension( path, extension, sizeof(extension) ) ) {
				console.Print( "Model::Register( '%s' ) Unable to determine model format\n", path );
				return NULL;
			}

			Model::Type type = GetTypeForExtension( extension );
			if ( type == Model::Type::OBJ ) {
				model = models[path] = new Obj();
			}
			else {
				SDL_assert( !"Model::Register() tried to instantiate invalid model type. Should not happen!" );
			}

			console.Print( "Model::Register( '%s' ) loaded for the first time\n", path );
			model->type = type;
			model->modelPath = path;
			model->LoadMeshes();

			return model;
		}

		Model::~Model() {
			Model *model = models[modelPath];
			if ( model ) {
				model->refCount--;
				if ( !model->refCount ) {
					console.Print( "Model::~Model( '%s' ) removed last model\n", modelPath.c_str() );
					models[modelPath] = nullptr;
				}
				else {
					console.Print( "Model::~Model( '%s' ) removing model\n", modelPath.c_str() );
				}
			}
			else {
				SDL_assert( !"Model::~Model() could not find model" );
				console.Print( "Model::~Model( '%s' ) could not find model\n", modelPath.c_str() );
			}
		}

		void Model::Draw( void ) const {
			Renderer::DrawModel( this );
		}

	} // namespace Renderer

} // namespace XS
