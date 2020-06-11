#pragma once

////////////////////////////////////////////////////////////////////////////////
// The MIT License (MIT)
//
// Copyright (c) 2017 Nicholas Frechette & Animation Compression Library contributors
// Copyright (c) 2018 Nicholas Frechette & Realtime Math contributors
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#include "rtm/math.h"
#include "rtm/scalarf.h"
#include "rtm/impl/compiler_utils.h"
#include "rtm/impl/memory_utils.h"
#include "rtm/impl/vector_common.h"

RTM_IMPL_FILE_PRAGMA_PUSH

namespace rtm
{
	//////////////////////////////////////////////////////////////////////////
	// Setters, getters, and casts
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// Loads an unaligned vector4 from memory.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_load(const float* input) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return _mm_loadu_ps(input);
#elif defined(RTM_NEON_INTRINSICS)
		return vld1q_f32(input);
#else
		return vector_set(input[0], input[1], input[2], input[3]);
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Loads an input scalar from memory into the [x] component and sets the [yzw] components to zero.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_load1(const float* input) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return _mm_load_ss(input);
#elif defined(RTM_NEON_INTRINSICS)
		return vld1q_lane_f32(input, vdupq_n_f32(0.0F), 0);
#else
		return vector_set(input[0], 0.0F, 0.0F, 0.0F);
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Loads an unaligned vector2 from memory and sets the [zw] components to zero.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_load2(const float* input) RTM_NO_EXCEPT
	{
		return vector_set(input[0], input[1], 0.0F, 0.0F);
	}

	//////////////////////////////////////////////////////////////////////////
	// Loads an unaligned vector3 from memory and sets the [w] component to zero.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_load3(const float* input) RTM_NO_EXCEPT
	{
		return vector_set(input[0], input[1], input[2], 0.0F);
	}

	//////////////////////////////////////////////////////////////////////////
	// Loads an unaligned vector4 from memory.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_load(const float4f* input) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return _mm_loadu_ps(&input->x);
#elif defined(RTM_NEON_INTRINSICS)
		return vld1q_f32(&input->x);
#else
		return vector_set(input->x, input->y, input->z, input->w);
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Loads an unaligned vector2 from memory and sets the [zw] components to zero.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_load2(const float2f* input) RTM_NO_EXCEPT
	{
		return vector_set(input->x, input->y, 0.0F, 0.0F);
	}

	//////////////////////////////////////////////////////////////////////////
	// Loads an unaligned vector3 from memory and sets the [w] component to zero.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_load3(const float3f* input) RTM_NO_EXCEPT
	{
		return vector_set(input->x, input->y, input->z, 0.0F);
	}

