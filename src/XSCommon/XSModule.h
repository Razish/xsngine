#pragma once

namespace XS {

	class Module {
	private:
		void *handle; // handle to the dynamic library

	public:
		// don't allow default instantiation
		Module() = delete;
		Module( const Module& ) = delete;
		Module& operator=( const Module& ) = delete;

		// dynamically load a module
		Module(
			const char *name,
			uint32_t version
		);

		~Module();
	};

} // namespace XS
