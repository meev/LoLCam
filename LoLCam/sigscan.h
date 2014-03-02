/*
Copyright (C) 2011 by João Francisco Biondo Trinca <wolfulus@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#pragma once

#include <iomanip>
#include <iostream>
#include <string>
#include <algorithm>
#include <list>

#ifdef _WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif

///
/// Pattern scanner namespace.
///
namespace PatternScanner
{

	/// Basic type for buffers.
	typedef unsigned char	Byte;

	/// Basic type for buffer pointers.
	typedef Byte*			BytePtr;

	/// Basic size type.
	typedef size_t			Size;

	///
	/// Critical section
	///
	class CriticalSection
	{
	private:
		///
		/// Stores the critical section information
		///
#ifdef _WIN32
		CRITICAL_SECTION section;
#else
		pthread_mutex_t	section;
#endif

	public:
		///
		/// Initializes the current critical section
		///
		CriticalSection()
		{
#ifdef _WIN32
			InitializeCriticalSection(&this->section);
#else
			this->section = PTHREAD_MUTEX_INITIALIZER;
#endif
		}

		///
		/// Deletes the current critical section
		///
		~CriticalSection()
		{
#ifdef _WIN32
			DeleteCriticalSection(&this->section);
#endif
		}

		///
		/// Unlocks the current critical section
		///
		void lock()
		{
#ifdef _WIN32
			EnterCriticalSection(&this->section);
#else 
			pthread_mutex_lock(&this->section);
#endif
		}

		///
		/// Locks the current critical section
		///
		void unlock()
		{
#ifdef _WIN32
			LeaveCriticalSection(&this->section);
#else
			pthread_mutex_unlock(&this->section);
#endif
		}

	};

	///
	/// Scope locking a critical section
	///
	class ScopedLocker
	{
	private:
		///
		/// The current critical section
		///
		CriticalSection& section;

	public:
		///
		/// Creates the locker and locks the section
		///
		ScopedLocker(CriticalSection& sec)
			: section(sec)
		{
			this->section.lock();
		}

		///
		/// Releases the section locker and deletes the object
		///
		~ScopedLocker()
		{
			this->section.unlock();
		}

	};

	///
	/// Namespace containing all scanner states.
	///
	namespace States
	{

		///
		/// Base scanning interface.
		///
		class BaseState
		{
		public: 
			///
			/// Must return the size of the matched content.
			///
			virtual Size size() = 0;

			///
			/// Returns true if the current position of the buffer
			/// matches all the contents in the current state.
			///
			virtual bool execute(const BytePtr buffer) = 0;

			///
			/// Must release itself (states may contain different sizes of data).
			///
			virtual void release() = 0;

		};

		///
		/// State used to match a whole byte.
		///
		class MatchByte : public BaseState
		{
		private:
			/// Value to be matched against.
			Byte _value;

		public:
			///
			/// Creates an instance of the state.
			///
			MatchByte(Byte value)
				: _value(value)
			{
			}

			///
			/// Returns the size of a byte.
			///
			virtual Size size()
			{
				return sizeof(Byte);
			}

			///
			/// Matches against a single byte of data.
			///
			virtual bool execute(const BytePtr buffer)
			{
				if(buffer[0] == this->_value)
				{
					return true; 
				}
				return false;
			}

			///
			/// Release itself
			///
			virtual void release()
			{
				delete this;
			}

		};

		///
		/// State used to match the left nibble of a byte.
		///
		class MatchLeftNibble : public BaseState
		{
		private:
			/// Value to be matched against.
			Byte _value;

		public:
			///
			/// Creates an instance of the state.
			///
			MatchLeftNibble(Byte value)
				: _value(value)
			{
			}

			///
			/// Returns the size of a byte.
			///
			virtual Size size()
			{
				return 1;
			}

			///
			/// Matches the left nibble of the current buffer byte.
			///
			virtual bool execute(const BytePtr buffer)
			{
				if((buffer[0] & 0xF0) == (this->_value & 0xF0))
				{
					return true;
				}
				return false;
			}

			///
			/// Releases itself.
			///
			virtual void release()
			{
				delete this;
			}

		};

		///
		/// State used to match the right nibble of a byte.
		///
		class MatchRightNibble : public BaseState
		{
		private:
			/// Value to be matched against.
			Byte _value;

		public:
			///
			/// Creates an instance of the state.
			///
			MatchRightNibble(Byte value)
				: _value(value)
			{
			}

			///
			/// Returns the size of a byte.
			///
			virtual Size size()
			{
				return sizeof(Byte);
			}

			///
			/// Matches the left nibble of the current buffer byte.
			///
			virtual bool execute(const BytePtr buffer)
			{
				if((buffer[0] & 0x0F) == (this->_value & 0x0F))
				{
					return true;
				}
				return false;
			}

			///
			/// Releases itself.
			///
			virtual void release()
			{
				delete this;
			}

		};

		///
		/// State used to skip a byte (or some bytes) from the scan.
		///
		class SkipBytes : public BaseState
		{
		private:
			/// Current number of bytes to be skipped.
			Size _bytes;

		public:
			///
			/// Creates an instance of the state.
			///
			SkipBytes(Size bytes)
				: _bytes(bytes)
			{
			}

			///
			/// Returns the specified byte count.
			///
			virtual Size size()
			{
				return this->_bytes;
			}

			///
			/// Does nothing, returns always success.
			///
			virtual bool execute(const BytePtr buffer)
			{
				return true;
			}

			///
			/// Releases itself.
			///
			virtual void release()
			{
				delete this;
			}

		};

	};

	///
	/// Pattern scanner class.
	///
	class Scanner
	{
	public:
		///
		/// The current state list type.
		///
		typedef std::list<States::BaseState*>	StateList;

		///
		/// The current state list iterator type.
		///
		typedef StateList::iterator				StateListIterator;

	private:
		/// Current list of states
		StateList _states;

		/// Current pattern length
		Size _length;

		/// Current critical section
		CriticalSection	_section;

		///
		/// Converts a hexadecimal character to the specified byte value.
		///
		Byte toByte(char digit)
		{
			if(digit >= '0' && digit <= '9')
			{
				return digit - '0';
			} 
			else if(digit >= 'a' && digit <= 'f')
			{
				return digit - 'a' + 10;
			}
			else if(digit >= 'A' && digit <= 'F')
			{
				return digit - 'A' + 10;
			}
			throw std::exception("invalid hexadecimal digit");
		}

	public:
		///
		/// Creates an instance of the scanner.
		///
		Scanner()
		{
			this->reset();
		}

		///
		/// Creates an instance of the scanner and compiles the specified pattern.
		///
		Scanner(std::string pattern)
		{
			this->compile(pattern);
		}

		///
		/// Destroys the current scanner.
		///
		~Scanner()
		{
			this->reset();
		}

		///
		/// Returns the length of the compiled pattern.
		///
		Size length()
		{
			return this->_length;
		}

		///
		/// Resets the scanner states (must recompile against other patther).
		///
		void reset()
		{
			ScopedLocker locker(this->_section);
			this->_length = 0;
			std::for_each(this->_states.begin(), this->_states.end(), [this] (States::BaseState* match) {
				match->release();
			});
			this->_states.clear();
		}

		///
		/// Compiles the specified pattern
		///
		bool compile(std::string pattern)
		{
			ScopedLocker locker(this->_section);

			this->reset();

			std::remove_if(pattern.begin(), pattern.end(), isspace);
			
			int patternLength = pattern.length();
			if(patternLength  % 2 != 0)
			{
				throw std::exception("invalid pattern length");
				return false;
			}
			else if(pattern.length() == 0)
			{
				throw std::exception("empty patters are not allowed.");
				return false;
			}

			// TODO: add new pattern styles like: AA{5}BB*CC

			char byte[2] = { 0, 0 };
			for(std::string::size_type i = 0; i < pattern.length(); i += 2)
			{
				byte[0] = pattern.at(i);
				byte[1] = pattern.at(i+1);
				Byte value = 0;
				if(byte[0] != '?')
				{
					value |= this->toByte(byte[0]) << 4;
				}
				if(byte[1] != '?')
				{
					value |= this->toByte(byte[1]);
				}
				if(byte[0] == '?' && byte[1] != '?')
				{
					this->_states.push_back(new States::MatchRightNibble(value));
				}
				else if(byte[0] != '?' && byte[1] == '?')
				{
					this->_states.push_back(new States::MatchLeftNibble(value));
				}
				else if(byte[0] != '?' && byte[1] != '?')
				{
					this->_states.push_back(new States::MatchByte(value));
				}
				else
				{
					this->_states.push_back(new States::SkipBytes(1));
				}
			}

			std::for_each(this->_states.begin(), this->_states.end(), [this] (States::BaseState* match) {
				this->_length += match->size();
			});

			return true;
		}

		///
		/// Search the current compiled pattern into the specified buffer.
		///
		bool search(BytePtr buffer, Size size, Size& offset)
		{
			ScopedLocker locker(this->_section);

			BytePtr processBuffer = buffer;
			BytePtr originalBuffer = buffer;

			if(size < this->length())
			{
				return false;
			}

			if(this->_states.size() == 0)
			{
				return false;
			}

			Size len = size - this->length() + 1;
			for(Size off = 0; off < len; off++, processBuffer++)
			{
				bool error = false;
				BytePtr workingBuffer = processBuffer;
				for(StateListIterator it = this->_states.begin(); it != this->_states.end(); it++)
				{
					if(!(*it)->execute(workingBuffer))
					{
						error = true;
						break;
					}
					workingBuffer += (*it)->size();
				}
				if(!error)
				{
					offset = off;
					return true;
				}
			}

			return false;
		}

	};

}