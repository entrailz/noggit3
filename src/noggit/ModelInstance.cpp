// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <math/frustum.hpp>
#include <noggit/Log.h>
#include <noggit/Misc.h> // checkinside
#include <noggit/Model.h> // Model, etc.
#include <noggit/ModelInstance.h>
#include <opengl/primitives.hpp>
#include <opengl/scoped.hpp>
#include <opengl/shader.hpp>

ModelInstance::ModelInstance(std::string const& filename)
  : model (filename)
{
}

ModelInstance::ModelInstance(std::string const& filename, MPQFile* f)
  : model (filename)
{
  float ff[4];

  f->read(ff, 12);
  pos = math::vector_3d(ff[0], ff[2], -ff[1]);

  f->read(ff, 16);
  _wmo_orientation = math::quaternion (-ff[3], ff[1], ff[2], ff[0]);

  f->read(&scale, 4);
  f->read(&uid, 4);
  lcol = math::vector_3d(((uid & 0xff0000) >> 16) / 255.0f, ((uid & 0x00ff00) >> 8) / 255.0f, (uid & 0x0000ff) / 255.0f);
}

ModelInstance::ModelInstance(std::string const& filename, ENTRY_MDDF const*d)
  : model (filename)
{
	uid = d->uniqueID;
	pos = math::vector_3d(d->pos[0], d->pos[1], d->pos[2]);
	dir = math::vector_3d(d->rot[0], d->rot[1], d->rot[2]);
	// scale factor - divide by 1024. blizzard devs must be on crack, why not just use a float?
	scale = d->scale / 1024.0f;

  recalcExtents();
}

void ModelInstance::draw ( opengl::scoped::use_program& m2_shader
                         , math::frustum const& frustum
                         , const float& cull_distance
                         , const math::vector_3d& camera
                         , bool force_box
                         , bool all_boxes
                         , bool draw_fog
                         , bool is_current_selection
                         , int animtime
                         )
{
  if (is_visible(frustum, cull_distance, camera))
  {
    return;
  }  

  m2_shader.uniform("transform", _transform_mat_transposed);

  //! \todo add toggle draw particles and set customizable distance
  model->draw (m2_shader, draw_fog, animtime, false);
  
}

void ModelInstance::draw ( math::frustum const& frustum
                         , const float& cull_distance
                         , const math::vector_3d& camera
                         , bool force_box
                         , bool all_boxes
                         , bool draw_fog
                         , bool is_current_selection
                         , int animtime
                         )
{
  if (is_visible(frustum, cull_distance, camera))
  {
    return;
  }    

  {
    opengl::scoped::matrix_pusher const matrix;

    gl.multMatrixf (_transform_mat_transposed);

    //! \todo add toggle draw particles and set customizable distance
    model->draw (draw_fog, animtime, false);
  }

  if (is_current_selection || all_boxes)
  {
    draw_box(is_current_selection);
  }
}

void ModelInstance::draw_box (bool is_current_selection)
{
  opengl::scoped::matrix_pusher const matrix;

  gl.multMatrixf (_transform_mat_transposed);

  opengl::scoped::bool_setter<GL_LIGHTING, GL_FALSE> lighting;
  opengl::scoped::bool_setter<GL_FOG, GL_FALSE> fog;
  opengl::scoped::bool_setter<GL_COLOR_MATERIAL, GL_FALSE> color_material;

  opengl::texture::set_active_texture(0);
  opengl::texture::disable_texture();
  opengl::texture::set_active_texture(1);
  opengl::texture::disable_texture();

  gl.enable(GL_BLEND);
  gl.blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  if (is_current_selection)
  {
    opengl::primitives::wire_box ( misc::transform_model_box_coords(model->header.BoundingBoxMin)
                                 , misc::transform_model_box_coords(model->header.BoundingBoxMax)
                                 ).draw ({ 1.0f, 1.0f, 0.0f, 1.0f }, 1.0f);

    opengl::primitives::wire_box ( misc::transform_model_box_coords(model->header.VertexBoxMin)
                                 , misc::transform_model_box_coords(model->header.VertexBoxMax)
                                 ).draw ({1.0f, 1.0f, 1.0f, 1.0f}, 1.0f);

    gl.color4fv(math::vector_4d(1.0f, 0.0f, 0.0f, 1.0f));
    gl.begin(GL_LINES);
    gl.vertex3f(0.0f, 0.0f, 0.0f);
    gl.vertex3f(model->header.VertexBoxMax.x + model->header.VertexBoxMax.x / 5.0f, 0.0f, 0.0f);
    gl.end();

    gl.color4fv(math::vector_4d(0.0f, 1.0f, 0.0f, 1.0f));
    gl.begin(GL_LINES);
    gl.vertex3f(0.0f, 0.0f, 0.0f);
    gl.vertex3f(0.0f, model->header.VertexBoxMax.z + model->header.VertexBoxMax.z / 5.0f, 0.0f);
    gl.end();

    gl.color4fv(math::vector_4d(0.0f, 0.0f, 1.0f, 1.0f));
    gl.begin(GL_LINES);
    gl.vertex3f(0.0f, 0.0f, 0.0f);
    gl.vertex3f(0.0f, 0.0f, model->header.VertexBoxMax.y + model->header.VertexBoxMax.y / 5.0f);
    gl.end();
  }
  else
  {
    opengl::primitives::wire_box ( misc::transform_model_box_coords(model->header.VertexBoxMin)
                                 , misc::transform_model_box_coords(model->header.VertexBoxMax)
                                 ).draw ({0.5f, 0.5f, 0.5f, 1.0f}, 1.0f);
  }
}

