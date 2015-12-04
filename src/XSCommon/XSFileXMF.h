#pragma once

#include <vector>

#include "XSCommon/XSVector.h"

namespace XS {

	namespace Common {

		struct XMFMesh {
			std::vector<vector3>	vertices;
			std::vector<vector3>	normals;
			std::vector<vector2>	UVs;
			std::vector<uint32_t>	indices;
			std::string				textureName;

			// returns true if we had to compute normals ourself
			bool ValidateNormals(
				void
			);
		};

		class FileXMF {
		private:
			static uint32_t		version;

			std::vector<XMFMesh *>	meshes;

		public:
			FileXMF(
				const char *buffer,
				size_t bufferSize
			);
			~FileXMF();

			// don't allow default instantiation
			FileXMF() = delete;
			FileXMF( const FileXMF& ) = delete;
			FileXMF& operator=( const FileXMF& ) = delete;

			// load the mesh from disk
			size_t GetMeshCount(
				void
			) const;

			XMFMesh *GetMeshFromIndex(
				size_t index
			);
		};

	} // namespace Common

} // namespace XS
