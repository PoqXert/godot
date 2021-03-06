/*************************************************************************/
/*  rendering_server_wrap_mt.h                                           */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2020 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2020 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#ifndef RENDERING_SERVER_WRAP_MT_H
#define RENDERING_SERVER_WRAP_MT_H

#include "core/command_queue_mt.h"
#include "core/os/thread.h"
#include "servers/rendering_server.h"

class RenderingServerWrapMT : public RenderingServer {
	// the real visual server
	mutable RenderingServer *rendering_server;

	mutable CommandQueueMT command_queue;

	static void _thread_callback(void *_instance);
	void thread_loop();

	Thread::ID server_thread;
	volatile bool exit;
	Thread *thread;
	volatile bool draw_thread_up;
	bool create_thread;

	uint64_t draw_pending;
	void thread_draw(bool p_swap_buffers, double frame_step);
	void thread_flush();

	void thread_exit();

	Mutex alloc_mutex;

	int pool_max_size;

	//#define DEBUG_SYNC

	static RenderingServerWrapMT *singleton_mt;

#ifdef DEBUG_SYNC
#define SYNC_DEBUG print_line("sync on: " + String(__FUNCTION__));
#else
#define SYNC_DEBUG
#endif

public:
#define ServerName RenderingServer
#define ServerNameWrapMT RenderingServerWrapMT
#define server_name rendering_server
#include "servers/server_wrap_mt_common.h"

	//these go pass-through, as they can be called from any thread
	virtual RID texture_2d_create(const Ref<Image> &p_image) { return rendering_server->texture_2d_create(p_image); }
	virtual RID texture_2d_layered_create(const Vector<Ref<Image>> &p_layers, TextureLayeredType p_layered_type) { return rendering_server->texture_2d_layered_create(p_layers, p_layered_type); }
	virtual RID texture_3d_create(const Vector<Ref<Image>> &p_slices) { return rendering_server->texture_3d_create(p_slices); }
	virtual RID texture_proxy_create(RID p_base) { return rendering_server->texture_proxy_create(p_base); }

	//goes pass-through
	virtual void texture_2d_update_immediate(RID p_texture, const Ref<Image> &p_image, int p_layer = 0) { rendering_server->texture_2d_update_immediate(p_texture, p_image, p_layer); }
	//these go through command queue if they are in another thread
	FUNC3(texture_2d_update, RID, const Ref<Image> &, int)
	FUNC4(texture_3d_update, RID, const Ref<Image> &, int, int)
	FUNC2(texture_proxy_update, RID, RID)

	//these also go pass-through
	virtual RID texture_2d_placeholder_create() { return rendering_server->texture_2d_placeholder_create(); }
	virtual RID texture_2d_layered_placeholder_create(TextureLayeredType p_type) { return rendering_server->texture_2d_layered_placeholder_create(p_type); }
	virtual RID texture_3d_placeholder_create() { return rendering_server->texture_3d_placeholder_create(); }

	FUNC1RC(Ref<Image>, texture_2d_get, RID)
	FUNC2RC(Ref<Image>, texture_2d_layer_get, RID, int)
	FUNC3RC(Ref<Image>, texture_3d_slice_get, RID, int, int)

	FUNC2(texture_replace, RID, RID)

	FUNC3(texture_set_size_override, RID, int, int)
// FIXME: Disabled during Vulkan refactoring, should be ported.
#if 0
	FUNC2(texture_bind, RID, uint32_t)
#endif

	FUNC3(texture_set_detect_3d_callback, RID, TextureDetectCallback, void *)
	FUNC3(texture_set_detect_normal_callback, RID, TextureDetectCallback, void *)
	FUNC3(texture_set_detect_roughness_callback, RID, TextureDetectRoughnessCallback, void *)

	FUNC2(texture_set_path, RID, const String &)
	FUNC1RC(String, texture_get_path, RID)
	FUNC1S(texture_debug_usage, List<TextureInfo> *)

	FUNC2(texture_set_force_redraw_if_visible, RID, bool)

	/* SHADER API */

	FUNCRID(shader)

	FUNC2(shader_set_code, RID, const String &)
	FUNC1RC(String, shader_get_code, RID)

	FUNC2SC(shader_get_param_list, RID, List<PropertyInfo> *)

	FUNC3(shader_set_default_texture_param, RID, const StringName &, RID)
	FUNC2RC(RID, shader_get_default_texture_param, RID, const StringName &)
	FUNC2RC(Variant, shader_get_param_default, RID, const StringName &)

