if(CONFIG_XTENSA_HAL)

  zephyr_interface_library_named(XTENSA_HAL)

  target_include_directories(XTENSA_HAL INTERFACE include)
  target_include_directories(XTENSA_HAL INTERFACE zephyr/soc/${SOC_NAME}/)

  zephyr_library_named(modules_xtensa_hal)

  zephyr_include_directories(
    include
    zephyr/soc/${SOC_NAME}/
    )

  zephyr_library_sources(
    src/hal/attribute.c
    src/hal/cache_asm.S
    src/hal/cache.c
    src/hal/clock.S
    src/hal/coherence.c
    src/hal/debug.c
    src/hal/debug_hndlr.S
    src/hal/disass.c
    src/hal/int_asm.S
    src/hal/interrupts.c
    src/hal/memcopy.S
    src/hal/mem_ecc_parity.S
    src/hal/misc.c
    src/hal/miscellaneous.S
    src/hal/mmu.c
    src/hal/mp_asm.S
    src/hal/set_region_translate.c
    src/hal/state_asm.S
    src/hal/state.c
    src/hal/syscache_asm.S
    src/hal/windowspill_asm.S
    )

  zephyr_library_link_libraries(XTENSA_HAL)
  target_link_libraries(XTENSA_HAL INTERFACE zephyr_interface)

endif()
