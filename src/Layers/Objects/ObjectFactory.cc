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

      m_object_shader = ShaderSystem::acquire("Shaders/Layers/Objects/shader");
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
      uniform_meta_data->add_buffer_uniform(EspUniformShaderStage::ESP_FRAG_STAGE, sizeof(glm::vec3));

      m_spline_shader = ShaderSystem::acquire("Shaders/Layers/Objects/Spline/shader");
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

    // create c0 bezier surface shader
    {
      auto uniform_meta_data = EspUniformMetaData::create();
      uniform_meta_data->establish_descriptor_set();
      uniform_meta_data->add_buffer_uniform(EspUniformShaderStage::ESP_ALL_STAGES, sizeof(glm::mat4));
      uniform_meta_data->add_buffer_uniform(EspUniformShaderStage::ESP_FRAG_STAGE, sizeof(glm::vec3));
      uniform_meta_data->establish_descriptor_set();
      uniform_meta_data->add_buffer_uniform(EspUniformShaderStage::ESP_ALL_STAGES, sizeof(int) * 2);

      m_c0_surface_shader = ShaderSystem::acquire("Shaders/Layers/Objects/BezierSurface/C0/shader");
      m_c0_surface_shader->set_attachment_formats({ EspBlockFormat::ESP_FORMAT_R8G8B8A8_UNORM });
      m_c0_surface_shader->enable_multisampling(EspSampleCountFlag::ESP_SAMPLE_COUNT_4_BIT);
      m_c0_surface_shader->enable_depth_test(EspDepthBlockFormat::ESP_FORMAT_D32_SFLOAT,
                                             EspCompareOp::ESP_COMPARE_OP_LESS);
      m_c0_surface_shader->set_vertex_layouts({ SurfaceInit::S_MODEL_PARAMS.get_vertex_layouts() });
      m_c0_surface_shader->set_worker_layout(std::move(uniform_meta_data));
      m_c0_surface_shader->set_rasterizer_settings(
          { .m_polygon_mode = ESP_POLYGON_MODE_LINE, .m_cull_mode = ESP_CULL_MODE_NONE });
      m_c0_surface_shader->set_input_assembly_settings({ .m_primitive_topology = ESP_PRIMITIVE_TOPOLOGY_PATCH_LIST });
      m_c0_surface_shader->build_worker();
    }

    // create c2 bezier surface shader
    {
      auto uniform_meta_data = EspUniformMetaData::create();
      uniform_meta_data->establish_descriptor_set();
      uniform_meta_data->add_buffer_uniform(EspUniformShaderStage::ESP_ALL_STAGES, sizeof(glm::mat4));
      uniform_meta_data->add_buffer_uniform(EspUniformShaderStage::ESP_FRAG_STAGE, sizeof(glm::vec3));
      uniform_meta_data->establish_descriptor_set();
      uniform_meta_data->add_buffer_uniform(EspUniformShaderStage::ESP_ALL_STAGES, sizeof(int) * 2);

      m_c2_surface_shader = ShaderSystem::acquire("Shaders/Layers/Objects/BezierSurface/C2/shader");
      m_c2_surface_shader->set_attachment_formats({ EspBlockFormat::ESP_FORMAT_R8G8B8A8_UNORM });
      m_c2_surface_shader->enable_multisampling(EspSampleCountFlag::ESP_SAMPLE_COUNT_4_BIT);
      m_c2_surface_shader->enable_depth_test(EspDepthBlockFormat::ESP_FORMAT_D32_SFLOAT,
                                             EspCompareOp::ESP_COMPARE_OP_LESS);
      m_c2_surface_shader->set_vertex_layouts({ SurfaceInit::S_MODEL_PARAMS.get_vertex_layouts() });
      m_c2_surface_shader->set_worker_layout(std::move(uniform_meta_data));
      m_c2_surface_shader->set_rasterizer_settings(
          { .m_polygon_mode = ESP_POLYGON_MODE_LINE, .m_cull_mode = ESP_CULL_MODE_NONE });
      m_c2_surface_shader->set_input_assembly_settings({ .m_primitive_topology = ESP_PRIMITIVE_TOPOLOGY_PATCH_LIST });
      m_c2_surface_shader->build_worker();
    }

    // create control line shader
    {
      auto uniform_meta_data = EspUniformMetaData::create();
      uniform_meta_data->establish_descriptor_set();
      uniform_meta_data->add_buffer_uniform(EspUniformShaderStage::ESP_VTX_STAGE, sizeof(glm::mat4));
      uniform_meta_data->add_buffer_uniform(EspUniformShaderStage::ESP_FRAG_STAGE, sizeof(glm::vec3));

      m_control_line_shader = ShaderSystem::acquire("Shaders/Layers/Objects/ControlLine/shader");
      m_control_line_shader->set_attachment_formats({ EspBlockFormat::ESP_FORMAT_R8G8B8A8_UNORM });
      m_control_line_shader->enable_multisampling(EspSampleCountFlag::ESP_SAMPLE_COUNT_4_BIT);
      m_control_line_shader->enable_depth_test(EspDepthBlockFormat::ESP_FORMAT_D32_SFLOAT,
                                               EspCompareOp::ESP_COMPARE_OP_LESS);
      m_control_line_shader->set_vertex_layouts({ PointInit::S_MODEL_PARAMS.get_vertex_layouts() });
      m_control_line_shader->set_worker_layout(std::move(uniform_meta_data));
      m_control_line_shader->set_rasterizer_settings(
          { .m_polygon_mode = ESP_POLYGON_MODE_LINE, .m_cull_mode = ESP_CULL_MODE_NONE });
      m_control_line_shader->set_input_assembly_settings({ .m_primitive_topology = ESP_PRIMITIVE_TOPOLOGY_LINE_LIST });
      m_control_line_shader->build_worker();
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

  PointComponent& ObjectFactory::create_surface_point(glm::vec3 position)
  {
    auto entity = s_instance->m_scene->create_entity();

    entity->add_component<PointComponent>(entity->get_id(), false);
    auto& point = entity->get_component<PointComponent>();
    point.get_info()->set_removeable(false);

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

  C0SplineComponent& ObjectFactory::create_c0_spline()
  {
    std::vector<PointComponent> control_points{};
    for (auto&& [entity, point] : s_instance->m_scene->get_view<PointComponent>())
    {
      if (point.get_info()->selected()) { control_points.push_back(point); }
    }

    auto entity = s_instance->m_scene->create_entity();

    entity->add_component<C0SplineComponent>(entity->get_id(), s_instance->m_scene, control_points);
    auto& spline             = entity->get_component<C0SplineComponent>();
    auto [vertices, indices] = spline.reconstruct();

    auto spline_model = std::make_shared<Model>(vertices,
                                                indices,
                                                std::vector<std::shared_ptr<EspTexture>>{},
                                                SplineInit::S_MODEL_PARAMS);

    entity->add_component<ControlLineComponent>(entity->get_id(), vertices.size());
    auto& control_line        = entity->get_component<ControlLineComponent>();
    auto control_line_indices = control_line.get_indices();

    auto control_line_model = std::make_shared<Model>(vertices,
                                                      control_line_indices,
                                                      std::vector<std::shared_ptr<EspTexture>>{},
                                                      SplineInit::S_MODEL_PARAMS);
    entity->add_component<ModelComponent>(
        std::vector<ModelComponentParams>{ { spline_model, s_instance->m_spline_shader },
                                           { control_line_model, s_instance->m_control_line_shader } });

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
    auto& spline             = entity->get_component<C2SplineComponent>();
    auto [vertices, indices] = spline.reconstruct();

    auto spline_model = std::make_shared<Model>(vertices,
                                                indices,
                                                std::vector<std::shared_ptr<EspTexture>>{},
                                                SplineInit::S_MODEL_PARAMS);

    entity->add_component<ControlLineComponent>(entity->get_id(), vertices.size());
    auto& control_line        = entity->get_component<ControlLineComponent>();
    auto control_line_indices = control_line.get_indices();

    auto control_line_model = std::make_shared<Model>(vertices,
                                                      control_line_indices,
                                                      std::vector<std::shared_ptr<EspTexture>>{},
                                                      SplineInit::S_MODEL_PARAMS);

    entity->add_component<ModelComponent>(
        std::vector<ModelComponentParams>{ { spline_model, s_instance->m_spline_shader },
                                           { control_line_model, s_instance->m_control_line_shader } });

    spline.get_node()->attach_entity(entity);

    s_instance->m_scene->get_root().add_child(spline.get_node());

    return spline;
  }

  C2InterpolationSplineComponent& ObjectFactory::create_c2_interpolation_spline()
  {
    std::vector<PointComponent> control_points{};
    for (auto&& [entity, point] : s_instance->m_scene->get_view<PointComponent>())
    {
      if (point.get_info()->selected()) { control_points.push_back(point); }
    }

    auto entity = s_instance->m_scene->create_entity();

    entity->add_component<C2InterpolationSplineComponent>(entity->get_id(), s_instance->m_scene, control_points);
    auto& spline             = entity->get_component<C2InterpolationSplineComponent>();
    auto [vertices, indices] = spline.reconstruct();

    auto spline_model = std::make_shared<Model>(vertices,
                                                indices,
                                                std::vector<std::shared_ptr<EspTexture>>{},
                                                SplineInit::S_MODEL_PARAMS);

    entity->add_component<ControlLineComponent>(entity->get_id(), vertices.size());
    auto& control_line              = entity->get_component<ControlLineComponent>();
    auto [control_line_vertices, _] = spline.reconstruct_base();
    control_line.set_vertex_count(control_line_vertices.size());
    auto control_line_indices = control_line.get_indices();

    auto control_line_model = std::make_shared<Model>(control_line_vertices,
                                                      control_line_indices,
                                                      std::vector<std::shared_ptr<EspTexture>>{},
                                                      SplineInit::S_MODEL_PARAMS);

    entity->add_component<ModelComponent>(
        std::vector<ModelComponentParams>{ { spline_model, s_instance->m_spline_shader },
                                           { control_line_model, s_instance->m_control_line_shader } });

    spline.get_node()->attach_entity(entity);

    s_instance->m_scene->get_root().add_child(spline.get_node());

    return spline;
  }

  C0BezierSurfaceComponent& ObjectFactory::create_c0_bezier_surface(CreateSurfaceData data, glm::vec3 position)
  {
    auto entity = s_instance->m_scene->create_entity();

    entity->add_component<C0BezierSurfaceComponent>(entity->get_id(), s_instance->m_scene, data);
    auto& surface = entity->get_component<C0BezierSurfaceComponent>();
    surface.translate(position);
    auto [vertices, indices] = surface.reconstruct();

    auto surface_model = std::make_shared<Model>(vertices,
                                                 indices,
                                                 std::vector<std::shared_ptr<EspTexture>>{},
                                                 SurfaceInit::S_MODEL_PARAMS);

    entity->add_component<ControlLineComponent>(entity->get_id(), vertices.size());
    auto control_line_indices = surface.get_control_line_indices();

    auto control_line_model = std::make_shared<Model>(vertices,
                                                      control_line_indices,
                                                      std::vector<std::shared_ptr<EspTexture>>{},
                                                      SplineInit::S_MODEL_PARAMS);

    entity->add_component<ModelComponent>(
        std::vector<ModelComponentParams>{ { surface_model, s_instance->m_c0_surface_shader, 0, 1 },
                                           { control_line_model, s_instance->m_control_line_shader } });

    surface.get_node()->attach_entity(entity);

    s_instance->m_scene->get_root().add_child(surface.get_node());

    return surface;
  }

  C2BezierSurfaceComponent& ObjectFactory::create_c2_bezier_surface(CreateSurfaceData data, glm::vec3 position)
  {
    auto entity = s_instance->m_scene->create_entity();

    entity->add_component<C2BezierSurfaceComponent>(entity->get_id(), s_instance->m_scene, data);
    auto& surface = entity->get_component<C2BezierSurfaceComponent>();
    surface.translate(position);
    auto [vertices, indices] = surface.reconstruct();

    auto surface_model = std::make_shared<Model>(vertices,
                                                 indices,
                                                 std::vector<std::shared_ptr<EspTexture>>{},
                                                 SurfaceInit::S_MODEL_PARAMS);

    entity->add_component<ControlLineComponent>(entity->get_id(), vertices.size());
    auto control_line_indices = surface.get_control_line_indices();

    auto control_line_model = std::make_shared<Model>(vertices,
                                                      control_line_indices,
                                                      std::vector<std::shared_ptr<EspTexture>>{},
                                                      SplineInit::S_MODEL_PARAMS);

    entity->add_component<ModelComponent>(
        std::vector<ModelComponentParams>{ { surface_model, s_instance->m_c2_surface_shader, 0, 1 },
                                           { control_line_model, s_instance->m_control_line_shader } });

    surface.get_node()->attach_entity(entity);

    s_instance->m_scene->get_root().add_child(surface.get_node());

    return surface;
  }

  PointComponent& ObjectFactory::get_control_point(uint32_t id)
  {
    return s_instance->m_scene->get_component<PointComponent>(id);
  }
} // namespace mg1
