// Licensed under the MIT License <http://opensource.org/licenses/MIT>
// Author: Andrew Stalin <andrew.stalin@gmail.com>
//
// THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
// IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
// FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
// AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
// LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef _IOSTREAMS_STREAM_H_
#define _IOSTREAMS_STREAM_H_

#include <cstdint>
#include <ios>
#include <string>
#include <vector>
#include "iostreams/transform/string_transform/string_transform.h"
#include "iostreams/error.h"

namespace iostreams
{
	template<typename byte_type>
	struct IStream
	{
		static_assert(std::is_same<char, byte_type>::value || std::is_same<uint8_t, byte_type>::value, "byte_type must be char or uint8_t");

		using size_type = uint64_t;
		using count_type = size_t;
		using off_type = typename std::char_traits<byte_type>::off_type;

		virtual ~IStream() {};
		
		virtual std::string to_string(IToStringTransform<byte_type>& transformer) const = 0;
		virtual size_type size() const = 0;
		virtual size_type tell() const = 0;
		virtual void seek(off_type off, std::ios_base::seekdir way = std::ios_base::beg) = 0;
		virtual void resize(size_type size) = 0;
		virtual count_type write(const byte_type* data, count_type size) = 0;
		virtual count_type read(byte_type* buffer, count_type count) = 0;

		count_type read(off_type off, byte_type* buffer, count_type count)
		{
			seek(off);
			return read(buffer, count);
		}

		count_type write(off_type off, const byte_type* data, count_type size)
		{
			seek(off);
			return write(data, size);
		}

		template<typename collection_type>
		collection_type read_all()
		{
			static_assert(std::is_same<std::basic_string<byte_type>, collection_type>::value 
				|| std::is_same<std::vector<byte_type>, collection_type>::value, "collection_type must be std::string or std::vector");

			seek(0);
			auto stream_size = size();
			collection_type buffer;

			if (stream_size > 0)
			{
				if (buffer.max_size() < stream_size)
				{
					throw IOStreamsException(errors::STREAM_SIZE_TOO_BIG);
				}

				auto buffer_size = static_cast<typename collection_type::size_type>(stream_size);
				buffer.resize(buffer_size);
				read((byte_type*)buffer.data(), buffer_size);
			}

			return buffer;
		}
	};

	template<typename byte_type>
	void transform(IStream<byte_type>* source_stream, IStream<byte_type>* destination_stream, ITransform<byte_type, byte_type>& transformer);
}
#endif