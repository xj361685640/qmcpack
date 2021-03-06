
IF (NOT QMC_CUDA)
 IF (NOT QMC_COMPLEX)
#
# [Fe(CO)6]2+ molecule gaussian tests, with BFD ECP using Gamess
# Ref for ref  VMC test
#  "kinetic"   "163.8407 0.0020") # kinetic energy
#  "totenergy" "-250.43439 0.00032") # total energy
#  "eeenergy"  "568.6009 0.0016") # e-e energy
#  "samples"   "625868800 0.0") # samples

  LIST(APPEND FeCO6_GMS_SHORT "kinetic" "163.8407 0.23") # kinetic energy
  LIST(APPEND FeCO6_GMS_SHORT "eeenergy" "568.6009 0.18") # e-e energy
  LIST(APPEND FeCO6_GMS_SHORT "totenergy" "-250.43439 0.035") # total energy
  LIST(APPEND FeCO6_GMS_SHORT "samples" "48000 0.0") # samples
  QMC_RUN_AND_CHECK(short-FeCO6_gms-vmc_noj
                    "${CMAKE_SOURCE_DIR}/tests/molecules/FeCO6_b3lyp_gms"
                    vmc_short_noj
                    vmc_short_noj.in.xml
                    1 16
                    TRUE
                    0 FeCO6_GMS_SHORT # VMC
                    )

  LIST(APPEND FeCO6_GMS_LONG "kinetic" "163.8407 0.088") # kinetic energy
  LIST(APPEND FeCO6_GMS_LONG "eeenergy" "568.6009 0.067") # e-e energy
  LIST(APPEND FeCO6_GMS_LONG "totenergy" "-250.43439 0.012") # total energy
  LIST(APPEND FeCO6_GMS_LONG "samples" "480000 0.0") # samples
  QMC_RUN_AND_CHECK(long-FeCO6_gms-vmc_noj
                    "${CMAKE_SOURCE_DIR}/tests/molecules/FeCO6_b3lyp_gms"
                    vmc_long_noj
                    vmc_long_noj.in.xml
                    1 16
                    TRUE
                    0 FeCO6_GMS_LONG # VMC
                    )

 ELSE()
    MESSAGE("Skipping  FeCO6_b3lyp_gms tests because gaussian basis sets are not supported by complex build (QMC_COMPLEX=1)")
 ENDIF()
ELSE()
    MESSAGE("Skipping FeCO6_b3lyp_gms tests because gaussian basis sets are not supported by CUDA build (QMC_CUDA=1)")
ENDIF()

