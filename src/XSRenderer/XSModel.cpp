#include <map>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSString.h"
#include "XSRenderer/XSModel.h"
#include "XSRenderer/XSRenderer.h"

namespace XS {

	namespace Renderer {

		// list of registered models
		static uint32_t numModels = 0u;
		static std::map<uint32_t, Model*> models;

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
			Model *model = models[numModels];

			if ( model ) {
				console.Print( "Model::Register() using existing model for '%s' (%02u)\n", path, model->id );
				return model;
			}

			char extension[XS_MAX_FILENAME];
			if ( !File::GetExtension( path, extension, sizeof(extension) ) ) {
				console.Print( "Model::Register() Unable to determine model format for '%s'\n", path );
				return NULL;
			}

			model = models[numModels] = new Model();
			model->id = numModels++;
			model->type = GetTypeForExtension( extension );
			console.Print( "Model::Register() loaded '%s' for the first time\n", path );

			return model;
		}

		Model::~Model() {
			Model *model = models[id];
			if ( model ) {
				console.Print( "Model::~Model() removing model with id %02u\n", id );
				models[id] = nullptr;
			}
			else {
				SDL_assert( !"Model::~Model() could not find model" );
				console.Print( "Model::~Model() could not find model with id %02u\n", id );
			}
		}

	} // namespace Renderer

} // namespace XS