void ModelInstance::update_transform_matrix()
{
  math::matrix_4x4 mat (math::matrix_4x4 (math::matrix_4x4::translation, pos)
          * math::matrix_4x4 (math::matrix_4x4::rotation_yzx
                              , { math::degrees (-dir.z)
                              , math::degrees (dir.y - 90.0f)
                              , math::degrees (dir.x)
                              }
          )
          * math::matrix_4x4 (math::matrix_4x4::scale, scale)
          );

  _transform_mat_inverted = mat.inverted();
  _transform_mat_transposed = mat.transposed();
}

//! \todo  Get this drawn on the 2D view.
/*void ModelInstance::drawMapTile()
{
if(CheckUniques(uid))
return;

opengl::scoped::matrix_pusher const matrix;

gl.translatef(pos.x/CHUNKSIZE, pos.z/CHUNKSIZE, pos.y);
gl.rotatef(-90.0f, 1, 0, 0);
gl.rotatef(dir.y - 90.0f, 0, 1, 0);
gl.rotatef(-dir.x, 0, 0, 1);
gl.rotatef(dir.z, 1, 0, 0);
gl.scalef(1/CHUNKSIZE,1/CHUNKSIZE,1/CHUNKSIZE);
gl.scalef(sc,sc,sc);

model->draw();
}*/

void ModelInstance::intersect ( math::ray const& ray
                              , selection_result* results
                              , int animtime
                              )
{
  math::ray subray (_transform_mat_inverted, ray);

  if ( !subray.intersect_bounds ( fixCoordSystem (model->header.VertexBoxMin)
                                , fixCoordSystem (model->header.VertexBoxMax)
                                )
     )
  {
    return;
  }

  for (auto&& result : model->intersect (subray, animtime))
  {
    //! \todo why is only sc important? these are relative to subray,
    //! so should be inverted by model_matrix?
    results->emplace_back (result * scale, selected_model_type (this));
  }
}


void ModelInstance::draw_wmo ( const math::vector_3d& ofs
                             , const math::degrees rotation
                             , math::frustum const& frustum
                             , bool draw_fog
                             , int animtime
                             )
{
  math::vector_3d tpos(ofs + pos);
  math::rotate (ofs.x, ofs.z, &tpos.x, &tpos.z, rotation);
  if (!frustum.intersectsSphere(tpos, model->rad*scale)) return;

  opengl::scoped::matrix_pusher const matrix;

  gl.translatef(pos.x, pos.y, pos.z);
  gl.multMatrixf (math::matrix_4x4 (math::matrix_4x4::rotation, _wmo_orientation));
  gl.scalef(scale, -scale, -scale);

  //! \todo draw particles from wmo's doodads ?
  model->draw (draw_fog, animtime, false);
}

void ModelInstance::resetDirection(){
  dir.x = 0;
  //dir.y=0; only reset incline
  dir.z = 0;
}

bool ModelInstance::isInsideRect(math::vector_3d rect[2]) const
{
  return misc::rectOverlap(extents, rect);
}

