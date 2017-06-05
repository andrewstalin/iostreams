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

#include "iostreams/array.h"
#include <algorithm>
#include <cassert>

namespace iostreams
{
	template<typename byte_type>
	ArrayStream<byte_type>::ArrayStream(std::vector<byte_type>&& data)
		: data_(std::move(data))
	{}

	template<typename byte_type>
	ArrayStream<byte_type>::ArrayStream(ArrayStream&& stream)
		: data_(std::move(stream.data_))
		, position_(stream.position_)
	{
		stream.position_ = 0;
	}

	template<typename byte_type>
	ArrayStream<byte_type>& ArrayStream<byte_type>::operator=(ArrayStream&& stream)
	{
		data_ = std::move(stream.data_);
		position_ = stream.position_;

		stream.position_ = 0;

		return *this;
	}

	template<typename byte_type>
	ArrayStream<byte_type> ArrayStream<byte_type>::create(const std::string& str, IFromStringTransform<byte_type>& transformer)
	{
		auto required_size = transformer.required_size(str.size());
		ArrayStream<byte_type> stream;
		stream.reserve(required_size);

		transformer.update(str.data(), str.size(), [&stream](const byte_type* data, count_type size)
		{
			stream.write(data, size);
		});

		transformer.update_final([&stream](const byte_type* data, count_type size)
		{
			stream.write(data, size);
		});

		stream.seek(0);
		return stream;
	}

	template<typename byte_type>
	std::string ArrayStream<byte_type>::to_string(IToStringTransform<byte_type>& transformer) const
	{
		std::string result;
		const auto data_size = data_.size();

		if (data_size > 0)
		{
			THROW_IF(data_size > result.max_size(), IOStreamsException(errors::STREAM_SIZE_TOO_BIG));
			result.reserve(transformer.required_size(data_size));

			transformer.update(data_.data(), data_size, [&result](const char* data, count_type size)
			{
				result.insert(result.end(), data, data + size);
			});

			transformer.update_final([&result](const char* data, count_type size)
			{
				result.insert(result.end(), data, data + size);
			});
		}

		return result;
	}

	template<typename byte_type>
	void ArrayStream<byte_type>::resize(size_type size)
	{
		data_.resize(static_cast<size_t>(size));

		if (position_ > size)
		{
			position_ = size;
		}
	}

	template<typename byte_type>
	void ArrayStream<byte_type>::reserve(size_type size)
	{
		data_.reserve(static_cast<size_t>(size));
	}

	template<typename byte_type>
	void ArrayStream<byte_type>::seek(off_type off, std::ios_base::seekdir way)
	{
		if (way == std::ios_base::cur)
		{
			off += static_cast<off_type>(position_);
		}
		else if (way == std::ios_base::end)
		{
			off += static_cast<off_type>(data_.size());
		}

		THROW_IF(off < 0 || static_cast<size_type>(off) > data_.size(), IOStreamsException(errors::OUT_OF_RANGE));
		position_ = static_cast<size_type>(off);
	}

	template<typename byte_type>
	typename ArrayStream<byte_type>::count_type ArrayStream<byte_type>::write(const byte_type* data, count_type size)
	{
		if (data_.size() < position_ + size)
		{
			data_.resize(static_cast<size_t>(position_ + size));
		}

		std::memcpy(data_.data() + position_, data, static_cast<size_t>(size));
		position_ += size;
		return size;
	}

	template<typename byte_type>
	typename ArrayStream<byte_type>::count_type ArrayStream<byte_type>::read(byte_type* buffer, count_type count)
	{
		assert(buffer != nullptr);

		count_type read_bytes{ 0 };
		const auto data_size = size();

		if (data_size > 0 && position_ < data_size)
		{
			read_bytes = std::min<count_type>(count, static_cast<count_type>(data_size - position_));
			std::memcpy(buffer, data_.data() + position_, read_bytes);
			position_ += read_bytes;
		}

		return read_bytes;
	}

	template class ArrayStream<uint8_t>;
	template class ArrayStream<char>;
}