	/* COMMON MATERIAL API */

	FUNCRID(material)

	FUNC2(material_set_shader, RID, RID)

	FUNC3(material_set_param, RID, const StringName &, const Variant &)
	FUNC2RC(Variant, material_get_param, RID, const StringName &)

	FUNC2(material_set_render_priority, RID, int)
	FUNC2(material_set_next_pass, RID, RID)

	/* MESH API */

	virtual RID mesh_create_from_surfaces(const Vector<SurfaceData> &p_surfaces) {
		return rendering_server->mesh_create_from_surfaces(p_surfaces);
	}

	FUNCRID(mesh)

	FUNC2(mesh_add_surface, RID, const SurfaceData &)

	FUNC1RC(int, mesh_get_blend_shape_count, RID)

	FUNC2(mesh_set_blend_shape_mode, RID, BlendShapeMode)
	FUNC1RC(BlendShapeMode, mesh_get_blend_shape_mode, RID)

	FUNC4(mesh_surface_update_region, RID, int, int, const Vector<uint8_t> &)

	FUNC3(mesh_surface_set_material, RID, int, RID)
	FUNC2RC(RID, mesh_surface_get_material, RID, int)

	FUNC2RC(SurfaceData, mesh_get_surface, RID, int)

	FUNC1RC(int, mesh_get_surface_count, RID)

	FUNC2(mesh_set_custom_aabb, RID, const AABB &)
	FUNC1RC(AABB, mesh_get_custom_aabb, RID)

	FUNC1(mesh_clear, RID)

	/* MULTIMESH API */

	FUNCRID(multimesh)

	FUNC5(multimesh_allocate, RID, int, MultimeshTransformFormat, bool, bool)
	FUNC1RC(int, multimesh_get_instance_count, RID)

	FUNC2(multimesh_set_mesh, RID, RID)
	FUNC3(multimesh_instance_set_transform, RID, int, const Transform &)
	FUNC3(multimesh_instance_set_transform_2d, RID, int, const Transform2D &)
	FUNC3(multimesh_instance_set_color, RID, int, const Color &)
	FUNC3(multimesh_instance_set_custom_data, RID, int, const Color &)

	FUNC1RC(RID, multimesh_get_mesh, RID)
	FUNC1RC(AABB, multimesh_get_aabb, RID)

	FUNC2RC(Transform, multimesh_instance_get_transform, RID, int)
	FUNC2RC(Transform2D, multimesh_instance_get_transform_2d, RID, int)
	FUNC2RC(Color, multimesh_instance_get_color, RID, int)
	FUNC2RC(Color, multimesh_instance_get_custom_data, RID, int)

	FUNC2(multimesh_set_buffer, RID, const Vector<float> &)
	FUNC1RC(Vector<float>, multimesh_get_buffer, RID)

	FUNC2(multimesh_set_visible_instances, RID, int)
	FUNC1RC(int, multimesh_get_visible_instances, RID)

	/* IMMEDIATE API */

	FUNCRID(immediate)
	FUNC3(immediate_begin, RID, PrimitiveType, RID)
	FUNC2(immediate_vertex, RID, const Vector3 &)
	FUNC2(immediate_normal, RID, const Vector3 &)
	FUNC2(immediate_tangent, RID, const Plane &)
	FUNC2(immediate_color, RID, const Color &)
	FUNC2(immediate_uv, RID, const Vector2 &)
	FUNC2(immediate_uv2, RID, const Vector2 &)
	FUNC1(immediate_end, RID)
	FUNC1(immediate_clear, RID)
	FUNC2(immediate_set_material, RID, RID)
	FUNC1RC(RID, immediate_get_material, RID)

	/* SKELETON API */

	FUNCRID(skeleton)
	FUNC3(skeleton_allocate, RID, int, bool)
	FUNC1RC(int, skeleton_get_bone_count, RID)
	FUNC3(skeleton_bone_set_transform, RID, int, const Transform &)
	FUNC2RC(Transform, skeleton_bone_get_transform, RID, int)
	FUNC3(skeleton_bone_set_transform_2d, RID, int, const Transform2D &)
	FUNC2RC(Transform2D, skeleton_bone_get_transform_2d, RID, int)
	FUNC2(skeleton_set_base_transform_2d, RID, const Transform2D &)

	/* Light API */

