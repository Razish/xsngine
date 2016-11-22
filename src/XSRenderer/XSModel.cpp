#include <unordered_map>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSFileXMF.h"
#include "XSRenderer/XSModel.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSMesh.h"

namespace Renderer {

	// list of registered models
	static std::unordered_map<std::string, Model*> models;

	// public, static
	Model::Handle Model::Register( const char *path ) {
		//HACK: assume null path means raw model data
		if ( !path ) {
			Model *model = new Model();
			model->handle = numRegistered++;
			return model->handle;
		}

		//TODO: check for duplicates? or only for mesh info etc?
		// for now, just add a new entry
		Model *model = models[path];

		if ( model ) {
			console.Print( PrintLevel::Debug, "Loading model \"%s\" using existing model (loaded %i times)\n",
				XS_FUNCTION,
				path,
				model->refCount
			);
			model->refCount++;
			return model->handle;
		}

		model = models[path] = new Model();

		console.Print( PrintLevel::Debug, "%s loaded \"%s\" for the first time\n",
			XS_FUNCTION,
			path
		);
		model->modelPath = path;
		model->handle = numRegistered++;

		const File f( path, FileMode::ReadBinary );
		if ( !f.isOpen ) {
			console.Print( PrintLevel::Normal, "Failed to load model \"%s\", returning invalid handle\n",
				model->modelPath.c_str()
			);
			return invalidHandle;
		}

		char *buffer = new char[f.length + 1];
		{
			f.Read( reinterpret_cast<uint8_t *>( buffer ) );
			//FIXME: dispatch to correct file reader based on extension
			Common::FileXMF xmf( buffer, f.length );
			size_t numMeshes = xmf.GetMeshCount();
			SDL_assert( numMeshes > 0u );
			for ( size_t i = 0u; i < numMeshes; i++ ) {
				Common::XMFMesh *xmfMesh = xmf.GetMeshFromIndex( i );
				if ( !xmfMesh ) {
					break;
				}
				/*bool computedNormals = */xmfMesh->ValidateNormals();
				Mesh *rdMesh = new Mesh();

				//TODO: profile copying, optimise
				rdMesh->vertices = xmfMesh->vertices;
				rdMesh->normals = xmfMesh->normals;
				rdMesh->UVs = xmfMesh->UVs;
				rdMesh->indices = xmfMesh->indices;

				rdMesh->CreateMaterial();

				model->AddMesh( rdMesh );
			}
		}
		delete[] buffer;

		//model->LoadMeshes();

		return model->handle;
	}

	Model::~Model() {
		Model *model = models[modelPath];
		if ( model ) {
			model->refCount--;
			if ( !model->refCount ) {
				console.Print( PrintLevel::Normal, "%s removed last model for \"%s\"\n",
					XS_FUNCTION,
					modelPath.c_str()
				);
				models[modelPath] = nullptr;
			}
			else {
				console.Print( PrintLevel::Normal, "%s removing model \"%s\"\n",
					XS_FUNCTION,
					modelPath.c_str()
				);
			}
		}
		else if ( !modelPath.empty() ) {
			SDL_assert( !"could not find model" );
			console.Print( PrintLevel::Normal, "%s could not find model for \"%s\"\n",
				XS_FUNCTION,
				modelPath.c_str()
			);
		}

		meshes.clear();
	}

	void Model::Draw( const RenderInfo &info ) const {
		Renderer::DrawModel( this, info );
	}

	void Model::AddMesh( Mesh *mesh ) {
		SDL_assert( mesh );
		mesh->Upload();
		meshes.push_back( mesh );
	}

} // namespace Renderer
