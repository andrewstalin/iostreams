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

#include "iostreams/transform/compression/gzip_transform.h"
#include "compression_utils.h"
#include "iostreams/error.h"

namespace iostreams
{
	template<typename byte_type>
	GZipTransform<byte_type>::~GZipTransform()
	{
		try
		{
			if (!is_close_)
			{
				is_close_ = true;
				deflateEnd(&deflate_stream_);
				deflate_stream_ = { 0 };
			}
		}
		catch (...)
		{
		}
	}

	template<typename byte_type>
	GZipTransform<byte_type> GZipTransform<byte_type>::create()
	{
		return GZipTransform<byte_type>::create(CompressionLevel::NORMAL);
	}

	template<typename byte_type>
	GZipTransform<byte_type> GZipTransform<byte_type>::create(CompressionLevel level)
	{
		z_stream deflate_stream = { 0 };
		deflate_stream.zalloc = Z_NULL;
		deflate_stream.zfree = Z_NULL;

		auto rc = deflateInit2(&deflate_stream, get_zlib_level(level), Z_DEFLATED, GZIP_WINDOW_BITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
		THROW_IF(rc != Z_OK, ZLibException(rc, "deflateInit2", deflate_stream.msg));
		return GZipTransform(deflate_stream);
	}

	template<typename byte_type>
	void GZipTransform<byte_type>::update(const byte_type* data, size_type size, const transform_handler& handler, bool is_final)
	{
		deflate_stream_.next_in = is_final ? nullptr : reinterpret_cast<Bytef*>(const_cast<byte_type*>(data));
		deflate_stream_.avail_in = size;

		auto out_size = deflateBound(&deflate_stream_, size) + 12;

		if (out_size > buffer_.size())
		{
			buffer_.resize(out_size);
		}

		deflate_stream_.next_out = buffer_.data();
		deflate_stream_.avail_out = out_size;

		auto rc = deflate(&deflate_stream_, is_final ? Z_FINISH : Z_PARTIAL_FLUSH);
		THROW_IF((!is_final && rc != Z_OK) || (is_final && rc != Z_STREAM_END), ZLibException(rc, "deflate", deflate_stream_.msg));

		if (handler != nullptr)
		{
			handler(reinterpret_cast<byte_type*>(buffer_.data()), out_size - deflate_stream_.avail_out);
		}
	}

	template<typename byte_type>
	void GZipTransform<byte_type>::update(const byte_type* data, size_type size, const transform_handler& handler)
	{
		if (data != nullptr && size > 0)
		{
			THROW_IF(is_close_, IOStreamsException(errors::STREAM_CLOSE));
			update(data, size, handler, false);
		}
	}

	template<typename byte_type>
	void GZipTransform<byte_type>::update_final(const transform_handler& handler)
	{
		if (!is_close_)
		{
			is_close_ = true;
			update(nullptr, 0, handler, true);

			deflateEnd(&deflate_stream_);
			deflate_stream_ = { 0 };
		}
	}

	template class GZipTransform<uint8_t>;
	template class GZipTransform<char>;
}

#endif