	FUNCRID(directional_light)
	FUNCRID(omni_light)
	FUNCRID(spot_light)

	FUNC2(light_set_color, RID, const Color &)
	FUNC3(light_set_param, RID, LightParam, float)
	FUNC2(light_set_shadow, RID, bool)
	FUNC2(light_set_shadow_color, RID, const Color &)
	FUNC2(light_set_projector, RID, RID)
	FUNC2(light_set_negative, RID, bool)
	FUNC2(light_set_cull_mask, RID, uint32_t)
	FUNC2(light_set_reverse_cull_face_mode, RID, bool)
	FUNC2(light_set_use_gi, RID, bool)

	FUNC2(light_omni_set_shadow_mode, RID, LightOmniShadowMode)

	FUNC2(light_directional_set_shadow_mode, RID, LightDirectionalShadowMode)
	FUNC2(light_directional_set_blend_splits, RID, bool)
	FUNC2(light_directional_set_shadow_depth_range_mode, RID, LightDirectionalShadowDepthRangeMode)

	/* PROBE API */

	FUNCRID(reflection_probe)

	FUNC2(reflection_probe_set_update_mode, RID, ReflectionProbeUpdateMode)
	FUNC2(reflection_probe_set_intensity, RID, float)
	FUNC2(reflection_probe_set_interior_ambient, RID, const Color &)
	FUNC2(reflection_probe_set_interior_ambient_energy, RID, float)
	FUNC2(reflection_probe_set_interior_ambient_probe_contribution, RID, float)
	FUNC2(reflection_probe_set_max_distance, RID, float)
	FUNC2(reflection_probe_set_extents, RID, const Vector3 &)
	FUNC2(reflection_probe_set_origin_offset, RID, const Vector3 &)
	FUNC2(reflection_probe_set_as_interior, RID, bool)
	FUNC2(reflection_probe_set_enable_box_projection, RID, bool)
	FUNC2(reflection_probe_set_enable_shadows, RID, bool)
	FUNC2(reflection_probe_set_cull_mask, RID, uint32_t)
	FUNC2(reflection_probe_set_resolution, RID, int)

	/* DECAL API */

	FUNCRID(decal)

	FUNC2(decal_set_extents, RID, const Vector3 &)
	FUNC3(decal_set_texture, RID, DecalTexture, RID)
	FUNC2(decal_set_emission_energy, RID, float)
	FUNC2(decal_set_albedo_mix, RID, float)
	FUNC2(decal_set_modulate, RID, const Color &)
	FUNC2(decal_set_cull_mask, RID, uint32_t)
	FUNC4(decal_set_distance_fade, RID, bool, float, float)
	FUNC3(decal_set_fade, RID, float, float)
	FUNC2(decal_set_normal_fade, RID, float)

	/* BAKED LIGHT API */

	FUNCRID(gi_probe)

	FUNC8(gi_probe_allocate, RID, const Transform &, const AABB &, const Vector3i &, const Vector<uint8_t> &, const Vector<uint8_t> &, const Vector<uint8_t> &, const Vector<int> &)

	FUNC1RC(AABB, gi_probe_get_bounds, RID)
	FUNC1RC(Vector3i, gi_probe_get_octree_size, RID)
	FUNC1RC(Vector<uint8_t>, gi_probe_get_octree_cells, RID)
	FUNC1RC(Vector<uint8_t>, gi_probe_get_data_cells, RID)
	FUNC1RC(Vector<uint8_t>, gi_probe_get_distance_field, RID)
	FUNC1RC(Vector<int>, gi_probe_get_level_counts, RID)
	FUNC1RC(Transform, gi_probe_get_to_cell_xform, RID)

	FUNC2(gi_probe_set_dynamic_range, RID, float)
	FUNC1RC(float, gi_probe_get_dynamic_range, RID)

	FUNC2(gi_probe_set_propagation, RID, float)
	FUNC1RC(float, gi_probe_get_propagation, RID)

	FUNC2(gi_probe_set_energy, RID, float)
	FUNC1RC(float, gi_probe_get_energy, RID)

	FUNC2(gi_probe_set_ao, RID, float)
	FUNC1RC(float, gi_probe_get_ao, RID)

	FUNC2(gi_probe_set_ao_size, RID, float)
	FUNC1RC(float, gi_probe_get_ao_size, RID)

	FUNC2(gi_probe_set_bias, RID, float)
	FUNC1RC(float, gi_probe_get_bias, RID)

