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

#ifndef _IOSTREAMS_ZIP_STREAM_H_
#define _IOSTREAMS_ZIP_STREAM_H_

#ifdef USE_ZLIB
#include "iostreams/transform/compression/compression_level.h"
#include "iostreams/stream.h"
#include "contrib/minizip/zip.h"
#include <memory>

namespace iostreams
{
	template<typename byte_type>
	class ZipStream
	{
	public:
		using stream_type = IStream<byte_type>;
		using count_type = typename stream_type::count_type;

	private:
		std::shared_ptr<stream_type> stream_;
		zipFile zip_file_;
		bool is_close_{ false };

	public:
		ZipStream(const ZipStream&) = delete;
		ZipStream& operator=(const ZipStream&) = delete;

		ZipStream(ZipStream&& val) noexcept
			: stream_(std::move(val.stream_))
			, zip_file_(val.zip_file_)
			, is_close_(val.is_close_)
		{
			val.zip_file_ = nullptr;
			val.is_close_ = true;
		}

		ZipStream& operator=(ZipStream&& val) noexcept
		{
			stream_ = std::move(val.stream_);
			zip_file_ = val.zip_file_;
			is_close_ = val.is_close_;

			val.zip_file_ = nullptr;
			val.is_close_ = true;
			
			return *this;
		}

		~ZipStream()
		{
			try
			{
				close();
			}
			catch (...)
			{}
		}

		static ZipStream create(const std::shared_ptr<stream_type>& stream)
		{
			return ZipStream::create(stream, nullptr, CompressionLevel::NORMAL, true);
		}

		static ZipStream create(const std::shared_ptr<stream_type>& stream, const std::string& entry_name)
		{
			return ZipStream::create(stream, entry_name.c_str(), CompressionLevel::NORMAL, true);
		}

		static ZipStream create(const std::shared_ptr<stream_type>& stream, CompressionLevel level)
		{
			return ZipStream::create(stream, nullptr, level, true);
		}

		static ZipStream create(const std::shared_ptr<stream_type>& stream, const char* entry_name, CompressionLevel level, bool add_zip64_external_info);

		void write(const byte_type* data, count_type size);
		void close();

	private:
		ZipStream(const std::shared_ptr<stream_type>& stream, zipFile zip_file)
			: stream_(stream), zip_file_(zip_file)
		{}
	};
}
#endif
#endif