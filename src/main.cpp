#include "parser.h"
#include "tags_writer.h"
#include "token_iterator.h"

#include <memory>
#include <stdexcept>

int main(int argc, char *argv[])
{
  try
  {
    char const* fname = argc < 2 ? "test.lua" : argv[1];
    std::shared_ptr<FILE> file(fopen(fname, "rb"), [](FILE* f){if (f) fclose(f);});
    if (!file)
      throw std::runtime_error(std::string("Failed to open file: ") + fname);

    TokenIterator iter(&*file);
    TagsWriter writer(fname, &*file, stdout);
    Parse(iter, writer);
  }
  catch(std::exception const& e)
  {
    fprintf(stderr, "Error: %s\n", e.what());
    return 1;
  }
}
