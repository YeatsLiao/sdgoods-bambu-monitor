set(app_sources "main.c" "apps/app_template.c")
if(NOT CONFIG_BAMBU_BASELINE)
    list(APPEND app_sources
        "bambu/bambu_state.c" "bambu/bambu_report.c" "bambu/bambu_pages.c"
        "bambu/bambu_mqtt.c" "bambu/app_config.c" "bambu/wifi_prov.c"
        "bambu/prov_form.c" "bambu/pairing_probe.c"
        "bambu/monitor_service.c" "bambu/monitor_preferences.c"
        "apps/ui_monitor.c" "apps/monitor_skin.c"
        "fonts/bambu_font_16.c" "fonts/bambu_font_20.c" "fonts/bambu_digits_72.c"
        "fonts/bambu_bold_18.c" "fonts/bambu_bold_22.c" "fonts/bambu_bold_26.c"
        "fonts/bambu_num_44.c" "fonts/bambu_num_64.c" "fonts/bambu_num_88.c" "fonts/bambu_num_128.c")
    if(CONFIG_BAMBU_SKIN_COUNT GREATER 1)
        list(APPEND app_sources "assets/scene_printer.c" "assets/scene_cat.c" "assets/scene_night.c" "assets/cat_peek.c")
    endif()
    if(CONFIG_BAMBU_SKIN_COUNT GREATER 6)
        list(APPEND app_sources "assets/art_handheld.c" "assets/art_robot.c"
            "assets/art_bamboo.c" "assets/art_space.c" "assets/robot_head.c" "assets/astronaut_head.c" "assets/paper_tile.c" "assets/bamboo_leaf.c")
    endif()
    if(CONFIG_BAMBU_SKIN_COUNT GREATER 12)
        list(APPEND app_sources "apps/monitor_art.c" "assets/art_farm.c" "assets/art_cyber.c"
            "assets/art_ocean.c" "assets/art_snow.c" "assets/art_desert.c" "assets/art_magic.c")
    endif()
endif()
idf_component_register(SRCS ${app_sources}
    INCLUDE_DIRS "." "apps" "bambu"
    REQUIRES sdgoods_board sdgoods_launcher driver esp_event esp_netif
        esp_timer esp_wifi freertos heap log lvgl nvs_flash mqtt json lwip
        esp_http_server dns_server)
set(_bv_dir ${CMAKE_CURRENT_BINARY_DIR})
add_custom_target(gen_build_version ALL
    COMMAND ${CMAKE_COMMAND} -D BIN_DIR=${_bv_dir}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/gen_build_version.cmake
    BYPRODUCTS ${_bv_dir}/build_version.h)
add_dependencies(${COMPONENT_LIB} gen_build_version)
target_include_directories(${COMPONENT_LIB} PRIVATE ${_bv_dir})
idf_build_get_property(_patch_python PYTHON)
execute_process(COMMAND ${_patch_python}
    ${CMAKE_CURRENT_SOURCE_DIR}/patches/apply_lvgl_patches.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..
    RESULT_VARIABLE patch_result)
if(NOT patch_result EQUAL 0)
    message(FATAL_ERROR "LVGL patch verification failed")
endif()
# Offline LVGL is already patched in the radio snapshot. The original upstream
# script handles managed_components; explicitly verify the local copy as well.
foreach(patch_file gifdec.c gifdec.h lv_gif.c)
    execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files
        ${CMAKE_CURRENT_SOURCE_DIR}/patches/lvgl-8.3.11-gif-psram/${patch_file}
        ${CMAKE_CURRENT_SOURCE_DIR}/../components/lvgl/src/extra/libs/gif/${patch_file}
        RESULT_VARIABLE offline_patch_result)
    if(NOT offline_patch_result EQUAL 0)
        message(FATAL_ERROR "Offline LVGL patch mismatch: ${patch_file}")
    endif()
endforeach()
