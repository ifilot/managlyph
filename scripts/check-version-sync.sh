#!/usr/bin/env bash
set -euo pipefail

config_version=$(sed -n 's/^#define PROGRAM_VERSION "\([^"]*\)"$/\1/p' src/config.h)
installer_version=$(sed -n 's/^!define APP_VERSION "\([^"]*\)"$/\1/p' installer.nsi)

if [[ -z "${config_version}" ]]; then
  echo "ERROR: Could not read PROGRAM_VERSION from src/config.h"
  exit 1
fi

if [[ -z "${installer_version}" ]]; then
  echo "ERROR: Could not read APP_VERSION from installer.nsi"
  exit 1
fi

if [[ "${config_version}" != "${installer_version}" ]]; then
  echo "ERROR: Version mismatch detected"
  echo "  src/config.h PROGRAM_VERSION=${config_version}"
  echo "  installer.nsi APP_VERSION=${installer_version}"
  exit 1
fi

echo "Versions are in sync: ${config_version}"
