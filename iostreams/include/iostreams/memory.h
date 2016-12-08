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

#ifndef _IOSTREAMS_MEMORY_H_
#define _IOSTREAMS_MEMORY_H_

#include "iostreams/stream.h"
#include <vector>

namespace iostreams
{
	template<typename byte_type>
	class MemoryStream : public IStream<byte_type>
	{
	public:
		using size_type = typename IStream<byte_type>::size_type;
		using count_type = typename IStream<byte_type>::count_type;
		using off_type = typename IStream<byte_type>::off_type;

	private:
		count_type block_size_;
		mutable count_type block_index_{ 0 };

		count_type relative_position_{ 0 };
		size_type size_{ 0 };
		size_type capacity_{ 0 };
		std::vector<std::vector<byte_type>> blocks_;

	public:
		static constexpr count_type DEFAULT_BLOCK_SIZE{ 1024 * 1024 };

		MemoryStream()
			: block_size_(MemoryStream::DEFAULT_BLOCK_SIZE)
		{}

		explicit MemoryStream(count_type block_size)
			: block_size_(block_size != 0 ? block_size : MemoryStream::DEFAULT_BLOCK_SIZE)
		{}

		explicit MemoryStream(std::vector<byte_type>&& block)
			: block_size_(!block.empty() ? block.size() : MemoryStream::DEFAULT_BLOCK_SIZE), size_(block.size()), capacity_(block.size())
		{
			blocks_.push_back(std::forward<std::vector<byte_type>>(block));
		}

		MemoryStream(MemoryStream&& stream)
			: block_size_(stream.block_size_)
			, block_index_(stream.block_index_)
			, relative_position_(stream.relative_position_)
			, size_(stream.size_)
			, capacity_(stream.capacity_)
			, blocks_(std::move(stream.blocks_))
		{
			stream.block_index_ = 0;
			stream.relative_position_ = 0;
			stream.size_ = 0;
			stream.capacity_ = 0;
		}

		MemoryStream& operator=(MemoryStream&& stream)
		{
			block_size_ = stream.block_size_;
			block_index_ = stream.block_index_;
			relative_position_ = stream.relative_position_;
			size_ = stream.size_;
			capacity_ = stream.capacity_;
			blocks_ = std::move(stream.blocks_);

			stream.block_index_ = 0;
			stream.relative_position_ = 0;
			stream.size_ = 0;
			stream.capacity_ = 0;

			return *this;
		}

		MemoryStream(const MemoryStream&) = default;
		MemoryStream& operator=(const MemoryStream&) = default;

		~MemoryStream() {}

		static MemoryStream create(const std::string& str, IFromStringTransform<byte_type>& transformer);

		size_type capacity() const { return capacity_;  }
		count_type block_size() const { return block_size_; }
		size_type size() const override { return size_; }

		size_type tell() const override
		{
			return current_position();
		}

		std::string to_string(IToStringTransform<byte_type>& transformer) const override;
		void seek(off_type off, std::ios_base::seekdir way = std::ios_base::beg) override;
		void resize(size_type new_size) override;
		void reserve(size_type count);
		count_type read(byte_type* buffer, count_type count) override;
		count_type write(const byte_type* data, count_type count) override;

	private:
		inline size_type current_position() const
		{
			return static_cast<size_type>(block_index_ * block_size_ + relative_position_);
		}
	};
}

#endif