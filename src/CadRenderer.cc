#include "CadRenderer.hh"
#include "Layers/Objects/Cursor/CursorComponent.hh"

static void prepare_camera_on_scene();

static CameraController* s_camera = nullptr;
static Scene* s_scene             = nullptr;

namespace mg1
{
  void CadRenderer::create() {}

  void CadRenderer::terminate()
  {
    s_camera = nullptr;
    s_scene  = nullptr;
  }

  void CadRenderer::begin_scene(CameraController* camera, Scene* scene)
  {
    if (camera) { s_camera = camera; }
    if (scene) { s_scene = scene; }

    prepare_camera_on_scene();
  }

  void CadRenderer::end_scene() {}

  void CadRenderer::render()
  {
    ESP_ASSERT(s_camera, "Unable to render - camera is null")
    ESP_ASSERT(s_scene, "Unable to render - scene is null")

    for (auto&& [entity, model] : s_scene->get_view<ModelComponent>())
    {
      model.draw();
    }
  }

  CameraController* CadRenderer::get_camera() { return s_camera; }

  Scene* CadRenderer::get_scene() { return s_scene; }

  void CadRenderer::set_camera(CameraController* camera) { s_camera = camera; }
  void CadRenderer::set_scene(Scene* scene) { s_scene = scene; }
} // namespace mg1

static void prepare_camera_on_scene()
{
  glm::mat4 vp = s_camera->get_projection() * s_camera->get_view();

  //  // special handling for cursor component
  //  for (auto&& [entity, cursor, model] : s_scene->get_view<mg1::CursorComponent, ModelComponent>())
  //  {
  //    glm::mat4 mvp = glm::translate(vp, cursor.get_position());
  //    for (auto& draw : model.get_draw_data())
  //    {
  //      draw->m_uniform_manager->update_buffer_uniform(0, 0, 0, sizeof(glm::mat4), &mvp);
  //    }
  //  }
  //
  //  for (auto&& [entity, transform, model] :
  //       s_scene->get_view<TransformComponent, ModelComponent>(entt::exclude<mg1::CursorComponent>))
  //  {
  //    glm::mat4 mvp = vp * transform.get_model_mat(); // TODO:
  //    for (auto& draw : model.get_draw_data())
  //    {
  //      draw->m_uniform_manager->update_buffer_uniform(0, 0, 0, sizeof(glm::mat4), &mvp);
  //    }
  //  }

  s_scene->get_root().act(
      [&vp](Node* node)
      {
        auto entity = node->get_entity();
        auto model  = entity->try_get_component<ModelComponent>();
        if (model)
        {
          glm::mat4 mvp = vp;
          if (entity->has_component<mg1::CursorComponent>())
          {
            auto cursor = entity->get_component<mg1::CursorComponent>();
            mvp         = glm::translate(mvp, cursor.get_position());
          }
          else { mvp *= node->get_model_mat(); }
          model->update_buffer_uniform(0, 0, 0, sizeof(glm::mat4), &mvp);
        }
      });
}
