#include "ObjectFactory.hh"

namespace mg1
{
  ObjectFactory* ObjectFactory::s_instance = nullptr;

  ObjectFactory::ObjectFactory(esp::Scene* scene, ObjectSelector* object_selector) :
      IEventable(), m_scene{ scene }, m_object_selector{ object_selector }
  {
    // create shader
    {
      auto uniform_meta_data = EspUniformMetaData::create();
      uniform_meta_data->establish_descriptor_set();
      uniform_meta_data->add_buffer_uniform(EspUniformShaderStage::ESP_VTX_STAGE, sizeof(glm::mat4));
      uniform_meta_data->add_buffer_uniform(EspUniformShaderStage::ESP_FRAG_STAGE, sizeof(glm::vec3));

      m_object_shader = ShaderSystem::acquire("Shaders/ObjectLayer/shader");
      m_object_shader->set_attachment_formats({ EspBlockFormat::ESP_FORMAT_R8G8B8A8_UNORM });
      m_object_shader->enable_multisampling(EspSampleCountFlag::ESP_SAMPLE_COUNT_4_BIT);
      m_object_shader->enable_depth_test(EspDepthBlockFormat::ESP_FORMAT_D32_SFLOAT, EspCompareOp::ESP_COMPARE_OP_LESS);
      m_object_shader->set_vertex_layouts({ TorusInit::S_MODEL_PARAMS.get_vertex_layouts() });
      m_object_shader->set_worker_layout(std::move(uniform_meta_data));
      m_object_shader->set_rasterizer_settings(
          { .m_polygon_mode = ESP_POLYGON_MODE_LINE, .m_cull_mode = ESP_CULL_MODE_NONE });
      m_object_shader->build_worker();
    }

    // create spline shader
    {
      auto uniform_meta_data = EspUniformMetaData::create();
      uniform_meta_data->establish_descriptor_set();
      uniform_meta_data->add_buffer_uniform(EspUniformShaderStage::ESP_VTX_STAGE, sizeof(glm::mat4));
      uniform_meta_data->add_buffer_uniform(EspUniformShaderStage::ESP_GEOM_STAGE, sizeof(int));
      uniform_meta_data->add_buffer_uniform(EspUniformShaderStage::ESP_FRAG_STAGE, sizeof(SplineColorUbo));

      m_spline_shader = ShaderSystem::acquire("Shaders/ObjectLayer/Spline/shader");
      m_spline_shader->set_attachment_formats({ EspBlockFormat::ESP_FORMAT_R8G8B8A8_UNORM });
      m_spline_shader->enable_multisampling(EspSampleCountFlag::ESP_SAMPLE_COUNT_4_BIT);
      m_spline_shader->enable_depth_test(EspDepthBlockFormat::ESP_FORMAT_D32_SFLOAT, EspCompareOp::ESP_COMPARE_OP_LESS);
      m_spline_shader->set_vertex_layouts({ SplineInit::S_MODEL_PARAMS.get_vertex_layouts() });
      m_spline_shader->set_worker_layout(std::move(uniform_meta_data));
      m_spline_shader->set_rasterizer_settings(
          { .m_polygon_mode = ESP_POLYGON_MODE_POINT, .m_cull_mode = ESP_CULL_MODE_NONE });
      m_spline_shader->set_input_assembly_settings(
          { .m_primitive_topology = ESP_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY });
      m_spline_shader->build_worker();
    }

    // create c2 spline shader
    {
      auto uniform_meta_data = EspUniformMetaData::create();
      uniform_meta_data->establish_descriptor_set();
      uniform_meta_data->add_buffer_uniform(EspUniformShaderStage::ESP_ALL_STAGES, sizeof(glm::mat4));
      uniform_meta_data->add_buffer_uniform(EspUniformShaderStage::ESP_GEOM_STAGE, sizeof(C2SplineUbo));
      uniform_meta_data->add_buffer_uniform(EspUniformShaderStage::ESP_FRAG_STAGE, sizeof(SplineColorUbo));

      m_c2_spline_shader = ShaderSystem::acquire("Shaders/ObjectLayer/C2Spline/shader");
      m_c2_spline_shader->set_attachment_formats({ EspBlockFormat::ESP_FORMAT_R8G8B8A8_UNORM });
      m_c2_spline_shader->enable_multisampling(EspSampleCountFlag::ESP_SAMPLE_COUNT_4_BIT);
      m_c2_spline_shader->enable_depth_test(EspDepthBlockFormat::ESP_FORMAT_D32_SFLOAT,
                                            EspCompareOp::ESP_COMPARE_OP_LESS);
      m_c2_spline_shader->set_vertex_layouts({ SplineInit::S_MODEL_PARAMS.get_vertex_layouts() });
      m_c2_spline_shader->set_worker_layout(std::move(uniform_meta_data));
      m_c2_spline_shader->set_rasterizer_settings(
          { .m_polygon_mode = ESP_POLYGON_MODE_POINT, .m_cull_mode = ESP_CULL_MODE_NONE });
      m_c2_spline_shader->set_input_assembly_settings(
          { .m_primitive_topology = ESP_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY });
      m_c2_spline_shader->build_worker();
    }
  }

