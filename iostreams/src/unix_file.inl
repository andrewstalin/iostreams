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

#if defined (__linux__) || defined (__APPLE__)

#include "iostreams/file.h"
#include "liberror/exception.h"
#include <unistd.h>
#include <fcntl.h>

#ifdef __MACH__
	#define lseek64 lseek
	#define ftruncate64 ftruncate
#endif

namespace iostreams
{
	template<typename byte_type>
	class FileStream<byte_type>::FileImpl
	{
	private:
		int fd_{ -1 };
		bool auto_flush_{ false };

	public:
		using size_type = typename FileStream<byte_type>::size_type;
		using count_type = typename FileStream<byte_type>::count_type;
		using off_type = typename FileStream<byte_type>::off_type;

		FileImpl(int hd)
			: fd_(hd)
		{}

		FileImpl(FileImpl&& stream)
			: fd_(stream.fd_)
			, auto_flush_(stream.auto_flush_)
		{
			stream.fd_ = -1;
		}

		FileImpl& operator=(FileImpl&& stream)
		{
			fd_ = stream.fd_;
			auto_flush_ = stream.auto_flush_;
			stream.fd_ = -1;
			return *this;
		}

		FileImpl(const FileImpl&) = delete;
		FileImpl& operator=(const FileImpl&) = delete;

		~FileImpl()
		{
			close();
		}

		static std::unique_ptr<FileImpl> create(FILE* file)
		{
			auto fd = ::fileno(file);
			THROW_IF(fd == -1, POSIX_ERROR("fileno"));
			return std::make_unique<FileImpl>(fd);
		}

		static std::unique_ptr<FileImpl> open(const char* path, FileAccess file_access, FileMode file_mode, FileShare file_share, uint64_t flags)
		{
#if defined (__linux__)
			if (UINTPTR_MAX == 0xFFFFFFFFFFFFFFFF)
			{
				flags |= O_LARGEFILE;
			}
#endif

			switch (file_access)
			{
			case FileAccess::READ:
				flags |= O_RDONLY;
				break;
			case FileAccess::WRITE:
				flags |= O_WRONLY;
				break;
			case FileAccess::READ_WRITE:
				flags |= O_RDWR;
				break;
			case FileAccess::NONE:
				throw IOStreamsException(errors::BAD_FILE_ACCESS_VALUE);
			}

			switch (file_mode)
			{
			case FileMode::F_CREATE_ALWAYS:
				flags |= O_CREAT | O_TRUNC;
				break;
			case FileMode::F_CREATE_NEW:
				flags |= O_CREAT | O_EXCL;
				break;
			case FileMode::F_OPEN_ALWAYS:
				flags |= O_CREAT;
				break;
			case FileMode::F_OPEN_EXISTING:
				break;
			case FileMode::F_TRUNCATE_EXISTING:
				flags |= O_TRUNC;
				break;
			case FileMode::NONE:
				throw IOStreamsException(errors::BAD_FILE_MODE_VALUE);
			}

			int open_mode = static_cast<int>(file_mode);

			if (file_share == FileShare::NONE 
				&& (file_access == FileAccess::READ_WRITE || file_access == FileAccess::WRITE))
			{
				open_mode = 0644;
			}

			auto fd = file_share == FileShare::NONE 
				? ::open(path, static_cast<int>(flags)) 
				: ::open(path, static_cast<int>(flags), open_mode);
			THROW_IF(fd == -1, POSIX_ERROR("open"));
			return std::make_unique<FileImpl>(fd);
		}

		bool auto_flush() const { return auto_flush_; }
		void set_auto_flush(bool val) { auto_flush_ = val; }

		void close()
		{
			if (fd_ != -1)
			{
				::close(fd_);
				fd_ = -1;
			}
		}

		size_type size() const
		{
			struct stat stat_buffer;
			THROW_IF(::fstat(fd_, &stat_buffer) != 0, POSIX_ERROR("fstat"));
			return static_cast<size_type>(stat_buffer.st_size);
		}

		size_type tell() const
		{
			return static_cast<size_type>(::lseek64(fd_, 0, SEEK_CUR));
		}

		void seek(off_type off, std::ios_base::seekdir way = std::ios_base::beg)
		{
			FileStream<byte_type>::check_seek_range(off, way, tell(), size());
			THROW_IF(::lseek64(fd_, off, way) == -1, POSIX_ERROR("lseek64"));
		}

		void resize(size_type new_size)
		{
			auto file_size = size();

			if (file_size != new_size)
			{
				if (file_size > new_size)
				{
					THROW_IF(::ftruncate64(fd_, new_size) != 0, POSIX_ERROR("ftruncate64"));
					THROW_IF(::lseek64(fd_, 0, std::ios_base::end) == -1, POSIX_ERROR("lseek64"));
				}
				else
				{
					auto current_position = tell();
					THROW_IF(::lseek64(fd_, new_size - 1, std::ios_base::beg) == -1, POSIX_ERROR("lseek64"));

					byte_type byte{0};
					write(&byte, 1);

					THROW_IF(::lseek64(fd_, current_position, std::ios_base::beg) == -1, POSIX_ERROR("lseek64"));
				}
			}
		}

		count_type read(byte_type* buffer, count_type count)
		{
			count_type read_bytes{ 0 };
			ssize_t ret;

			while (count != 0 && (ret = ::read(fd_, buffer, count)) != 0)
			{
				if (ret == -1)
				{
					THROW_IF(errno != EINTR, POSIX_ERROR("read"));
				}
				else
				{
					count -= ret;
					read_bytes += ret;
					buffer += ret;
				}
			}

			return read_bytes;
		}

		count_type write(const byte_type* data, count_type count)
		{
			count_type written_bytes{ 0 };
			ssize_t ret;

			while (count != 0 && (ret = ::write(fd_, data, count)) != 0)
			{
				if (ret == -1)
				{
					THROW_IF(errno != EINTR, POSIX_ERROR("write"));
				}
				else
				{
					written_bytes += ret;
					count -= ret;
					data += ret;
				}
			}

			if (auto_flush_)
			{
				THROW_IF(::fsync(fd_) == -1, POSIX_ERROR("fsync"));
			}

			return written_bytes;
		}
	};
}
#endif