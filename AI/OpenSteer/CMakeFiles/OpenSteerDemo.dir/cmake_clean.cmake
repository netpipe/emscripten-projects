file(REMOVE_RECURSE
  "OpenSteerDemo.pdb"
  "OpenSteerDemo.js"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/OpenSteerDemo.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