  std::unique_ptr<ObjectFactory> ObjectFactory::create(Scene* scene, ObjectSelector* object_selector)
  {
    ESP_ASSERT(ObjectFactory::s_instance == nullptr, "Object factory already exists!");
    ObjectFactory::s_instance = new ObjectFactory(scene, object_selector);

    return std::unique_ptr<ObjectFactory>{ ObjectFactory::s_instance };
  }

  TorusComponent& ObjectFactory::create_torus(glm::vec3 position)
  {
    auto entity = s_instance->m_scene->create_entity();

    entity->add_component<TorusComponent>(entity->get_id());
    auto& torus = entity->get_component<TorusComponent>();

    auto [vertices, indices] = torus.reconstruct();
    auto model               = std::make_shared<Model>(vertices,
                                         indices,
                                         std::vector<std::shared_ptr<EspTexture>>{},
                                         TorusInit::S_MODEL_PARAMS);
    entity->add_component<ModelComponent>(model, s_instance->m_object_shader);

    torus.get_node()->attach_entity(entity);
    torus.get_node()->translate(position);

    s_instance->m_scene->get_root().add_child(torus.get_node());

    return torus;
  }

  PointComponent& ObjectFactory::create_point(glm::vec3 position)
  {
    auto entity = s_instance->m_scene->create_entity();

    entity->add_component<PointComponent>(entity->get_id(), false);
    auto& point = entity->get_component<PointComponent>();

    auto [vertices, indices] = point.reconstruct();
    auto model               = std::make_shared<Model>(vertices,
                                         indices,
                                         std::vector<std::shared_ptr<EspTexture>>{},
                                         PointInit::S_MODEL_PARAMS);
    entity->add_component<ModelComponent>(model, s_instance->m_object_shader);

    point.get_node()->attach_entity(entity);
    point.get_node()->translate(position);

    s_instance->m_scene->get_root().add_child(point.get_node());

    return point;
  }

  PointComponent& ObjectFactory::create_bernstein_point(glm::vec3 position)
  {
    auto entity = s_instance->m_scene->create_entity();

    entity->add_component<PointComponent>(entity->get_id(), true);
    auto& point = entity->get_component<PointComponent>();

    auto [vertices, indices] = point.reconstruct();
    auto model               = std::make_shared<Model>(vertices,
                                         indices,
                                         std::vector<std::shared_ptr<EspTexture>>{},
                                         PointInit::S_MODEL_PARAMS);
    entity->add_component<ModelComponent>(model, s_instance->m_object_shader);

    point.get_node()->attach_entity(entity);
    point.get_node()->translate(position);

    s_instance->m_scene->get_root().add_child(point.get_node());

    return point;
  }

  SplineComponent& ObjectFactory::create_spline()
  {
    std::vector<PointComponent> control_points{};
    for (auto&& [entity, point] : s_instance->m_scene->get_view<PointComponent>())
    {
      if (point.get_info()->selected()) { control_points.push_back(point); }
    }

    auto entity = s_instance->m_scene->create_entity();

    entity->add_component<SplineComponent>(entity->get_id(), s_instance->m_scene, control_points);
    auto& spline = entity->get_component<SplineComponent>();

    auto [vertices, indices] = spline.reconstruct();
    auto model               = std::make_shared<Model>(vertices,
                                         indices,
                                         std::vector<std::shared_ptr<EspTexture>>{},
                                         SplineInit::S_MODEL_PARAMS);
    entity->add_component<ModelComponent>(model, s_instance->m_spline_shader);

    spline.get_node()->attach_entity(entity);

    s_instance->m_scene->get_root().add_child(spline.get_node());

    return spline;
  }

  C2SplineComponent& ObjectFactory::create_c2_spline()
  {
    std::vector<PointComponent> control_points{};
    for (auto&& [entity, point] : s_instance->m_scene->get_view<PointComponent>())
    {
      if (point.get_info()->selected()) { control_points.push_back(point); }
    }

    auto entity = s_instance->m_scene->create_entity();

    entity->add_component<C2SplineComponent>(entity->get_id(), s_instance->m_scene, control_points);
    auto& spline = entity->get_component<C2SplineComponent>();

    auto [vertices, indices] = spline.reconstruct();
    auto model               = std::make_shared<Model>(vertices,
                                         indices,
                                         std::vector<std::shared_ptr<EspTexture>>{},
                                         SplineInit::S_MODEL_PARAMS);
    entity->add_component<ModelComponent>(model, s_instance->m_c2_spline_shader);

    spline.get_node()->attach_entity(entity);

    s_instance->m_scene->get_root().add_child(spline.get_node());

    return spline;
  }
} // namespace mg1