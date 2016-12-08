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

#ifndef _IOSTREAMS_TRANSFORM_HEX_H_
#define _IOSTREAMS_TRANSFORM_HEX_H_

#include "iostreams/transform/string_transform/string_transform.h"
#include <string>

namespace iostreams
{
	//todo: hex string format
	template<typename byte_type>
	class ToHexTransform : public IToStringTransform<byte_type>
	{
	public:
		using base_type = IToStringTransform<byte_type>;
		using value_type = byte_type;
		using size_type = typename base_type::size_type;
		using transform_handler = typename base_type::TransformHandler;

		ToHexTransform()
			: base_type()
		{}

		ToHexTransform(size_t buffer_size)
			: base_type(buffer_size)
		{}

		ToHexTransform(const ToHexTransform&) = default;
		ToHexTransform& operator=(const ToHexTransform&) = default;

		size_type required_size(size_type size) const override { return size * 2; }

		void update(const byte_type* data, size_type size, const transform_handler& handler) override;
		void update_final(const transform_handler& handler) override;
	};

	template<typename byte_type>
	class FromHexTransform : public IFromStringTransform<byte_type>
	{
	private:
		char ch_{ 0 };

	public:
		using base_type = IFromStringTransform<byte_type>;
		using value_type = byte_type;
		using size_type = typename base_type::size_type;
		using transform_handler = typename base_type::TransformHandler;

		FromHexTransform()
			: base_type()
		{}

		FromHexTransform(size_t buffer_size)
			: base_type(buffer_size)
		{}

		FromHexTransform(const FromHexTransform&) = default;
		FromHexTransform& operator=(const FromHexTransform&) = default;

		size_type required_size(size_type size) const override;

		void update(const char* data, size_type size, const transform_handler& handler) override;
		void update_final(const transform_handler& handler) override;
	};
}
#endif