	FUNC2(gi_probe_set_normal_bias, RID, float)
	FUNC1RC(float, gi_probe_get_normal_bias, RID)

	FUNC2(gi_probe_set_interior, RID, bool)
	FUNC1RC(bool, gi_probe_is_interior, RID)

	FUNC2(gi_probe_set_use_two_bounces, RID, bool)
	FUNC1RC(bool, gi_probe_is_using_two_bounces, RID)

	FUNC2(gi_probe_set_anisotropy_strength, RID, float)
	FUNC1RC(float, gi_probe_get_anisotropy_strength, RID)

	/* LIGHTMAP CAPTURE */

	FUNCRID(lightmap)
	FUNC3(lightmap_set_textures, RID, RID, bool)
	FUNC2(lightmap_set_probe_bounds, RID, const AABB &)
	FUNC2(lightmap_set_probe_interior, RID, bool)
	FUNC5(lightmap_set_probe_capture_data, RID, const PackedVector3Array &, const PackedColorArray &, const PackedInt32Array &, const PackedInt32Array &)
	FUNC1RC(PackedVector3Array, lightmap_get_probe_capture_points, RID)
	FUNC1RC(PackedColorArray, lightmap_get_probe_capture_sh, RID)
	FUNC1RC(PackedInt32Array, lightmap_get_probe_capture_tetrahedra, RID)
	FUNC1RC(PackedInt32Array, lightmap_get_probe_capture_bsp_tree, RID)

	FUNC1(lightmap_set_probe_capture_update_speed, float)

	/* PARTICLES */

	FUNCRID(particles)

	FUNC2(particles_set_emitting, RID, bool)
	FUNC1R(bool, particles_get_emitting, RID)
	FUNC2(particles_set_amount, RID, int)
	FUNC2(particles_set_lifetime, RID, float)
	FUNC2(particles_set_one_shot, RID, bool)
	FUNC2(particles_set_pre_process_time, RID, float)
	FUNC2(particles_set_explosiveness_ratio, RID, float)
	FUNC2(particles_set_randomness_ratio, RID, float)
	FUNC2(particles_set_custom_aabb, RID, const AABB &)
	FUNC2(particles_set_speed_scale, RID, float)
	FUNC2(particles_set_use_local_coordinates, RID, bool)
	FUNC2(particles_set_process_material, RID, RID)
	FUNC2(particles_set_fixed_fps, RID, int)
	FUNC2(particles_set_fractional_delta, RID, bool)
	FUNC1R(bool, particles_is_inactive, RID)
	FUNC1(particles_request_process, RID)
	FUNC1(particles_restart, RID)

	FUNC2(particles_set_draw_order, RID, RS::ParticlesDrawOrder)

	FUNC2(particles_set_draw_passes, RID, int)
	FUNC3(particles_set_draw_pass_mesh, RID, int, RID)
	FUNC2(particles_set_emission_transform, RID, const Transform &)

	FUNC1R(AABB, particles_get_current_aabb, RID)

	/* CAMERA API */

	FUNCRID(camera)
	FUNC4(camera_set_perspective, RID, float, float, float)
	FUNC4(camera_set_orthogonal, RID, float, float, float)
	FUNC5(camera_set_frustum, RID, float, Vector2, float, float)
	FUNC2(camera_set_transform, RID, const Transform &)
	FUNC2(camera_set_cull_mask, RID, uint32_t)
	FUNC2(camera_set_environment, RID, RID)
	FUNC2(camera_set_camera_effects, RID, RID)
	FUNC2(camera_set_use_vertical_aspect, RID, bool)

	/* VIEWPORT TARGET API */

	FUNCRID(viewport)

	FUNC2(viewport_set_use_xr, RID, bool)

	FUNC3(viewport_set_size, RID, int, int)

	FUNC2(viewport_set_active, RID, bool)
	FUNC2(viewport_set_parent_viewport, RID, RID)

	FUNC2(viewport_set_clear_mode, RID, ViewportClearMode)

	FUNC3(viewport_attach_to_screen, RID, const Rect2 &, DisplayServer::WindowID)
	FUNC2(viewport_set_render_direct_to_screen, RID, bool)

	FUNC2(viewport_set_update_mode, RID, ViewportUpdateMode)

	FUNC1RC(RID, viewport_get_texture, RID)

