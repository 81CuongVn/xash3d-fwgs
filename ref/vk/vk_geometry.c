#include "vk_geometry.h"
#include "vk_buffer.h"
#include "vk_staging.h"
#include "vk_framectl.h" // MAX_CONCURRENT_FRAMES

#define MAX_BUFFER_VERTICES_STATIC (128 * 1024)
#define MAX_BUFFER_INDICES_STATIC (MAX_BUFFER_VERTICES_STATIC * 3)
#define GEOMETRY_BUFFER_STATIC_SIZE ALIGN_UP(MAX_BUFFER_VERTICES_STATIC * sizeof(vk_vertex_t) + MAX_BUFFER_INDICES_STATIC * sizeof(uint16_t), sizeof(vk_vertex_t))

#define MAX_BUFFER_VERTICES_DYNAMIC (128 * 1024 * 2)
#define MAX_BUFFER_INDICES_DYNAMIC (MAX_BUFFER_VERTICES_DYNAMIC * 3)
#define GEOMETRY_BUFFER_DYNAMIC_SIZE ALIGN_UP(MAX_BUFFER_VERTICES_DYNAMIC * sizeof(vk_vertex_t) + MAX_BUFFER_INDICES_DYNAMIC * sizeof(uint16_t), sizeof(vk_vertex_t))

#define GEOMETRY_BUFFER_SIZE (GEOMETRY_BUFFER_STATIC_SIZE + GEOMETRY_BUFFER_DYNAMIC_SIZE)

static struct {
	vk_buffer_t buffer;
	r_debuffer_t alloc;
} g_geom;

qboolean R_GeometryBufferAllocAndLock( r_geometry_buffer_lock_t *lock, int vertex_count, int index_count, r_geometry_lifetime_t lifetime ) {
	const uint32_t vertices_size = vertex_count * sizeof(vk_vertex_t);
	const uint32_t indices_size = index_count * sizeof(uint16_t);
	const uint32_t total_size = vertices_size + indices_size;

	const uint32_t offset = R_DEBuffer_Alloc(&g_geom.alloc, (lifetime == LifetimeSingleFrame) ? LifetimeDynamic : LifetimeStatic, total_size, sizeof(vk_vertex_t));
	if (offset == ALO_ALLOC_FAILED) {
		/* gEngine.Con_Printf(S_ERROR "Cannot allocate %s geometry buffer for %d vertices (%d bytes) and %d indices (%d bytes)\n", */
		/* 	lifetime == LifetimeSingleFrame ? "dynamic" : "static", */
		/* 	vertex_count, vertices_size, index_count, indices_size); */
		return false;
	}

	{
		const uint32_t vertices_offset = offset / sizeof(vk_vertex_t);
		const uint32_t indices_offset = (offset + vertices_size) / sizeof(uint16_t);
		const vk_staging_buffer_args_t staging_args = {
			.buffer = g_geom.buffer.buffer,
			.offset = offset,
			.size = total_size,
			.alignment = 4,
		};

		const vk_staging_region_t staging = R_VkStagingLockForBuffer(staging_args);
		ASSERT(staging.ptr);

		ASSERT( offset % sizeof(vk_vertex_t) == 0 );
		ASSERT( (offset + vertices_size) % sizeof(uint16_t) == 0 );

		*lock = (r_geometry_buffer_lock_t) {
			.vertices = {
				.count = vertex_count,
				.ptr = (vk_vertex_t *)staging.ptr,
				.unit_offset = vertices_offset,
			},
			.indices = {
				.count = index_count,
				.ptr = (uint16_t *)((char*)staging.ptr + vertices_size),
				.unit_offset = indices_offset,
			},
			.impl_ = {
				.staging_handle = staging.handle,
			},
		};
	}

	return true;
}

void R_GeometryBufferUnlock( const r_geometry_buffer_lock_t *lock ) {
	R_VkStagingUnlock(lock->impl_.staging_handle);
}

void R_GeometryBuffer_MapClear( void ) {
	R_DEBuffer_Init(&g_geom.alloc, GEOMETRY_BUFFER_STATIC_SIZE, GEOMETRY_BUFFER_DYNAMIC_SIZE);
}

qboolean R_GeometryBuffer_Init(void) {
	// TODO device memory and friends (e.g. handle mobile memory ...)

	if (!VK_BufferCreate("geometry buffer", &g_geom.buffer, GEOMETRY_BUFFER_SIZE,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | (vk_core.rtx ? VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR : 0),
		(vk_core.rtx ? VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT : 0)))
		return false;

	R_GeometryBuffer_MapClear();
	return true;
}

void R_GeometryBuffer_Shutdown(void) {
	VK_BufferDestroy( &g_geom.buffer );
}

void R_GeometryBuffer_Flip(void) {
	R_DEBuffer_Flip(&g_geom.alloc);
}

VkBuffer R_GeometryBuffer_Get(void) {
	return g_geom.buffer.buffer;
}
