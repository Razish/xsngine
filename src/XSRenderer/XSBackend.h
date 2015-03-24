#pragma once

namespace XS {

	class Cvar;
	struct vector4;

	namespace Renderer {

		namespace Backend {

			extern Cvar *r_zRange;

			extern GLuint defaultVao;
			extern GLuint defaultPbo;

			void Init(
				void
			);

			void Shutdown(
				void
			);

			void SetWireframe(
				bool on
			);

			bool GetWireframe(
				void
			);

			void ClearBuffer(
				bool clearColour,
				bool clearDepth,
				const vector4 &colour
			);

			void ToggleDepthTest(
				bool enabled
			);

			enum class DepthFunc {
				LessOrEqual,
			};

			void SetDepthFunction(
				DepthFunc func
			);

			void ToggleAlphaBlending(
				bool enabled
			);

			enum class BlendFunc {
				Zero,
				One,
				SourceColour,
				OneMinusSourceColour,
				DestColour,
				OneMinusDestColour,
				SourceAlpha,
				OneMinusSourceAlpha,
				DestAlpha,
				OneMinusDestAlpha,
				ConstantColour,
				OneMinusConstantColour,
				ConstantAlpha,
				OneMinusConstantAlpha,
				SourceAlphaSaturate,
			};

			void SetBlendFunction(
				BlendFunc sourceFunc,
				BlendFunc destFunc
			);

		} // namespace Backend

	} // namespace Renderer

} // namespace XS
