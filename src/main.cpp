#include "digitator.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

/**
 * @brief Application config options.
 */
struct Appconfig {
  std::string input_file {};
  std::string output_file {};
};

/**
 * @brief Processes the args given by CLI.
 * @param args The args in a vector of strings format.
 * @param app_config The config for the app.
 * @return True if the arguments were OK. It can only fail if there's no input
 *  file specified.
 */
bool processArgs(const std::vector<std::string>& args, Appconfig& app_config);

int main(int argc, char* argv[]) {
  using namespace dgt;

  Appconfig app_config {};

  if (!processArgs(std::vector<std::string>(argv, argv + argc), app_config)) {
    std::cerr << "No input file provided. Aborting.\n";
    return 1;
  }

  Digitator digitator {};
  const auto result {digitator.processFile(app_config.input_file)};

  if (app_config.output_file != "") {
    // write to file
    std::ofstream output_file(app_config.output_file);
    output_file << result;
    output_file.close();
  }

  return 0;
}

bool processArgs(const std::vector<std::string>& args, Appconfig& app_config) {
  bool input_file_found {false};
  for (std::size_t i = 0; i < args.size(); ++i) {

    if (args[i].find("-i") != std::string::npos) {
      // we probably have an input file
      // check that we are not in the last arg
      if (i < args.size() - 1) {
        app_config.input_file = args[i + 1];
        input_file_found = true;
      }
    }

    if (args[i].find("-o") != std::string::npos) {
      if (i < args.size() - 1) {
        app_config.output_file = args[i + 1];
      }
    }
  }
  return input_file_found;
}