	FUNC2(viewport_set_hide_scenario, RID, bool)
	FUNC2(viewport_set_hide_canvas, RID, bool)
	FUNC2(viewport_set_disable_environment, RID, bool)

	FUNC2(viewport_attach_camera, RID, RID)
	FUNC2(viewport_set_scenario, RID, RID)
	FUNC2(viewport_attach_canvas, RID, RID)

	FUNC2(viewport_remove_canvas, RID, RID)
	FUNC3(viewport_set_canvas_transform, RID, RID, const Transform2D &)
	FUNC2(viewport_set_transparent_background, RID, bool)

	FUNC2(viewport_set_global_canvas_transform, RID, const Transform2D &)
	FUNC4(viewport_set_canvas_stacking, RID, RID, int, int)
	FUNC2(viewport_set_shadow_atlas_size, RID, int)
	FUNC3(viewport_set_shadow_atlas_quadrant_subdivision, RID, int, int)
	FUNC2(viewport_set_msaa, RID, ViewportMSAA)
	FUNC2(viewport_set_screen_space_aa, RID, ViewportScreenSpaceAA)

	//this passes directly to avoid stalling, but it's pretty dangerous, so don't call after freeing a viewport
	virtual int viewport_get_render_info(RID p_viewport, ViewportRenderInfo p_info) {
		return rendering_server->viewport_get_render_info(p_viewport, p_info);
	}

	FUNC2(viewport_set_debug_draw, RID, ViewportDebugDraw)

	FUNC2(viewport_set_measure_render_time, RID, bool)
	virtual float viewport_get_measured_render_time_cpu(RID p_viewport) const {
		return rendering_server->viewport_get_measured_render_time_cpu(p_viewport);
	}
	virtual float viewport_get_measured_render_time_gpu(RID p_viewport) const {
		return rendering_server->viewport_get_measured_render_time_gpu(p_viewport);
	}

	FUNC1(directional_shadow_atlas_set_size, int)

	/* SKY API */

	FUNCRID(sky)
	FUNC2(sky_set_radiance_size, RID, int)
	FUNC2(sky_set_mode, RID, SkyMode)
	FUNC2(sky_set_material, RID, RID)
	FUNC4R(Ref<Image>, sky_bake_panorama, RID, float, bool, const Size2i &)

	/* ENVIRONMENT API */

	FUNCRID(environment)

	FUNC2(environment_set_background, RID, EnvironmentBG)
	FUNC2(environment_set_sky, RID, RID)
	FUNC2(environment_set_sky_custom_fov, RID, float)
	FUNC2(environment_set_sky_orientation, RID, const Basis &)
	FUNC2(environment_set_bg_color, RID, const Color &)
	FUNC2(environment_set_bg_energy, RID, float)
	FUNC2(environment_set_canvas_max_layer, RID, int)
	FUNC7(environment_set_ambient_light, RID, const Color &, EnvironmentAmbientSource, float, float, EnvironmentReflectionSource, const Color &)

// FIXME: Disabled during Vulkan refactoring, should be ported.
#if 0
	FUNC2(environment_set_camera_feed_id, RID, int)
#endif
	FUNC6(environment_set_ssr, RID, bool, int, float, float, float)
	FUNC1(environment_set_ssr_roughness_quality, EnvironmentSSRRoughnessQuality)

	FUNC9(environment_set_ssao, RID, bool, float, float, float, float, float, EnvironmentSSAOBlur, float)

	FUNC2(environment_set_ssao_quality, EnvironmentSSAOQuality, bool)

	FUNC11(environment_set_glow, RID, bool, int, float, float, float, float, EnvironmentGlowBlendMode, float, float, float)
	FUNC1(environment_glow_set_use_bicubic_upscale, bool)

	FUNC9(environment_set_tonemap, RID, EnvironmentToneMapper, float, float, bool, float, float, float, float)

	FUNC6(environment_set_adjustment, RID, bool, float, float, float, RID)

	FUNC5(environment_set_fog, RID, bool, const Color &, const Color &, float)
	FUNC7(environment_set_fog_depth, RID, bool, float, float, float, bool, float)
	FUNC5(environment_set_fog_height, RID, bool, float, float, float)

	FUNC3R(Ref<Image>, environment_bake_panorama, RID, bool, const Size2i &)

	FUNC2(screen_space_roughness_limiter_set_active, bool, float)
	FUNC1(sub_surface_scattering_set_quality, SubSurfaceScatteringQuality)
	FUNC2(sub_surface_scattering_set_scale, float, float)