bool ModelInstance::is_visible(math::frustum const& frustum, const float& cull_distance, const math::vector_3d& camera) const
{
  float dist = (pos - camera).length() - model->rad * scale;
  if(dist >= cull_distance)
    return false;

  return frustum.intersectsSphere(pos, model->rad * scale);
}

void ModelInstance::recalcExtents()
{
  update_transform_matrix();

  math::vector_3d min (math::vector_3d::max()), vertex_box_min (min);
  math::vector_3d max (math::vector_3d::min()), vertex_box_max (max);;
  math::matrix_4x4 rot (_transform_mat_transposed.transposed());

  math::vector_3d bounds[8 * 2];
  math::vector_3d *ptr = bounds;

  *ptr++ = rot * misc::transform_model_box_coords(math::vector_3d(model->header.BoundingBoxMax.x, model->header.BoundingBoxMax.y, model->header.BoundingBoxMin.z));
  *ptr++ = rot * misc::transform_model_box_coords(math::vector_3d(model->header.BoundingBoxMin.x, model->header.BoundingBoxMax.y, model->header.BoundingBoxMin.z));
  *ptr++ = rot * misc::transform_model_box_coords(math::vector_3d(model->header.BoundingBoxMin.x, model->header.BoundingBoxMin.y, model->header.BoundingBoxMin.z));
  *ptr++ = rot * misc::transform_model_box_coords(math::vector_3d(model->header.BoundingBoxMax.x, model->header.BoundingBoxMin.y, model->header.BoundingBoxMin.z));
  *ptr++ = rot * misc::transform_model_box_coords(math::vector_3d(model->header.BoundingBoxMax.x, model->header.BoundingBoxMin.y, model->header.BoundingBoxMax.z));
  *ptr++ = rot * misc::transform_model_box_coords(math::vector_3d(model->header.BoundingBoxMax.x, model->header.BoundingBoxMax.y, model->header.BoundingBoxMax.z));
  *ptr++ = rot * misc::transform_model_box_coords(math::vector_3d(model->header.BoundingBoxMin.x, model->header.BoundingBoxMax.y, model->header.BoundingBoxMax.z));
  *ptr++ = rot * misc::transform_model_box_coords(math::vector_3d(model->header.BoundingBoxMin.x, model->header.BoundingBoxMin.y, model->header.BoundingBoxMax.z));

  *ptr++ = rot * misc::transform_model_box_coords(math::vector_3d(model->header.VertexBoxMax.x, model->header.VertexBoxMax.y, model->header.VertexBoxMin.z));
  *ptr++ = rot * misc::transform_model_box_coords(math::vector_3d(model->header.VertexBoxMin.x, model->header.VertexBoxMax.y, model->header.VertexBoxMin.z));
  *ptr++ = rot * misc::transform_model_box_coords(math::vector_3d(model->header.VertexBoxMin.x, model->header.VertexBoxMin.y, model->header.VertexBoxMin.z));
  *ptr++ = rot * misc::transform_model_box_coords(math::vector_3d(model->header.VertexBoxMax.x, model->header.VertexBoxMin.y, model->header.VertexBoxMin.z));
  *ptr++ = rot * misc::transform_model_box_coords(math::vector_3d(model->header.VertexBoxMax.x, model->header.VertexBoxMin.y, model->header.VertexBoxMax.z));
  *ptr++ = rot * misc::transform_model_box_coords(math::vector_3d(model->header.VertexBoxMax.x, model->header.VertexBoxMax.y, model->header.VertexBoxMax.z));
  *ptr++ = rot * misc::transform_model_box_coords(math::vector_3d(model->header.VertexBoxMin.x, model->header.VertexBoxMax.y, model->header.VertexBoxMax.z));
  *ptr++ = rot * misc::transform_model_box_coords(math::vector_3d(model->header.VertexBoxMin.x, model->header.VertexBoxMin.y, model->header.VertexBoxMax.z));


  for (int i = 0; i < 8 * 2; ++i)
  {
    misc::extract_v3d_min_max (bounds[i], min, max);
    // vertex box only for size_cat
    if (i >= 8)
    {
      misc::extract_v3d_min_max (bounds[i], vertex_box_min, vertex_box_max);
    }
  }

  extents[0] = min;
  extents[1] = max;

  size_cat = std::max( vertex_box_max.x - vertex_box_min.x
                     , std::max( vertex_box_max.y - vertex_box_min.y
                               , vertex_box_max.z - vertex_box_min.z
                               )
                     );
}