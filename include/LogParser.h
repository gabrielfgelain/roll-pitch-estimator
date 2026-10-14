#pragma once
#include "Sample.h"

#include <optional>
#include <string>
#include <vector>

std::optional<AccelSample> parseLine(const std::string &line);

std::vector<AccelSample> parseLogFile(const std::string &path);