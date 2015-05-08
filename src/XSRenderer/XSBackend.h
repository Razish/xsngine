#pragma once

namespace XS {

	class Cvar;
	struct vector4;

	namespace Renderer {

		namespace Backend {

			extern Cvar *r_zRange;
			extern Cvar *r_wireframe;

			extern GLuint defaultVao;
			extern GLuint defaultPbo;

			void Init(
				void
			);

			void Shutdown(
				void
			);

			void ToggleWireframe(
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

			void ToggleDepthWrite(
				bool enabled
			);

			enum class DepthFunc {
				LessOrEqual,
			};

			void SetDepthFunction(
				DepthFunc func
			);


			void ToggleStencilTest(
				bool enabled
			);

			enum class StencilFunc {
				Never,
				Always,
				Equal,
				NotEqual,
				Less,
				LessOrEqual,
				GreaterOrEqual,
				Greater,
			};

			void SetStencilFunction(
				StencilFunc func
			);

			enum class StencilOp {
				Keep,
				Zero,
				Replace,
				Increment,
				IncrementWrap,
				Decrement,
				Invert,
			};

			void SetStencilOp(
				StencilOp op
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
