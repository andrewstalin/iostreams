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

#include "iostreams/transform/string_transform/hex.h"
#include "iostreams/error.h"
#include <cassert>

static constexpr char LOWERCASE_HEX_MAP[]{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
//static constexpr char UPPERCASE_HEX_MAP[]{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

static constexpr char SPACE_CHAR{ ' ' };

inline uint8_t HexToDigit(char ch)
{
    switch (ch)
    {
        case '0': return 0x0;
        case '1': return 0x1;
        case '2': return 0x2;
        case '3': return 0x3;
        case '4': return 0x4;
        case '5': return 0x5;
        case '6': return 0x6;
        case '7': return 0x7;
        case '8': return 0x8;
        case '9': return 0x9;
        case 'a': return 0xa;
        case 'A': return 0xa;
        case 'b': return 0xb;
        case 'B': return 0xb;
        case 'c': return 0xc;
        case 'C': return 0xc;
        case 'd': return 0xd;
        case 'D': return 0xd;
        case 'e': return 0xe;
        case 'E': return 0xe;
        case 'f': return 0xf;
        case 'F': return 0xf;
		default: throw iostreams::IOStreamsException(iostreams::errors::BAD_HEX_CHARACTER);
    }
}

namespace iostreams
{
	template<typename byte_type>
	void ToHexTransform<byte_type>::update(const byte_type* data, size_type size, const transform_handler& handler)
	{
		if (data != nullptr && size > 0)
		{
			while (size-- > 0)
			{
				if (this->buffer_index_ + 2 > this->buffer_.size())
				{
					this->flush(handler);
				}

				this->buffer_[this->buffer_index_++] = LOWERCASE_HEX_MAP[(*data >> 4) & 0x0F];
				this->buffer_[this->buffer_index_++] = LOWERCASE_HEX_MAP[*data++ & 0x0F];
			}
		}
	}

	template<typename byte_type>
	void ToHexTransform<byte_type>::update_final(const transform_handler& handler)
	{
		this->flush(handler);
	}

	template<typename byte_type>
	typename FromHexTransform<byte_type>::size_type FromHexTransform<byte_type>::required_size(size_type size) const
	{ 
		//THROW_IF(size % 2 != 0, IOStreamsException(errors::BAD_HEX_STRING_LENGTH));
		return size / 2; 
	}

	template<typename byte_type>
	void FromHexTransform<byte_type>::update(const char* data, size_type size, const transform_handler& handler)
	{
		if (data != nullptr && size > 0)
		{
			auto get_char = [&data, &size]() -> char
			{
				if (size == 0)
				{
					return SPACE_CHAR;
				}

				char byte{ *data++ };
				--size;

				while (::isspace(byte) && size > 0)
				{
					byte = *data++;
					--size;
				}

				return byte;
			};

			if (ch_ != 0)
			{
				auto byte = get_char();

				if (byte == SPACE_CHAR)
				{
					return;
				}

				if (this->buffer_index_ + 1 > this->buffer_.size())
				{
					this->flush(handler);
				}

				this->buffer_[this->buffer_index_++] = (HexToDigit(ch_) << 4) | HexToDigit(byte);
				ch_ = 0;
			}

			while (size >= 2)
			{
				auto byte1 = get_char();

				if (byte1 == SPACE_CHAR)
				{
					break;
				}

				auto byte2 = get_char();

				if (byte2 == SPACE_CHAR)
				{
					if (ch_ != 0)
					{
						byte2 = ch_;
						ch_ = 0;
					}
					else
					{
						ch_ = byte1;
						break;
					}
				}

				if (this->buffer_index_ + 1 > this->buffer_.size())
				{
					this->flush(handler);
				}

				this->buffer_[this->buffer_index_++] = (HexToDigit(byte1) << 4) | HexToDigit(byte2);
			}

			if (size != 0)
			{
				auto byte = get_char();
				ch_ = byte == SPACE_CHAR ? 0 : byte;
			}
		}
	}

	template<typename byte_type>
	void FromHexTransform<byte_type>::update_final(const transform_handler& handler)
	{
		this->flush(handler);
		ch_ = 0;
	}

	template class ToHexTransform<uint8_t>;
	template class ToHexTransform<char>;

	template class FromHexTransform<uint8_t>;
	template class FromHexTransform<char>;
}