	FUNCRID(camera_effects)

	FUNC2(camera_effects_set_dof_blur_quality, DOFBlurQuality, bool)
	FUNC1(camera_effects_set_dof_blur_bokeh_shape, DOFBokehShape)

	FUNC8(camera_effects_set_dof_blur, RID, bool, float, float, bool, float, float, float)
	FUNC3(camera_effects_set_custom_exposure, RID, bool, float)

	FUNC1(shadows_quality_set, ShadowQuality);
	FUNC1(directional_shadow_quality_set, ShadowQuality);

	FUNCRID(scenario)

	FUNC2(scenario_set_debug, RID, ScenarioDebugMode)
	FUNC2(scenario_set_environment, RID, RID)
	FUNC2(scenario_set_camera_effects, RID, RID)
	FUNC2(scenario_set_fallback_environment, RID, RID)

	/* INSTANCING API */
	FUNCRID(instance)

	FUNC2(instance_set_base, RID, RID)
	FUNC2(instance_set_scenario, RID, RID)
	FUNC2(instance_set_layer_mask, RID, uint32_t)
	FUNC2(instance_set_transform, RID, const Transform &)
	FUNC2(instance_attach_object_instance_id, RID, ObjectID)
	FUNC3(instance_set_blend_shape_weight, RID, int, float)
	FUNC3(instance_set_surface_material, RID, int, RID)
	FUNC2(instance_set_visible, RID, bool)

	FUNC2(instance_set_custom_aabb, RID, AABB)

	FUNC2(instance_attach_skeleton, RID, RID)
	FUNC2(instance_set_exterior, RID, bool)

	FUNC2(instance_set_extra_visibility_margin, RID, real_t)

	// don't use these in a game!
	FUNC2RC(Vector<ObjectID>, instances_cull_aabb, const AABB &, RID)
	FUNC3RC(Vector<ObjectID>, instances_cull_ray, const Vector3 &, const Vector3 &, RID)
	FUNC2RC(Vector<ObjectID>, instances_cull_convex, const Vector<Plane> &, RID)

	FUNC3(instance_geometry_set_flag, RID, InstanceFlags, bool)
	FUNC2(instance_geometry_set_cast_shadows_setting, RID, ShadowCastingSetting)
	FUNC2(instance_geometry_set_material_override, RID, RID)

	FUNC5(instance_geometry_set_draw_range, RID, float, float, float, float)
	FUNC2(instance_geometry_set_as_instance_lod, RID, RID)
	FUNC4(instance_geometry_set_lightmap, RID, RID, const Rect2 &, int)

	FUNC3(instance_geometry_set_shader_parameter, RID, const StringName &, const Variant &)
	FUNC2RC(Variant, instance_geometry_get_shader_parameter, RID, const StringName &)
	FUNC2RC(Variant, instance_geometry_get_shader_parameter_default_value, RID, const StringName &)
	FUNC2SC(instance_geometry_get_shader_parameter_list, RID, List<PropertyInfo> *)

	/* BAKE */

	FUNC3R(TypedArray<Image>, bake_render_uv2, RID, const Vector<RID> &, const Size2i &)

	/* CANVAS (2D) */

	FUNCRID(canvas)
	FUNC3(canvas_set_item_mirroring, RID, RID, const Point2 &)
	FUNC2(canvas_set_modulate, RID, const Color &)
	FUNC3(canvas_set_parent, RID, RID, float)
	FUNC1(canvas_set_disable_scale, bool)

	FUNCRID(canvas_item)
	FUNC2(canvas_item_set_parent, RID, RID)

	FUNC2(canvas_item_set_visible, RID, bool)
	FUNC2(canvas_item_set_light_mask, RID, int)

	FUNC2(canvas_item_set_update_when_visible, RID, bool)

	FUNC2(canvas_item_set_transform, RID, const Transform2D &)
	FUNC2(canvas_item_set_clip, RID, bool)
	FUNC2(canvas_item_set_distance_field_mode, RID, bool)
	FUNC3(canvas_item_set_custom_rect, RID, bool, const Rect2 &)
	FUNC2(canvas_item_set_modulate, RID, const Color &)
	FUNC2(canvas_item_set_self_modulate, RID, const Color &)

	FUNC2(canvas_item_set_draw_behind_parent, RID, bool)

