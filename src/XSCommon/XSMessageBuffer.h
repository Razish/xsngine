#pragma once

#include <vector>
#include <string>

namespace XS {

	class Logger;

	class MessageBuffer {

	public:
		using Message = std::string;

	private:
		using Container = std::vector<Message>;

		Container	 buffer;
		Logger		*log;

	public:
		// don't allow default instantiation
		MessageBuffer() = delete;
		MessageBuffer( const MessageBuffer& ) = delete;
		MessageBuffer& operator=( const MessageBuffer& ) = delete;

		MessageBuffer(
			const char *logfile
		);

		~MessageBuffer();

		class iterator : public Container::iterator {

		public:
			iterator( Container::iterator c )
				: Container::iterator( c )
			{
			}

			Message &operator*( void ) {
				return Container::iterator::operator *();
			}

		};

		iterator begin( void ) {
			return iterator( buffer.begin() );
		}

		iterator end( void ) {
			return iterator( buffer.end() );
		}

		size_t size( void ) const {
			return buffer.size();
		}

		// append a message to the buffer
		void Append(
			const Message &message
		);

		// check if the buffer is empty
		bool IsEmpty(
			void
		) const XS_WARN_UNUSED_RESULT;

		// clear the message buffer
		void Clear(
			void
		);

	};

} // namespace XS
