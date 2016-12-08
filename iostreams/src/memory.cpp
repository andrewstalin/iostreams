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

#include "iostreams/memory.h"
#include "iostreams/error.h"
#include <algorithm>
#include <cassert>

namespace iostreams
{
	template<typename byte_type>
	MemoryStream<byte_type> MemoryStream<byte_type>::create(const std::string& str, IFromStringTransform<byte_type>& transformer)
	{
		auto required_size = transformer.required_size(str.size());
		auto block_size = MemoryStream::DEFAULT_BLOCK_SIZE > required_size ? required_size : MemoryStream::DEFAULT_BLOCK_SIZE;
		MemoryStream<byte_type> stream(block_size);

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
	std::string MemoryStream<byte_type>::to_string(IToStringTransform<byte_type>& transformer) const
	{
		std::string result;

		if (!blocks_.empty())
		{
			auto stream_size = size();

			if (stream_size > result.max_size())
			{
				throw IOStreamsException(errors::STREAM_SIZE_TOO_BIG);
			}

			result.reserve(transformer.required_size(static_cast<count_type>(stream_size)));
			size_type read_bytes{ 0 };

			for (auto i = 0u; i < blocks_.size() - 1; ++i)
			{
				const auto& block = blocks_[i];
				read_bytes += block.size();

				transformer.update(block.data(), block.size(), [&result](const char* data, count_type size)
				{
					result.insert(result.end(), data, data + size);
				});
			}

			const auto& block = blocks_.back();

			transformer.update(block.data(), static_cast<count_type>(stream_size - read_bytes), [&result](const char* data, count_type size)
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
	void MemoryStream<byte_type>::seek(off_type off, std::ios_base::seekdir way)
	{
		auto position = current_position();

		if (way == std::ios_base::cur)
		{
			off += position;
		}
		else if (way == std::ios_base::end)
		{
			off += size_;
		}

		THROW_IF(off < 0 || static_cast<size_type>(off) > size_, IOStreamsException(errors::OUT_OF_RANGE));
		block_index_ = static_cast<count_type>(off / block_size_);
		relative_position_ = off % block_size_;
	}

	template<typename byte_type>
	void MemoryStream<byte_type>::reserve(size_type count)
	{
		if (capacity_ < count)
		{
			auto required_size = count - capacity_;
			auto required_blocks = static_cast<count_type>(required_size / block_size_ + (required_size % block_size_ == 0 ? 0 : 1));
			blocks_.reserve(blocks_.size() + required_blocks);

			while (required_blocks > 0)
			{
				blocks_.push_back(std::vector<byte_type>(block_size_));
				--required_blocks;
			}

			capacity_ = blocks_.size() * block_size_;
		}
	}

	template<typename byte_type>
	void MemoryStream<byte_type>::resize(size_type new_size)
	{
		if (size_ > new_size)
		{
			auto required_blocks = static_cast<count_type>(new_size / block_size_ + (new_size % block_size_ == 0 ? 0 : 1));

			if (new_size == 0)
			{
				block_index_ = 0;
				relative_position_ = 0;
				capacity_ = 0;
			}
			else
			{
				auto position = current_position();

				if (position >= new_size)
				{
					block_index_ = required_blocks - 1;
					relative_position_ = block_size_ - 1;
					capacity_ = block_size_ * required_blocks;
				}
			}

			blocks_.resize(required_blocks);
		}
		else
		{
			reserve(new_size);
		}

		size_ = new_size;
	}

	template<typename byte_type>
	typename MemoryStream<byte_type>::count_type MemoryStream<byte_type>::read(byte_type* buffer, count_type count)
	{
		assert(buffer != nullptr);
		count_type read_bytes{ 0 };
		auto position = current_position();

		if (size_ > 0 && position < size_)
		{
			count = static_cast<count_type>(std::min<count_type>(count, static_cast<count_type>(size_ - position)));

			while (count)
			{
				assert(relative_position_ <= block_size_);
				auto proccesed = std::min<count_type>(count, block_size_ - relative_position_);
				std::memcpy(buffer, blocks_[block_index_].data() + relative_position_, proccesed);
				buffer += proccesed;
				count -= proccesed;
				read_bytes += proccesed;
				relative_position_ += proccesed;

				if (relative_position_ >= block_size_)
				{
					relative_position_ = 0;
					++block_index_;
				}
			}
		}

		return read_bytes;
	}

	template<typename byte_type>
	typename MemoryStream<byte_type>::count_type MemoryStream<byte_type>::write(const byte_type* data, count_type count)
	{
		assert(data != nullptr);
		count_type written_bytes{ 0 };

		if (count > 0)
		{
			reserve(count + current_position());

			while (count > 0)
			{
				assert(relative_position_ <= block_size_);
				auto proccesed = std::min<count_type>(count, block_size_ - relative_position_);
				std::copy(data, data + proccesed, blocks_[block_index_].begin() + relative_position_);
				data += proccesed;
				count -= proccesed;
				relative_position_ += proccesed;
				written_bytes += proccesed;

				if (relative_position_ >= block_size_)
				{
					relative_position_ = 0;
					++block_index_;
				}
			}

			auto position = current_position();

			if (position > size_)
			{
				size_ = position;
			}
		}

		return written_bytes;
	}

	template class MemoryStream<uint8_t>;
	template class MemoryStream<char>;
}