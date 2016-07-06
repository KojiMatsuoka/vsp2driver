/*************************************************************************/ /*
 VSP2

 Copyright (C) 2015 Renesas Electronics Corporation

 License        Dual MIT/GPLv2

 The contents of this file are subject to the MIT license as set out below.

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 Alternatively, the contents of this file may be used under the terms of
 the GNU General Public License Version 2 ("GPL") in which case the provisions
 of GPL are applicable instead of those above.

 If you wish to allow use of your version of this file only under the terms of
 GPL, and not to allow others to use your version of this file under the terms
 of the MIT license, indicate your decision by deleting the provisions above
 and replace them with the notice and other provisions required by GPL as set
 out in the file called "GPL-COPYING" included in this distribution. If you do
 not delete the provisions above, a recipient may use your version of this file
 under the terms of either the MIT license or GPL.

 This License is also included in this distribution in the file called
 "MIT-COPYING".

 EXCEPT AS OTHERWISE STATED IN A NEGOTIATED AGREEMENT: (A) THE SOFTWARE IS
 PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 PURPOSE AND NONINFRINGEMENT; AND (B) IN NO EVENT SHALL THE AUTHORS OR
 COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


 GPLv2:
 If you wish to use this file under the terms of GPL, following terms are
 effective.

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; version 2 of the License.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/ /*************************************************************************/

#ifndef __VSP2_ENTITY_H__
#define __VSP2_ENTITY_H__

#include <linux/list.h>
#include <linux/spinlock.h>

#include <media/v4l2-subdev.h>

struct vsp2_device;
struct vsp2_pipeline;

enum vsp2_entity_type {
	VSP2_ENTITY_BRU,
	VSP2_ENTITY_LUT,
	VSP2_ENTITY_CLU,
	VSP2_ENTITY_RPF,
	VSP2_ENTITY_UDS,
	VSP2_ENTITY_HGO,
	VSP2_ENTITY_HGT,
	VSP2_ENTITY_WPF,
};

/**
 * struct vsp2_entity_operations - Entity operations
 * @destroy:	Destroy the entity.
 * @set_memory:	Setup memory buffer access. This operation applies the settings
 *		stored in the rwpf mem field to the hardware. Valid for RPF and
 *		WPF only.
 * @configure:	Setup the hardware based on the entity state (pipeline, formats,
 *		selection rectangles, ...)
 */
struct vsp2_entity_operations {
	void (*destroy)(struct vsp2_entity *);
	void (*set_memory)(struct vsp2_entity *);
	void (*configure)(struct vsp2_entity *, struct vsp2_pipeline *);
};

struct vsp2_entity {
	struct vsp2_device *vsp2;

	const struct vsp2_entity_operations *ops;

	enum vsp2_entity_type type;
	unsigned int index;

	struct list_head list_dev;
	struct list_head list_pipe;

	struct media_pad *pads;
	unsigned int source_pad;

	struct media_entity *sink;
	unsigned int sink_pad;

	struct v4l2_subdev subdev;
	struct v4l2_subdev_pad_config *config;
};

static inline struct vsp2_entity *to_vsp2_entity(struct v4l2_subdev *subdev)
{
	return container_of(subdev, struct vsp2_entity, subdev);
}

int vsp2_entity_init(struct vsp2_device *vsp2, struct vsp2_entity *entity,
		     const char *name, unsigned int num_pads,
		     const struct v4l2_subdev_ops *ops, u32 function);
void vsp2_entity_destroy(struct vsp2_entity *entity);

extern const struct v4l2_subdev_internal_ops vsp2_subdev_internal_ops;

int vsp2_entity_link_setup(struct media_entity *entity,
			   const struct media_pad *local,
			   const struct media_pad *remote, u32 flags);

struct v4l2_subdev_pad_config *
vsp2_entity_get_pad_config(struct vsp2_entity *entity,
			   struct v4l2_subdev_pad_config *cfg,
			   enum v4l2_subdev_format_whence which);
struct v4l2_mbus_framefmt *
vsp2_entity_get_pad_format(struct vsp2_entity *entity,
			   struct v4l2_subdev_pad_config *cfg,
			   unsigned int pad);
struct v4l2_rect *
vsp2_entity_get_pad_compose(struct vsp2_entity *entity,
			    struct v4l2_subdev_pad_config *cfg,
			    unsigned int pad);
int vsp2_entity_init_cfg(struct v4l2_subdev *subdev,
			 struct v4l2_subdev_pad_config *cfg);

void vsp2_entity_route_setup(struct vsp2_entity *source);

int vsp2_subdev_get_pad_format(struct v4l2_subdev *subdev,
			       struct v4l2_subdev_pad_config *cfg,
			       struct v4l2_subdev_format *fmt);
int vsp2_subdev_enum_mbus_code(struct v4l2_subdev *subdev,
			       struct v4l2_subdev_pad_config *cfg,
			       struct v4l2_subdev_mbus_code_enum *code,
			       const unsigned int *codes, unsigned int ncodes);
int vsp2_subdev_enum_frame_size(struct v4l2_subdev *subdev,
				struct v4l2_subdev_pad_config *cfg,
				struct v4l2_subdev_frame_size_enum *fse,
				unsigned int min_w, unsigned int min_h,
				unsigned int max_w, unsigned int max_h);

#endif /* __VSP2_ENTITY_H__ */