	FUNC2(canvas_item_set_default_texture_filter, RID, CanvasItemTextureFilter)
	FUNC2(canvas_item_set_default_texture_repeat, RID, CanvasItemTextureRepeat)

	FUNC5(canvas_item_add_line, RID, const Point2 &, const Point2 &, const Color &, float)
	FUNC4(canvas_item_add_polyline, RID, const Vector<Point2> &, const Vector<Color> &, float)
	FUNC4(canvas_item_add_multiline, RID, const Vector<Point2> &, const Vector<Color> &, float)
	FUNC3(canvas_item_add_rect, RID, const Rect2 &, const Color &)
	FUNC4(canvas_item_add_circle, RID, const Point2 &, float, const Color &)
	FUNC11(canvas_item_add_texture_rect, RID, const Rect2 &, RID, bool, const Color &, bool, RID, RID, const Color &, CanvasItemTextureFilter, CanvasItemTextureRepeat)
	FUNC12(canvas_item_add_texture_rect_region, RID, const Rect2 &, RID, const Rect2 &, const Color &, bool, RID, RID, const Color &, bool, CanvasItemTextureFilter, CanvasItemTextureRepeat)
	FUNC15(canvas_item_add_nine_patch, RID, const Rect2 &, const Rect2 &, RID, const Vector2 &, const Vector2 &, NinePatchAxisMode, NinePatchAxisMode, bool, const Color &, RID, RID, const Color &, CanvasItemTextureFilter, CanvasItemTextureRepeat)
	FUNC11(canvas_item_add_primitive, RID, const Vector<Point2> &, const Vector<Color> &, const Vector<Point2> &, RID, float, RID, RID, const Color &, CanvasItemTextureFilter, CanvasItemTextureRepeat)
	FUNC10(canvas_item_add_polygon, RID, const Vector<Point2> &, const Vector<Color> &, const Vector<Point2> &, RID, RID, RID, const Color &, CanvasItemTextureFilter, CanvasItemTextureRepeat)
	FUNC14(canvas_item_add_triangle_array, RID, const Vector<int> &, const Vector<Point2> &, const Vector<Color> &, const Vector<Point2> &, const Vector<int> &, const Vector<float> &, RID, int, RID, RID, const Color &, CanvasItemTextureFilter, CanvasItemTextureRepeat)
	FUNC10(canvas_item_add_mesh, RID, const RID &, const Transform2D &, const Color &, RID, RID, RID, const Color &, CanvasItemTextureFilter, CanvasItemTextureRepeat)
	FUNC8(canvas_item_add_multimesh, RID, RID, RID, RID, RID, const Color &, CanvasItemTextureFilter, CanvasItemTextureRepeat)
	FUNC8(canvas_item_add_particles, RID, RID, RID, RID, RID, const Color &, CanvasItemTextureFilter, CanvasItemTextureRepeat)
	FUNC2(canvas_item_add_set_transform, RID, const Transform2D &)
	FUNC2(canvas_item_add_clip_ignore, RID, bool)
	FUNC2(canvas_item_set_sort_children_by_y, RID, bool)
	FUNC2(canvas_item_set_z_index, RID, int)
	FUNC2(canvas_item_set_z_as_relative_to_parent, RID, bool)
	FUNC3(canvas_item_set_copy_to_backbuffer, RID, bool, const Rect2 &)
	FUNC2(canvas_item_attach_skeleton, RID, RID)

	FUNC1(canvas_item_clear, RID)
	FUNC2(canvas_item_set_draw_index, RID, int)

	FUNC2(canvas_item_set_material, RID, RID)

	FUNC2(canvas_item_set_use_parent_material, RID, bool)

	FUNC0R(RID, canvas_light_create)
	FUNC2(canvas_light_attach_to_canvas, RID, RID)
	FUNC2(canvas_light_set_enabled, RID, bool)
	FUNC2(canvas_light_set_scale, RID, float)
	FUNC2(canvas_light_set_transform, RID, const Transform2D &)
	FUNC2(canvas_light_set_texture, RID, RID)
	FUNC2(canvas_light_set_texture_offset, RID, const Vector2 &)
	FUNC2(canvas_light_set_color, RID, const Color &)
	FUNC2(canvas_light_set_height, RID, float)
	FUNC2(canvas_light_set_energy, RID, float)
	FUNC3(canvas_light_set_z_range, RID, int, int)
	FUNC3(canvas_light_set_layer_range, RID, int, int)
	FUNC2(canvas_light_set_item_cull_mask, RID, int)
	FUNC2(canvas_light_set_item_shadow_cull_mask, RID, int)

