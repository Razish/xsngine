#pragma once

namespace XS {

	class Module {

	public:
		using Version = uint32_t;

	private:
		void *handle; // handle to the dynamic library

	public:
		using InitFunc = const char * (*)( Version );

		// don't allow default instantiation
		Module() = delete;
		Module( const Module& ) = delete;
		Module& operator=( const Module& ) = delete;

		// dynamically load a module
		Module(
			const char *name,
			Version version
		);

		~Module();
	};

} // namespace XS
