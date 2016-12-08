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

#include "iostreams/transform/compression/ungzip_transform.h"
#include "compression_utils.h"
#include "iostreams/error.h"
#include <cassert>

namespace iostreams
{
	template<typename byte_type>
	UnGZipTransform<byte_type>::~UnGZipTransform()
	{
		try
		{
			if (!is_close_)
			{
				close();
			}
		}
		catch (...)
		{
		}
	}

	template<typename byte_type>
	UnGZipTransform<byte_type> UnGZipTransform<byte_type>::create()
	{
		z_stream inflate_stream = { 0 };
		inflate_stream.zalloc = Z_NULL;
		inflate_stream.zfree = Z_NULL;

		auto rc = inflateInit2(&inflate_stream, GZIP_WINDOW_BITS);
		THROW_IF(rc != Z_OK, ZLibException(rc, "inflateInit2", inflate_stream.msg));
		return UnGZipTransform(inflate_stream);
	}

	template<typename byte_type>
	void UnGZipTransform<byte_type>::update(const byte_type* data, size_type size, const transform_handler& handler)
	{
		if (data != nullptr && size > 0)
		{
			THROW_IF(is_close_, IOStreamsException(errors::STREAM_CLOSE));

			inflate_stream_.next_in = reinterpret_cast<Bytef*>(const_cast<byte_type*>(data));;
			inflate_stream_.avail_in = size;

			while (inflate_stream_.avail_in != 0)
			{
				inflate_stream_.next_out = reinterpret_cast<Bytef*>(const_cast<byte_type*>(buffer_));
				inflate_stream_.avail_out = UnGZipTransform::BUFFER_SIZE;
				auto rc = inflate(&inflate_stream_, Z_SYNC_FLUSH);

				if (rc == Z_OK || rc == Z_STREAM_END)
				{
					handler(buffer_, UnGZipTransform::BUFFER_SIZE - inflate_stream_.avail_out);

					if (rc == Z_STREAM_END)
					{
						break;
					}
				}
				else
				{
					throw ZLibException(rc, "inflate", inflate_stream_.msg);
				}
			}
		}
	}

	template<typename byte_type>
	void UnGZipTransform<byte_type>::update_final(const transform_handler& handler)
	{
		close();
	}

	template<typename byte_type>
	void UnGZipTransform<byte_type>::close()
	{
		is_close_ = true;
		inflateEnd(&inflate_stream_);
		inflate_stream_ = { 0 };
	}

	template class UnGZipTransform<uint8_t>;
	template class UnGZipTransform<char>;
}

#endif