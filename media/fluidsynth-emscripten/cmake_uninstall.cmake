IF(NOT EXISTS "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/media/fluidsynth-emscripten/install_manifest.txt")
  MESSAGE(FATAL_ERROR "Cannot find install manifest: \"/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/media/fluidsynth-emscripten/install_manifest.txt\"")
ENDIF(NOT EXISTS "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/media/fluidsynth-emscripten/install_manifest.txt")

FILE(READ "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/media/fluidsynth-emscripten/install_manifest.txt" files)
STRING(REGEX REPLACE "\n" ";" files "${files}")
FOREACH(file ${files})
  MESSAGE(STATUS "Uninstalling \"${file}\"")
  IF(EXISTS "${file}")
    EXEC_PROGRAM(
      "/usr/bin/cmake" ARGS "-E remove \"${file}\""
      OUTPUT_VARIABLE rm_out
      RETURN_VALUE rm_retval
      )
    IF("${rm_retval}" STREQUAL 0)
    ELSE("${rm_retval}" STREQUAL 0)
      MESSAGE(FATAL_ERROR "Problem when removing \"${file}\"")
    ENDIF("${rm_retval}" STREQUAL 0)
  ELSE(EXISTS "${file}")
    MESSAGE(STATUS "File \"${file}\" does not exist.")
  ENDIF(EXISTS "${file}")
ENDFOREACH(file)
