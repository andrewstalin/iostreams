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

#ifndef _IOSTREAMS_TRANSFORM_BASE64_H_
#define _IOSTREAMS_TRANSFORM_BASE64_H_

#include "iostreams/transform/string_transform/string_transform.h"
#include <string>

namespace iostreams
{
	template<typename byte_type>
	class FromBase64Transform : public IFromStringTransform<byte_type>
	{
	private:
		uint8_t i_{ 0 };
		char block4_[4];

	public:
		using base_type = IFromStringTransform<byte_type>;
		using value_type = byte_type;
		using size_type = typename base_type::size_type;
		using transform_handler = typename base_type::TransformHandler;

		FromBase64Transform()
			: base_type()
		{}

		FromBase64Transform(size_t buffer_size)
			: base_type(buffer_size)
		{}

		FromBase64Transform(const FromBase64Transform&) = default;
		FromBase64Transform& operator=(const FromBase64Transform&) = default;

		size_type required_size(size_type size) const override;

		void update(const char* data, size_type size, const transform_handler& handler) override;
		void update_final(const transform_handler& handler) override;
	};

	template<typename byte_type>
	class ToBase64Transform : public IToStringTransform<byte_type>
	{
	private:
		//todo: InsertLineBreaks
		uint8_t i_{ 0 };
		uint8_t block3_[3];

	public:
		using base_type = IToStringTransform<byte_type>;
		using value_type = byte_type;
		using size_type = typename base_type::size_type;
		using transform_handler = typename base_type::TransformHandler;

		ToBase64Transform()
			: base_type()
		{}

		ToBase64Transform(size_t buffer_size)
			: base_type(buffer_size)
		{}

		ToBase64Transform(const ToBase64Transform&) = default;
		ToBase64Transform& operator=(const ToBase64Transform&) = default;

		size_type required_size(size_type size) const override { return size / 3 * 4 + (size % 3 != 0 ? 4 : 0); }

		void update(const byte_type* data, size_type size, const transform_handler& handler) override;
		void update_final(const transform_handler& handler) override;
	};
}
#endif
