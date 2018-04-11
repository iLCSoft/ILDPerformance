function echo_subdir_base {
  detector_model=$1
  ilcsoft_version=$2
  ildconfig_version=$3

  echo "${ilcsoft_version}_ILDConfig_${ildconfig_version}_${detector_model}"
}
