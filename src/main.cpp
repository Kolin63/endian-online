#include <cassert>
#include <cstdlib>
#include <string>

#include "bot.hpp"
#include "cli_args.hpp"
#include "globals.hpp"
#include "mod_loader.hpp"

int main(int argc, char** argv) {
  end::CliArgs cli_args{argc, argv};

  if (cli_args.error) return 0;

  end::Bot endbot{cli_args.instance_dir, cli_args.run_discord};
  end::globals::bot = &endbot;
  assert(end::globals::bot != nullptr);

  end::ModLoader::LoadMods();

  return 0;
}
