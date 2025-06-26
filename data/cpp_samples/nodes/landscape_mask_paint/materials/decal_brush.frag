/* Copyright (C) 2005-2020, UNIGINE. All rights reserved.
 *
 * This file is a part of the UNIGINE 2 SDK.
 *
 * Your use and / or redistribution of this software in source and / or
 * binary form, with or without modification, is subject to: (i) your
 * ongoing acceptance of and compliance with the terms and conditions of
 * the UNIGINE License Agreement; and (ii) your inclusion of this notice
 * in any version of this software that you use or redistribute.
 * A copy of the UNIGINE License Agreement is available by contacting
 * UNIGINE. at http://unigine.com/
 */

#include <core/materials/shaders/render/common.h>

STRUCT_FRAG_BEGIN
	INIT_MRT(float4, 0)
	INIT_MRT(float4, 1)
	#ifdef !WATER_DECAL
		INIT_MRT(float4, 2)
		INIT_MRT(float4, 3)
	#endif
STRUCT_FRAG_END

#ifdef BASE_MAPPING_TRIPLANAR
	#undef PARALLAX
#endif

#ifdef PARALLAX && PARALLAX_DEPTH_CUTOUT
	#define USE_CUSTOM_DEPTH
#endif

#ifdef PARALLAX
	#include <core/materials/shaders/api/parallax.h>
#endif

INIT_TEXTURE(0, TEX_ALBEDO)
INIT_TEXTURE(1, TEX_NORMAL)
INIT_TEXTURE(2, TEX_METALNESS)
INIT_TEXTURE(3, TEX_MASK)

#ifdef DETAIL
	#ifdef DETAIL_3D_TEXTURE
		INIT_TEXTURE_3D(4, TEX_ALBEDO_DETAIL)
		INIT_TEXTURE_3D(5, TEX_NORMAL_DETAIL)
		INIT_TEXTURE_3D(6, TEX_SHADING_DETAIL)
	#else
		INIT_TEXTURE(4, TEX_ALBEDO_DETAIL)
		INIT_TEXTURE(5, TEX_NORMAL_DETAIL)
		INIT_TEXTURE(6, TEX_SHADING_DETAIL)
	#endif
#endif

INIT_TEXTURE(12, TEX_DEPTH)
INIT_TEXTURE(13, TEX_GBUFFER_NORMAL)
#ifdef OUT_GBUFFER_MATERIAL_MASK
	INIT_TEXTURE_UINT(14, TEX_GBUFFER_MATERIAL_MASK)
#endif

#include <core/materials/base/objects/decal/shaders/fragment/common.h>

CBUFFER(parameters)
	
	UNIFORM float4 m_albedo;
	UNIFORM float4 m_shading;//metalness=X roughness=Y f0=Z microfiber=W
	
	UNIFORM float m_normal_scale;
	UNIFORM float m_translucent;
	UNIFORM float4 m_uv_transform;
	
	UNIFORM float4 m_visible;//albedo=X metalness=Y roughness=Z normal=W"
	
	UNIFORM float4 m_fade;// fade_threshold=X fade_pow=Y
	
	#ifdef BASE_MAPPING_TRIPLANAR
		UNIFORM float m_triplanar_blend;
	#endif
	
	#ifdef DETAIL
		UNIFORM float4	m_detail_albedo;
		UNIFORM float4	m_detail_shading;
		
		UNIFORM float	m_detail_albedo_visible;
		UNIFORM float4	m_detail_shading_visible;
		UNIFORM float	m_detail_normal_visible;
		
		UNIFORM float	m_detail_visible;
		UNIFORM float	m_detail_visible_threshold;
		
		UNIFORM float4	m_detail_uv_transform;
		
		#ifdef DETAIL_MAPPING_TRIPLANAR
			UNIFORM float m_detail_triplanar_blend;
		#endif
		
		#ifdef DETAIL_3D_TEXTURE
			UNIFORM float m_detail_3d_transform;
		#endif
	#endif
	
	#ifdef PARALLAX
		PARALLAX_PARAMETERS
		#ifdef PARALLAX_SHADOW
			PARALLAX_SHADOW_PARAMETERS
		#endif
	#endif
	
END

#define OUT_DECAL_ALBEDO	OUT_MRT(0)
#define OUT_DECAL_NORMAL	OUT_MRT(1)

#ifdef !WATER_DECAL
	#define OUT_DECAL_SHADING	OUT_MRT(2)
	#define OUT_DECAL_ROUGHNESS	OUT_MRT(3)
#endif

