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

#include "iostreams/file.h"
#include "iostreams/error.h"
#include <vector>
#include <cassert>

static constexpr uint16_t CHUNK_SIZE{ 4 * 1024 };

#ifdef _WIN32
#include "win_file.inl"
#elif defined (__linux__) || defined (__APPLE__)
#include "unix_file.inl"
#endif

#define CHECK_STREAM_STATE if (pimpl_ == nullptr) throw IOStreamsException(errors::STREAM_CLOSE)

template<typename byte_type>
iostreams::FileStream<byte_type>::FileStream(FileStream&& stream)
	: path_(std::move(stream.path_))
	, pimpl_(std::move(stream.pimpl_))
	, file_access_(stream.file_access_)
	, file_mode_(stream.file_mode_)
	, file_share_(stream.file_share_)
{}

template<typename byte_type>
iostreams::FileStream<byte_type>& iostreams::FileStream<byte_type>::operator=(FileStream&& stream)
{
	path_ = std::move(stream.path_);
	pimpl_ = std::move(stream.pimpl_);
	file_access_ = stream.file_access_;
	file_mode_ = stream.file_mode_;
	file_share_ = stream.file_share_;

	return *this;
}

template<typename byte_type>
iostreams::FileStream<byte_type>::FileStream(std::unique_ptr<FileImpl> pimpl, const char* path, FileAccess file_access, FileMode file_mode, FileShare file_share)
	: path_(path)
	, pimpl_(std::move(pimpl))
	, file_access_(file_access)
	, file_mode_(file_mode)
	, file_share_(file_share)
{}

template<typename byte_type>
iostreams::FileStream<byte_type>::FileStream(std::unique_ptr<FileImpl> pimpl)
	: pimpl_(std::move(pimpl))
	, file_access_(FileAccess::NONE)
	, file_mode_(FileMode::NONE)
	, file_share_(FileShare::NONE)
{}

template<typename byte_type>
iostreams::FileStream<byte_type>::~FileStream() {}

template<typename byte_type>
iostreams::FileStream<byte_type> iostreams::FileStream<byte_type>::create(FILE* file)
{
	return FileStream<byte_type>(FileImpl::create(file));
}

template<typename byte_type>
iostreams::FileStream<byte_type> iostreams::FileStream<byte_type>::open(const char* path, FileAccess file_access, FileMode file_mode, FileShare file_share, uint64_t flags)
{
	return FileStream<byte_type>(FileImpl::open(path, file_access, file_mode, file_share, flags), path, file_access, file_mode, file_share);
}

template<typename byte_type>
bool iostreams::FileStream<byte_type>::auto_flush() const
{
	CHECK_STREAM_STATE;
	return pimpl_->auto_flush();
}

template<typename byte_type>
void iostreams::FileStream<byte_type>::set_auto_flush(bool val)
{
	CHECK_STREAM_STATE;
	return pimpl_->set_auto_flush(val);
}

template<typename byte_type>
void iostreams::FileStream<byte_type>::close()
{
	if (pimpl_)
	{
		pimpl_->close();
		pimpl_ = nullptr;
	}
}

template<typename byte_type>
std::string iostreams::FileStream<byte_type>::to_string(IToStringTransform<byte_type>& transformer) const
{
	std::string result;
	auto file_size = size();

	if (file_size > 0)
	{
		if (file_size > result.max_size())
		{
			throw IOStreamsException(errors::STREAM_SIZE_TOO_BIG);
		}

		result.reserve(transformer.required_size(static_cast<count_type>(file_size)));

		auto current_position = tell();
		auto self = const_cast<FileStream<byte_type>*>(this);
		self->seek(0);

		std::vector<byte_type> buffer(CHUNK_SIZE);
		count_type bytes_read{ 0 };
		count_type i = static_cast<count_type>(file_size / CHUNK_SIZE + (file_size % CHUNK_SIZE != 0 ? 1 : 0));

		while (i-- > 0)
		{
			bytes_read = self->read(buffer.data(), buffer.size());
			transformer.update(buffer.data(), bytes_read, [&result](const char* data, count_type size)
			{
				result.insert(result.end(), data, data + size);
			});
		}

		transformer.update_final([&result](const char* data, count_type size)
		{
			result.insert(result.end(), data, data + size);
		});

		self->seek(current_position);
	}

	return result;
}

template<typename byte_type>
typename iostreams::FileStream<byte_type>::size_type iostreams::FileStream<byte_type>::size() const
{
	CHECK_STREAM_STATE;
    return pimpl_->size();
}

template<typename byte_type>
typename iostreams::FileStream<byte_type>::size_type iostreams::FileStream<byte_type>::tell() const
{
	CHECK_STREAM_STATE;
    return pimpl_->tell();
}

template<typename byte_type>
void iostreams::FileStream<byte_type>::seek(off_type off, std::ios_base::seekdir way)
{
	CHECK_STREAM_STATE;
    return pimpl_->seek(off, way);
}

template<typename byte_type>
void iostreams::FileStream<byte_type>::resize(size_type size)
{
	CHECK_STREAM_STATE;
    pimpl_->resize(size);
}

template<typename byte_type>
typename iostreams::FileStream<byte_type>::count_type iostreams::FileStream<byte_type>::read(byte_type* buffer, count_type count)
{
	CHECK_STREAM_STATE;
    return pimpl_->read(buffer, count);
}

template<typename byte_type>
typename iostreams::FileStream<byte_type>::count_type iostreams::FileStream<byte_type>::write(const byte_type* data, count_type size)
{
	CHECK_STREAM_STATE;
    return pimpl_->write(data, size);
}

template<typename byte_type>
void iostreams::FileStream<byte_type>::check_seek_range(off_type off, std::ios_base::seekdir way, size_type current_position, size_type size)
{
	if (way == std::ios_base::cur)
	{
		off += current_position;
	}
	else if (way == std::ios_base::end)
	{
		off += size;
	}

	THROW_IF(off < 0 || static_cast<size_type>(off) > size, IOStreamsException(errors::OUT_OF_RANGE));
}

template class iostreams::FileStream<uint8_t>;
template class iostreams::FileStream<char>;