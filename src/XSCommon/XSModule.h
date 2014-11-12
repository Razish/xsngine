#pragma once

namespace XS {

	class Module {
	private:
		void *handle;

	public:
		Module() = delete;
		Module( const Module& ) = delete;
		Module& operator=( const Module& ) = delete;

		Module( const char *name, uint32_t version );
		~Module();
	};

}
