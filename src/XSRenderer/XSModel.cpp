#include <unordered_map>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSFile.h"
#include "XSRenderer/XSModel.h"
#include "XSRenderer/XSModelObj.h"
#include "XSRenderer/XSModelXMF.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSMesh.h"

namespace XS {

	namespace Renderer {

		// list of registered models
		static std::unordered_map<std::string, Model*> models;

		// map file extensions to internal model formats
		static const struct {
			const char	*name;
			ModelType	 type;
		} extensionTable[] = {
			{ "obj", ModelType::OBJ },
			{ "xmf", ModelType::XMF },
		};
		static ModelType GetTypeForExtension( const char *string ) {
			for ( auto &extension : extensionTable ) {
				if ( !String::Compare( string, extension.name ) ) {
					return extension.type;
				}
			}
			return ModelType::Invalid;
		}

		// public, static
		Model *Model::Register( const char *path ) {
			//TODO: check for duplicates? or only for mesh info etc?
			// for now, just add a new entry
			Model *model = models[path];

			if ( model ) {
				console.Print( PrintLevel::Debug,
					"%s for '%s' using existing model (loaded %i times)\n",
					XS_FUNCTION,
					path,
					model->refCount
				);
				model->refCount++;
				return model;
			}

			char extension[XS_MAX_FILENAME];
			if ( !File::GetExtension( path, extension, sizeof(extension) ) ) {
				console.Print( PrintLevel::Normal,
					"%s for '%s' unable to determine model format\n",
					XS_FUNCTION,
					path
				);
				return NULL;
			}

			//TODO: factory?
			ModelType type = GetTypeForExtension( extension );
			if ( type == ModelType::OBJ ) {
				model = models[path] = new Obj();
			}
			else if ( type == ModelType::XMF ) {
				model = models[path] = new XMF();
			}
			else {
				SDL_assert( !"tried to instantiate invalid model type. Should not happen!" );
				return NULL;
			}

			console.Print( PrintLevel::Debug, "%s loaded '%s' for the first time\n", XS_FUNCTION, path );
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
					console.Print( PrintLevel::Normal,
						"%s removed last model for '%s'\n",
						XS_FUNCTION,
						modelPath.c_str()
					);
					models[modelPath] = nullptr;
				}
				else {
					console.Print( PrintLevel::Normal, "%s removing model '%s'\n", XS_FUNCTION, modelPath.c_str() );
				}
			}
			else {
				SDL_assert( !"could not find model" );
				console.Print( PrintLevel::Normal,
					"%s could not find model for '%s'\n",
					XS_FUNCTION,
					modelPath.c_str()
				);
			}
			for ( auto mesh : meshes ) {
				delete mesh;
			}
		}

		void Model::Draw( void ) const {
			Renderer::DrawModel( this );
		}

	} // namespace Renderer

} // namespace XS