	FUNC2(canvas_light_set_mode, RID, CanvasLightMode)

	FUNC2(canvas_light_set_shadow_enabled, RID, bool)
	FUNC2(canvas_light_set_shadow_buffer_size, RID, int)
	FUNC2(canvas_light_set_shadow_filter, RID, CanvasLightShadowFilter)
	FUNC2(canvas_light_set_shadow_color, RID, const Color &)
	FUNC2(canvas_light_set_shadow_smooth, RID, float)

	FUNCRID(canvas_light_occluder)
	FUNC2(canvas_light_occluder_attach_to_canvas, RID, RID)
	FUNC2(canvas_light_occluder_set_enabled, RID, bool)
	FUNC2(canvas_light_occluder_set_polygon, RID, RID)
	FUNC2(canvas_light_occluder_set_transform, RID, const Transform2D &)
	FUNC2(canvas_light_occluder_set_light_mask, RID, int)

	FUNCRID(canvas_occluder_polygon)
	FUNC3(canvas_occluder_polygon_set_shape, RID, const Vector<Vector2> &, bool)
	FUNC2(canvas_occluder_polygon_set_shape_as_lines, RID, const Vector<Vector2> &)

	FUNC2(canvas_occluder_polygon_set_cull_mode, RID, CanvasOccluderPolygonCullMode)

	/* GLOBAL VARIABLES */

	FUNC3(global_variable_add, const StringName &, GlobalVariableType, const Variant &)
	FUNC1(global_variable_remove, const StringName &)
	FUNC0RC(Vector<StringName>, global_variable_get_list)
	FUNC2(global_variable_set, const StringName &, const Variant &)
	FUNC2(global_variable_set_override, const StringName &, const Variant &)
	FUNC1RC(GlobalVariableType, global_variable_get_type, const StringName &)
	FUNC1RC(Variant, global_variable_get, const StringName &)
	FUNC1(global_variables_load_settings, bool)
	FUNC0(global_variables_clear)

	/* BLACK BARS */

	FUNC4(black_bars_set_margins, int, int, int, int)
	FUNC4(black_bars_set_images, RID, RID, RID, RID)

	/* FREE */

	FUNC1(free, RID)

	/* EVENT QUEUING */

	FUNC3(request_frame_drawn_callback, Object *, const StringName &, const Variant &)

	virtual void init();
	virtual void finish();
	virtual void draw(bool p_swap_buffers, double frame_step);
	virtual void sync();
	FUNC0RC(bool, has_changed)

	/* RENDER INFO */

	//this passes directly to avoid stalling
	virtual int get_render_info(RenderInfo p_info) {
		return rendering_server->get_render_info(p_info);
	}

	virtual String get_video_adapter_name() const {
		return rendering_server->get_video_adapter_name();
	}

	virtual String get_video_adapter_vendor() const {
		return rendering_server->get_video_adapter_vendor();
	}

	FUNC4(set_boot_image, const Ref<Image> &, const Color &, bool, bool)
	FUNC1(set_default_clear_color, const Color &)

	FUNC0R(RID, get_test_cube)

	FUNC1(set_debug_generate_wireframes, bool)

	virtual bool has_feature(Features p_feature) const {
		return rendering_server->has_feature(p_feature);
	}
	virtual bool has_os_feature(const String &p_feature) const {
		return rendering_server->has_os_feature(p_feature);
	}

	FUNC1(call_set_use_vsync, bool)

	static void set_use_vsync_callback(bool p_enable);

	virtual bool is_low_end() const {
		return rendering_server->is_low_end();
	}

	virtual uint64_t get_frame_profile_frame() {
		return rendering_server->get_frame_profile_frame();
	}

	virtual void set_frame_profiling_enabled(bool p_enabled) {
		rendering_server->set_frame_profiling_enabled(p_enabled);
	}

	virtual Vector<FrameProfileArea> get_frame_profile() {
		return rendering_server->get_frame_profile();
	}

	RenderingServerWrapMT(RenderingServer *p_contained, bool p_create_thread);
	~RenderingServerWrapMT();

#undef ServerName
#undef ServerNameWrapMT
#undef server_name
};

#ifdef DEBUG_SYNC
#undef DEBUG_SYNC
#endif
#undef SYNC_DEBUG

#endif
