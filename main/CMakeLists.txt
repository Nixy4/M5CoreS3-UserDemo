set(src_dirs        
  . 
  ./apps/faceRecogniz
  ./hal)

set(include_dirs    
  . 
  ./apps/faceRecogniz
  ./hal
  )

set(requires        
  mooncake
  M5CoreS3-UserBSP
  )

idf_component_register(
  SRC_DIRS ${src_dirs} 
  INCLUDE_DIRS ${include_dirs}
  REQUIRES ${requirs}
  )

component_compile_options(-ffast-math -O3)

