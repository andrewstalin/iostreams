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

#ifndef _IOSTREAMS_GZIP_TRANSAFORM_H_
#define _IOSTREAMS_GZIP_TRANSAFORM_H_

#ifdef USE_ZLIB
#include "iostreams/transform/transform.h"
#include "iostreams/transform/compression/compression_level.h"
#include "zlib.h"
#include <vector>

namespace iostreams
{
	//todo: gzip header
	template<typename byte_type>
	class GZipTransform : public ITransform<byte_type, byte_type>
	{
	private:
		static constexpr size_t BUFFER_SIZE{ 4 * 1024 };

		z_stream deflate_stream_;
		std::vector<Bytef> buffer_;
		bool is_close_{ false };

	public:
		using base_type = ITransform<byte_type, byte_type>;
		using size_type = typename base_type::size_type;
		using transform_handler = typename base_type::TransformHandler;

		GZipTransform(GZipTransform&& val)
			: deflate_stream_(val.deflate_stream_)
			, buffer_(std::move(val.buffer_))
			, is_close_(val.is_close_)
		{
			val.deflate_stream_ = { 0 };
			val.is_close_ = true;
		}

		GZipTransform(const GZipTransform&) = delete;
		GZipTransform& operator=(const GZipTransform&) = delete;

		~GZipTransform();

		static GZipTransform create();
		static GZipTransform create(CompressionLevel level);

		void update(const byte_type* data, size_type size, const transform_handler& handler) override;
		void update_final(const transform_handler& handler) override;

	private:
		GZipTransform(z_stream deflate_stream)
			: deflate_stream_(deflate_stream)
		{}

		void update(const byte_type* data, size_type size, const transform_handler& handler, bool is_final);
	};
}
#endif
#endif