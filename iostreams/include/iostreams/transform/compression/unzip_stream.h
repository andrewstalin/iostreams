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

#ifndef _IOSTREAMS_UNZIP_STREAM_H_
#define _IOSTREAMS_UNZIP_STREAM_H_

#ifdef USE_ZLIB
#include "iostreams/transform/compression/compression_level.h"
#include "iostreams/stream.h"
#include "contrib/minizip/unzip.h"
#include <memory>

namespace iostreams
{
	struct ZipArchiveEntry
	{
		std::string name;
		uint64_t compressed_size{ 0 };
		uint64_t uncompressed_size{ 0 };
		uint64_t index{ 0 };
		bool is_directory{ false };
	};

	template<typename byte_type>
	class UnZipStream
	{
	public:
		using stream_type = IStream<byte_type>;
		using count_type = typename stream_type::count_type;

	private:
		struct UnZipFileGuard
		{
			unzFile val;

			UnZipFileGuard(unzFile val)
				: val(val)
			{}

			UnZipFileGuard(const UnZipFileGuard&) = delete;
			UnZipFileGuard& operator=(const UnZipFileGuard&) = delete;

			UnZipFileGuard(UnZipFileGuard&& guard)
				: val(guard.val)
			{
				guard.val = nullptr;
			}

			UnZipFileGuard& operator=(UnZipFileGuard&& guard)
			{
				val = guard.val;
				guard.val = nullptr;
				return *this;
			}

			~UnZipFileGuard()
			{
				try
				{
					close();
				}
				catch(...)
				{ }
			}

			void close();
		};

		std::shared_ptr<stream_type> stream_;
		UnZipFileGuard unzip_file_guard_;
		bool is_close_{ false };

	public:
		UnZipStream(const UnZipStream&) = delete;
		UnZipStream& operator=(const UnZipStream&) = delete;

		UnZipStream(UnZipStream&& val) noexcept
			: stream_(std::move(val.stream_))
			, unzip_file_guard_(std::move(val.unzip_file_guard_))
			, is_close_(val.is_close_)
		{
			val.is_close_ = true;
		}

		UnZipStream& operator=(UnZipStream&& val) noexcept
		{
			stream_ = std::move(val.stream_);
			unzip_file_guard_ = std::move(val.unzip_file_guard_);
			is_close_ = val.is_close_;

			val.is_close_ = true;
			return *this;
		}

		~UnZipStream()
		{
			try
			{
				close();
			}
			catch (...)
			{}
		}

		static UnZipStream create(const std::shared_ptr<stream_type>& stream, const char* entry_name);
		static UnZipStream create(const std::shared_ptr<stream_type>& stream, uint64_t entry_index);

		static std::vector<ZipArchiveEntry> get_entries_info(const std::shared_ptr<stream_type>& stream);

		ZipArchiveEntry get_entry_info() const;

		count_type read(byte_type* buffer, count_type size);
		
		void close();

	private:
		UnZipStream(const std::shared_ptr<stream_type>& stream, unzFile unzip_file)
			: stream_(stream), unzip_file_guard_(unzip_file)
		{}
	};
}
#endif
#endif