#ifdef DETAIL_BLEND_OVERLAY
	#define DETAIL_BLEND(V0, V1, K) V0 = overlay(V0, V1, K * detail_albedo.a);
#elif DETAIL_BLEND_MULTIPLY
	#define DETAIL_BLEND(V0, V1, K) V0 *= lerp(V1, float4_one, float4_one - K * detail_albedo.a);
#else
	#define DETAIL_BLEND(V0, V1, K) V0 = lerp(V0, V1, K * detail_albedo.a);
#endif

earlydepthstencil
MAIN_FRAG_BEGIN(FRAGMENT_IN)
	
	float4 color = m_albedo;
	
	#include <core/materials/base/objects/decal/shaders/fragment/common.h>
	
	#ifdef !SCREEN_PROJECTION
		color.a *= float(texcoord.z > 0.0f && texcoord.z < 1.0f);
	#endif

	// detail mapping
	#ifdef DETAIL
		
		#define UV_DETAIL(TYPE, TEXCOORD) TYPE detail_uv = uvTransform(TEXCOORD, m_detail_uv_transform);
		
		#ifdef DETAIL_MAPPING_TRIPLANAR
			UV_DETAIL(float4, world_position.xyyz * 0.5f + float4_one * 0.5f);
			WEIGHT_TRIPLANAR(detail_weight, m_detail_triplanar_blend)
		#elif DETAIL_MAPPING_WORLD
			UV_DETAIL(float2, texcoord_world.xy);
		#else
			UV_DETAIL(float2, texcoord.xy);
		#endif
		
		#ifdef DETAIL_3D_TEXTURE && DETAIL
			#ifdef DETAIL_MAPPING_TRIPLANAR
				#define TEXTURE_DETAIL(TEXTURE_ID)	(TEXTURE(TEXTURE_ID, float3(detail_uv.zw, m_detail_3d_transform)) * detail_weight.x + \
													 TEXTURE(TEXTURE_ID, float3(detail_uv.xw, m_detail_3d_transform)) * detail_weight.y + \
													 TEXTURE(TEXTURE_ID, float3(detail_uv.xy, m_detail_3d_transform)) * detail_weight.z )
			#else
				#define TEXTURE_DETAIL(TEXTURE_ID) TEXTURE(TEXTURE_ID, float3(detail_uv, m_detail_3d_transform))
			#endif
		#else
			#ifdef DETAIL_MAPPING_TRIPLANAR
				#define TEXTURE_DETAIL(TEXTURE_ID) TEXTURE_TRIPLANAR(TEXTURE_ID, detail_uv, detail_weight)
			#else
				#define TEXTURE_DETAIL(TEXTURE_ID) TEXTURE(TEXTURE_ID, detail_uv)
			#endif
		#endif
		
	#endif
	
	// parallax
	#ifdef PARALLAX
		
		#include <core/materials/shaders/api/parallax.h>
		
		#ifdef PARALLAX_SHADOW && OUT_DECAL_ALBEDO
			ParallaxShadowIn parallax_shadow_in;
			
			float3 light_dir = mul3(s_scattering_sun_dir, s_imodelview);
			parallax_shadow_in.light_ts.x = dot(decal_tangent, light_dir);
			parallax_shadow_in.light_ts.y = dot(decal_binormal, light_dir);
			parallax_shadow_in.light_ts.z = dot(geometry_n, light_dir);
			
			PARALLAX_SHADOW_INIT(parallax_shadow_in)
			
			color.rgb *= parallaxShadow(parallax_shadow_in, parallax_in, parallax, TEXTURE_OUT(TEX_PARALLAX));
		#endif
		
		parallax.uv_offset /= m_uv_transform.xy;
		
		#ifdef DETAIL && (!DETAIL_MAPPING_TRIPLANAR)
			detail_uv -= uvTransform(parallax.uv_offset, m_detail_uv_transform);
		#endif
		
	#endif
	
	#ifdef OPAQUE
		color.rgb *= TEXTURE_BASE(TEX_ALBEDO).rgb;
	#else
		color.rgb *= TEXTURE_BASE(TEX_ALBEDO).rgb;
		color.a *= TEXTURE_BASE(TEX_MASK).a;
	#endif
	
	#ifdef ANGLE_FADE && (!SCREEN_PROJECTION)
		color.a *= pow(saturate(dot(g_normal, decal_normal) - m_fade.z), max(EPSILON, m_fade.w));
	#endif
	
	#ifdef DISTANCE_FADE && (!SCREEN_PROJECTION)
		color.a *= pow(saturate(texcoord.z * m_fade.x), max(EPSILON, m_fade.y));
	#endif
	
	color.a = saturate(color.a * s_decal_fade);
	
	#ifdef ALPHA_TEST
		color.a = float(color.a > 0.5f);
	#endif
	
	#ifdef !SCREEN_PROJECTION
		if(color.a <= EPSILON) discard;
	#endif
	
	#ifdef DETAIL
		float4 detail_albedo = TEXTURE_DETAIL(TEX_ALBEDO_DETAIL) * m_detail_albedo;
		detail_albedo.a = saturate((detail_albedo.a - (1.0f - m_detail_visible * 0.5f) + m_detail_visible_threshold * 0.5f) / max(m_detail_visible_threshold, EPSILON));
		
		#ifdef DETAIL_ALPHA_BLENDABLE && DETAIL
			color.a *= detail_albedo.a;
		#endif
	#endif
		
	/******************************************************************************\
	*
	* OUT_DECAL_ALBEDO: albedo
	*
	\******************************************************************************/
	#ifdef OUT_DECAL_ALBEDO
		#ifdef DETAIL
			DETAIL_BLEND(color, detail_albedo, float4(1, 1, 1, 0) * m_detail_albedo_visible)
		#endif
		
		OUT_DECAL_ALBEDO.rgb = color.rgb;
		SET_DECAL_ALPHA(OUT_DECAL_ALBEDO, m_visible.x);
		
	#endif
	
	#ifdef OUT_DECAL_SHADING || OUT_DECAL_ROUGHNESS
		float4 shading = float4(TEXTURE_BASE(TEX_METALNESS).xy, 1.0f, 1.0f) * m_shading;
		#ifdef DETAIL
			float4 detail_shading = m_detail_shading;
			detail_shading.xy *= TEXTURE_DETAIL(TEX_SHADING_DETAIL).xy;
			
			DETAIL_BLEND(shading, detail_shading, m_detail_shading_visible)
		#endif
	#endif
	
	/******************************************************************************\
	*
	* OUT_DECAL_SHADING: metalness, specular, translucent
	*
	\******************************************************************************/
	#ifdef OUT_DECAL_SHADING
		OUT_DECAL_SHADING.r = shading.x; //metalness
		
		#ifdef SPECULAR_MAP
			OUT_DECAL_SHADING.g = shading.z; //specular
		#else
			OUT_DECAL_SHADING.g = m_shading.z; //specular
		#endif
		
		OUT_DECAL_SHADING.b = m_translucent; //translucent
		
		SET_DECAL_ALPHA(OUT_DECAL_SHADING, m_visible.y);
	#endif
	
	/******************************************************************************\
	*
	* OUT_DECAL_ROUGHNESS: roughness, microfiber
	*
	\******************************************************************************/
	#ifdef OUT_DECAL_ROUGHNESS
		OUT_DECAL_ROUGHNESS.r = shading.y; //roughness
		
		#ifdef MICROFIBER_MAP
			OUT_DECAL_ROUGHNESS.g = shading.w; //microfiber
		#else
			OUT_DECAL_ROUGHNESS.g = m_shading.w; //microfiber
		#endif
		
		OUT_DECAL_ROUGHNESS.r = lerpOne(OUT_DECAL_ROUGHNESS.r, OUT_DECAL_ROUGHNESS.g);
		
		SET_DECAL_ALPHA(OUT_DECAL_ROUGHNESS, m_visible.z);
	#endif
	
	#ifdef OUT_DECAL_NORMAL
		float3 normal = float3_zero;
		normal.xy = TEXTURE_BASE(TEX_NORMAL).xy;
		
		#ifdef DETAIL
			normal.xy += TEXTURE_DETAIL(TEX_NORMAL_DETAIL).xy * m_detail_normal_visible * detail_albedo.a;
		#endif
		
		normal.xy *= m_normal_scale;
		
		#ifdef NORMAL_SUBSTITUTE
			normal.z = normalReconstructZ(normal);
			normal =
				decal_tangent * normal.x +
				decal_binormal * normal.y +
				decal_normal * normal.z;
		#else
			normal =
				decal_tangent * normal.x +
				decal_binormal * normal.y;
		#endif
		
		OUT_DECAL_NORMAL.xyz = normal;
		
		#ifdef NORMAL_SUBSTITUTE
			OUT_DECAL_NORMAL.a = 1.0f - color.a * m_visible.w;
			OUT_DECAL_NORMAL.rgb *= 1.0f - OUT_DECAL_NORMAL.a;
		#else
			OUT_DECAL_NORMAL.a = 1.0f;
			OUT_DECAL_NORMAL.rgb *= color.a * m_visible.w;
		#endif
	#endif
	
MAIN_FRAG_END