	//////////////////////////////////////////////////////////////////////////
	// Loads an input scalar from memory into the [xyzw] components.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_broadcast(const float* input) RTM_NO_EXCEPT
	{
#if defined(RTM_AVX_INTRINSICS)
		return _mm_broadcast_ss(input);
#elif defined(RTM_SSE2_INTRINSICS)
		return _mm_load_ps1(input);
#elif defined(RTM_NEON_INTRINSICS)
		return vld1q_dup_f32(input);
#else
		return vector_set(*input);
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Casts a quaternion to a vector4.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL quat_to_vector(quatf_arg0 input) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS) || defined(RTM_NEON_INTRINSICS)
		return input;
#else
		return vector4f{ input.x, input.y, input.z, input.w };
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Casts a vector4 float64 variant to a float32 variant.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_cast(const vector4d& input) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return _mm_shuffle_ps(_mm_cvtpd_ps(input.xy), _mm_cvtpd_ps(input.zw), _MM_SHUFFLE(1, 0, 1, 0));
#else
		return vector_set(float(input.x), float(input.y), float(input.z), float(input.w));
#endif
	}

	namespace rtm_impl
	{
		//////////////////////////////////////////////////////////////////////////
		// This is a helper struct to allow a single consistent API between
		// various vector types when the semantics are identical but the return
		// type differs. Implicit coercion is used to return the desired value
		// at the call site.
		//////////////////////////////////////////////////////////////////////////
		struct vector4f_vector_get_x
		{
			inline RTM_SIMD_CALL operator float() const RTM_NO_EXCEPT
			{
#if defined(RTM_SSE2_INTRINSICS)
				return _mm_cvtss_f32(input);
#elif defined(RTM_NEON_INTRINSICS)
				return vgetq_lane_f32(input, 0);
#else
				return input.x;
#endif
			}

#if defined(RTM_SSE2_INTRINSICS)
			inline RTM_SIMD_CALL operator scalarf() const RTM_NO_EXCEPT
			{
				return scalarf{ input };
			}
#endif

			vector4f input;
		};
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns the vector4 [x] component.
	//////////////////////////////////////////////////////////////////////////
	constexpr rtm_impl::vector4f_vector_get_x RTM_SIMD_CALL vector_get_x(vector4f_arg0 input) RTM_NO_EXCEPT
	{
		return rtm_impl::vector4f_vector_get_x{ input };
	}

	namespace rtm_impl
	{
		//////////////////////////////////////////////////////////////////////////
		// This is a helper struct to allow a single consistent API between
		// various vector types when the semantics are identical but the return
		// type differs. Implicit coercion is used to return the desired value
		// at the call site.
		//////////////////////////////////////////////////////////////////////////
		struct vector4f_vector_get_y
		{
			inline RTM_SIMD_CALL operator float() const RTM_NO_EXCEPT
			{
#if defined(RTM_SSE2_INTRINSICS)
				return _mm_cvtss_f32(_mm_shuffle_ps(input, input, _MM_SHUFFLE(1, 1, 1, 1)));
#elif defined(RTM_NEON_INTRINSICS)
				return vgetq_lane_f32(input, 1);
#else
				return input.y;
#endif
			}

#if defined(RTM_SSE2_INTRINSICS)
			inline RTM_SIMD_CALL operator scalarf() const RTM_NO_EXCEPT
			{
				return scalarf{ _mm_shuffle_ps(input, input, _MM_SHUFFLE(1, 1, 1, 1)) };
			}
#endif

			vector4f input;
		};
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns the vector4 [y] component.
	//////////////////////////////////////////////////////////////////////////
	constexpr rtm_impl::vector4f_vector_get_y RTM_SIMD_CALL vector_get_y(vector4f_arg0 input) RTM_NO_EXCEPT
	{
		return rtm_impl::vector4f_vector_get_y{ input };
	}

	namespace rtm_impl
	{
		//////////////////////////////////////////////////////////////////////////
		// This is a helper struct to allow a single consistent API between
		// various vector types when the semantics are identical but the return
		// type differs. Implicit coercion is used to return the desired value
		// at the call site.
		//////////////////////////////////////////////////////////////////////////
		struct vector4f_vector_get_z
		{
			inline RTM_SIMD_CALL operator float() const RTM_NO_EXCEPT
			{
#if defined(RTM_SSE2_INTRINSICS)
				return _mm_cvtss_f32(_mm_shuffle_ps(input, input, _MM_SHUFFLE(2, 2, 2, 2)));
#elif defined(RTM_NEON_INTRINSICS)
				return vgetq_lane_f32(input, 2);
#else
				return input.z;
#endif
			}

#if defined(RTM_SSE2_INTRINSICS)
			inline RTM_SIMD_CALL operator scalarf() const RTM_NO_EXCEPT
			{
				return scalarf{ _mm_shuffle_ps(input, input, _MM_SHUFFLE(2, 2, 2, 2)) };
			}
#endif

			vector4f input;
		};
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns the vector4 [z] component.
	//////////////////////////////////////////////////////////////////////////
	constexpr rtm_impl::vector4f_vector_get_z RTM_SIMD_CALL vector_get_z(vector4f_arg0 input) RTM_NO_EXCEPT
	{
		return rtm_impl::vector4f_vector_get_z{ input };
	}

	namespace rtm_impl
	{
		//////////////////////////////////////////////////////////////////////////
		// This is a helper struct to allow a single consistent API between
		// various vector types when the semantics are identical but the return
		// type differs. Implicit coercion is used to return the desired value
		// at the call site.
		//////////////////////////////////////////////////////////////////////////
		struct vector4f_vector_get_w
		{
			inline RTM_SIMD_CALL operator float() const RTM_NO_EXCEPT
			{
#if defined(RTM_SSE2_INTRINSICS)
				return _mm_cvtss_f32(_mm_shuffle_ps(input, input, _MM_SHUFFLE(3, 3, 3, 3)));
#elif defined(RTM_NEON_INTRINSICS)
				return vgetq_lane_f32(input, 3);
#else
				return input.w;
#endif
			}

#if defined(RTM_SSE2_INTRINSICS)
			inline RTM_SIMD_CALL operator scalarf() const RTM_NO_EXCEPT
			{
				return scalarf{ _mm_shuffle_ps(input, input, _MM_SHUFFLE(3, 3, 3, 3)) };
			}
#endif

			vector4f input;
		};
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns the vector4 [w] component.
	//////////////////////////////////////////////////////////////////////////
	constexpr rtm_impl::vector4f_vector_get_w RTM_SIMD_CALL vector_get_w(vector4f_arg0 input) RTM_NO_EXCEPT
	{
		return rtm_impl::vector4f_vector_get_w{ input };
	}

	namespace rtm_impl
	{
		//////////////////////////////////////////////////////////////////////////
		// This is a helper struct to allow a single consistent API between
		// various vector types when the semantics are identical but the return
		// type differs. Implicit coercion is used to return the desired value
		// at the call site.
		//////////////////////////////////////////////////////////////////////////
		template<mix4 component>
		struct vector4f_vector_get_component_static
		{
			inline RTM_SIMD_CALL operator float() const RTM_NO_EXCEPT
			{
				const mix4 xyzw = mix4(int(component) % 4);
				if (rtm_impl::static_condition<xyzw == mix4::x>::test())
					return vector_get_x(input);
				else if (rtm_impl::static_condition<xyzw == mix4::y>::test())
					return vector_get_y(input);
				else if (rtm_impl::static_condition<xyzw == mix4::z>::test())
					return vector_get_z(input);
				else
					return vector_get_w(input);
			}

#if defined(RTM_SSE2_INTRINSICS)
			inline RTM_SIMD_CALL operator scalarf() const RTM_NO_EXCEPT
			{
				const mix4 xyzw = mix4(int(component) % 4);
				if (rtm_impl::static_condition<xyzw == mix4::x>::test())
					return vector_get_x(input);
				else if (rtm_impl::static_condition<xyzw == mix4::y>::test())
					return vector_get_y(input);
				else if (rtm_impl::static_condition<xyzw == mix4::z>::test())
					return vector_get_z(input);
				else
					return vector_get_w(input);
			}
#endif

			vector4f input;
		};
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns the vector4 desired component.
	//////////////////////////////////////////////////////////////////////////
	template<mix4 component>
	constexpr rtm_impl::vector4f_vector_get_component_static<component> RTM_SIMD_CALL vector_get_component(vector4f_arg0 input) RTM_NO_EXCEPT
	{
		return rtm_impl::vector4f_vector_get_component_static<component>{ input };
	}

	namespace rtm_impl
	{
		//////////////////////////////////////////////////////////////////////////
		// This is a helper struct to allow a single consistent API between
		// various vector types when the semantics are identical but the return
		// type differs. Implicit coercion is used to return the desired value
		// at the call site.
		//////////////////////////////////////////////////////////////////////////
		struct vector4f_vector_get_component
		{
			inline RTM_SIMD_CALL operator float() const RTM_NO_EXCEPT
			{
				const mix4 xyzw = mix4(int(component) % 4);
				if (xyzw == mix4::x)
					return vector_get_x(input);
				else if (xyzw == mix4::y)
					return vector_get_y(input);
				else if (xyzw == mix4::z)
					return vector_get_z(input);
				else
					return vector_get_w(input);
			}

#if defined(RTM_SSE2_INTRINSICS)
			inline RTM_SIMD_CALL operator scalarf() const RTM_NO_EXCEPT
			{
				const mix4 xyzw = mix4(int(component) % 4);
				if (xyzw == mix4::x)
					return vector_get_x(input);
				else if (xyzw == mix4::y)
					return vector_get_y(input);
				else if (xyzw == mix4::z)
					return vector_get_z(input);
				else
					return vector_get_w(input);
			}
#endif

			vector4f input;
			mix4 component;
			int padding[3];
		};
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns the vector4 desired component.
	//////////////////////////////////////////////////////////////////////////
	constexpr rtm_impl::vector4f_vector_get_component RTM_SIMD_CALL vector_get_component(vector4f_arg0 input, mix4 component) RTM_NO_EXCEPT
	{
		return rtm_impl::vector4f_vector_get_component{ input, component, { 0 } };
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns the smallest component in the input vector as a scalar.
	//////////////////////////////////////////////////////////////////////////
	constexpr rtm_impl::vector4f_get_min_component RTM_SIMD_CALL vector_get_min_component(vector4f_arg0 input) RTM_NO_EXCEPT
	{
		return rtm_impl::vector4f_get_min_component{ input };
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns the largest component in the input vector as a scalar.
	//////////////////////////////////////////////////////////////////////////
	constexpr rtm_impl::vector4f_get_max_component RTM_SIMD_CALL vector_get_max_component(vector4f_arg0 input) RTM_NO_EXCEPT
	{
		return rtm_impl::vector4f_get_max_component{ input };
	}

	//////////////////////////////////////////////////////////////////////////
	// Sets the vector4 [x] component and returns the new value.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_set_x(vector4f_arg0 input, float lane_value) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return _mm_move_ss(input, _mm_set_ss(lane_value));
#elif defined(RTM_NEON_INTRINSICS)
		return vsetq_lane_f32(lane_value, input, 0);
#else
		return vector4f{ lane_value, input.y, input.z, input.w };
#endif
	}

#if defined(RTM_SSE2_INTRINSICS)
	//////////////////////////////////////////////////////////////////////////
	// Sets the vector4 [x] component and returns the new value.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_set_x(vector4f_arg0 input, scalarf_arg1 lane_value) RTM_NO_EXCEPT
	{
		return _mm_move_ss(input, lane_value.value);
	}
#endif

	//////////////////////////////////////////////////////////////////////////
	// Sets the vector4 [y] component and returns the new value.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_set_y(vector4f_arg0 input, float lane_value) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE4_INTRINSICS)
		return _mm_insert_ps(input, _mm_set_ss(lane_value), 0x10);
#elif defined(RTM_SSE2_INTRINSICS)
		const __m128 yxzw = _mm_shuffle_ps(input, input, _MM_SHUFFLE(3, 2, 0, 1));
		const __m128 vxzw = _mm_move_ss(yxzw, _mm_set_ss(lane_value));
		return _mm_shuffle_ps(vxzw, vxzw, _MM_SHUFFLE(3, 2, 0, 1));
#elif defined(RTM_NEON_INTRINSICS)
		return vsetq_lane_f32(lane_value, input, 1);
#else
		return vector4f{ input.x, lane_value, input.z, input.w };
#endif
	}

#if defined(RTM_SSE2_INTRINSICS)
	//////////////////////////////////////////////////////////////////////////
	// Sets the vector4 [y] component and returns the new value.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_set_y(vector4f_arg0 input, scalarf_arg1 lane_value) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE4_INTRINSICS)
		return _mm_insert_ps(input, lane_value.value, 0x10);
#else
		const __m128 yxzw = _mm_shuffle_ps(input, input, _MM_SHUFFLE(3, 2, 0, 1));
		const __m128 vxzw = _mm_move_ss(yxzw, lane_value.value);
		return _mm_shuffle_ps(vxzw, vxzw, _MM_SHUFFLE(3, 2, 0, 1));
#endif
	}
#endif

	//////////////////////////////////////////////////////////////////////////
	// Sets the vector4 [z] component and returns the new value.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_set_z(vector4f_arg0 input, float lane_value) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE4_INTRINSICS)
		return _mm_insert_ps(input, _mm_set_ss(lane_value), 0x20);
#elif defined(RTM_SSE2_INTRINSICS)
		const __m128 zyxw = _mm_shuffle_ps(input, input, _MM_SHUFFLE(3, 0, 1, 2));
		const __m128 vyxw = _mm_move_ss(zyxw, _mm_set_ss(lane_value));
		return _mm_shuffle_ps(vyxw, vyxw, _MM_SHUFFLE(3, 0, 1, 2));
#elif defined(RTM_NEON_INTRINSICS)
		return vsetq_lane_f32(lane_value, input, 2);
#else
		return vector4f{ input.x, input.y, lane_value, input.w };
#endif
	}

#if defined(RTM_SSE2_INTRINSICS)
	//////////////////////////////////////////////////////////////////////////
	// Sets the vector4 [z] component and returns the new value.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_set_z(vector4f_arg0 input, scalarf_arg1 lane_value) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE4_INTRINSICS)
		return _mm_insert_ps(input, lane_value.value, 0x20);
#else
		const __m128 yxzw = _mm_shuffle_ps(input, input, _MM_SHUFFLE(3, 0, 1, 2));
		const __m128 vxzw = _mm_move_ss(yxzw, lane_value.value);
		return _mm_shuffle_ps(vxzw, vxzw, _MM_SHUFFLE(3, 0, 1, 2));
#endif
	}
#endif

	//////////////////////////////////////////////////////////////////////////
	// Sets the vector4 [w] component and returns the new value.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_set_w(vector4f_arg0 input, float lane_value) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE4_INTRINSICS)
		return _mm_insert_ps(input, _mm_set_ss(lane_value), 0x30);
#elif defined(RTM_SSE2_INTRINSICS)
		const __m128 wyzx = _mm_shuffle_ps(input, input, _MM_SHUFFLE(0, 2, 1, 3));
		const __m128 vyzx = _mm_move_ss(wyzx, _mm_set_ss(lane_value));
		return _mm_shuffle_ps(vyzx, vyzx, _MM_SHUFFLE(0, 2, 1, 3));
#elif defined(RTM_NEON_INTRINSICS)
		return vsetq_lane_f32(lane_value, input, 3);
#else
		return vector4f{ input.x, input.y, input.z, lane_value };
#endif
	}

#if defined(RTM_SSE2_INTRINSICS)
	//////////////////////////////////////////////////////////////////////////
	// Sets the vector4 [w] component and returns the new value.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_set_w(vector4f_arg0 input, scalarf_arg1 lane_value) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE4_INTRINSICS)
		return _mm_insert_ps(input, lane_value.value, 0x30);
#else
		const __m128 yxzw = _mm_shuffle_ps(input, input, _MM_SHUFFLE(0, 2, 1, 3));
		const __m128 vxzw = _mm_move_ss(yxzw, lane_value.value);
		return _mm_shuffle_ps(vxzw, vxzw, _MM_SHUFFLE(0, 2, 1, 3));
#endif
	}
#endif

	//////////////////////////////////////////////////////////////////////////
	// Returns a floating point pointer to the vector4 data.
	//////////////////////////////////////////////////////////////////////////
	inline const float* RTM_SIMD_CALL vector_to_pointer(const vector4f& input) RTM_NO_EXCEPT
	{
		return reinterpret_cast<const float*>(&input);
	}

	//////////////////////////////////////////////////////////////////////////
	// Writes a vector4 to unaligned memory.
	//////////////////////////////////////////////////////////////////////////
	inline void RTM_SIMD_CALL vector_store(vector4f_arg0 input, float* output) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		_mm_storeu_ps(output, input);
#else
		output[0] = vector_get_x(input);
		output[1] = vector_get_y(input);
		output[2] = vector_get_z(input);
		output[3] = vector_get_w(input);
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Writes a vector1 to unaligned memory.
	//////////////////////////////////////////////////////////////////////////
	inline void RTM_SIMD_CALL vector_store1(vector4f_arg0 input, float* output) RTM_NO_EXCEPT
	{
		output[0] = vector_get_x(input);
	}

	//////////////////////////////////////////////////////////////////////////
	// Writes a vector2 to unaligned memory.
	//////////////////////////////////////////////////////////////////////////
	inline void RTM_SIMD_CALL vector_store2(vector4f_arg0 input, float* output) RTM_NO_EXCEPT
	{
		output[0] = vector_get_x(input);
		output[1] = vector_get_y(input);
	}

	//////////////////////////////////////////////////////////////////////////
	// Writes a vector3 to unaligned memory.
	//////////////////////////////////////////////////////////////////////////
	inline void RTM_SIMD_CALL vector_store3(vector4f_arg0 input, float* output) RTM_NO_EXCEPT
	{
		output[0] = vector_get_x(input);
		output[1] = vector_get_y(input);
		output[2] = vector_get_z(input);
	}

	//////////////////////////////////////////////////////////////////////////
	// Writes a vector4 to unaligned memory.
	//////////////////////////////////////////////////////////////////////////
	inline void RTM_SIMD_CALL vector_store(vector4f_arg0 input, uint8_t* output) RTM_NO_EXCEPT
	{
		std::memcpy(output, &input, sizeof(vector4f));
	}

	//////////////////////////////////////////////////////////////////////////
	// Writes a vector1 to unaligned memory.
	//////////////////////////////////////////////////////////////////////////
	inline void RTM_SIMD_CALL vector_store1(vector4f_arg0 input, uint8_t* output) RTM_NO_EXCEPT
	{
		std::memcpy(output, &input, sizeof(float) * 1);
	}

	//////////////////////////////////////////////////////////////////////////
	// Writes a vector2 to unaligned memory.
	//////////////////////////////////////////////////////////////////////////
	inline void RTM_SIMD_CALL vector_store2(vector4f_arg0 input, uint8_t* output) RTM_NO_EXCEPT
	{
		std::memcpy(output, &input, sizeof(float) * 2);
	}

	//////////////////////////////////////////////////////////////////////////
	// Writes a vector3 to unaligned memory.
	//////////////////////////////////////////////////////////////////////////
	inline void RTM_SIMD_CALL vector_store3(vector4f_arg0 input, uint8_t* output) RTM_NO_EXCEPT
	{
		std::memcpy(output, &input, sizeof(float) * 3);
	}

	//////////////////////////////////////////////////////////////////////////
	// Writes a vector4 to unaligned memory.
	//////////////////////////////////////////////////////////////////////////
	inline void RTM_SIMD_CALL vector_store(vector4f_arg0 input, float4f* output) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		_mm_storeu_ps(&output->x, input);
#else
		output->x = vector_get_x(input);
		output->y = vector_get_y(input);
		output->z = vector_get_z(input);
		output->w = vector_get_w(input);
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Writes a vector2 to unaligned memory.
	//////////////////////////////////////////////////////////////////////////
	inline void RTM_SIMD_CALL vector_store2(vector4f_arg0 input, float2f* output) RTM_NO_EXCEPT
	{
		output->x = vector_get_x(input);
		output->y = vector_get_y(input);
	}

	//////////////////////////////////////////////////////////////////////////
	// Writes a vector3 to unaligned memory.
	//////////////////////////////////////////////////////////////////////////
	inline void RTM_SIMD_CALL vector_store3(vector4f_arg0 input, float3f* output) RTM_NO_EXCEPT
	{
		output->x = vector_get_x(input);
		output->y = vector_get_y(input);
		output->z = vector_get_z(input);
	}



	//////////////////////////////////////////////////////////////////////////
	// Arithmetic
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// Per component addition of the two inputs: lhs + rhs
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_add(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return _mm_add_ps(lhs, rhs);
#elif defined(RTM_NEON_INTRINSICS)
		return vaddq_f32(lhs, rhs);
#else
		return vector_set(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Per component subtraction of the two inputs: lhs - rhs
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_sub(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return _mm_sub_ps(lhs, rhs);
#elif defined(RTM_NEON_INTRINSICS)
		return vsubq_f32(lhs, rhs);
#else
		return vector_set(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Per component multiplication of the two inputs: lhs * rhs
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_mul(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return _mm_mul_ps(lhs, rhs);
#elif defined(RTM_NEON_INTRINSICS)
		return vmulq_f32(lhs, rhs);
#else
		return vector_set(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Per component multiplication of the vector by a scalar: lhs * rhs
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_mul(vector4f_arg0 lhs, float rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_NEON_INTRINSICS)
		return vmulq_n_f32(lhs, rhs);
#else
		return vector_mul(lhs, vector_set(rhs));
#endif
	}

#if defined(RTM_SSE2_INTRINSICS)
	//////////////////////////////////////////////////////////////////////////
	// Per component multiplication of the vector by a scalar: lhs * rhs
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_mul(vector4f_arg0 lhs, scalarf_arg1 rhs) RTM_NO_EXCEPT
	{
		return _mm_mul_ps(lhs, _mm_shuffle_ps(rhs.value, rhs.value, _MM_SHUFFLE(0, 0, 0, 0)));
	}
#endif

	//////////////////////////////////////////////////////////////////////////
	// Per component division of the two inputs: lhs / rhs
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_div(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return _mm_div_ps(lhs, rhs);
#elif defined (RTM_NEON64_INTRINSICS)
		return vdivq_f32(lhs, rhs);
#elif defined(RTM_NEON_INTRINSICS)
		// Perform two passes of Newton-Raphson iteration on the hardware estimate
		float32x4_t x0 = vrecpeq_f32(rhs);

		// First iteration
		float32x4_t x1 = vmulq_f32(x0, vrecpsq_f32(x0, rhs));

		// Second iteration
		float32x4_t x2 = vmulq_f32(x1, vrecpsq_f32(x1, rhs));
		return vmulq_f32(lhs, x2);
#else
		return vector_set(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w);
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Per component maximum of the two inputs: max(lhs, rhs)
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_max(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return _mm_max_ps(lhs, rhs);
#elif defined(RTM_NEON_INTRINSICS)
		return vmaxq_f32(lhs, rhs);
#else
		return vector_set(scalar_max(lhs.x, rhs.x), scalar_max(lhs.y, rhs.y), scalar_max(lhs.z, rhs.z), scalar_max(lhs.w, rhs.w));
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Per component minimum of the two inputs: min(lhs, rhs)
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_min(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return _mm_min_ps(lhs, rhs);
#elif defined(RTM_NEON_INTRINSICS)
		return vminq_f32(lhs, rhs);
#else
		return vector_set(scalar_min(lhs.x, rhs.x), scalar_min(lhs.y, rhs.y), scalar_min(lhs.z, rhs.z), scalar_min(lhs.w, rhs.w));
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Per component clamping of an input between a minimum and a maximum value: min(max_value, max(min_value, input))
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_clamp(vector4f_arg0 input, vector4f_arg1 min_value, vector4f_arg2 max_value) RTM_NO_EXCEPT
	{
		return vector_min(max_value, vector_max(min_value, input));
	}

	//////////////////////////////////////////////////////////////////////////
	// Per component absolute of the input: abs(input)
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_abs(vector4f_arg0 input) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		const __m128i abs_mask = _mm_set_epi32(0x7FFFFFFFULL, 0x7FFFFFFFULL, 0x7FFFFFFFULL, 0x7FFFFFFFULL);
		return _mm_and_ps(input, _mm_castsi128_ps(abs_mask));
#elif defined(RTM_NEON_INTRINSICS)
		return vabsq_f32(input);
#else
		return vector_set(scalar_abs(input.x), scalar_abs(input.y), scalar_abs(input.z), scalar_abs(input.w));
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Per component negation of the input: -input
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_neg(vector4f_arg0 input) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		constexpr __m128 signs = { -0.0F, -0.0F, -0.0F, -0.0F };
		return _mm_xor_ps(input, signs);
#elif defined(RTM_NEON_INTRINSICS)
		return vnegq_f32(input);
#else
		return vector_mul(input, -1.0f);
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Per component reciprocal of the input: 1.0 / input
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_reciprocal(vector4f_arg0 input) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		// Perform two passes of Newton-Raphson iteration on the hardware estimate
		__m128 x0 = _mm_rcp_ps(input);

		// First iteration
		__m128 x1 = _mm_sub_ps(_mm_add_ps(x0, x0), _mm_mul_ps(input, _mm_mul_ps(x0, x0)));

		// Second iteration
		__m128 x2 = _mm_sub_ps(_mm_add_ps(x1, x1), _mm_mul_ps(input, _mm_mul_ps(x1, x1)));
		return x2;
#elif defined(RTM_NEON_INTRINSICS)
		// Perform two passes of Newton-Raphson iteration on the hardware estimate
		float32x4_t x0 = vrecpeq_f32(input);

		// First iteration
		float32x4_t x1 = vmulq_f32(x0, vrecpsq_f32(x0, input));

		// Second iteration
		float32x4_t x2 = vmulq_f32(x1, vrecpsq_f32(x1, input));
		return x2;
#else
		return vector_div(vector_set(1.0F), input);
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Per component returns the smallest integer value not less than the input.
	// vector_ceil([1.8, 1.0, -1.8, -1.0]) = [2.0, 1.0, -1.0, -1.0]
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_ceil(vector4f_arg0 input) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE4_INTRINSICS)
		return _mm_ceil_ps(input);
#elif defined(RTM_SSE2_INTRINSICS)
		// NaN, +- Infinity, and numbers larger or equal to 2^23 remain unchanged
		// since they have no fractional part.

		const __m128i abs_mask = _mm_set_epi32(0x7FFFFFFFULL, 0x7FFFFFFFULL, 0x7FFFFFFFULL, 0x7FFFFFFFULL);
		const __m128 fractional_limit = _mm_set_ps1(8388608.0F); // 2^23

		// Build our mask, larger values that have no fractional part, and infinities will be true
		// Smaller values and NaN will be false
		__m128 abs_input = _mm_and_ps(input, _mm_castsi128_ps(abs_mask));
		__m128 is_input_large = _mm_cmpge_ps(abs_input, fractional_limit);

		// Test if our input is NaN with (value != value), it is only true for NaN
		__m128 is_nan = _mm_cmpneq_ps(input, input);

		// Combine our masks to determine if we should return the original value
		__m128 use_original_input = _mm_or_ps(is_input_large, is_nan);

		// Convert to an integer and back. This does banker's rounding by default
		__m128 integer_part = _mm_cvtepi32_ps(_mm_cvtps_epi32(input));

		// Test if the returned value is smaller than the original.
		// A positive input will round towards zero and be lower when we need it to be greater.
		__m128 is_positive = _mm_cmplt_ps(integer_part, input);

		// Convert our mask to a float, ~0 yields -1.0 since it is a valid signed integer
		// Negative values will yield a 0.0 bias
		__m128 bias = _mm_cvtepi32_ps(_mm_castps_si128(is_positive));

		// Subtract our bias to properly handle positive values
		integer_part = _mm_sub_ps(integer_part, bias);

		return _mm_or_ps(_mm_and_ps(use_original_input, input), _mm_andnot_ps(use_original_input, integer_part));
#else
		return vector_set(scalar_ceil(vector_get_x(input)), scalar_ceil(vector_get_y(input)), scalar_ceil(vector_get_z(input)), scalar_ceil(vector_get_w(input)));
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Per component returns the largest integer value not greater than the input.
	// vector_floor([1.8, 1.0, -1.8, -1.0]) = [1.0, 1.0, -2.0, -1.0]
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_floor(vector4f_arg0 input) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE4_INTRINSICS)
		return _mm_floor_ps(input);
#elif defined(RTM_SSE2_INTRINSICS)
		// NaN, +- Infinity, and numbers larger or equal to 2^23 remain unchanged
		// since they have no fractional part.

		const __m128i abs_mask = _mm_set_epi32(0x7FFFFFFFULL, 0x7FFFFFFFULL, 0x7FFFFFFFULL, 0x7FFFFFFFULL);
		const __m128 fractional_limit = _mm_set_ps1(8388608.0F); // 2^23

		// Build our mask, larger values that have no fractional part, and infinities will be true
		// Smaller values and NaN will be false
		__m128 abs_input = _mm_and_ps(input, _mm_castsi128_ps(abs_mask));
		__m128 is_input_large = _mm_cmpge_ps(abs_input, fractional_limit);

		// Test if our input is NaN with (value != value), it is only true for NaN
		__m128 is_nan = _mm_cmpneq_ps(input, input);

		// Combine our masks to determine if we should return the original value
		__m128 use_original_input = _mm_or_ps(is_input_large, is_nan);

		// Convert to an integer and back. This does banker's rounding by default
		__m128 integer_part = _mm_cvtepi32_ps(_mm_cvtps_epi32(input));

		// Test if the returned value is greater than the original.
		// A negative input will round towards zero and be greater when we need it to be smaller.
		__m128 is_negative = _mm_cmpgt_ps(integer_part, input);

		// Convert our mask to a float, ~0 yields -1.0 since it is a valid signed integer
		// Positive values will yield a 0.0 bias
		__m128 bias = _mm_cvtepi32_ps(_mm_castps_si128(is_negative));

		// Add our bias to properly handle negative values
		integer_part = _mm_add_ps(integer_part, bias);

		return _mm_or_ps(_mm_and_ps(use_original_input, input), _mm_andnot_ps(use_original_input, integer_part));
#else
		return vector_set(scalar_floor(vector_get_x(input)), scalar_floor(vector_get_y(input)), scalar_floor(vector_get_z(input)), scalar_floor(vector_get_w(input)));
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// 3D cross product: lhs x rhs
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_cross3(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		// cross(a, b).zxy = (a * b.yzx) - (a.yzx * b)
		__m128 lhs_yzx = _mm_shuffle_ps(lhs, lhs, _MM_SHUFFLE(3, 0, 2, 1));
		__m128 rhs_yzx = _mm_shuffle_ps(rhs, rhs, _MM_SHUFFLE(3, 0, 2, 1));
		__m128 tmp_zxy = _mm_sub_ps(_mm_mul_ps(lhs, rhs_yzx), _mm_mul_ps(lhs_yzx, rhs));

		// cross(a, b) = ((a * b.yzx) - (a.yzx * b)).yzx
		return _mm_shuffle_ps(tmp_zxy, tmp_zxy, _MM_SHUFFLE(3, 0, 2, 1));
#else
		// cross(a, b) = (a.yzx * b.zxy) - (a.zxy * b.yzx)
		const float lhs_x = vector_get_x(lhs);
		const float lhs_y = vector_get_y(lhs);
		const float lhs_z = vector_get_z(lhs);
		const float rhs_x = vector_get_x(rhs);
		const float rhs_y = vector_get_y(rhs);
		const float rhs_z = vector_get_z(rhs);
		return vector_set((lhs_y * rhs_z) - (lhs_z * rhs_y), (lhs_z * rhs_x) - (lhs_x * rhs_z), (lhs_x * rhs_y) - (lhs_y * rhs_x));
#endif
	}

	namespace rtm_impl
	{
		//////////////////////////////////////////////////////////////////////////
		// This is a helper struct to allow a single consistent API between
		// various vector types when the semantics are identical but the return
		// type differs. Implicit coercion is used to return the desired value
		// at the call site.
		//////////////////////////////////////////////////////////////////////////
		struct vector4f_vector_dot
		{
			inline RTM_SIMD_CALL operator float() const RTM_NO_EXCEPT
			{
#if defined(RTM_SSE4_INTRINSICS) && 0
				// SSE4 dot product instruction isn't precise enough
				return _mm_cvtss_f32(_mm_dp_ps(lhs, rhs, 0xFF));
#elif defined(RTM_SSE2_INTRINSICS)
				__m128 x2_y2_z2_w2 = _mm_mul_ps(lhs, rhs);
				__m128 z2_w2_0_0 = _mm_shuffle_ps(x2_y2_z2_w2, x2_y2_z2_w2, _MM_SHUFFLE(0, 0, 3, 2));
				__m128 x2z2_y2w2_0_0 = _mm_add_ps(x2_y2_z2_w2, z2_w2_0_0);
				__m128 y2w2_0_0_0 = _mm_shuffle_ps(x2z2_y2w2_0_0, x2z2_y2w2_0_0, _MM_SHUFFLE(0, 0, 0, 1));
				__m128 x2y2z2w2_0_0_0 = _mm_add_ps(x2z2_y2w2_0_0, y2w2_0_0_0);
				return _mm_cvtss_f32(x2y2z2w2_0_0_0);
#else
				return (vector_get_x(lhs) * vector_get_x(rhs)) + (vector_get_y(lhs) * vector_get_y(rhs)) + (vector_get_z(lhs) * vector_get_z(rhs)) + (vector_get_w(lhs) * vector_get_w(rhs));
#endif
			}

#if defined(RTM_SSE2_INTRINSICS)
			inline RTM_SIMD_CALL operator scalarf() const RTM_NO_EXCEPT
			{
#if defined(RTM_SSE4_INTRINSICS) && 0
				// SSE4 dot product instruction isn't precise enough
				return scalarf{ _mm_cvtss_f32(_mm_dp_ps(lhs, rhs, 0xFF)) };
#else
				__m128 x2_y2_z2_w2 = _mm_mul_ps(lhs, rhs);
				__m128 z2_w2_0_0 = _mm_shuffle_ps(x2_y2_z2_w2, x2_y2_z2_w2, _MM_SHUFFLE(0, 0, 3, 2));
				__m128 x2z2_y2w2_0_0 = _mm_add_ps(x2_y2_z2_w2, z2_w2_0_0);
				__m128 y2w2_0_0_0 = _mm_shuffle_ps(x2z2_y2w2_0_0, x2z2_y2w2_0_0, _MM_SHUFFLE(0, 0, 0, 1));
				__m128 x2y2z2w2_0_0_0 = _mm_add_ps(x2z2_y2w2_0_0, y2w2_0_0_0);
				return scalarf{ x2y2z2w2_0_0_0 };
#endif
			}
#endif

			inline RTM_SIMD_CALL operator vector4f() const RTM_NO_EXCEPT
			{
#if defined(RTM_SSE4_INTRINSICS) && 0
				// SSE4 dot product instruction isn't precise enough
				return _mm_dp_ps(lhs, rhs, 0xFF);
#elif defined(RTM_SSE2_INTRINSICS)
				__m128 x2_y2_z2_w2 = _mm_mul_ps(lhs, rhs);
				__m128 z2_w2_0_0 = _mm_shuffle_ps(x2_y2_z2_w2, x2_y2_z2_w2, _MM_SHUFFLE(0, 0, 3, 2));
				__m128 x2z2_y2w2_0_0 = _mm_add_ps(x2_y2_z2_w2, z2_w2_0_0);
				__m128 y2w2_0_0_0 = _mm_shuffle_ps(x2z2_y2w2_0_0, x2z2_y2w2_0_0, _MM_SHUFFLE(0, 0, 0, 1));
				__m128 x2y2z2w2_0_0_0 = _mm_add_ps(x2z2_y2w2_0_0, y2w2_0_0_0);
				return _mm_shuffle_ps(x2y2z2w2_0_0_0, x2y2z2w2_0_0_0, _MM_SHUFFLE(0, 0, 0, 0));
#elif defined(RTM_NEON_INTRINSICS)
				float32x4_t x2_y2_z2_w2 = vmulq_f32(lhs, rhs);
				float32x2_t x2_y2 = vget_low_f32(x2_y2_z2_w2);
				float32x2_t z2_w2 = vget_high_f32(x2_y2_z2_w2);
				float32x2_t x2z2_y2w2 = vadd_f32(x2_y2, z2_w2);
				float32x2_t x2y2z2w2 = vpadd_f32(x2z2_y2w2, x2z2_y2w2);
				return vcombine_f32(x2y2z2w2, x2y2z2w2);
#else
				scalarf result = *this;
				return vector_set(result);
#endif
			}

			vector4f lhs;
			vector4f rhs;
		};
	}

	//////////////////////////////////////////////////////////////////////////
	// 4D dot product: lhs . rhs
	//////////////////////////////////////////////////////////////////////////
	constexpr rtm_impl::vector4f_vector_dot RTM_SIMD_CALL vector_dot(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
		return rtm_impl::vector4f_vector_dot{ lhs, rhs };
	}

	//////////////////////////////////////////////////////////////////////////
	// 4D dot product: lhs . rhs
	//////////////////////////////////////////////////////////////////////////
	RTM_DEPRECATED("Use vector_dot instead, to be removed in v2.0")
	inline scalarf RTM_SIMD_CALL vector_dot_as_scalar(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE4_INTRINSICS) && 0
		// SSE4 dot product instruction isn't precise enough
		return _mm_cvtss_f32(_mm_dp_ps(lhs, rhs, 0xFF));
#elif defined(RTM_SSE2_INTRINSICS)
		__m128 x2_y2_z2_w2 = _mm_mul_ps(lhs, rhs);
		__m128 z2_w2_0_0 = _mm_shuffle_ps(x2_y2_z2_w2, x2_y2_z2_w2, _MM_SHUFFLE(0, 0, 3, 2));
		__m128 x2z2_y2w2_0_0 = _mm_add_ps(x2_y2_z2_w2, z2_w2_0_0);
		__m128 y2w2_0_0_0 = _mm_shuffle_ps(x2z2_y2w2_0_0, x2z2_y2w2_0_0, _MM_SHUFFLE(0, 0, 0, 1));
		__m128 x2y2z2w2_0_0_0 = _mm_add_ps(x2z2_y2w2_0_0, y2w2_0_0_0);
		return scalarf{ x2y2z2w2_0_0_0 };
#elif defined(RTM_NEON_INTRINSICS)
		float32x4_t x2_y2_z2_w2 = vmulq_f32(lhs, rhs);
		float32x2_t x2_y2 = vget_low_f32(x2_y2_z2_w2);
		float32x2_t z2_w2 = vget_high_f32(x2_y2_z2_w2);
		float32x2_t x2z2_y2w2 = vadd_f32(x2_y2, z2_w2);
		float32x2_t x2y2z2w2 = vpadd_f32(x2z2_y2w2, x2z2_y2w2);
		return vget_lane_f32(x2y2z2w2, 0);
#else
		return (vector_get_x(lhs) * vector_get_x(rhs)) + (vector_get_y(lhs) * vector_get_y(rhs)) + (vector_get_z(lhs) * vector_get_z(rhs)) + (vector_get_w(lhs) * vector_get_w(rhs));
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// 4D dot product replicated in all components: lhs . rhs
	//////////////////////////////////////////////////////////////////////////
	RTM_DEPRECATED("Use vector_dot instead, to be removed in v2.0")
	inline vector4f RTM_SIMD_CALL vector_dot_as_vector(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE4_INTRINSICS) && 0
		// SSE4 dot product instruction isn't precise enough
		return _mm_dp_ps(lhs, rhs, 0xFF);
#elif defined(RTM_SSE2_INTRINSICS)
		__m128 x2_y2_z2_w2 = _mm_mul_ps(lhs, rhs);
		__m128 z2_w2_0_0 = _mm_shuffle_ps(x2_y2_z2_w2, x2_y2_z2_w2, _MM_SHUFFLE(0, 0, 3, 2));
		__m128 x2z2_y2w2_0_0 = _mm_add_ps(x2_y2_z2_w2, z2_w2_0_0);
		__m128 y2w2_0_0_0 = _mm_shuffle_ps(x2z2_y2w2_0_0, x2z2_y2w2_0_0, _MM_SHUFFLE(0, 0, 0, 1));
		__m128 x2y2z2w2_0_0_0 = _mm_add_ps(x2z2_y2w2_0_0, y2w2_0_0_0);
		return _mm_shuffle_ps(x2y2z2w2_0_0_0, x2y2z2w2_0_0_0, _MM_SHUFFLE(0, 0, 0, 0));
#elif defined(RTM_NEON_INTRINSICS)
		float32x4_t x2_y2_z2_w2 = vmulq_f32(lhs, rhs);
		float32x2_t x2_y2 = vget_low_f32(x2_y2_z2_w2);
		float32x2_t z2_w2 = vget_high_f32(x2_y2_z2_w2);
		float32x2_t x2z2_y2w2 = vadd_f32(x2_y2, z2_w2);
		float32x2_t x2y2z2w2 = vpadd_f32(x2z2_y2w2, x2z2_y2w2);
		return vcombine_f32(x2y2z2w2, x2y2z2w2);
#else
		return vector_set(vector_dot(lhs, rhs));
#endif
	}

	namespace rtm_impl
	{
		//////////////////////////////////////////////////////////////////////////
		// This is a helper struct to allow a single consistent API between
		// various vector types when the semantics are identical but the return
		// type differs. Implicit coercion is used to return the desired value
		// at the call site.
		//////////////////////////////////////////////////////////////////////////
		struct vector4f_vector_dot3
		{
			inline RTM_SIMD_CALL operator float() const RTM_NO_EXCEPT
			{
#if defined(RTM_SSE4_INTRINSICS) && 0
				// SSE4 dot product instruction isn't precise enough
				return _mm_cvtss_f32(_mm_dp_ps(lhs, rhs, 0x7F));
#elif defined(RTM_SSE2_INTRINSICS)
				__m128 x2_y2_z2_w2 = _mm_mul_ps(lhs, rhs);
				__m128 y2_0_0_0 = _mm_shuffle_ps(x2_y2_z2_w2, x2_y2_z2_w2, _MM_SHUFFLE(0, 0, 0, 1));
				__m128 x2y2_0_0_0 = _mm_add_ss(x2_y2_z2_w2, y2_0_0_0);
				__m128 z2_0_0_0 = _mm_shuffle_ps(x2_y2_z2_w2, x2_y2_z2_w2, _MM_SHUFFLE(0, 0, 0, 2));
				__m128 x2y2z2_0_0_0 = _mm_add_ss(x2y2_0_0_0, z2_0_0_0);
				return _mm_cvtss_f32(x2y2z2_0_0_0);
#elif defined(RTM_NEON_INTRINSICS)
				float32x4_t x2_y2_z2_w2 = vmulq_f32(lhs, rhs);
				float32x2_t x2_y2 = vget_low_f32(x2_y2_z2_w2);
				float32x2_t z2_w2 = vget_high_f32(x2_y2_z2_w2);
				float32x2_t x2y2_x2y2 = vpadd_f32(x2_y2, x2_y2);
				float32x2_t z2_z2 = vdup_lane_f32(z2_w2, 0);
				float32x2_t x2y2z2_x2y2z2 = vadd_f32(x2y2_x2y2, z2_z2);
				return vget_lane_f32(x2y2z2_x2y2z2, 0);
#else
				return (vector_get_x(lhs) * vector_get_x(rhs)) + (vector_get_y(lhs) * vector_get_y(rhs)) + (vector_get_z(lhs) * vector_get_z(rhs));
#endif
			}

#if defined(RTM_SSE2_INTRINSICS)
			inline RTM_SIMD_CALL operator scalarf() const RTM_NO_EXCEPT
			{
				__m128 x2_y2_z2_w2 = _mm_mul_ps(lhs, rhs);
				__m128 y2_0_0_0 = _mm_shuffle_ps(x2_y2_z2_w2, x2_y2_z2_w2, _MM_SHUFFLE(0, 0, 0, 1));
				__m128 x2y2_0_0_0 = _mm_add_ss(x2_y2_z2_w2, y2_0_0_0);
				__m128 z2_0_0_0 = _mm_shuffle_ps(x2_y2_z2_w2, x2_y2_z2_w2, _MM_SHUFFLE(0, 0, 0, 2));
				return scalarf{ _mm_add_ss(x2y2_0_0_0, z2_0_0_0) };
			}
#endif

			vector4f lhs;
			vector4f rhs;
		};
	}

	//////////////////////////////////////////////////////////////////////////
	// 3D dot product: lhs . rhs
	//////////////////////////////////////////////////////////////////////////
	constexpr rtm_impl::vector4f_vector_dot3 RTM_SIMD_CALL vector_dot3(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
		return rtm_impl::vector4f_vector_dot3{ lhs, rhs };
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns the squared length/norm of the vector4.
	//////////////////////////////////////////////////////////////////////////
	constexpr rtm_impl::vector4f_vector_dot RTM_SIMD_CALL vector_length_squared(vector4f_arg0 input) RTM_NO_EXCEPT
	{
		return rtm_impl::vector4f_vector_dot{ input, input };
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns the squared length/norm of the vector3.
	//////////////////////////////////////////////////////////////////////////
	constexpr rtm_impl::vector4f_vector_dot3 RTM_SIMD_CALL vector_length_squared3(vector4f_arg0 input) RTM_NO_EXCEPT
	{
		return rtm_impl::vector4f_vector_dot3{ input, input };
	}

	namespace rtm_impl
	{
		//////////////////////////////////////////////////////////////////////////
		// This is a helper struct to allow a single consistent API between
		// various vector types when the semantics are identical but the return
		// type differs. Implicit coercion is used to return the desired value
		// at the call site.
		//////////////////////////////////////////////////////////////////////////
		struct vector4f_vector_length
		{
			inline RTM_SIMD_CALL operator float() const RTM_NO_EXCEPT
			{
				const scalarf len_sq = vector_length_squared(input);
				return scalar_cast(scalar_sqrt(len_sq));
			}

#if defined(RTM_SSE2_INTRINSICS)
			inline RTM_SIMD_CALL operator scalarf() const RTM_NO_EXCEPT
			{
				const scalarf len_sq = vector_length_squared(input);
				return scalar_sqrt(len_sq);
			}
#endif

			vector4f input;
		};
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns the length/norm of the vector4.
	//////////////////////////////////////////////////////////////////////////
	constexpr rtm_impl::vector4f_vector_length RTM_SIMD_CALL vector_length(vector4f_arg0 input) RTM_NO_EXCEPT
	{
		return rtm_impl::vector4f_vector_length{ input };
	}

	namespace rtm_impl
	{
		//////////////////////////////////////////////////////////////////////////
		// This is a helper struct to allow a single consistent API between
		// various vector types when the semantics are identical but the return
		// type differs. Implicit coercion is used to return the desired value
		// at the call site.
		//////////////////////////////////////////////////////////////////////////
		struct vector4f_vector_length3
		{
			inline RTM_SIMD_CALL operator float() const RTM_NO_EXCEPT
			{
				const scalarf len_sq = vector_length_squared3(input);
				return scalar_cast(scalar_sqrt(len_sq));
			}

#if defined(RTM_SSE2_INTRINSICS)
			inline RTM_SIMD_CALL operator scalarf() const RTM_NO_EXCEPT
			{
				const scalarf len_sq = vector_length_squared3(input);
				return scalar_sqrt(len_sq);
			}
#endif

			vector4f input;
		};
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns the length/norm of the vector3.
	//////////////////////////////////////////////////////////////////////////
	constexpr rtm_impl::vector4f_vector_length3 RTM_SIMD_CALL vector_length3(vector4f_arg0 input) RTM_NO_EXCEPT
	{
		return rtm_impl::vector4f_vector_length3{ input };
	}

	namespace rtm_impl
	{
		//////////////////////////////////////////////////////////////////////////
		// This is a helper struct to allow a single consistent API between
		// various vector types when the semantics are identical but the return
		// type differs. Implicit coercion is used to return the desired value
		// at the call site.
		//////////////////////////////////////////////////////////////////////////
		struct vector4f_vector_length_reciprocal
		{
			inline RTM_SIMD_CALL operator float() const RTM_NO_EXCEPT
			{
				const scalarf len_sq = vector_length_squared(input);
				return scalar_cast(scalar_sqrt_reciprocal(len_sq));
			}

#if defined(RTM_SSE2_INTRINSICS)
			inline RTM_SIMD_CALL operator scalarf() const RTM_NO_EXCEPT
			{
				const scalarf len_sq = vector_length_squared(input);
				return scalar_sqrt_reciprocal(len_sq);
			}
#endif

			vector4f input;
		};
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns the reciprocal length/norm of the vector4.
	//////////////////////////////////////////////////////////////////////////
	constexpr rtm_impl::vector4f_vector_length_reciprocal RTM_SIMD_CALL vector_length_reciprocal(vector4f_arg0 input) RTM_NO_EXCEPT
	{
		return rtm_impl::vector4f_vector_length_reciprocal{ input };
	}

	namespace rtm_impl
	{
		//////////////////////////////////////////////////////////////////////////
		// This is a helper struct to allow a single consistent API between
		// various vector types when the semantics are identical but the return
		// type differs. Implicit coercion is used to return the desired value
		// at the call site.
		//////////////////////////////////////////////////////////////////////////
		struct vector4f_vector_length_reciprocal3
		{
			inline RTM_SIMD_CALL operator float() const RTM_NO_EXCEPT
			{
				const scalarf len_sq = vector_length_squared3(input);
				return scalar_cast(scalar_sqrt_reciprocal(len_sq));
			}

#if defined(RTM_SSE2_INTRINSICS)
			inline RTM_SIMD_CALL operator scalarf() const RTM_NO_EXCEPT
			{
				const scalarf len_sq = vector_length_squared3(input);
				return scalar_sqrt_reciprocal(len_sq);
			}
#endif

			vector4f input;
		};
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns the reciprocal length/norm of the vector3.
	//////////////////////////////////////////////////////////////////////////
	constexpr rtm_impl::vector4f_vector_length_reciprocal3 RTM_SIMD_CALL vector_length_reciprocal3(vector4f_arg0 input) RTM_NO_EXCEPT
	{
		return rtm_impl::vector4f_vector_length_reciprocal3{ input };
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns the distance between two 3D points.
	//////////////////////////////////////////////////////////////////////////
	inline rtm_impl::vector4f_vector_length3 RTM_SIMD_CALL vector_distance3(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
		const vector4f difference = vector_sub(lhs, rhs);
		return rtm_impl::vector4f_vector_length3{ difference };
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns a normalized vector3.
	// If the length of the input is not finite or zero, the result is undefined.
	// For a safe alternative, supply a fallback value and a threshold.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_normalize3(vector4f_arg0 input) RTM_NO_EXCEPT
	{
		// Reciprocal is more accurate to normalize with
		const scalarf len_sq = vector_length_squared3(input);
		return vector_mul(input, scalar_sqrt_reciprocal(len_sq));
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns a normalized vector3.
	// If the length of the input is below the supplied threshold, the
	// fall back value is returned instead.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_normalize3(vector4f_arg0 input, vector4f_arg1 fallback, float threshold = 1.0E-8F) RTM_NO_EXCEPT
	{
		// Reciprocal is more accurate to normalize with
		const scalarf len_sq = vector_length_squared3(input);
		if (scalar_cast(len_sq) >= threshold)
			return vector_mul(input, scalar_sqrt_reciprocal(len_sq));
		else
			return fallback;
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns per component the fractional part of the input.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_fraction(vector4f_arg0 input) RTM_NO_EXCEPT
	{
		return vector_set(scalar_fraction(vector_get_x(input)), scalar_fraction(vector_get_y(input)), scalar_fraction(vector_get_z(input)), scalar_fraction(vector_get_w(input)));
	}

	//////////////////////////////////////////////////////////////////////////
	// Per component multiplication/addition of the three inputs: v2 + (v0 * v1)
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_mul_add(vector4f_arg0 v0, vector4f_arg1 v1, vector4f_arg2 v2) RTM_NO_EXCEPT
	{
#if defined(RTM_NEON64_INTRINSICS)
		return vfmaq_f32(v2, v0, v1);
#elif defined(RTM_NEON_INTRINSICS)
		return vmlaq_f32(v2, v0, v1);
#else
		return vector_add(vector_mul(v0, v1), v2);
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Per component multiplication/addition of the three inputs: v2 + (v0 * s1)
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_mul_add(vector4f_arg0 v0, float s1, vector4f_arg2 v2) RTM_NO_EXCEPT
	{
#if defined(RTM_NEON64_INTRINSICS)
		return vfmaq_n_f32(v2, v0, s1);
#elif defined(RTM_NEON_INTRINSICS)
		return vmlaq_n_f32(v2, v0, s1);
#else
		return vector_add(vector_mul(v0, s1), v2);
#endif
	}

#if defined(RTM_SSE2_INTRINSICS)
	//////////////////////////////////////////////////////////////////////////
	// Per component multiplication/addition of the three inputs: v2 + (v0 * s1)
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_mul_add(vector4f_arg0 v0, scalarf_arg1 s1, vector4f_arg2 v2) RTM_NO_EXCEPT
	{
		return vector_add(vector_mul(v0, s1), v2);
	}
#endif

	//////////////////////////////////////////////////////////////////////////
	// Per component negative multiplication/subtraction of the three inputs: -((v0 * v1) - v2)
	// This is mathematically equivalent to: v2 - (v0 * v1)
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_neg_mul_sub(vector4f_arg0 v0, vector4f_arg1 v1, vector4f_arg2 v2) RTM_NO_EXCEPT
	{
#if defined(RTM_NEON64_INTRINSICS)
		return vfmsq_f32(v2, v0, v1);
#elif defined(RTM_NEON_INTRINSICS)
		return vmlsq_f32(v2, v0, v1);
#else
		return vector_sub(v2, vector_mul(v0, v1));
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Per component negative multiplication/subtraction of the three inputs: -((v0 * s1) - v2)
	// This is mathematically equivalent to: v2 - (v0 * s1)
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_neg_mul_sub(vector4f_arg0 v0, float s1, vector4f_arg2 v2) RTM_NO_EXCEPT
	{
#if defined(RTM_NEON64_INTRINSICS)
		return vfmsq_n_f32(v2, v0, s1);
#elif defined(RTM_NEON_INTRINSICS)
		return vmlsq_n_f32(v2, v0, s1);
#else
		return vector_sub(v2, vector_mul(v0, s1));
#endif
	}

#if defined(RTM_SSE2_INTRINSICS)
	//////////////////////////////////////////////////////////////////////////
	// Per component negative multiplication/subtraction of the three inputs: -((v0 * s1) - v2)
	// This is mathematically equivalent to: v2 - (v0 * s1)
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_neg_mul_sub(vector4f_arg0 v0, scalarf_arg1 s1, vector4f_arg2 v2) RTM_NO_EXCEPT
	{
		return vector_sub(v2, vector_mul(v0, s1));
	}
#endif

	//////////////////////////////////////////////////////////////////////////
	// Per component linear interpolation of the two inputs at the specified alpha.
	// The formula used is: ((1.0 - alpha) * start) + (alpha * end).
	// Interpolation is stable and will return 'start' when alpha is 0.0 and 'end' when it is 1.0.
	// This is the same instruction count when FMA is present but it might be slightly slower
	// due to the extra multiplication compared to: start + (alpha * (end - start)).
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_lerp(vector4f_arg0 start, vector4f_arg1 end, float alpha) RTM_NO_EXCEPT
	{
		// ((1.0 - alpha) * start) + (alpha * end) == (start - alpha * start) + (alpha * end)
		return vector_mul_add(end, alpha, vector_neg_mul_sub(start, alpha, start));
	}

#if defined(RTM_SSE2_INTRINSICS)
	//////////////////////////////////////////////////////////////////////////
	// Per component linear interpolation of the two inputs at the specified alpha.
	// The formula used is: ((1.0 - alpha) * start) + (alpha * end).
	// Interpolation is stable and will return 'start' when alpha is 0.0 and 'end' when it is 1.0.
	// This is the same instruction count when FMA is present but it might be slightly slower
	// due to the extra multiplication compared to: start + (alpha * (end - start)).
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_lerp(vector4f_arg0 start, vector4f_arg1 end, scalarf_arg2 alpha) RTM_NO_EXCEPT
	{
		// ((1.0 - alpha) * start) + (alpha * end) == (start - alpha * start) + (alpha * end)
		const vector4f alpha_v = vector_set(alpha);
		return vector_mul_add(end, alpha_v, vector_neg_mul_sub(start, alpha_v, start));
	}
#endif



	//////////////////////////////////////////////////////////////////////////
	// Comparisons and masking
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// Returns per component ~0 if equal, otherwise 0: lhs == rhs ? ~0 : 0
	//////////////////////////////////////////////////////////////////////////
	inline mask4i RTM_SIMD_CALL vector_equal(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return _mm_cmpeq_ps(lhs, rhs);
#elif defined(RTM_NEON_INTRINSICS)
		return vceqq_f32(lhs, rhs);
#else
		return mask4i{ rtm_impl::get_mask_value(lhs.x == rhs.x), rtm_impl::get_mask_value(lhs.y == rhs.y), rtm_impl::get_mask_value(lhs.z == rhs.z), rtm_impl::get_mask_value(lhs.w == rhs.w) };
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns per component ~0 if less than, otherwise 0: lhs < rhs ? ~0 : 0
	//////////////////////////////////////////////////////////////////////////
	inline mask4i RTM_SIMD_CALL vector_less_than(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return _mm_cmplt_ps(lhs, rhs);
#elif defined(RTM_NEON_INTRINSICS)
		return vcltq_f32(lhs, rhs);
#else
		return mask4i{ rtm_impl::get_mask_value(lhs.x < rhs.x), rtm_impl::get_mask_value(lhs.y < rhs.y), rtm_impl::get_mask_value(lhs.z < rhs.z), rtm_impl::get_mask_value(lhs.w < rhs.w) };
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns per component ~0 if less equal, otherwise 0: lhs <= rhs ? ~0 : 0
	//////////////////////////////////////////////////////////////////////////
	inline mask4i RTM_SIMD_CALL vector_less_equal(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return _mm_cmple_ps(lhs, rhs);
#elif defined(RTM_NEON_INTRINSICS)
		return vcleq_f32(lhs, rhs);
#else
		return mask4i{ rtm_impl::get_mask_value(lhs.x <= rhs.x), rtm_impl::get_mask_value(lhs.y <= rhs.y), rtm_impl::get_mask_value(lhs.z <= rhs.z), rtm_impl::get_mask_value(lhs.w <= rhs.w) };
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns per component ~0 if greater than, otherwise 0: lhs > rhs ? ~0 : 0
	//////////////////////////////////////////////////////////////////////////
	inline mask4i RTM_SIMD_CALL vector_greater_than(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return _mm_cmpgt_ps(lhs, rhs);
#elif defined(RTM_NEON_INTRINSICS)
		return vcgt_f32(lhs, rhs);
#else
		return mask4i{ rtm_impl::get_mask_value(lhs.x > rhs.x), rtm_impl::get_mask_value(lhs.y > rhs.y), rtm_impl::get_mask_value(lhs.z > rhs.z), rtm_impl::get_mask_value(lhs.w > rhs.w) };
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns per component ~0 if greater equal, otherwise 0: lhs >= rhs ? ~0 : 0
	//////////////////////////////////////////////////////////////////////////
	inline mask4i RTM_SIMD_CALL vector_greater_equal(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return _mm_cmpge_ps(lhs, rhs);
#elif defined(RTM_NEON_INTRINSICS)
		return vcgeq_f32(lhs, rhs);
#else
		return mask4i{ rtm_impl::get_mask_value(lhs.x >= rhs.x), rtm_impl::get_mask_value(lhs.y >= rhs.y), rtm_impl::get_mask_value(lhs.z >= rhs.z), rtm_impl::get_mask_value(lhs.w >= rhs.w) };
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns true if all 4 components are less than, otherwise false: all(lhs < rhs)
	//////////////////////////////////////////////////////////////////////////
	inline bool RTM_SIMD_CALL vector_all_less_than(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return _mm_movemask_ps(_mm_cmplt_ps(lhs, rhs)) == 0xF;
#elif defined(RTM_NEON_INTRINSICS)
		uint32x4_t mask = vcltq_f32(lhs, rhs);
		uint8x8x2_t mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15 = vzip_u8(vget_low_u8(mask), vget_high_u8(mask));
		uint16x4x2_t mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15 = vzip_u16(mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[0], mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[1]);
		return vget_lane_u32(mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15.val[0], 0) == 0xFFFFFFFFU;
#else
		return lhs.x < rhs.x && lhs.y < rhs.y && lhs.z < rhs.z && lhs.w < rhs.w;
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns true if all 2 components are less than, otherwise false: all(lhs < rhs)
	//////////////////////////////////////////////////////////////////////////
	inline bool RTM_SIMD_CALL vector_all_less_than2(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return (_mm_movemask_ps(_mm_cmplt_ps(lhs, rhs)) & 0x3) == 0x3;
#elif defined(RTM_NEON_INTRINSICS)
		uint32x2_t mask = vclt_f32(vget_low_f32(lhs), vget_low_f32(rhs));
		return vget_lane_u64(mask, 0) == 0xFFFFFFFFFFFFFFFFu;
#else
		return lhs.x < rhs.x && lhs.y < rhs.y;
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns true if all 3 components are less than, otherwise false: all(lhs < rhs)
	//////////////////////////////////////////////////////////////////////////
	inline bool RTM_SIMD_CALL vector_all_less_than3(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return (_mm_movemask_ps(_mm_cmplt_ps(lhs, rhs)) & 0x7) == 0x7;
#elif defined(RTM_NEON_INTRINSICS)
		uint32x4_t mask = vcltq_f32(lhs, rhs);
		uint8x8x2_t mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15 = vzip_u8(vget_low_u8(mask), vget_high_u8(mask));
		uint16x4x2_t mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15 = vzip_u16(mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[0], mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[1]);
		return (vget_lane_u32(mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15.val[0], 0) & 0x00FFFFFFU) == 0x00FFFFFFU;
#else
		return lhs.x < rhs.x && lhs.y < rhs.y && lhs.z < rhs.z;
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns true if any 4 components are less than, otherwise false: any(lhs < rhs)
	//////////////////////////////////////////////////////////////////////////
	inline bool RTM_SIMD_CALL vector_any_less_than(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return _mm_movemask_ps(_mm_cmplt_ps(lhs, rhs)) != 0;
#elif defined(RTM_NEON_INTRINSICS)
		uint32x4_t mask = vcltq_f32(lhs, rhs);
		uint8x8x2_t mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15 = vzip_u8(vget_low_u8(mask), vget_high_u8(mask));
		uint16x4x2_t mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15 = vzip_u16(mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[0], mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[1]);
		return vget_lane_u32(mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15.val[0], 0) != 0;
#else
		return lhs.x < rhs.x || lhs.y < rhs.y || lhs.z < rhs.z || lhs.w < rhs.w;
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns true if any 2 components are less than, otherwise false: any(lhs < rhs)
	//////////////////////////////////////////////////////////////////////////
	inline bool RTM_SIMD_CALL vector_any_less_than2(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return (_mm_movemask_ps(_mm_cmplt_ps(lhs, rhs)) & 0x3) != 0;
#elif defined(RTM_NEON_INTRINSICS)
		uint32x2_t mask = vclt_f32(vget_low_f32(lhs), vget_low_f32(rhs));
		return vget_lane_u64(mask, 0) != 0;
#else
		return lhs.x < rhs.x || lhs.y < rhs.y;
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns true if any 3 components are less than, otherwise false: any(lhs < rhs)
	//////////////////////////////////////////////////////////////////////////
	inline bool RTM_SIMD_CALL vector_any_less_than3(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return (_mm_movemask_ps(_mm_cmplt_ps(lhs, rhs)) & 0x7) != 0;
#elif defined(RTM_NEON_INTRINSICS)
		uint32x4_t mask = vcltq_f32(lhs, rhs);
		uint8x8x2_t mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15 = vzip_u8(vget_low_u8(mask), vget_high_u8(mask));
		uint16x4x2_t mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15 = vzip_u16(mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[0], mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[1]);
		return (vget_lane_u32(mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15.val[0], 0) & 0x00FFFFFFU) != 0;
#else
		return lhs.x < rhs.x || lhs.y < rhs.y || lhs.z < rhs.z;
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns true if all 4 components are less equal, otherwise false: all(lhs <= rhs)
	//////////////////////////////////////////////////////////////////////////
	inline bool RTM_SIMD_CALL vector_all_less_equal(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return _mm_movemask_ps(_mm_cmple_ps(lhs, rhs)) == 0xF;
#elif defined(RTM_NEON_INTRINSICS)
		uint32x4_t mask = vcleq_f32(lhs, rhs);
		uint8x8x2_t mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15 = vzip_u8(vget_low_u8(mask), vget_high_u8(mask));
		uint16x4x2_t mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15 = vzip_u16(mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[0], mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[1]);
		return vget_lane_u32(mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15.val[0], 0) == 0xFFFFFFFFU;
#else
		return lhs.x <= rhs.x && lhs.y <= rhs.y && lhs.z <= rhs.z && lhs.w <= rhs.w;
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns true if all 2 components are less equal, otherwise false: all(lhs <= rhs)
	//////////////////////////////////////////////////////////////////////////
	inline bool RTM_SIMD_CALL vector_all_less_equal2(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return (_mm_movemask_ps(_mm_cmple_ps(lhs, rhs)) & 0x3) == 0x3;
#elif defined(RTM_NEON_INTRINSICS)
		uint32x2_t mask = vcle_f32(vget_low_f32(lhs), vget_low_f32(rhs));
		return vget_lane_u64(mask, 0) == 0xFFFFFFFFFFFFFFFFULL;
#else
		return lhs.x <= rhs.x && lhs.y <= rhs.y;
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns true if all 3 components are less equal, otherwise false: all(lhs <= rhs)
	//////////////////////////////////////////////////////////////////////////
	inline bool RTM_SIMD_CALL vector_all_less_equal3(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return (_mm_movemask_ps(_mm_cmple_ps(lhs, rhs)) & 0x7) == 0x7;
#elif defined(RTM_NEON_INTRINSICS)
		uint32x4_t mask = vcleq_f32(lhs, rhs);
		uint8x8x2_t mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15 = vzip_u8(vget_low_u8(mask), vget_high_u8(mask));
		uint16x4x2_t mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15 = vzip_u16(mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[0], mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[1]);
		return (vget_lane_u32(mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15.val[0], 0) & 0x00FFFFFFU) == 0x00FFFFFFU;
#else
		return lhs.x <= rhs.x && lhs.y <= rhs.y && lhs.z <= rhs.z;
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns true if any 4 components are less equal, otherwise false: any(lhs <= rhs)
	//////////////////////////////////////////////////////////////////////////
	inline bool RTM_SIMD_CALL vector_any_less_equal(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return _mm_movemask_ps(_mm_cmple_ps(lhs, rhs)) != 0;
#elif defined(RTM_NEON_INTRINSICS)
		uint32x4_t mask = vcleq_f32(lhs, rhs);
		uint8x8x2_t mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15 = vzip_u8(vget_low_u8(mask), vget_high_u8(mask));
		uint16x4x2_t mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15 = vzip_u16(mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[0], mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[1]);
		return vget_lane_u32(mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15.val[0], 0) != 0;
#else
		return lhs.x <= rhs.x || lhs.y <= rhs.y || lhs.z <= rhs.z || lhs.w <= rhs.w;
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns true if any 2 components are less equal, otherwise false: any(lhs <= rhs)
	//////////////////////////////////////////////////////////////////////////
	inline bool RTM_SIMD_CALL vector_any_less_equal2(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return (_mm_movemask_ps(_mm_cmple_ps(lhs, rhs)) & 0x3) != 0;
#elif defined(RTM_NEON_INTRINSICS)
		uint32x2_t mask = vcle_f32(vget_low_f32(lhs), vget_low_f32(rhs));
		return vget_lane_u64(mask, 0) != 0;
#else
		return lhs.x <= rhs.x || lhs.y <= rhs.y;
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns true if any 3 components are less equal, otherwise false: any(lhs <= rhs)
	//////////////////////////////////////////////////////////////////////////
	inline bool RTM_SIMD_CALL vector_any_less_equal3(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return (_mm_movemask_ps(_mm_cmple_ps(lhs, rhs)) & 0x7) != 0;
#elif defined(RTM_NEON_INTRINSICS)
		uint32x4_t mask = vcleq_f32(lhs, rhs);
		uint8x8x2_t mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15 = vzip_u8(vget_low_u8(mask), vget_high_u8(mask));
		uint16x4x2_t mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15 = vzip_u16(mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[0], mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[1]);
		return (vget_lane_u32(mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15.val[0], 0) & 0x00FFFFFFU) != 0;
#else
		return lhs.x <= rhs.x || lhs.y <= rhs.y || lhs.z <= rhs.z;
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns true if all 4 components are greater equal, otherwise false: all(lhs >= rhs)
	//////////////////////////////////////////////////////////////////////////
	inline bool RTM_SIMD_CALL vector_all_greater_equal(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return _mm_movemask_ps(_mm_cmpge_ps(lhs, rhs)) == 0xF;
#elif defined(RTM_NEON_INTRINSICS)
		uint32x4_t mask = vcgeq_f32(lhs, rhs);
		uint8x8x2_t mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15 = vzip_u8(vget_low_u8(mask), vget_high_u8(mask));
		uint16x4x2_t mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15 = vzip_u16(mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[0], mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[1]);
		return vget_lane_u32(mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15.val[0], 0) == 0xFFFFFFFFU;
#else
		return lhs.x >= rhs.x && lhs.y >= rhs.y && lhs.z >= rhs.z && lhs.w >= rhs.w;
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns true if all 2 components are greater equal, otherwise false: all(lhs >= rhs)
	//////////////////////////////////////////////////////////////////////////
	inline bool RTM_SIMD_CALL vector_all_greater_equal2(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return (_mm_movemask_ps(_mm_cmpge_ps(lhs, rhs)) & 0x3) == 0x3;
#elif defined(RTM_NEON_INTRINSICS)
		uint32x2_t mask = vcge_f32(vget_low_f32(lhs), vget_low_f32(rhs));
		return vget_lane_u64(mask, 0) == 0xFFFFFFFFFFFFFFFFULL;
#else
		return lhs.x >= rhs.x && lhs.y >= rhs.y;
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns true if all 3 components are greater equal, otherwise false: all(lhs >= rhs)
	//////////////////////////////////////////////////////////////////////////
	inline bool RTM_SIMD_CALL vector_all_greater_equal3(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return (_mm_movemask_ps(_mm_cmpge_ps(lhs, rhs)) & 0x7) == 0x7;
#elif defined(RTM_NEON_INTRINSICS)
		uint32x4_t mask = vcgeq_f32(lhs, rhs);
		uint8x8x2_t mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15 = vzip_u8(vget_low_u8(mask), vget_high_u8(mask));
		uint16x4x2_t mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15 = vzip_u16(mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[0], mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[1]);
		return (vget_lane_u32(mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15.val[0], 0) & 0x00FFFFFFU) == 0x00FFFFFFU;
#else
		return lhs.x >= rhs.x && lhs.y >= rhs.y && lhs.z >= rhs.z;
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns true if any 4 components are greater equal, otherwise false: any(lhs >= rhs)
	//////////////////////////////////////////////////////////////////////////
	inline bool RTM_SIMD_CALL vector_any_greater_equal(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return _mm_movemask_ps(_mm_cmpge_ps(lhs, rhs)) != 0;
#elif defined(RTM_NEON_INTRINSICS)
		uint32x4_t mask = vcgeq_f32(lhs, rhs);
		uint8x8x2_t mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15 = vzip_u8(vget_low_u8(mask), vget_high_u8(mask));
		uint16x4x2_t mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15 = vzip_u16(mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[0], mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[1]);
		return vget_lane_u32(mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15.val[0], 0) != 0;
#else
		return lhs.x >= rhs.x || lhs.y >= rhs.y || lhs.z >= rhs.z || lhs.w >= rhs.w;
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns true if any 2 components are greater equal, otherwise false: any(lhs >= rhs)
	//////////////////////////////////////////////////////////////////////////
	inline bool RTM_SIMD_CALL vector_any_greater_equal2(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return (_mm_movemask_ps(_mm_cmpge_ps(lhs, rhs)) & 0x3) != 0;
#elif defined(RTM_NEON_INTRINSICS)
		uint32x2_t mask = vcge_f32(vget_low_f32(lhs), vget_low_f32(rhs));
		return vget_lane_u64(mask, 0) != 0;
#else
		return lhs.x >= rhs.x || lhs.y >= rhs.y;
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns true if any 3 components are greater equal, otherwise false: any(lhs >= rhs)
	//////////////////////////////////////////////////////////////////////////
	inline bool RTM_SIMD_CALL vector_any_greater_equal3(vector4f_arg0 lhs, vector4f_arg1 rhs) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		return (_mm_movemask_ps(_mm_cmpge_ps(lhs, rhs)) & 0x7) != 0;
#elif defined(RTM_NEON_INTRINSICS)
		uint32x4_t mask = vcgeq_f32(lhs, rhs);
		uint8x8x2_t mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15 = vzip_u8(vget_low_u8(mask), vget_high_u8(mask));
		uint16x4x2_t mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15 = vzip_u16(mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[0], mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[1]);
		return (vget_lane_u32(mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15.val[0], 0) & 0x00FFFFFFU) != 0;
#else
		return lhs.x >= rhs.x || lhs.y >= rhs.y || lhs.z >= rhs.z;
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns true if all 4 components are near equal, otherwise false: all(abs(lhs - rhs) <= threshold)
	//////////////////////////////////////////////////////////////////////////
	inline bool RTM_SIMD_CALL vector_all_near_equal(vector4f_arg0 lhs, vector4f_arg1 rhs, float threshold = 0.00001F) RTM_NO_EXCEPT
	{
		return vector_all_less_equal(vector_abs(vector_sub(lhs, rhs)), vector_set(threshold));
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns true if all 2 components are near equal, otherwise false: all(abs(lhs - rhs) <= threshold)
	//////////////////////////////////////////////////////////////////////////
	inline bool RTM_SIMD_CALL vector_all_near_equal2(vector4f_arg0 lhs, vector4f_arg1 rhs, float threshold = 0.00001F) RTM_NO_EXCEPT
	{
		return vector_all_less_equal2(vector_abs(vector_sub(lhs, rhs)), vector_set(threshold));
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns true if all 3 components are near equal, otherwise false: all(abs(lhs - rhs) <= threshold)
	//////////////////////////////////////////////////////////////////////////
	inline bool RTM_SIMD_CALL vector_all_near_equal3(vector4f_arg0 lhs, vector4f_arg1 rhs, float threshold = 0.00001F) RTM_NO_EXCEPT
	{
		return vector_all_less_equal3(vector_abs(vector_sub(lhs, rhs)), vector_set(threshold));
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns true if any 4 components are near equal, otherwise false: any(abs(lhs - rhs) <= threshold)
	//////////////////////////////////////////////////////////////////////////
	inline bool RTM_SIMD_CALL vector_any_near_equal(vector4f_arg0 lhs, vector4f_arg1 rhs, float threshold = 0.00001F) RTM_NO_EXCEPT
	{
		return vector_any_less_equal(vector_abs(vector_sub(lhs, rhs)), vector_set(threshold));
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns true if any 2 components are near equal, otherwise false: any(abs(lhs - rhs) <= threshold)
	//////////////////////////////////////////////////////////////////////////
	inline bool RTM_SIMD_CALL vector_any_near_equal2(vector4f_arg0 lhs, vector4f_arg1 rhs, float threshold = 0.00001F) RTM_NO_EXCEPT
	{
		return vector_any_less_equal2(vector_abs(vector_sub(lhs, rhs)), vector_set(threshold));
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns true if any 3 components are near equal, otherwise false: any(abs(lhs - rhs) <= threshold)
	//////////////////////////////////////////////////////////////////////////
	inline bool RTM_SIMD_CALL vector_any_near_equal3(vector4f_arg0 lhs, vector4f_arg1 rhs, float threshold = 0.00001F) RTM_NO_EXCEPT
	{
		return vector_any_less_equal3(vector_abs(vector_sub(lhs, rhs)), vector_set(threshold));
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns true if all 4 components are finite (not NaN/Inf), otherwise false: all(finite(input))
	//////////////////////////////////////////////////////////////////////////
	inline bool RTM_SIMD_CALL vector_is_finite(vector4f_arg0 input) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		const __m128i abs_mask = _mm_set_epi32(0x7FFFFFFFULL, 0x7FFFFFFFULL, 0x7FFFFFFFULL, 0x7FFFFFFFULL);
		__m128 abs_input = _mm_and_ps(input, _mm_castsi128_ps(abs_mask));

		const __m128 infinity = _mm_set_ps1(std::numeric_limits<float>::infinity());
		__m128 is_infinity = _mm_cmpeq_ps(abs_input, infinity);

		__m128 is_nan = _mm_cmpneq_ps(input, input);

		__m128 is_not_finite = _mm_or_ps(is_infinity, is_nan);
		return _mm_movemask_ps(is_not_finite) == 0;
#else
		return scalar_is_finite(vector_get_x(input)) && scalar_is_finite(vector_get_y(input)) && scalar_is_finite(vector_get_z(input)) && scalar_is_finite(vector_get_w(input));
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns true if all 2 components are finite (not NaN/Inf), otherwise false: all(finite(input))
	//////////////////////////////////////////////////////////////////////////
	inline bool RTM_SIMD_CALL vector_is_finite2(vector4f_arg0 input) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		const __m128i abs_mask = _mm_set_epi32(0x7FFFFFFFULL, 0x7FFFFFFFULL, 0x7FFFFFFFULL, 0x7FFFFFFFULL);
		__m128 abs_input = _mm_and_ps(input, _mm_castsi128_ps(abs_mask));

		const __m128 infinity = _mm_set_ps1(std::numeric_limits<float>::infinity());
		__m128 is_infinity = _mm_cmpeq_ps(abs_input, infinity);

		__m128 is_nan = _mm_cmpneq_ps(input, input);

		__m128 is_not_finite = _mm_or_ps(is_infinity, is_nan);
		return (_mm_movemask_ps(is_not_finite) & 0x3) == 0;
#else
		return scalar_is_finite(vector_get_x(input)) && scalar_is_finite(vector_get_y(input));
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns true if all 3 components are finite (not NaN/Inf), otherwise false: all(finite(input))
	//////////////////////////////////////////////////////////////////////////
	inline bool RTM_SIMD_CALL vector_is_finite3(vector4f_arg0 input) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		const __m128i abs_mask = _mm_set_epi32(0x7FFFFFFFULL, 0x7FFFFFFFULL, 0x7FFFFFFFULL, 0x7FFFFFFFULL);
		__m128 abs_input = _mm_and_ps(input, _mm_castsi128_ps(abs_mask));

		const __m128 infinity = _mm_set_ps1(std::numeric_limits<float>::infinity());
		__m128 is_infinity = _mm_cmpeq_ps(abs_input, infinity);

		__m128 is_nan = _mm_cmpneq_ps(input, input);

		__m128 is_not_finite = _mm_or_ps(is_infinity, is_nan);
		return (_mm_movemask_ps(is_not_finite) & 0x7) == 0;
#else
		return scalar_is_finite(vector_get_x(input)) && scalar_is_finite(vector_get_y(input)) && scalar_is_finite(vector_get_z(input));
#endif
	}



	//////////////////////////////////////////////////////////////////////////
	// Swizzling, permutations, and mixing
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// Per component selection depending on the mask: mask != 0 ? if_true : if_false
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_select(mask4i_arg0 mask, vector4f_arg1 if_true, vector4f_arg2 if_false) RTM_NO_EXCEPT
	{
#if defined(RTM_AVX_INTRINSICS)
		return _mm_blendv_ps(if_false, if_true, mask);
#elif defined(RTM_SSE2_INTRINSICS)
		return _mm_or_ps(_mm_andnot_ps(mask, if_false), _mm_and_ps(if_true, mask));
#elif defined(RTM_NEON_INTRINSICS)
		return vbslq_f32(mask, if_true, if_false);
#else
		return vector4f{ rtm_impl::select(mask.x, if_true.x, if_false.x), rtm_impl::select(mask.y, if_true.y, if_false.y), rtm_impl::select(mask.z, if_true.z, if_false.z), rtm_impl::select(mask.w, if_true.w, if_false.w) };
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Mixes two inputs and returns the desired components.
	// [xyzw] indexes into the first input while [abcd] indexes in the second.
	//////////////////////////////////////////////////////////////////////////
	template<mix4 comp0, mix4 comp1, mix4 comp2, mix4 comp3>
	inline vector4f RTM_SIMD_CALL vector_mix(vector4f_arg0 input0, vector4f_arg1 input1) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		// All four components come from input 0
		if (rtm_impl::is_mix_xyzw(comp0) && rtm_impl::is_mix_xyzw(comp1) && rtm_impl::is_mix_xyzw(comp2) && rtm_impl::is_mix_xyzw(comp3))
			return _mm_shuffle_ps(input0, input0, _MM_SHUFFLE(int(comp3) % 4, int(comp2) % 4, int(comp1) % 4, int(comp0) % 4));

		// All four components come from input 1
		if (rtm_impl::is_mix_abcd(comp0) && rtm_impl::is_mix_abcd(comp1) && rtm_impl::is_mix_abcd(comp2) && rtm_impl::is_mix_abcd(comp3))
			return _mm_shuffle_ps(input1, input1, _MM_SHUFFLE(int(comp3) % 4, int(comp2) % 4, int(comp1) % 4, int(comp0) % 4));

		// First two components come from input 0, second two come from input 1
		if (rtm_impl::is_mix_xyzw(comp0) && rtm_impl::is_mix_xyzw(comp1) && rtm_impl::is_mix_abcd(comp2) && rtm_impl::is_mix_abcd(comp3))
			return _mm_shuffle_ps(input0, input1, _MM_SHUFFLE(int(comp3) % 4, int(comp2) % 4, int(comp1) % 4, int(comp0) % 4));

		// First two components come from input 1, second two come from input 0
		if (rtm_impl::is_mix_abcd(comp0) && rtm_impl::is_mix_abcd(comp1) && rtm_impl::is_mix_xyzw(comp2) && rtm_impl::is_mix_xyzw(comp3))
			return _mm_shuffle_ps(input1, input0, _MM_SHUFFLE(int(comp3) % 4, int(comp2) % 4, int(comp1) % 4, int(comp0) % 4));

		// Low words from both inputs are interleaved
		if (rtm_impl::static_condition<comp0 == mix4::x && comp1 == mix4::a && comp2 == mix4::y && comp3 == mix4::b>::test())
			return _mm_unpacklo_ps(input0, input1);

		// Low words from both inputs are interleaved
		if (rtm_impl::static_condition<comp0 == mix4::a && comp1 == mix4::x && comp2 == mix4::b && comp3 == mix4::y>::test())
			return _mm_unpacklo_ps(input1, input0);

		// High words from both inputs are interleaved
		if (rtm_impl::static_condition<comp0 == mix4::z && comp1 == mix4::c && comp2 == mix4::w && comp3 == mix4::d>::test())
			return _mm_unpackhi_ps(input0, input1);

		// High words from both inputs are interleaved
		if (rtm_impl::static_condition<comp0 == mix4::c && comp1 == mix4::z && comp2 == mix4::d && comp3 == mix4::w>::test())
			return _mm_unpackhi_ps(input1, input0);
#endif	// defined(RTM_SSE2_INTRINSICS)

		// Slow code path, not yet optimized or not using intrinsics
		const float x = rtm_impl::is_mix_xyzw(comp0) ? vector_get_component<comp0>(input0) : vector_get_component<comp0>(input1);
		const float y = rtm_impl::is_mix_xyzw(comp1) ? vector_get_component<comp1>(input0) : vector_get_component<comp1>(input1);
		const float z = rtm_impl::is_mix_xyzw(comp2) ? vector_get_component<comp2>(input0) : vector_get_component<comp2>(input1);
		const float w = rtm_impl::is_mix_xyzw(comp3) ? vector_get_component<comp3>(input0) : vector_get_component<comp3>(input1);
		return vector_set(x, y, z, w);
	}

	//////////////////////////////////////////////////////////////////////////
	// Replicates the [x] component in all components.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_dup_x(vector4f_arg0 input) RTM_NO_EXCEPT { return vector_mix<mix4::x, mix4::x, mix4::x, mix4::x>(input, input); }

	//////////////////////////////////////////////////////////////////////////
	// Replicates the [y] component in all components.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_dup_y(vector4f_arg0 input) RTM_NO_EXCEPT { return vector_mix<mix4::y, mix4::y, mix4::y, mix4::y>(input, input); }

	//////////////////////////////////////////////////////////////////////////
	// Replicates the [z] component in all components.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_dup_z(vector4f_arg0 input) RTM_NO_EXCEPT { return vector_mix<mix4::z, mix4::z, mix4::z, mix4::z>(input, input); }

	//////////////////////////////////////////////////////////////////////////
	// Replicates the [w] component in all components.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_dup_w(vector4f_arg0 input) RTM_NO_EXCEPT { return vector_mix<mix4::w, mix4::w, mix4::w, mix4::w>(input, input); }

	//////////////////////////////////////////////////////////////////////////
	// Miscellaneous
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// Returns per component the sign of the input vector: input >= 0.0 ? 1.0 : -1.0
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_sign(vector4f_arg0 input) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		constexpr __m128 signs = { -0.0F, -0.0F, -0.0F, -0.0F };
		constexpr __m128 one = { 1.0F, 1.0F, 1.0F, 1.0F };
		const __m128 sign_bits = _mm_and_ps(input, signs);	// Mask out the sign bit
		return _mm_or_ps(sign_bits, one);					// Copy the sign bit onto +-1.0f
#else
		const mask4i mask = vector_greater_equal(input, vector_zero());
		return vector_select(mask, vector_set(1.0F), vector_set(-1.0F));
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns per component the input with the sign of the control value.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_copy_sign(vector4f_arg0 input, vector4f_arg1 control_sign) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		const __m128 sign_bit = _mm_set_ps1(-0.0F);
		__m128 signs = _mm_and_ps(sign_bit, control_sign);
		__m128 abs_input = _mm_andnot_ps(sign_bit, input);
		return _mm_or_ps(abs_input, signs);
#else
		float x = vector_get_x(input);
		float y = vector_get_y(input);
		float z = vector_get_z(input);
		float w = vector_get_w(input);

		float x_sign = vector_get_x(control_sign);
		float y_sign = vector_get_y(control_sign);
		float z_sign = vector_get_z(control_sign);
		float w_sign = vector_get_w(control_sign);

		return vector_set(std::copysign(x, x_sign), std::copysign(y, y_sign), std::copysign(z, z_sign), std::copysign(w, w_sign));
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns per component the rounded input using a symmetric algorithm.
	// vector_round_symmetric(1.5) = 2.0
	// vector_round_symmetric(1.2) = 1.0
	// vector_round_symmetric(-1.5) = -2.0
	// vector_round_symmetric(-1.2) = -1.0
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_round_symmetric(vector4f_arg0 input) RTM_NO_EXCEPT
	{
		// NaN, +- Infinity, and numbers larger or equal to 2^23 remain unchanged
		// since they have no fractional part.

#if defined(RTM_SSE4_INTRINSICS)
		const __m128 sign_mask = _mm_set_ps(-0.0F, -0.0F, -0.0F, -0.0F);
		__m128 sign = _mm_and_ps(input, sign_mask);

		// For positive values, we add a bias of 0.5.
		// For negative values, we add a bias of -0.5.
		__m128 bias = _mm_or_ps(sign, _mm_set_ps1(0.5F));
		__m128 biased_input = _mm_add_ps(input, bias);

		__m128 floored = _mm_floor_ps(biased_input);
		__m128 ceiled = _mm_ceil_ps(biased_input);
		__m128 is_positive = _mm_cmpge_ps(input, _mm_setzero_ps());

		return vector_select(is_positive, floored, ceiled);
#elif defined(RTM_SSE2_INTRINSICS)
		const __m128i abs_mask = _mm_set_epi32(0x7FFFFFFFULL, 0x7FFFFFFFULL, 0x7FFFFFFFULL, 0x7FFFFFFFULL);
		const __m128 fractional_limit = _mm_set_ps1(8388608.0F); // 2^23

		// Build our mask, larger values that have no fractional part, and infinities will be true
		// Smaller values and NaN will be false
		__m128 abs_input = _mm_and_ps(input, _mm_castsi128_ps(abs_mask));
		__m128 is_input_large = _mm_cmpge_ps(abs_input, fractional_limit);

		// Test if our input is NaN with (value != value), it is only true for NaN
		__m128 is_nan = _mm_cmpneq_ps(input, input);

		// Combine our masks to determine if we should return the original value
		__m128 use_original_input = _mm_or_ps(is_input_large, is_nan);

		const __m128 sign_mask = _mm_set_ps(-0.0F, -0.0F, -0.0F, -0.0F);
		__m128 sign = _mm_and_ps(input, sign_mask);

		// For positive values, we add a bias of 0.5.
		// For negative values, we add a bias of -0.5.
		__m128 bias = _mm_or_ps(sign, _mm_set_ps1(0.5F));
		__m128 biased_input = _mm_add_ps(input, bias);

		// Convert to an integer with truncation and back, this rounds towards zero.
		__m128 integer_part = _mm_cvtepi32_ps(_mm_cvttps_epi32(biased_input));

		return _mm_or_ps(_mm_and_ps(use_original_input, input), _mm_andnot_ps(use_original_input, integer_part));
#else
		const vector4f half = vector_set(0.5F);
		const vector4f floored = vector_floor(vector_add(input, half));
		const vector4f ceiled = vector_ceil(vector_sub(input, half));
		const mask4i is_greater_equal = vector_greater_equal(input, vector_zero());
		return vector_select(is_greater_equal, floored, ceiled);
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns per component the rounded input using a symmetric algorithm.
	// vector_symmetric_round(1.5) = 2.0
	// vector_symmetric_round(1.2) = 1.0
	// vector_symmetric_round(-1.5) = -2.0
	// vector_symmetric_round(-1.2) = -1.0
	//////////////////////////////////////////////////////////////////////////
	RTM_DEPRECATED("Use vector_round_symmetric instead, to be removed in v2.0")
	inline vector4f RTM_SIMD_CALL vector_symmetric_round(vector4f_arg0 input) RTM_NO_EXCEPT
	{
		return vector_round_symmetric(input);
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns per component the rounded input using banker's rounding (half to even).
	// vector_round_bankers(2.5) = 2.0
	// vector_round_bankers(1.5) = 2.0
	// vector_round_bankers(1.2) = 1.0
	// vector_round_bankers(-2.5) = -2.0
	// vector_round_bankers(-1.5) = -2.0
	// vector_round_bankers(-1.2) = -1.0
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_round_bankers(vector4f_arg0 input) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE4_INTRINSICS)
		return _mm_round_ps(input, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
#elif defined(RTM_SSE2_INTRINSICS)
		const __m128 sign_mask = _mm_set_ps(-0.0F, -0.0F, -0.0F, -0.0F);
		__m128 sign = _mm_and_ps(input, sign_mask);

		// We add the largest integer that a 32 bit floating point number can represent and subtract it afterwards.
		// This relies on the fact that if we had a fractional part, the new value cannot be represented accurately
		// and IEEE 754 will perform rounding for us. The default rounding mode is Banker's rounding.
		// This has the effect of removing the fractional part while simultaneously rounding.
		// Use the same sign as the input value to make sure we handle positive and negative values.
		const __m128 fractional_limit = _mm_set_ps1(8388608.0F); // 2^23
		__m128 truncating_offset = _mm_or_ps(sign, fractional_limit);
		__m128 integer_part = _mm_sub_ps(_mm_add_ps(input, truncating_offset), truncating_offset);

		// If our input was so large that it had no fractional part, return it unchanged
		// Otherwise return our integer part
		const __m128i abs_mask = _mm_set_epi32(0x7FFFFFFFULL, 0x7FFFFFFFULL, 0x7FFFFFFFULL, 0x7FFFFFFFULL);
		__m128 abs_input = _mm_and_ps(input, _mm_castsi128_ps(abs_mask));
		__m128 is_input_large = _mm_cmpge_ps(abs_input, fractional_limit);
		return _mm_or_ps(_mm_and_ps(is_input_large, input), _mm_andnot_ps(is_input_large, integer_part));
#else
		scalarf x = scalar_round_bankers(scalarf(vector_get_x(input)));
		scalarf y = scalar_round_bankers(scalarf(vector_get_y(input)));
		scalarf z = scalar_round_bankers(scalarf(vector_get_z(input)));
		scalarf w = scalar_round_bankers(scalarf(vector_get_w(input)));
		return vector_set(x, y, z, w);
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns per component the sine of the input angle.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_sin(vector4f_arg0 input) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		// Use a degree 11 minimax approximation polynomial
		// See: GPGPU Programming for Games and Science (David H. Eberly)

		// Remap our input in the [-pi, pi] range
		__m128 quotient = _mm_mul_ps(input, _mm_set_ps1(1.0F / 6.283185307179586476925286766559005768F));
		quotient = vector_round_bankers(quotient);
		quotient = _mm_mul_ps(quotient, _mm_set_ps1(6.283185307179586476925286766559005768F));
		__m128 x = _mm_sub_ps(input, quotient);

		// Remap our input in the [-pi/2, pi/2] range
		const __m128 sign_mask = _mm_set_ps(-0.0F, -0.0F, -0.0F, -0.0F);
		__m128 sign = _mm_and_ps(x, sign_mask);
		__m128 reference = _mm_or_ps(sign, _mm_set_ps1(3.141592653589793238462643383279502884F));

		const __m128 reflection = _mm_sub_ps(reference, x);
		const __m128i abs_mask = _mm_set_epi32(0x7FFFFFFFULL, 0x7FFFFFFFULL, 0x7FFFFFFFULL, 0x7FFFFFFFULL);
		const __m128 x_abs = _mm_and_ps(x, _mm_castsi128_ps(abs_mask));

		__m128 is_less_equal_than_half_pi = _mm_cmple_ps(x_abs, _mm_set_ps1(1.570796326794896619231321691639751442F));

#if defined(RTM_AVX_INTRINSICS)
		x = _mm_blendv_ps(reflection, x, is_less_equal_than_half_pi);
#else
		x = _mm_or_ps(_mm_andnot_ps(is_less_equal_than_half_pi, reflection), _mm_and_ps(x, is_less_equal_than_half_pi));
#endif

		// Calculate our value
		const __m128 x2 = _mm_mul_ps(x, x);
		__m128 result = _mm_add_ps(_mm_mul_ps(x2, _mm_set_ps1(-2.3828544692960918e-8F)), _mm_set_ps1(2.7521557770526783e-6F));
		result = _mm_add_ps(_mm_mul_ps(result, x2), _mm_set_ps1(-1.9840782426250314e-4F));
		result = _mm_add_ps(_mm_mul_ps(result, x2), _mm_set_ps1(8.3333303183525942e-3F));
		result = _mm_add_ps(_mm_mul_ps(result, x2), _mm_set_ps1(-1.6666666601721269e-1F));
		result = _mm_add_ps(_mm_mul_ps(result, x2), _mm_set_ps1(1.0F));
		result = _mm_mul_ps(result, x);
		return result;
#else
		scalarf x = scalar_sin(scalarf(vector_get_x(input)));
		scalarf y = scalar_sin(scalarf(vector_get_y(input)));
		scalarf z = scalar_sin(scalarf(vector_get_z(input)));
		scalarf w = scalar_sin(scalarf(vector_get_w(input)));
		return vector_set(x, y, z, w);
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns per component the arc-sine of the input.
	// Input value must be in the range [-1.0, 1.0].
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_asin(vector4f_arg0 input) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		// Use a degree 7 minimax approximation polynomial
		// See: GPGPU Programming for Games and Science (David H. Eberly)

		// We first calculate our scale: sqrt(1.0 - abs(value))
		// Use the sign bit to generate our absolute value since we'll re-use that constant
		const __m128 sign_bit = _mm_set_ps1(-0.0F);
		__m128 abs_value = _mm_andnot_ps(sign_bit, input);

		// Calculate our value
		__m128 result = _mm_add_ps(_mm_mul_ps(abs_value, _mm_set_ps1(-1.2690614339589956e-3F)), _mm_set_ps1(6.7072304676685235e-3F));
		result = _mm_add_ps(_mm_mul_ps(result, abs_value), _mm_set_ps1(-1.7162031184398074e-2F));
		result = _mm_add_ps(_mm_mul_ps(result, abs_value), _mm_set_ps1(3.0961594977611639e-2F));
		result = _mm_add_ps(_mm_mul_ps(result, abs_value), _mm_set_ps1(-5.0207843052845647e-2F));
		result = _mm_add_ps(_mm_mul_ps(result, abs_value), _mm_set_ps1(8.8986946573346160e-2F));
		result = _mm_add_ps(_mm_mul_ps(result, abs_value), _mm_set_ps1(-2.1459960076929829e-1F));
		result = _mm_add_ps(_mm_mul_ps(result, abs_value), _mm_set_ps1(1.5707963267948966F));

		// Scale our result
		__m128 scale = _mm_sqrt_ps(_mm_sub_ps(_mm_set_ps1(1.0F), abs_value));
		result = _mm_mul_ps(result, scale);

		// Normally the math is as follow:
		// If input is positive: PI/2 - result
		// If input is negative: PI/2 - (PI - result) = PI/2 - PI + result = -PI/2 + result

		// As such, the offset is PI/2 and it takes the sign of the input
		// This allows us to load a single constant from memory directly
		__m128 input_sign = _mm_and_ps(input, sign_bit);
		__m128 offset = _mm_or_ps(input_sign, _mm_set_ps1(1.570796326794896619231321691639751442F));

		// And our result has the opposite sign of the input
		result = _mm_xor_ps(result, _mm_xor_ps(input_sign, sign_bit));
		return _mm_add_ps(result, offset);
#else
		scalarf x = scalar_asin(scalarf(vector_get_x(input)));
		scalarf y = scalar_asin(scalarf(vector_get_y(input)));
		scalarf z = scalar_asin(scalarf(vector_get_z(input)));
		scalarf w = scalar_asin(scalarf(vector_get_w(input)));
		return vector_set(x, y, z, w);
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns per component the cosine of the input angle.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_cos(vector4f_arg0 input) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		// Use a degree 10 minimax approximation polynomial
		// See: GPGPU Programming for Games and Science (David H. Eberly)

		// Remap our input in the [-pi, pi] range
		__m128 quotient = _mm_mul_ps(input, _mm_set_ps1(1.0F / 6.283185307179586476925286766559005768F));
		quotient = vector_round_bankers(quotient);
		quotient = _mm_mul_ps(quotient, _mm_set_ps1(6.283185307179586476925286766559005768F));
		__m128 x = _mm_sub_ps(input, quotient);

		// Remap our input in the [-pi/2, pi/2] range
		const __m128 sign_mask = _mm_set_ps(-0.0F, -0.0F, -0.0F, -0.0F);
		__m128 x_sign = _mm_and_ps(x, sign_mask);
		__m128 reference = _mm_or_ps(x_sign, _mm_set_ps1(3.141592653589793238462643383279502884F));
		const __m128 reflection = _mm_sub_ps(reference, x);

		const __m128i abs_mask = _mm_set_epi32(0x7FFFFFFFULL, 0x7FFFFFFFULL, 0x7FFFFFFFULL, 0x7FFFFFFFULL);
		__m128 x_abs = _mm_and_ps(x, _mm_castsi128_ps(abs_mask));
		__m128 is_less_equal_than_half_pi = _mm_cmple_ps(x_abs, _mm_set_ps1(1.570796326794896619231321691639751442F));

#if defined(RTM_AVX_INTRINSICS)
		x = _mm_blendv_ps(reflection, x, is_less_equal_than_half_pi);
#else
		x = _mm_or_ps(_mm_andnot_ps(is_less_equal_than_half_pi, reflection), _mm_and_ps(x, is_less_equal_than_half_pi));
#endif

		// Calculate our value
		const __m128 x2 = _mm_mul_ps(x, x);
		__m128 result = _mm_add_ps(_mm_mul_ps(x2, _mm_set_ps1(-2.6051615464872668e-7F)), _mm_set_ps1(2.4760495088926859e-5F));
		result = _mm_add_ps(_mm_mul_ps(result, x2), _mm_set_ps1(-1.3888377661039897e-3F));
		result = _mm_add_ps(_mm_mul_ps(result, x2), _mm_set_ps1(4.1666638865338612e-2F));
		result = _mm_add_ps(_mm_mul_ps(result, x2), _mm_set_ps1(-4.9999999508695869e-1F));
		result = _mm_add_ps(_mm_mul_ps(result, x2), _mm_set_ps1(1.0F));

		// Remap into [-pi, pi]
		return _mm_or_ps(result, _mm_andnot_ps(is_less_equal_than_half_pi, sign_mask));
#else
		scalarf x = scalar_cos(scalarf(vector_get_x(input)));
		scalarf y = scalar_cos(scalarf(vector_get_y(input)));
		scalarf z = scalar_cos(scalarf(vector_get_z(input)));
		scalarf w = scalar_cos(scalarf(vector_get_w(input)));
		return vector_set(x, y, z, w);
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns per component the arc-cosine of the input.
	// Input value must be in the range [-1.0, 1.0].
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_acos(vector4f_arg0 input) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		// Use the identity: acos(value) + asin(value) = PI/2
		// This ends up being: acos(value) = PI/2 - asin(value)
		// Since asin(value) = PI/2 - sqrt(1.0 - polynomial(value))
		// Our end result is acos(value) = sqrt(1.0 - polynomial(value))
		// This means we can re-use the same polynomial as asin()
		// See: GPGPU Programming for Games and Science (David H. Eberly)

		// We first calculate our scale: sqrt(1.0 - abs(value))
		// Use the sign bit to generate our absolute value since we'll re-use that constant
		const __m128 sign_bit = _mm_set_ps1(-0.0F);
		__m128 abs_value = _mm_andnot_ps(sign_bit, input);

		// Calculate our value
		__m128 result = _mm_add_ps(_mm_mul_ps(abs_value, _mm_set_ps1(-1.2690614339589956e-3F)), _mm_set_ps1(6.7072304676685235e-3F));
		result = _mm_add_ps(_mm_mul_ps(result, abs_value), _mm_set_ps1(-1.7162031184398074e-2F));
		result = _mm_add_ps(_mm_mul_ps(result, abs_value), _mm_set_ps1(3.0961594977611639e-2F));
		result = _mm_add_ps(_mm_mul_ps(result, abs_value), _mm_set_ps1(-5.0207843052845647e-2F));
		result = _mm_add_ps(_mm_mul_ps(result, abs_value), _mm_set_ps1(8.8986946573346160e-2F));
		result = _mm_add_ps(_mm_mul_ps(result, abs_value), _mm_set_ps1(-2.1459960076929829e-1F));
		result = _mm_add_ps(_mm_mul_ps(result, abs_value), _mm_set_ps1(1.5707963267948966F));

		// Scale our result
		__m128 scale = _mm_sqrt_ps(_mm_sub_ps(_mm_set_ps1(1.0F), abs_value));
		result = _mm_mul_ps(result, scale);

		// Normally the math is as follow:
		// If input is positive: result
		// If input is negative: PI - result = -result + PI

		// As such, the offset is 0.0 when the input is positive and PI when negative
		__m128 is_input_negative = _mm_cmplt_ps(input, _mm_setzero_ps());
		__m128 offset = _mm_and_ps(is_input_negative, _mm_set_ps1(3.141592653589793238462643383279502884F));

		// And our result has the same sign of the input
		__m128 input_sign = _mm_and_ps(input, sign_bit);
		result = _mm_or_ps(result, input_sign);
		return _mm_add_ps(result, offset);
#else
		scalarf x = scalar_acos(scalarf(vector_get_x(input)));
		scalarf y = scalar_acos(scalarf(vector_get_y(input)));
		scalarf z = scalar_acos(scalarf(vector_get_z(input)));
		scalarf w = scalar_acos(scalarf(vector_get_w(input)));
		return vector_set(x, y, z, w);
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns per component the tangent of the input angle.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_tan(vector4f_arg0 angle) RTM_NO_EXCEPT
	{
		// Use the identity: tan(angle) = sin(angle) / cos(angle)
		vector4f sin_ = vector_sin(angle);
		vector4f cos_ = vector_cos(angle);

		mask4i is_cos_zero = vector_equal(cos_, vector_zero());
		vector4f signed_infinity = vector_copy_sign(vector_set(std::numeric_limits<float>::infinity()), angle);
		vector4f result = vector_div(sin_, cos_);
		return vector_select(is_cos_zero, signed_infinity, result);
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns per component the arc-tangent of the input.
	// Note that due to the sign ambiguity, atan cannot determine which quadrant
	// the value resides in.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_atan(vector4f_arg0 input) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		// Use a degree 13 minimax approximation polynomial
		// See: GPGPU Programming for Games and Science (David H. Eberly)

		// Discard our sign, we'll restore it later
		const __m128i abs_mask = _mm_set_epi32(0x7FFFFFFFULL, 0x7FFFFFFFULL, 0x7FFFFFFFULL, 0x7FFFFFFFULL);
		__m128 abs_value = _mm_and_ps(input, _mm_castsi128_ps(abs_mask));

		// Compute our value
		__m128 is_larger_than_one = _mm_cmpgt_ps(abs_value, _mm_set_ps1(1.0F));
		__m128 reciprocal = vector_reciprocal(abs_value);

		__m128 x = vector_select(is_larger_than_one, reciprocal, abs_value);

		__m128 x2 = _mm_mul_ps(x, x);

		__m128 result = _mm_add_ps(_mm_mul_ps(x2, _mm_set_ps1(7.2128853633444123e-3F)), _mm_set_ps1(-3.5059680836411644e-2F));
		result = _mm_add_ps(_mm_mul_ps(result, x2), _mm_set_ps1(8.1675882859940430e-2F));
		result = _mm_add_ps(_mm_mul_ps(result, x2), _mm_set_ps1(-1.3374657325451267e-1F));
		result = _mm_add_ps(_mm_mul_ps(result, x2), _mm_set_ps1(1.9856563505717162e-1F));
		result = _mm_add_ps(_mm_mul_ps(result, x2), _mm_set_ps1(-3.3324998579202170e-1F));
		result = _mm_add_ps(_mm_mul_ps(result, x2), _mm_set_ps1(1.0F));
		result = _mm_mul_ps(result, x);

		__m128 remapped = _mm_sub_ps(_mm_set_ps1(0.933189452F * 1.68325555F), result);

		// pi/2 - result, (0x3f6ee581 * 0x3fd774eb)
		result = vector_select(is_larger_than_one, remapped, result);

		// Keep the original sign
		return _mm_or_ps(result, _mm_and_ps(input, _mm_set_ps1(-0.0F)));
#else
		scalarf x = scalar_atan(scalarf(vector_get_x(input)));
		scalarf y = scalar_atan(scalarf(vector_get_y(input)));
		scalarf z = scalar_atan(scalarf(vector_get_z(input)));
		scalarf w = scalar_atan(scalarf(vector_get_w(input)));
		return vector_set(x, y, z, w);
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns per component the arc-tangent of [y/x] using the sign of the arguments to
	// determine the correct quadrant.
	// Y represents the proportion of the y-coordinate.
	// X represents the proportion of the x-coordinate.
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL vector_atan2(vector4f_arg0 y, vector4f_arg1 x) RTM_NO_EXCEPT
	{
#if defined(RTM_SSE2_INTRINSICS)
		// If X == 0.0 and Y != 0.0, we return PI/2 with the sign of Y
		// If X == 0.0 and Y == 0.0, we return 0.0
		// If X > 0.0, we return atan(y/x)
		// If X < 0.0, we return atan(y/x) + sign(Y) * PI
		// See: https://en.wikipedia.org/wiki/Atan2#Definition_and_computation

		const __m128 zero = _mm_setzero_ps();
		__m128 is_x_zero = _mm_cmpeq_ps(x, zero);
		__m128 is_y_zero = _mm_cmpeq_ps(y, zero);
		__m128 inputs_are_zero = _mm_and_ps(is_x_zero, is_y_zero);

		__m128 is_x_positive = _mm_cmpgt_ps(x, zero);

		const __m128 sign_mask = _mm_set_ps(-0.0F, -0.0F, -0.0F, -0.0F);
		__m128 y_sign = _mm_and_ps(y, sign_mask);

		// If X == 0.0, our offset is PI/2 otherwise it is PI both with the sign of Y
		__m128 half_pi = _mm_set_ps1(1.570796326794896619231321691639751442F);
		__m128 pi = _mm_set_ps1(3.141592653589793238462643383279502884F);
		__m128 offset = _mm_or_ps(_mm_and_ps(is_x_zero, half_pi), _mm_andnot_ps(is_x_zero, pi));
		offset = _mm_or_ps(offset, y_sign);

		// If X > 0.0, our offset is 0.0
		offset = _mm_andnot_ps(is_x_positive, offset);

		// If X == 0.0 and Y == 0.0, our offset is 0.0
		offset = _mm_andnot_ps(inputs_are_zero, offset);

		__m128 angle = _mm_div_ps(y, x);
		__m128 value = vector_atan(angle);

		// If X == 0.0, our value is 0.0 otherwise it is atan(y/x)
		value = _mm_or_ps(_mm_and_ps(is_x_zero, zero), _mm_andnot_ps(is_x_zero, value));

		// If X == 0.0 and Y == 0.0, our value is 0.0
		value = _mm_andnot_ps(inputs_are_zero, value);

		return _mm_add_ps(value, offset);
#else
		scalarf x_ = scalar_atan2(scalarf(vector_get_x(y)), scalarf(vector_get_x(x)));
		scalarf y_ = scalar_atan2(scalarf(vector_get_y(y)), scalarf(vector_get_y(x)));
		scalarf z_ = scalar_atan2(scalarf(vector_get_z(y)), scalarf(vector_get_z(x)));
		scalarf w_ = scalar_atan2(scalarf(vector_get_w(y)), scalarf(vector_get_w(x)));
		return vector_set(x_, y_, z_, w_);
#endif
	}
}

RTM_IMPL_FILE_PRAGMA_POP
