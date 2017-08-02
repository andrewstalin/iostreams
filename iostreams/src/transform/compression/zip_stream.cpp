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

#ifdef USE_ZLIB

#include "iostreams/transform/compression/zip_stream.h"
#include "compression_utils.h"
#include "zip.h"
#include "iostreams/error.h"
#include "babel/encoding.h"
#include <algorithm>
#include <cassert>

namespace iostreams
{
	template<typename byte_type>
	ZipStream<byte_type> ZipStream<byte_type>::create(const std::shared_ptr<stream_type>& stream, const char* entry_name, CompressionLevel level, bool add_zip64_external_info)
	{
		assert(stream != nullptr);

		auto filefunc = get_file_func<byte_type>(stream.get());
		auto zf = ::zipOpen2_64(nullptr, APPEND_STATUS_ADDINZIP, nullptr, &filefunc);
		THROW_IF(zf == nullptr, ZLibException(ZIP_ERRNO, "zipOpen2_64", ""));

		int rc{ ZIP_OK };

		if (entry_name != nullptr)
		{
#ifdef _WIN32
			auto en = babel::encode("UTF-8", "cp866", entry_name, strlen(entry_name));
			rc = ::zipOpenNewFileInZip64(zf, en.c_str(), nullptr, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, get_zlib_level(level), add_zip64_external_info);
#elif defined (__linux__) || defined (__APPLE__)
			rc = ::zipOpenNewFileInZip64(zf, entry_name, nullptr, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, get_zlib_level(level), add_zip64_external_info);
#endif
		}
		else
		{
			rc = ::zipOpenNewFileInZip64(zf, nullptr, nullptr, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, get_zlib_level(level), add_zip64_external_info);
		}

		THROW_IF(rc != ZIP_OK, ZLibException(rc, "zipOpenNewFileInZip64", ""));

		return ZipStream<byte_type>(stream, zf);
	}

	template<typename byte_type>
	void ZipStream<byte_type>::write(const byte_type* data, count_type size)
	{
		THROW_IF(is_close_, IOStreamsException(errors::STREAM_CLOSE));

		if (data != nullptr && size > 0)
		{
			auto rc = ::zipWriteInFileInZip(zip_file_, data, size);
			THROW_IF(rc != ZIP_OK, ZLibException(rc, "zipWriteInFileInZip", ""));
		}
	}

	template<typename byte_type>
	void ZipStream<byte_type>::close()
	{
		if (!is_close_)
		{
			is_close_ = true;

			if (zip_file_ != nullptr)
			{
				auto rc = ::zipCloseFileInZip(zip_file_);
				THROW_IF(rc != ZIP_OK, ZLibException(rc, "zipCloseFileInZip", ""));

				::zipClose(zip_file_, 0);
				THROW_IF(rc != ZIP_OK, ZLibException(rc, "zipClose", ""));

				zip_file_ = nullptr;
			}
		}
	}

	template class ZipStream<uint8_t>;
	template class ZipStream<char>;
}
#endif